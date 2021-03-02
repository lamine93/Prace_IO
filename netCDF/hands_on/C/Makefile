CC=mpicc
LDFLAGS=-lnetcdf

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

ALL=hands_on_par_write hands_on_par_read hands_on_compression

all: $(ALL)

hands_on_par_write: hands_on_par_write.o
	$(CC) -o $@ $^ $(LDFLAGS)

hands_on_par_read: hands_on_par_read.o
	$(CC) -o $@ $^ $(LDFLAGS)

hands_on_compression: hands_compression.o
	$(CC) -o $@ $^ $(LDFLAGS)

clean: 
	rm -f *.o

cleanall:
	rm -f *.o $(ALL)
