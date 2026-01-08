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
	@./pcx-dump -i title.pcx > data.h
	@./pcx-dump -i ezers.pcx >> data.h
	@./pcx-dump -i puduris1.pcx >> data.h
	@./pcx-dump -i puduris2.pcx >> data.h
	@./pcx-dump -i niedres.pcx >> data.h
	@./pcx-dump -i krasts.pcx >> data.h
	@./pcx-dump -i egle1.pcx >> data.h
	@./pcx-dump -i egle2.pcx >> data.h
	@./pcx-dump -i egle3.pcx >> data.h
	@./pcx-dump -i urbis.pcx >> data.h
	@./pcx-dump -i swirl.pcx >> data.h
	@./pcx-dump -i drill.pcx >> data.h
	@./pcx-dump -i hole.pcx >> data.h
	@./pcx-dump -i copene1.pcx >> data.h
	@./pcx-dump -i copene2.pcx >> data.h
	@./pcx-dump -i copene3.pcx >> data.h
	@./pcx-dump -i velk1.pcx >> data.h
	@./pcx-dump -i velk2.pcx >> data.h
	@./pcx-dump -i aukla1.pcx >> data.h
	@./pcx-dump -i aukla2.pcx >> data.h
	@./pcx-dump -i loms.pcx >> data.h
	@./pcx-dump -i ruffe.pcx >> data.h
	@./pcx-dump -i perch.pcx >> data.h
	@./pcx-dump -i asaris.pcx >> data.h
	@./pcx-dump -i makans.pcx >> data.h
	@./pcx-dump -i mormene.pcx >> data.h
	@./pcx-dump -i symbols.pcx >> data.h
	@./pcx-dump -i panel_1a.pcx >> data.h

zxs:
	@$(MAKE) CODE=0x8000 DATA=0x7800 prg
	@bin2tap -b liezere.bin

main.o: data.h

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
	rm -f pcx-dump data.h liezere* *.asm *.lst *.sym *.o
