#include <sys/stat.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>

#if defined(ZXS) || defined(C64)
static int PiB = 8;
#endif

#if defined(CPC)
static int PiB = 4;
#endif

static char option;

struct Header {
    short w, h;
} header;

typedef struct {
    unsigned char *ptr;
    int size;
} Buffer;

static const char *file_name = NULL;

static int __attribute__((unused)) is_file(const char *name) {
    return strcmp(file_name, name) == 0;
}

static int __attribute__((unused)) is_prefix(const char *name) {
    return strncmp(file_name, name, strlen(name)) == 0;
}

int estimate(int size);
int compress(void *dst, void *src, int size);

static void __attribute__((unused)) hexdump(unsigned char *buf, int size) {
    for (int i = 0; i < size; i++) {
	fprintf(stderr, "%02x ", buf[i]);
	if ((i & 0xf) == 0xf) {
	    fprintf(stderr, "\n");
	}
    }
    if ((size & 0xf) != 0x0) fprintf(stderr, "\n");
}

static void dump_buffer(void *ptr, int size, int step) {
    for (int i = 0; i < size; i++) {
	if (step == 1) {
	    printf(" 0x%02x,", * (unsigned char *) ptr);
	}
	else {
	    printf(" 0x%04x,", * (unsigned short *) ptr);
	}
	if ((i & 7) == 7) printf("\n");
	ptr += step;
    }
    if ((size & 7) != 0) printf("\n");
}

static char replace_char(char c) {
    switch (c) {
    case '/':
	return '_';
    case '.':
	return 0;
    default:
	return c;
    }
}

static void rm_ext_to(char *dst, const char *src) {
    while (*src) { *dst++ = replace_char(*src++); }
}

static char *rm_ext(const char *src) {
    static char buf[256];
    rm_ext_to(buf, src);
    return buf;
}

#if defined(CPC)
static int use_alternate_mapping(void) {
    return is_file("ranges.pcx")
	|| is_file("beigas.pcx")
	|| is_file("niedres.pcx");
}

static int use_alternate_panel(void) {
    return is_prefix("panel_2") || is_prefix("panel_3");
}

const unsigned char *color_table = NULL;

static void select_color(void) {
    static const unsigned char alternate_map[] = {
	0, 1, 0, 1, 3, 3, 2, 2,
	0, 3, 0, 1, 3, 3, 2, 2,
    };
    static const unsigned char default_map[] = {
	0, 1, 0, 2, 2, 1, 2, 3,
	0, 2, 2, 2, 2, 1, 2, 3,
    };
    static const unsigned char panel_map[] = {
	0, 1, 1, 2, 2, 3, 2, 3,
	0, 2, 2, 2, 2, 1, 2, 3,
    };
    if (use_alternate_mapping()) {
	color_table = alternate_map;
    }
    else if (use_alternate_panel()) {
	color_table = panel_map;
    }
    else {
	color_table = default_map;
    }
}
#else
static void select_color(void) { }
#endif

const unsigned char c64_look_up[] = {
    0x00, 0x06, 0x02, 0x04, 0x05, 0x03, 0x07, 0x0f,
    0x00, 0x0e, 0x02, 0x0a, 0x0c, 0x0d, 0x07, 0x01,
};

static unsigned char get_color(unsigned char *color) {
    unsigned char result = 0;
    if (color[0] >= 0x80) result |= 0x02;
    if (color[1] >= 0x80) result |= 0x04;
    if (color[2] >= 0x80) result |= 0x01;
    for (int i = 0; i < 3; i++) {
	if (color[i] > (result ? 0xf0 : 0x40)) {
	    result |= 0x40;
	    break;
	}
    }

    int __attribute__((unused)) index;
    index = (result & 0x07) + ((result & 0x40) >> 3);

#if defined(CPC)
    return color_table[index];
#elif defined(C64)
    return c64_look_up[index];
#else
    return result;
#endif
}

static int read_file(const char *file, unsigned char **buf) {
    struct stat st;
    if (stat(file, &st) != 0) {
	fprintf(stderr, "ERROR file \"%s\" not found\n", file);
	exit(-ENOENT);
    }

    *buf = malloc(st.st_size);
    int in = open(file, O_RDONLY);
    read(in, *buf, st.st_size);
    close(in);

    return st.st_size;
}

static unsigned char *read_pcx(const char *file) {
    unsigned char *buf;
    int palette_offset = 16;
    int length = read_file(file, &buf);
    header.w = (* (unsigned short *) (buf + 0x8)) + 1;
    header.h = (* (unsigned short *) (buf + 0xa)) + 1;
    if (buf[3] == 8) palette_offset = length - 768;
    int unpacked_size = header.w * header.h / (buf[3] == 8 ? 1 : 2);
    unsigned char *pixels = malloc(unpacked_size);

    int i = 128, j = 0;
    while (j < unpacked_size) {
	if ((buf[i] & 0xc0) == 0xc0) {
	    int count = buf[i++] & 0x3f;
	    while (count-- > 0) {
		pixels[j++] = buf[i];
	    }
	    i++;
	}
	else {
	    pixels[j++] = buf[i++];
	}
    }

    select_color();
    for (i = 0; i < unpacked_size; i++) {
	int entry = palette_offset + 3 * pixels[i];
	pixels[i] = get_color(buf + entry);
    }

    free(buf);
    return pixels;
}

static void compress_and_save(const char *name, Buffer buf) {
    unsigned char dst[estimate(buf.size)];
    fprintf(stderr, "compress \"%s\" ", name);
    int size = compress(dst, buf.ptr, buf.size);
    printf("const byte %s[] = {\n", name);
    dump_buffer(dst, size, 1);
    printf("};\n");
}

#if defined(ZXS) || defined(C64)
static int ink_index(int i) {
    return (i / header.w / 8) * (header.w / 8) + i % header.w / 8;
}

static unsigned short encode_pixel(unsigned char a, unsigned char b) {
    return a > b ? (b << 8) | a : (a << 8) | b;
}

#if defined(ZXS)
static unsigned char is_bright(unsigned char f, unsigned char b) {
    return (f > 7 || b > 7) ? 0x40 : 0x00;
}

static unsigned char encode_ink(unsigned short colors) {
    unsigned char b = colors >> 8;
    unsigned char f = colors & 0xff;
    return is_bright(f, b) | (f & 7) | ((b & 7) << 3);
}
#else
static unsigned char encode_ink(unsigned short colors) {
    return ((colors & 0xff) << 4) | (colors >> 8);
}
#endif

static unsigned char consume_pixels(unsigned char *buf, unsigned char on) {
    unsigned char ret = 0;
    for (int i = 0; i < 8; i++) {
	ret = ret << 1;
	ret |= (buf[i] == on || (buf[i] != 0 && option == 'p')) ? 1 : 0;
    }
    return ret;
}

static unsigned short on_pixel(unsigned char *buf, int i, int w) {
    static unsigned char prev_n = 0x00;
#if defined(C64)
    static unsigned char prev_p = 0x01;
#else
    static unsigned char prev_p = 0x47;
#endif
    unsigned char pixel = buf[i];
    for (int y = 0; y < 8; y++) {
	for (int x = 0; x < 8; x++) {
	    unsigned char next = buf[i + x];
	    if (next != pixel) {
		prev_n = next;
		prev_p = pixel;
		return encode_pixel(next, pixel);
	    }
	}
	i += w;
    }
    unsigned char other = pixel == prev_p ? prev_n : prev_p;
    if (pixel != 0) other = other & ~0x40;
    return encode_pixel(other, pixel);
}
#endif

#if defined(CPC)
static unsigned char consume_pixels(unsigned char *buf) {
    unsigned char ret = 0;
    for (int i = 0; i < PiB; i++) {
	ret = ret << 1;
	ret |= ((buf[i] >> 1) & 0x01);
	if (PiB != 4) continue;
	ret |= ((buf[i] << 4) & 0x10);
    }
    return ret;
}
#endif

static int image_size(void) {
    return header.w * header.h;
}

static int pixel_size(void) {
    return image_size() / PiB;
}

static int color_size(void) {
#if defined(ZXS) || defined(C64)
    return pixel_size() / 8;
#else
    return 0;
#endif
}

static int total_size(void) {
    return pixel_size() + color_size();
}

static void *convert_bitmap(unsigned char *buf) {
    unsigned char pixel[total_size()];

#if defined(ZXS) || defined(C64)
    int j = 0;
    unsigned char *color = pixel + pixel_size();
    unsigned short on[color_size()];

    for (int y = 0; y < header.h; y++) {
	for (int x = 0; x < header.w; x += 8) {
	    int i = y * header.w + x;
	    if (i / header.w % 8 == 0) {
		on[j++] = on_pixel(buf, i, header.w);
	    }
	    unsigned char data = on[ink_index(i)] & 0xff;
#if defined(C64)
	    int pos = (y >> 3) * header.w + x + (y & 7);
#else
	    int pos = i / 8;
#endif
	    pixel[pos] = consume_pixels(buf + i, data);
	}
    }
    for (int i = 0; i < color_size(); i++) {
	color[i] = encode_ink(on[i]);
    }
#endif

#if defined(CPC)
    for (int i = 0; i < image_size(); i += PiB) {
	pixel[i / PiB] = consume_pixels(buf + i);
    }
#endif

    memcpy(buf, pixel, total_size());
    return buf;
}

static int translate(int y) {
    return ((y & 7) << 3) | ((y >> 3) & 7) | (y & 0xc0);
}

static unsigned char *swizzle(unsigned char *buf) {
    unsigned char tmp[pixel_size()];
    for (int y = 0; y < header.h; y++) {
	memcpy(tmp + 32 * translate(y), buf + 32 * y, 32);
    }
    memcpy(buf, tmp, pixel_size());
    return buf;
}

const int HEADER_SIZE = 2;
static unsigned char *add_header(unsigned char *buf) {
    unsigned char *img = malloc(total_size() + HEADER_SIZE);
    memcpy(img + HEADER_SIZE, buf, total_size());
#if defined(C64)
    img[0] = header.w;
    img[1] = header.h / PiB;
#else
    img[0] = header.w / PiB;
    img[1] = header.h;
#endif
    free(buf);
    return img;
}

static Buffer read_bitmap(const char *name) {
    return (Buffer) {
	.ptr = add_header(convert_bitmap(read_pcx(name))),
	.size = total_size() + HEADER_SIZE,
    };
}

static Buffer raw_bitmap(const char *name) {
    return (Buffer) {
	.ptr = convert_bitmap(read_pcx(name)),
	.size = pixel_size(),
    };
}

static Buffer read_screen(const char *name) {
    return (Buffer) {
	.ptr = swizzle(convert_bitmap(read_pcx(name))),
	.size = total_size(),
    };
}

static void save_buffer(const char *name, Buffer buf) {
    compress_and_save(rm_ext(name), buf);
    free(buf.ptr);
}

static void save_image(const char *name) {
    save_buffer(name, read_bitmap(name));
}

static void save_bitmap(const char *name) {
    save_buffer(name, raw_bitmap(name));
}

static void save_screen(const char *name) {
    save_buffer(name, read_screen(name));
}

static char *upcase(char *str) {
    char *ptr = str;
    while (*ptr) {
	*ptr = toupper(*ptr);
	ptr++;
    }
    return str;
}

static void save_offset(const char *file_name, int offset) {
    printf("#define %s 0x%04x\n", upcase(rm_ext(file_name)), offset);
}

static void save_series(const char *name, char **files, int count) {
    int total = 0;
    Buffer bufs[count];

    for (int i = 0; i < count; i++) {
	bufs[i] = read_bitmap(files[i]);
	save_offset(files[i], total);
	total += bufs[i].size;
    }

    Buffer all = { .ptr = malloc(total), .size = 0 };

    for (int i = 0; i < count; i++) {
	memcpy(all.ptr + all.size, bufs[i].ptr, bufs[i].size);
	all.size += bufs[i].size;
    }

    compress_and_save(name, all);

    for (int i = 0; i < count; i++) {
	free(bufs[i].ptr);
    }
    free(all.ptr);
}

int main(int argc, char **argv) {
    if (argc < 3) {
	fprintf(stderr, "USAGE: pcx-dump [option] file.pcx\n");
	fprintf(stderr, "  -i   dump compressed image\n");
	fprintf(stderr, "  -s   dump compressed series\n");
	fprintf(stderr, "  -b   dump compressed bitmap\n");
	fprintf(stderr, "  -x   dump compressed screen\n");
	return 0;
    }

    option = argv[1][1];
    file_name = argv[2];

    switch (option) {
    case 'i':
	save_image(argv[2]);
	break;
    case 'b':
	PiB = 8;
	save_bitmap(argv[2]);
	break;
    case 'x':
	save_screen(argv[2]);
	break;
    case 's':
	save_series(argv[2], argv + 3, argc - 3);
	break;
    default:
	break;
    }
    return 0;
}
