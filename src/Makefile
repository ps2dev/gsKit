EE_OBJS = 	gsCore.o \
		gsFont.o \
		gsInit.o \
		gsMisc.o \
		gsPrimitive.o \
		gsTexture.o

all: $(EE_OBJS)
	$(EE_AR) rcs $(LIBGSKIT) $(EE_OBJS)

clean:
	rm -f $(EE_OBJS)

include ../Makefile.pref
include ../Makefile.global

