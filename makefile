TARGET = monome
OBJS = main.o ./Pgck/psp_main.o ./Pgck/Pgck.o ./Pgck/Pson.o

INCDIR = 
CFLAGS = -G4 -Wall -O3 
CXXFLAGS = $(CFLAGS)  -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

#PSP_FW_VERSION=271

PSPSDK=$(shell psp-config --pspsdk-path)
PSPBIN = $(PSPSDK)/../bin

LIBDIR =
LDFLAGS =
STDLIBS=  -lSDL_mixer -lSDLmain -lSDL_image -lSDL -lpng -ljpeg -lm -lz \
	-lvorbisfile -lvorbis  -logg \
	-lpspsdk -lpspctrl  -lpsprtc -lpsppower -lpspgu -lpspaudiolib -lpspaudio -lpsphprm
LIBS=$(shell $(PSPBIN)/sdl-config --libs) $(STDLIBS)$(YOURLIBS)


EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Monome
PSP_EBOOT_ICON = ebooticon.png
PSP_EBOOT_PIC1 = ebootfond.png

PSPSDK=$(shell psp-config --pspsdk-path)
DEFAULT_CFLAGS = $(shell $(SDL_CONFIG) --cflags)
include $(PSPSDK)/lib/build.mak

