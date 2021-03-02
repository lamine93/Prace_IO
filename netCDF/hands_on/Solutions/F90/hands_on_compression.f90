program hands_on_compression 
  use netcdf
  implicit none
  integer :: ierr
  
  character (len = *), parameter :: FILE_NAME = "var2d_compression.nc"
  integer,parameter :: NDIMS=2
  integer,parameter :: NI_GLO=1000
  integer,parameter :: NJ_GLO=1000

  integer :: field_glo(NI_GLO,NJ_GLO)
  integer :: t
  integer :: ncid,lat_dimid,lon_dimid,varid
  integer :: shuffle, deflate, deflate_level
  integer :: dimids(NDIMS), starts(NDIMS), counts(NDIMS)

  ! Create the netcdf file.
  call check(nf90_create(FILE_NAME,NF90_NETCDF4,ncid))
  
  ! Define the dimensions. NetCDF will hand back an ID for each. 
  call check(nf90_def_dim(ncid, "lat", NI_GLO, lat_dimid))
  call check(nf90_def_dim(ncid, "lon", NJ_GLO, lon_dimid))

  ! Initialize global array.
  call init_global(field_glo)

  ! Define variable var and add an attribute.
  call check(nf90_def_var(ncid, "var_2d_temp", nf90_int, &
                      (/ lat_dimid, lon_dimid/), varid))
  call check(nf90_put_att(ncid, varid, "standart_name", "dummy_temporal_variable"))

  ! Define compression level
  shuffle = 1;
  deflate = 1;
  deflate_level = 0;
  call check(nf90_def_var_deflate(ncid, varid, shuffle, deflate, deflate_level))

  ! End define mode. This tells netCDF we are done defining metadata.
  call check(nf90_enddef(ncid)) 

  ! Write the variable.
  call check(nf90_put_var(ncid, varid, field_glo))

  ! Close file.
  call check(nf90_close(ncid))

  contains
  ! This subroutine comes with the netCDF package. 
  ! It handles errors by printing an error message  and exiting with a non-zero status.
  subroutine check(status)
    integer,intent(in) :: status
    if(status /= nf90_noerr) then 
      print *, trim(nf90_strerror(status))
      stop "Stopped"
    end if
  end subroutine check

  ! This subroutine fills in global arrays containing values of the variable, longitude, and latitude.
  subroutine init_global(field_glo)
  integer,intent(inout) :: field_glo(NI_GLO,NJ_GLO)        
  integer i,j
  
  do j=1,NJ_GLO
    do i=1,NI_GLO
      field_glo(i,j)=(i-1)+(j-1)*NI_GLO
    enddo
  enddo
  end subroutine init_global

end program hands_on_compression
