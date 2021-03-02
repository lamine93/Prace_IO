#include "mpi.h"
#include <netcdf.h>
#include <netcdf_par.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NDIMS     2
#define NI_GLO    1000 
#define NJ_GLO    1000

#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); return 2;}

// Function fills in data
void fill_data(int* data, int ni, int nj)
{
  int i,j;
  for(i=0; i < ni; i++)
    for(j=0; j < nj; j++)
      data[j+i*nj] = j + i*nj;
}

int main (void)
{
  int mpirank, mpisize;
  /* Variables holding netCDF ids of the file and the variables stored in it */
  int ncid, varid;
  /* Variables holding dimensions: longitude, latitude  */
  int lon_dimid, lat_dimid, temp_dimid;
  /* Array holding dimensions of the 2D varialbe */
  int var_dimids[NDIMS];
  /* Compression variables */
  int shuffle, deflate, deflate_level;
  int err;
  int ni, nj, ibegin, jbegin;
  char filename[128] = "var2d_compression.nc";

  //--------------------------------------
  //  NetCDF metadata definitions
  //--------------------------------------

  /* Create a netcdf file */
  if (err=nc_create(filename, NC_NETCDF4, &ncid))
    ERR(err);

  /* Define two spatial dimensions */
  if (err=nc_def_dim(ncid, "lat", NI_GLO, &lat_dimid))
    ERR(err);
  if (err=nc_def_dim(ncid, "lon", NJ_GLO, &lon_dimid))
    ERR(err);

  /* Define a 2D temporal variable and add an attribute */
  var_dimids[0] = lat_dimid;
  var_dimids[1] = lon_dimid;
  if (err=nc_def_var(ncid, "var_2d", NC_INT, NDIMS, var_dimids, &varid))
    ERR(err);
  if (err=nc_put_att(ncid, varid, "standart_name", NC_CHAR, strlen("dummy_temporal_variable"), "dummy_temporal_variable"))
    ERR(err);

  /* Define compression level*/
  shuffle = 1;
  deflate = 1;
  deflate_level = 9;
  if (err=nc_def_var_deflate(ncid, varid, shuffle, deflate, deflate_level))
    ERR(err);

  /* Close metadata definition */
  if (err=nc_enddef(ncid))
    ERR(err);

  ni=NI_GLO; nj=NJ_GLO;
  /* Fill in data */
  int *data = (int *)malloc(sizeof(int)*ni*nj);
  fill_data(data, ni, nj);

  /* Write data */
  if (err=nc_put_var(ncid, varid, data)) ERR(err);

  /* Close the file */
  if (err=nc_close(ncid)) ERR(err);

  free(data);

  return 0;
}     

