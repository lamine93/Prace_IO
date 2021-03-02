#include "mpi.h"
#include <netcdf.h>
#include <netcdf_par.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N_GLO 10 

#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); return 2;}

struct point3D
{
  float x;
  float y;
  float z;
} point3D;

// Funciton fills in local domain data
void init_local_data(struct point3D* data, size_t n, size_t begin)
{
  for(size_t i=0; i < n; i++) {
      data[i].x = 0.0;
      data[i].y = 1.0;
      data[i].z = 2.0;
  }
}

int main (void)
{
  int ncid, dimid, varid, typeid;
  int err;
  size_t n, begin;
  char filename[128] = "write_struct.nc";

  // Create a netcdf file
  if (err=nc_create(filename, NC_NETCDF4, &ncid)) ERR(err);

  // Create a compound type
  if (err=nc_def_compound(ncid, sizeof(point3D), "point_3d", &typeid)) ERR(err);

  // Insert named fields into a compound type.
  {
    if (err=nc_insert_compound(ncid, typeid, "x", NC_COMPOUND_OFFSET(struct point3D,x), NC_FLOAT)) ERR(err);
    if (err=nc_insert_compound(ncid, typeid, "y", NC_COMPOUND_OFFSET(struct point3D,y), NC_FLOAT)) ERR(err);
    if (err=nc_insert_compound(ncid, typeid, "z", NC_COMPOUND_OFFSET(struct point3D,z), NC_FLOAT)) ERR(err);
  }
  // Define a dimension
  if (err=nc_def_dim(ncid, "n", N_GLO, &dimid)) ERR(err);

  // Define a variable 
  if (err=nc_def_var(ncid, "toto", typeid, 1, &dimid, &varid)) ERR(err);

  // Close metadata definition
  nc_enddef(ncid);

  // Initialize local data
  n = N_GLO; begin = 0;
  struct point3D *data = (struct point3D *)malloc(sizeof(struct point3D)*n);
  init_local_data(data, n, begin);

  //--------------------------------------
  // Write data and close the file  
  //--------------------------------------

  // Write data
  if (err=nc_put_var(ncid, varid, data)) ERR(err);
  
  // Close the file
  if (err=nc_close(ncid)) ERR(err);

  free(data);

  return 0;
}     

