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
  /* Variables holding netCDF ids of the file and three variables stored in it (longitude, latitude and a dummy 2D variable) */
  int ncid, lonid, latid, varid;
  /* Variables holding three dimensions: longitude, latitude and a temporal dimension  */
  int lon_dimid, lat_dimid, temp_dimid;
  /* Array holding three dimensions of the dummy 2D varialbe */
  int var_dimids[NDIMS+1];
  /* Start and count for parallel write of longitude, latitude and the variable */ 
  size_t start, count;
  size_t var_starts[NDIMS+1], var_counts[NDIMS+1];
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
  if (err=nc_create_par(filename, NC_NETCDF4, MPI_COMM_WORLD, MPI_INFO_NULL, &ncid))
    ERR(err);

  /* Define two spatial dimensions */
  if (err=nc_def_dim(ncid, "lat", NI_GLO, &lat_dimid))
    ERR(err);
  if (err=nc_def_dim(ncid, "lon", NJ_GLO, &lon_dimid))
    ERR(err);

  /* Define a temporal dimension */
  if (err=nc_def_dim(ncid, "time", NC_UNLIMITED, &temp_dimid))
    ERR(err);

  /* Define 1D variables that will hold latitude and longitude */
  if (err=nc_def_var(ncid, "lat", NC_FLOAT, 1, &lat_dimid, &latid)) 
    ERR(err);
  if (err=nc_put_att(ncid, latid, "standart_name", NC_CHAR, strlen("latitude"), "latitude"))  
    ERR(err);
  if (err=nc_put_att(ncid, latid, "units", NC_CHAR, strlen("degrees_north"), "degrees_north"))  
    ERR(err);
  if (err=nc_def_var(ncid, "lon", NC_FLOAT, 1, &lon_dimid, &lonid))
    ERR(err);
  if (err=nc_put_att(ncid, lonid, "standart_name", NC_CHAR, strlen("longitude"), "longitude"))
    ERR(err);
  if (err=nc_put_att(ncid, lonid, "units", NC_CHAR, strlen("degrees_east"), "degrees_east"))
    ERR(err);

  /* Define a 2D temporal variable and add an attribute */
  var_dimids[0] = temp_dimid;
  var_dimids[1] = lat_dimid;
  var_dimids[2] = lon_dimid;
  if (err=nc_def_var(ncid, "var_2d_temp", NC_INT, NDIMS+1, var_dimids, &varid))
    ERR(err);
  if (err=nc_put_att(ncid, varid, "standart_name", NC_CHAR, strlen("dummy_temporal_variable"), "dummy_temporal_variable"))
    ERR(err);

  /* Close metadata definition */
  if (err=nc_enddef(ncid))
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

  /* Write lon, lat.
   * Here, the third and the fourth arguments, start and count, should be of type 'size_t *'.*/
  start = ibegin; count = ni;
  if (err=nc_put_vara(ncid, latid, &start, &count, lat)) ERR(err);
  start = jbegin; count = nj;
  if (err=nc_put_vara(ncid, lonid, &start, &count, lon)) ERR(err);  

  var_starts[0] = 0;  var_starts[1] = ibegin;  var_starts[2] = jbegin;
  var_counts[0] = 1;  var_counts[1] = ni;      var_counts[2] = nj;

  /* Set mode to collective for temporal variable */
  if (err=nc_var_par_access(ncid, varid, NC_COLLECTIVE)) ERR(err);

  /* Write temporal data */
  for (int t=0; t<NTIME; ++t)
  { 
    var_starts[0] = t;
    if (err=nc_put_vara(ncid, varid, var_starts, var_counts, data)) ERR(err);
  }

  /* Close the file */
  if (err=nc_close(ncid)) ERR(err);

  free(data);
  free(lon);
  free(lat);

  MPI_Finalize();

  return 0;
}     

