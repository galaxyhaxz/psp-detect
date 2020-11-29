release: all
	mksfo 'PSP Detective' PARAM.SFO
	pack-pbp EBOOT.PBP PARAM.SFO ICON0.PNG NULL NULL NULL NULL detective.prx NULL

TARGET = detective
OBJS = main.o sys/detect_sys.o

BUILD_PRX = 1

INCDIR = 
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =
LIBS =

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
