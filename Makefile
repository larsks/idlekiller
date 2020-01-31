LDFLAGS = -shared
CFLAGS  = -fPIC
LIBS    = -ldl

all: idlekiller.so

idlekiller.so: idlekiller.o
	$(LD) $(LDFLAGS) -o $@ $< $(LIBS)

clean:
	rm -f idlekiller.so idlekiller.o
