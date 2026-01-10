ARCH ?= -mz80

MAKE := make --no-print-directory
SIZE := ls -l liezere.bin | cut -d " " -f 5

CFLAGS += --nostdinc --nostdlib --no-std-crt0
CFLAGS += --code-loc $(CODE) --data-loc $(DATA)

LFLAGS += -n -m -i -b _CODE=$(CODE) -b _DATA=$(DATA)

SRC := main.c data.c
OBJ := $(subst .c,.o,$(SRC))

all:	msg zxs
	@echo liezere build done
	@echo binary size $(shell $(SIZE))

msg:
	@echo building liezere

pcx:
	@gcc pcx-dump.c lz.c -Wall -o pcx-dump
	@./pcx-dump -i title.pcx	 > image.h
	@./pcx-dump -i ezers.pcx	>> image.h
	@./pcx-dump -i beigas.pcx	>> image.h
	@./pcx-dump -i hole.pcx		>> image.h
	@./pcx-dump -i copene1.pcx	>> image.h
	@./pcx-dump -i copene2.pcx	>> image.h
	@./pcx-dump -i copene3.pcx	>> image.h
	@./pcx-dump -i urbis.pcx	>> image.h
	@./pcx-dump -i swirl.pcx	>> image.h
	@./pcx-dump -i drill.pcx	>> image.h
	@./pcx-dump -i velk1.pcx	>> image.h
	@./pcx-dump -i velk2.pcx	>> image.h
	@./pcx-dump -i aukla1.pcx	>> image.h
	@./pcx-dump -i aukla2.pcx	>> image.h
	@./pcx-dump -i loms.pcx		>> image.h
	@./pcx-dump -i symbols.pcx	>> image.h

	@./pcx-dump -i niedres.pcx	 > data.h
	@./pcx-dump -i puduris1.pcx	>> data.h
	@./pcx-dump -i puduris2.pcx	>> data.h
	@./pcx-dump -i krasts.pcx	>> data.h
	@./pcx-dump -i egle1.pcx	>> data.h
	@./pcx-dump -i egle2.pcx	>> data.h
	@./pcx-dump -i egle3.pcx	>> data.h
	@./pcx-dump -i ruffe.pcx	>> data.h
	@./pcx-dump -i perch.pcx	>> data.h
	@./pcx-dump -i asaris.pcx	>> data.h
	@./pcx-dump -i makans.pcx	>> data.h
	@./pcx-dump -i mormene.pcx	>> data.h
	@./pcx-dump -i panel_0a.pcx	>> data.h
	@./pcx-dump -i panel_0b.pcx	>> data.h
	@./pcx-dump -i panel_0c.pcx	>> data.h
	@./pcx-dump -i panel_1a.pcx	>> data.h
	@./pcx-dump -i panel_1b.pcx	>> data.h
	@./pcx-dump -i panel_1c.pcx	>> data.h
	@./pcx-dump -i panel_2a.pcx	>> data.h
	@./pcx-dump -i panel_2b.pcx	>> data.h
	@./pcx-dump -i panel_2c.pcx	>> data.h

zxs:
	@$(MAKE) CODE=0x8000 DATA=0x7800 prg
	@bin2tap -b liezere.bin

data.o: data.h
main.o: image.h

%.o: %.c main.h
	@echo compile source file $<
	@sdcc $(ARCH) $(CFLAGS) $(TYPE) -c $< -o $@

prg: pcx $(OBJ)
	@sdld $(LFLAGS) liezere.ihx $(OBJ)
	@hex2bin liezere.ihx > /dev/null

fuse: zxs
	@echo running fuse emulator...
	@fuse --machine 128 --no-confirm-actions liezere.tap >/dev/null

clean:
	rm -f pcx-dump data.h image.h liezere* *.asm *.lst *.sym *.o
