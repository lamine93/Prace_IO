#include "mpi.h"
#include <netcdf.h>
#include <netcdf_par.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NDIMS     2
#define NTIME     2

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

// Funciton checks the correctness of the read data for each time step
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
  int mpirank, mpisize;
  /* Variables holding netCDF ids of the file and three variables stored in it (longitude, latitude and a dummy 2D variable) */
  int ncid, lonid, latid, varid;
  /* Variables holding longitude and latitude */
  int lon_dimid, lat_dimid;
  /* Array holding three dimensions of the dummy 2D varialbe */
  int var_dimids[NDIMS+1];
  /* Start and count for parallel read of the variable */ 
  size_t starts[NDIMS+1], counts[NDIMS+1];
  int err;
  size_t ni_glo, nj_glo;
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

  /* Open a netcdf file 
   * We'll be in data mode */
  if (err=nc_open_par(filename, NC_NOWRITE, MPI_COMM_WORLD, MPI_INFO_NULL, &ncid)) ERR(err);

  /* Get IDs of two dimensions, latitude and longitude */
  if (err=nc_inq_dimid(ncid, "lat", &lat_dimid)) ERR(err);
  if (err=nc_inq_dimid(ncid, "lon", &lon_dimid)) ERR(err);

  /* Get sizes of latitude and longitude */
  if (err=nc_inq_dimlen(ncid, lat_dimid, &ni_glo)) ERR(err);
  if (err=nc_inq_dimlen(ncid, lon_dimid, &nj_glo)) ERR(err);

  /* Assume that each MPI process reads the entirety of lat and lon */
  float *lat_glo = (float *)malloc(sizeof(float)*ni_glo);
  float *lon_glo = (float *)malloc(sizeof(float)*nj_glo);

  /* Get IDs of two varialbes, latitude and longitude */
  if (err=nc_inq_varid(ncid, "lat", &latid)) ERR(err);
  if (err=nc_inq_varid(ncid, "lon", &lonid)) ERR(err);

  /* Read latitude and longitude from the file */
  if (err=nc_get_var(ncid, latid, lat_glo)) ERR(err);
  if (err=nc_get_var(ncid, lonid, lon_glo)) ERR(err);

  /* Get ID of the temporal variable */
  if (err=nc_inq_varid(ncid, "var_2d_temp", &varid)) ERR(err);

  /* Decompose global domain
     Returned values: ni,nj,ibegin,jbegin */
  decompose_domain(mpisize,mpirank,ni_glo,nj_glo,&ni,&ibegin,&nj,&jbegin);
  int *data = (int *)malloc(sizeof(int)*ni*nj);
  
  //--------------------------------------
  // Read temporal data and close the file  
  //--------------------------------------

  starts[0] = 0;  starts[1] = ibegin;  starts[2] = jbegin;
  counts[0] = 1;  counts[1] = ni;      counts[2] = nj;

  /* Set mode to collective for temporal variable */
  //if (err=nc_var_par_access(ncid, varid, NC_COLLECTIVE)) ERR(err);

  /* Read temporal data */
  for (int t=0; t<NTIME; ++t)
  { 
    starts[0] = t;
    /* Read temporal variable */
    if (err=nc_get_vara(ncid, varid, starts, counts, data)) ERR(err);
    /* Check the correctness of the read data */
    check_read_data(data, ni, ibegin, nj, jbegin);
  }

  /* Close the file */
  if (err=nc_close(ncid)) ERR(err);

  free(data);
  free(lat_glo);
  free(lon_glo);

  MPI_Finalize();

  return 0;
}     

