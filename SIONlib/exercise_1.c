#include <stdlib.h>
#include <mpi.h>
#include <sion.h>

int main(int argc, char** argv) {
  /* SION parameters */
  int        sid, numFiles, globalrank;
  sion_int64 chunksize;
  sion_int32 fsblksize;
  MPI_Comm   lComm = MPI_COMM_NULL;

  /* initialize MPI */
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &globalrank);

  /* create a new file */
  chunksize  = //TODO
  numFiles  = //TODO
  fsblksize = //TODO
  sid = sion_paropen_mpi(/*TODO */);

  /* close file */
  sion_parclose_mpi(sid);

  /* finalize MPI */
  MPI_Finalize();

  return EXIT_SUCCESS;
}
