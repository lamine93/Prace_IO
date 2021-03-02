#include "mpi.h"
#include <netcdf.h>
#include <netcdf_par.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NI_GLO    5 
#define NJ_GLO    10
#define NDIMS 2

#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); return 2;}


// Function decomposes global domain of size NI_GLO*NJ_GLO in horizontal direction into local domains
void decompose_domain(int mpisize, int mpirank, int* ni, int* ibegin, int* nj, int* jbegin)
{
    *nj=NJ_GLO;
    *ibegin=0;
    *jbegin=0;
    for (int n=0; n<mpisize; ++n)
    {     
      *ni=NI_GLO/mpisize;
      if (n<(NI_GLO%mpisize)) ++(*ni);
      if (n==mpirank) break;
      (*ibegin)+=(*ni);
    }
}

// Funciton fills in local domain data
void init_local_data(int* data, int ni, int ibegin, int nj, int jbegin)
{
  for(int i=0; i < ni; i++)
    for(int j=0; j < nj; j++)
      data[j+i*nj] = (j+jbegin) + (i+ibegin)*nj;
}

int main (void)
{
  int dimids[NDIMS];
  size_t starts[NDIMS], counts[NDIMS];
  int mpirank,mpisize;
  int ncid,xdimid,ydimid,varid;
  int err;
  int ni, nj, ibegin, jbegin;
  char filename[128] = "par_write.nc";

  //--------------------------------------
  // Initialization MPI
  //--------------------------------------

  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD,&mpirank);
  MPI_Comm_size(MPI_COMM_WORLD,&mpisize);

  //--------------------------------------
  //  NetCDF metadata definitions
  //--------------------------------------

  // Create a netcdf file
  if (err=nc_create_par(filename, NC_NETCDF4, MPI_COMM_WORLD, MPI_INFO_NULL, &ncid)) ERR(err);

  // Define two spatial dimensions
  if (err=nc_def_dim(ncid, "ni", NI_GLO, &xdimid)) ERR(err);
  if (err=nc_def_dim(ncid, "nj", NJ_GLO, &ydimid)) ERR(err);
  
  // Define a 2D variable and add an attribute
  dimids[0] = xdimid; 
  dimids[1] = ydimid;
  if (err=nc_def_var(ncid, "toto", NC_INT, NDIMS, dimids, &varid)) ERR(err);
  if (err=nc_put_att(ncid, varid, "standart_name", NC_CHAR, strlen("Dummy variable"), "Dummy variable")) ERR(err);

  // Close metadata definition
  nc_enddef(ncid);

  //--------------------------------------
  // Local data initialization
  //--------------------------------------

  // Decompose global domain
  // Returned values: ni,nj,ibegin,jbegin
  decompose_domain(mpisize,mpirank,&ni,&ibegin,&nj,&jbegin);
  
  // Initialize local data
  int *data = (int *)malloc(sizeof(int)*ni*nj);
  init_local_data(data, ni, ibegin, nj, jbegin);

  //--------------------------------------
  // Write data and close the file  
  //--------------------------------------

  // Write data
  starts[0] = ibegin; starts[1] = jbegin;
  counts[0] = ni;     counts[1] = nj;
  if (err=nc_put_vara(ncid, varid, starts, counts, data)) ERR(err);
  
  // Close the file
  if (err=nc_close(ncid)) ERR(err);

  free(data);

  MPI_Finalize();

  return 0;
}     

