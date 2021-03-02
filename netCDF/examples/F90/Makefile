FC=mpiifort
LDFLAGS=-lnetcdff

%.o: %.f90
	$(FC) -c -o $@ $< $(CFLAGS)

ALL=example_par_write example_par_read

all: $(ALL)

example_par_write: example_par_write.o
	$(FC) -o $@ $^ $(LDFLAGS)

example_par_read: example_par_read.o
	$(FC) -o $@ $^ $(LDFLAGS)

clean: 
	rm -f *.o

cleanall:
	rm -f *.o $(ALL) 
