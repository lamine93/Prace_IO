#include <netcdf.h>
#include <netcdf_par.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); return 2;}

#define N 5

// Example is adapted from https://www.unidata.ucar.edu/software/netcdf/workshops/2010/groups-types/popup_groups-types_OpaqueCDL_2.html
typedef unsigned char opaque_t[11];

int main (void)
{
  //int mpirank, mpisize;
  int ncid, dimid, varid, typeid;
  int err;
  unsigned int enumval;
  char filename[128] = "write_opaque.nc";

  // Create a netcdf file
  if (err=nc_create(filename, NC_NETCDF4, &ncid)) ERR(err);

  // Create a vlen type
  if (err=nc_def_opaque(ncid, 11, "opaque_t", &typeid)) ERR(err);

  // Define a dimension
  if (err=nc_def_dim(ncid, "n", N, &dimid)) ERR(err);

  // Define a variable 
  if (err=nc_def_var(ncid, "sensor_data", typeid, 1, &dimid, &varid)) ERR(err);

  // Close metadata definition
  nc_enddef(ncid);
  
  // Initialize data
  opaque_t opaque_data[5] = {"\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b", "\xaa\xbb\xcc\xdd\xee\xff\xee\xdd\xcc\xbb\xaa", "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff", "\xca\xfe\xba\xbe\xca\xfe\xba\xbe\xca\xfe\xba", "\xcf\x0d\xef\xac\xed\x0c\xaf\xe0\xfa\xca\xde"} ;

  // Write data
  if (err=nc_put_var(ncid, varid, opaque_data)) ERR(err);
  
  // Close the file
  if (err=nc_close(ncid)) ERR(err);

  return 0;
}     

