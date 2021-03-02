#include <stdio.h>
#include <stdlib.h>
#include "sion.h"

int main(int argc, char** argv)
{
  int sid, numFiles, globalrank;
  sion_int64 *chunksizes = NULL;
  sion_int32 fsblksize;
  int *globalranks = NULL;
  int ntasks;
  int task;
  int i;
  int datasize_in_block;
  int *data;

  /* open file */
  sid = sion_open("parfile.sion", "r", &ntasks, &numFiles,
                  &chunksizes, &fsblksize, &globalranks,
                  NULL);


  for (task=0; task < ntasks; ++task) {
    /* swict hto task */
    sion_seek(sid, task, SION_CURRENT_BLK, SION_CURRENT_POS);
    printf ("Task: %i \n",task);
    /* read all chunks */
    while (! sion_feof(sid)) {
        /* check size of current data in chunk */
        datasize_in_block = sion_bytes_avail_in_block(sid);
        data = malloc(datasize_in_block);
        sion_fread(data,1,datasize_in_block,sid);
        for (i = 0; i < datasize_in_block/sizeof(int); i++) {
            printf ("%i ",data[i]);
        }
        printf ("\n");
        free(data);
    }
  }

  free(chunksizes);
  free(globalranks);

  /* close file */
  sion_close(sid);

  return EXIT_SUCCESS;
}
