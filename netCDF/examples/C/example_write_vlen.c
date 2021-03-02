#include <netcdf.h>
#include <netcdf_par.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); return 2;}

#define N 5

int main (void)
{
  //int mpirank, mpisize;
  int ncid, dimid, varid, typeid;
  int err;
  unsigned int enumval;
  char filename[128] = "write_vlen.nc";

  // Create a netcdf file
  if (err=nc_create(filename, NC_NETCDF4, &ncid)) ERR(err);

  // Create a vlen type
  if (err=nc_def_vlen(ncid, "vlen", NC_FLOAT, &typeid)) ERR(err);

  // Define a dimension
  if (err=nc_def_dim(ncid, "n", N, &dimid)) ERR(err);

  // Define a variable 
  if (err=nc_def_var(ncid, "ragged_array", typeid, 1, &dimid, &varid)) ERR(err);

  // Close metadata definition
  nc_enddef(ncid);
  
  // Initialize data
  float vlen_0[] = {0};
  float vlen_1[] = {1, 2};
  float vlen_2[] = {3, 4, 5};
  float vlen_3[] = {6, 7};
  float vlen_4[] = {8};
  // nc_vlen_t is a struct {size_t, *void}
  nc_vlen_t ragged_array_data[5] = {{1, (void*)vlen_0}, {2, (void*)vlen_1}, {3, (void*)vlen_2}, {2, (void*)vlen_3}, {1, (void*)vlen_4}} ;

  // Write data
  if (err=nc_put_var(ncid, varid, ragged_array_data)) ERR(err);
  
  // Close the file
  if (err=nc_close(ncid)) ERR(err);

  return 0;
}     

