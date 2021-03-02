#include "mpi.h"
#include <netcdf.h>
#include <netcdf_par.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NDIMS 2

#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); return 2;}

// Function decomposes global domain of size ni_glo*nj_glo in horizontal direction into local domains
void decompose_domain(int mpisize, int mpirank, int ni_glo, int nj_glo, int* ni, int* ibegin, int* nj, int* jbegin)
{
    *nj=nj_glo;
    *ibegin=0;
    *jbegin=0;
    for (int n=0; n<mpisize; ++n)
    {     
      *ni=ni_glo/mpisize;
      if (n<(ni_glo%mpisize)) ++(*ni);
      if (n==mpirank) break;
      (*ibegin)+=(*ni);
    }
}

// Funciton checks the correctness of the read data
void check_read_data(int* data, int ni, int ibegin, int nj, int jbegin)
{
  int ok = 0;
  for(int i=0; i < ni; i++)
    for(int j=0; j < nj; j++)
      if (data[j+i*nj] != (j+jbegin) + (i+ibegin)*nj)
        ++ok;
  if (ok == 0)
    printf("Parallel read is done successfully.\n");
  else
    printf("Error: something went wrong; data read from the file do not correspond to the expected.\n");
}

int main (void)
{
  size_t starts[NDIMS], counts[NDIMS];
  int mpirank,mpisize;
  int ncid,nidimid,njdimid,varid;
  int err;
  size_t ni_glo, nj_glo;
  int ni, nj, ibegin, jbegin;
  char filename[128] = "par_write_.nc";

  //--------------------------------------
  // Initialization MPI
  //--------------------------------------

  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD,&mpirank);
  MPI_Comm_size(MPI_COMM_WORLD,&mpisize);

  //--------------------------------------
  //  NetCDF metadata definitions
  //--------------------------------------

  // Open netcdf file
  if (err=nc_open_par(filename, NC_NOWRITE, MPI_COMM_WORLD, MPI_INFO_NULL, &ncid)) ERR(err);

  // Get IDs of two spatial dimensions given their names
  if (err=nc_inq_dimid(ncid, "ni",  &nidimid)) ERR(err);
  if (err=nc_inq_dimid(ncid, "nj",  &njdimid)) ERR(err);

  // Get two spatial dimensions
  if (err=nc_inq_dimlen(ncid, nidimid, &ni_glo)) ERR(err);
  if (err=nc_inq_dimlen(ncid, njdimid, &nj_glo)) ERR(err);

  // Get ID of the spacial variable given its name
  if (err=nc_inq_varid(ncid, "toto",  &varid)) ERR(err);

  // Decompose global domain
  // Returned values: ni,nj,ibegin,jbegin
  decompose_domain(mpisize,mpirank,ni_glo, nj_glo, &ni,&ibegin,&nj,&jbegin);
  int *data = (int *)malloc(sizeof(int)*ni*nj);

  //--------------------------------------
  // Read data and close the file  
  //--------------------------------------

  // Read data
  starts[0] = ibegin; starts[1] = jbegin;
  counts[0] = ni;     counts[1] = nj;
  if (err=nc_get_vara(ncid, varid, starts, counts, data)) ERR(err);
  
  // Close the file
  if (err=nc_close(ncid)) ERR(err);

  // Verify the read data
  check_read_data(data, ni, ibegin, nj, jbegin);  

  free(data);

  MPI_Finalize();

  return 0;
}     

