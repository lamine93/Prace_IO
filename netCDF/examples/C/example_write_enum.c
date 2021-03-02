#include <netcdf.h>
#include <netcdf_par.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); return 2;}

#define N 10

enum clouds {Clear, Cumulonimbus, Stratus, Stratocumulus, Cumulus, Altostratus, Nimbostratus, Altocumulus}; 

int main (void)
{
  //int mpirank, mpisize;
  int ncid, dimid, varid, typeid;
  int err;
  unsigned int enumval;
  char filename[128] = "write_enum.nc";

  // Create a netcdf file
  if (err=nc_create(filename, NC_NETCDF4, &ncid)) ERR(err);

  // Create an enum type
  if (err=nc_def_enum(ncid, NC_UINT, "cloud_types", &typeid)) ERR(err);

  // Example of this numeration is taken from https://www.unidata.ucar.edu/software/netcdf/workshops/2010/groups-types/popup_groups-types_EnumCDL_2.html
  // Insert named fields into a compound type.
  {
    enumval=0;
    if (err=nc_insert_enum(ncid, typeid, "Clear", &enumval)) ERR(err);
    enumval=1;
    if (err=nc_insert_enum(ncid, typeid, "Cumulonimbus", &enumval)) ERR(err);
    enumval=2;
    if (err=nc_insert_enum(ncid, typeid, "Stratus", &enumval)) ERR(err);
    enumval=3;
    if (err=nc_insert_enum(ncid, typeid, "Stratocumulus", &enumval)) ERR(err);
    enumval=4;
    if (err=nc_insert_enum(ncid, typeid, "Cumulus", &enumval)) ERR(err);
    enumval=5;
    if (err=nc_insert_enum(ncid, typeid, "Altostratus", &enumval)) ERR(err);
    enumval=6;
    if (err=nc_insert_enum(ncid, typeid, "Nimbostratus", &enumval)) ERR(err);
    enumval=7;
    if (err=nc_insert_enum(ncid, typeid, "Altocumulus", &enumval)) ERR(err);    

  }
  // Define a dimension
  if (err=nc_def_dim(ncid, "n", N, &dimid)) ERR(err);

  // Define a variable 
  if (err=nc_def_var(ncid, "clouds", typeid, 1, &dimid, &varid)) ERR(err);

  // Close metadata definition
  nc_enddef(ncid);
  
  // Initialize data
  unsigned int clouds [N] = {Cumulonimbus, Stratus, Cumulus, Altocumulus, Clear} ;

  // Write data
  if (err=nc_put_var(ncid, varid, clouds)) ERR(err);
  
  // Close the file
  if (err=nc_close(ncid)) ERR(err);

  return 0;
}     

