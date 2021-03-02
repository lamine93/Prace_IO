#include "mpi.h"
#include <netcdf.h>
#include <netcdf_par.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NDIMS     2
#define NI_GLO    5 
#define NJ_GLO    10
#define NTIME     2

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

// Function fills in local domain data
void fill_local_data(int* data, int ni, int ibegin, int nj, int jbegin)
{
  int i,j;
  for(i=0; i < ni; i++)
    for(j=0; j < nj; j++)
      data[j+i*nj] = (j+jbegin) + (i+ibegin)*nj;
}

// Function fills in local lon and lat
void fill_lon_lat(float* lon, float* lat, int ni, int ibegin, int nj, int jbegin)
{
  for(int i=0; i < ni; i++)
    lat[i] = -90. + 180./NI_GLO*(i+ibegin+0.5);
  for(int j=0; j < nj; j++)
    lon[j] = -180. + 360./NJ_GLO*(j+jbegin+0.5);
}

int main (void)
{
  int mpirank, mpisize;
  /* Define variables holding netCDF ids of the file and three variables stored in it (longitude, latitude and a dummy 2D variable) */
  ...
  /* Define variables holding three dimensions: longitude, latitude and a temporal dimension  */
  ...
  /* Define an array holding three dimensions of the dummy 2D varialbe */
  ...
  /* Define start and count for parallel write of longitude, latitude and the variable */ 
  ...
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

  /* Create a netcdf file */
  if (err=...)
    ERR(err);

  /* Define two spatial dimensions */
  if (err=...)
    ERR(err);
  if (err=...)
    ERR(err);

  /* Define a temporal dimension */
  if (err=...)
    ERR(err);

  /* Define latitude, a 1D varible called "lat" */
  if (err=...) 
    ERR(err);
  /* Add attribute "standart_name" holding the value "latitude"*/
  if (err=...)  
    ERR(err);
  /* Add attribute "units" holding the value "degrees_north"*/
  if (err=...)  
    ERR(err);
  /* Define longitude, a 1D varible called "lon" */
  if (err=...)
    ERR(err);
  /* Add attribute "standart_name" holding the value "longitude"*/
  if (err=...)
    ERR(err);
  /* Add attribute "units" holding the value "degrees_east"*/
  if (err=...)
    ERR(err);

  /* Define 3 dimensions of the temporal variable */
  var_dimids[0] = ...;
  var_dimids[1] = ...;
  var_dimids[2] = ...;
  if (err=...)
    ERR(err);
  /* Add attribute "standart_name" holding the value "dummy_temporal_variable"*/
  if (err=...)
    ERR(err);

  /* Close metadata definition */
  if (err=...)
    ERR(err);

  //--------------------------------------
  // Local data initialization
  //--------------------------------------

  /* Decompose global domain. Returned values: ni,nj,ibegin,jbegin */
  decompose_domain(mpisize,mpirank,&ni,&ibegin,&nj,&jbegin);
  
  /* Fill in local data: lon, lat and temporal varialbe */
  int *data = (int *)malloc(sizeof(int)*ni*nj);
  fill_local_data(data, ni, ibegin, nj, jbegin);
  
  float *lon = (float *)malloc(sizeof(float)*nj);
  float *lat = (float *)malloc(sizeof(float)*ni);
  fill_lon_lat(lon, lat, ni, ibegin, nj, jbegin);

  //--------------------------------------
  // Write data and close the file  
  //--------------------------------------

  /* Write latitude. */
  ...
  if (err=...)
    ERR(err);
  
  /* Write longitude. */
  ...
  if (err=...)
    ERR(err);  

  var_starts[0] = ...;  var_starts[1] = ...;  var_starts[2] = ...;
  var_counts[0] = ...;  var_counts[1] = ...;  var_counts[2] = ...;

  /* Set mode to collective for temporal variable */
  if (err=...)
    ERR(err);

  /* Write temporal data */
  for (int t=0; t<NTIME; ++t)
  { 
    var_starts[...] = t;
    if (err=...)
      ERR(err);
  }

  /* Close the file */
  if (err=...)
    ERR(err);

  free(data);
  free(lon);
  free(lat);

  MPI_Finalize();

  return 0;
}     

