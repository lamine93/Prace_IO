#include <stdlib.h>
#include <mpi.h>
#include <sion.h>

#define ARRAY_SIZE 1000

int main(int argc, char** argv) {
  int * data;
  int i;

  /* SION parameters */
  int        sid, numFiles, globalrank;
  sion_int64 chunksize;
  sion_int32 fsblksize;
  MPI_Comm   lComm = MPI_COMM_NULL;

  /* initialize MPI */
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &globalrank);

  /* create data */
  data = malloc(ARRAY_SIZE * sizeof(int));
  for(i=0; i<ARRAY_SIZE; ++i) {
     data[i] = globalrank;
  }

  /* create a new file */
  chunksize  = ARRAY_SIZE * sizeof(int);
  numFiles  = 1;
  fsblksize = -1;
  sid = sion_paropen_mpi("parfile.sion", "w", &numFiles,
                         MPI_COMM_WORLD, &lComm,
                         &chunksize, &fsblksize, &globalrank,
                         NULL, NULL);

  /* write data to file */
  sion_fwrite(/*TODO*/);

  /* close file */
  sion_parclose_mpi(sid);

  free(data);

  /* finalize MPI */
  MPI_Finalize();

  return EXIT_SUCCESS;
}
