ARCH ?= -mz80

CODE ?= 0x8000
DATA ?= 0x7800

MAKE := make --no-print-directory
SIZE := ls -l liezere.bin | cut -d " " -f 5

LANGUAGE ?= LATVIAN

CFLAGS += --nostdinc --nostdlib --no-std-crt0
CFLAGS += --code-loc $(CODE) --data-loc $(DATA)
CFLAGS += -D$(LANGUAGE)

LFLAGS += -n -m -i -b _CODE=$(CODE) -b _DATA=$(DATA)

SRC := main.c data.c play.c english.c latvian.c spanish.c
OBJ := $(subst .c,.o,$(SRC))

all:	msg zxs
	@echo liezere build done
	@echo binary size $(shell $(SIZE))

english:
	@LANGUAGE=ENGLISH $(MAKE)

spanish:
	@LANGUAGE=SPANISH $(MAKE)

msg:
	@echo building liezere

pcx:
	@gcc pcx-dump.c lz.c -Wall -o pcx-dump
	@./pcx-dump -i title.pcx	 > data.h
	@./pcx-dump -i ezers.pcx	>> data.h
	@./pcx-dump -i beigas.pcx	>> data.h
	@./pcx-dump -i hole.pcx		>> data.h
	@./pcx-dump -i copene1.pcx	>> data.h
	@./pcx-dump -i copene2.pcx	>> data.h
	@./pcx-dump -i copene3.pcx	>> data.h
	@./pcx-dump -i urbis.pcx	>> data.h
	@./pcx-dump -i swirl.pcx	>> data.h
	@./pcx-dump -i drill.pcx	>> data.h
	@./pcx-dump -i velk1.pcx	>> data.h
	@./pcx-dump -i velk2.pcx	>> data.h
	@./pcx-dump -i aukla1.pcx	>> data.h
	@./pcx-dump -i aukla2.pcx	>> data.h
	@./pcx-dump -i loms.pcx		>> data.h
	@./pcx-dump -i motils.pcx	>> data.h
	@./pcx-dump -i niedres.pcx	>> data.h
	@./pcx-dump -i puduris1.pcx	>> data.h
	@./pcx-dump -i puduris2.pcx	>> data.h
	@./pcx-dump -i krasts.pcx	>> data.h
	@./pcx-dump -i egle1.pcx	>> data.h
	@./pcx-dump -i egle2.pcx	>> data.h
	@./pcx-dump -i egle3.pcx	>> data.h
	@./pcx-dump -i weeds.pcx	>> data.h
	@./pcx-dump -i ruffe.pcx	>> data.h
	@./pcx-dump -i perch.pcx	>> data.h
	@./pcx-dump -i asaris.pcx	>> data.h
	@./pcx-dump -i decent.pcx	>> data.h
	@./pcx-dump -i large.pcx	>> data.h
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
	@./pcx-dump -i panel_3a.pcx	>> data.h
	@./pcx-dump -i panel_3b.pcx	>> data.h
	@./pcx-dump -i panel_3c.pcx	>> data.h
	@./pcx-dump -b symbols.pcx	>> data.h

tap:
	@gcc bin2tap.c -DADDRESS=$(CODE) -o bin2tap
	@./bin2tap loader.bin liezere.bin liezere.tap

zxs: prg tap

data.o: data.h

latvian.o: data.h

english.o: data.h

spanish.o: data.h

%.o: %.c main.h
	@echo compile source file $<
	@sdcc $(ARCH) $(CFLAGS) $(TYPE) -c $< -o $@

prg: pcx $(OBJ)
	@sdld $(LFLAGS) liezere.ihx $(OBJ)
	@makebin -p -yo A -o $(CODE) liezere.ihx liezere.bin

fuse: zxs
	@echo running fuse emulator...
	@fuse --machine 128 --no-confirm-actions liezere.tap >/dev/null

clean:
	rm -f pcx-dump bin2tap data.h liezere* *.asm *.lst *.sym *.o
