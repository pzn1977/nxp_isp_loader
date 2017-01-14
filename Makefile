CFLAGS = -Wall -Werror -O2
LDFLAGS =

all: nxp_isp_loader

clean:
	rm -f -v *.o *~

distclean: clean
	rm -f -v nxp_isp_loader

nxp_isp_loader: assure.o serial.o nxp_encode.o chip.o fileread.o loader.o
	$(CC) $(LDFLAGS) $^ -o $@
