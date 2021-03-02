CC=mpicc
LDFLAGS=-lnetcdf

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

ALL=example_par_write example_par_read example_write_enum example_write_struct example_write_opaque example_write_vlen

all: $(ALL)

example_par_write: example_par_write.o
	$(CC) -o $@ $^ $(LDFLAGS)

example_par_read: example_par_read.o
	$(CC) -o $@ $^ $(LDFLAGS)

example_write_enum: example_write_enum.o
	$(CC) -o $@ $^ $(LDFLAGS)

example_write_struct: example_write_struct.o
	$(CC) -o $@ $^ $(LDFLAGS)

example_write_opaque: example_write_opaque.o
	$(CC) -o $@ $^ $(LDFLAGS)

example_write_vlen: example_write_vlen.o
	$(CC) -o $@ $^ $(LDFLAGS)

clean: 
	rm -f *.o

cleanall:
	rm -f *.o $(ALL)
