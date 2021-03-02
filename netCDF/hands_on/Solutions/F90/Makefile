FC=mpiifort
LDFLAGS=-L${NFDIR}/lib -lnetcdff

%.o: %.f90
	$(FC) -c -o $@ $< $(CFLAGS)

ALL=hands_on_par_write hands_on_par_read hands_on_compression

all: $(ALL)

hands_on_par_write: hands_on_par_write.o
	$(FC) -o $@ $^ $(LDFLAGS)

hands_on_par_read: hands_on_par_read.o
	$(FC) -o $@ $^ $(LDFLAGS)

hands_on_compression: hands_on_compression.o
	$(FC) -o $@ $^ $(LDFLAGS)

clean: 
	rm -f *.o

cleanall:
	rm -f *.o $(ALL) *.nc
