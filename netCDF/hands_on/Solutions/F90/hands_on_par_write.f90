! This program creates a NetCDF file and writes a variable in parallel mode. 
! It is inspired by examples provided by NetCDF.
! Olga Abramkina (olga.abramkina@idris.fr), 2020

program hands_on_par_write 
  use netcdf
  implicit none
  include 'mpif.h'

  integer :: rank
  integer :: mpisize
  integer :: ierr
  
  character (len = *), parameter :: FILE_NAME = "par_write.nc"
  integer,parameter :: NDIMS=2
  integer,parameter :: NI_GLO=5
  integer,parameter :: NJ_GLO=10
  integer,parameter :: NTIME=2

  real:: lon_glo(NJ_GLO),lat_glo(NI_GLO)
  integer :: field_glo(NI_GLO,NJ_GLO)
  real,allocatable :: lon(:),lat(:)
  integer,allocatable :: field(:,:) 
  integer :: ni,ibegin,nj,jbegin
  integer :: t
  integer :: ncid,lat_dimid,lon_dimid,timedimid,varid,lonid,latid

  integer :: dimids(NDIMS), starts(NDIMS), counts(NDIMS)

  call MPI_Init(ierr)
  call MPI_Comm_rank(MPI_COMM_WORLD,rank,ierr)
  call MPI_Comm_size(MPI_COMM_WORLD,mpisize,ierr)

  ! Create the netcdf file.
  call check(nf90_create(FILE_NAME,IOR(NF90_NETCDF4,NF90_MPIIO),ncid,&
                         comm=MPI_COMM_WORLD,info=MPI_INFO_NULL))
  
  ! Define the dimensions. NetCDF will hand back an ID for each. 
  call check(nf90_def_dim(ncid, "lat", NI_GLO, lat_dimid))
  call check(nf90_def_dim(ncid, "lon", NJ_GLO, lon_dimid))
  call check(nf90_def_dim(ncid, "time", NF90_UNLIMITED, timedimid)) 

  ! Initialize global arrays.
  call init_global(lon_glo,lat_glo,field_glo)

  ! Decompose global domain. Returned values: ni,nj,ibegin,jbegin.
  call decompose_domain(mpisize,rank,ni,ibegin,nj,jbegin)

  ! Allocate and fill in local arrays. 
  allocate(lat(ni),lon(nj),field(ni,nj))
  lat(:)=lat_glo(ibegin:ibegin-1+ni)
  lon(:)=lon_glo(jbegin:jbegin-1+nj)
  field(:,:) = field_glo(ibegin:ibegin-1+ni,jbegin:jbegin-1+nj)

  ! Define variable containing latitude and add its attributes.
  call check(nf90_def_var(ncid, "lat", nf90_float, (/lat_dimid/), latid))
  call check(nf90_put_att(ncid, latid, "standart_name", "latitude"))
  call check(nf90_put_att(ncid, latid, "units", "degrees_north"))

  ! Define variable containing longitude and add its attributes.
  call check(nf90_def_var(ncid, "lon", nf90_float, (/lon_dimid/), lonid))
  call check(nf90_put_att(ncid, lonid, "standart_name", "longitude"))
  call check(nf90_put_att(ncid, lonid, "units", "degrees_east"))

  ! Define variable var and add an attribute.
  call check(nf90_def_var(ncid, "var_2d_temp", nf90_int, &
                      (/ lat_dimid, lon_dimid, timedimid /), varid))
  call check(nf90_put_att(ncid, varid, "standart_name", "dummy_temporal_variable"))

  ! End define mode. This tells netCDF we are done defining metadata.
  call check(nf90_enddef(ncid))

  ! Write latitude and longitude
  call check(nf90_put_var(ncid, latid, lat, start=(/ibegin/), count=(/ni/)))
  call check(nf90_put_var(ncid, lonid, lon, start=(/jbegin/), count=(/nj/)))

  ! Set mode to collective for the temporal variable.
  call check(nf90_var_par_access(ncid, varid, nf90_collective)) 

  ! Write the temporal variable.
  do t=1,NTIME
    call check(nf90_put_var(ncid, varid, field, start=(/ibegin,jbegin,t/), count=(/ni,nj,1/)))
  enddo

  ! Close file.
  call check(nf90_close(ncid))

  deallocate(lon, lat, field)
  call MPI_FINALIZE(ierr)

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
  subroutine init_global(lon_glo,lat_glo,field_glo)
  real,intent(inout) :: lon_glo(NJ_GLO),lat_glo(NI_GLO)
  integer,intent(inout) :: field_glo(NI_GLO,NJ_GLO)        
  integer i,j
  
  do i=1,NI_GLO
    lat_glo(i) = -90.+180./NI_GLO*(i-0.5)
  enddo
  do j=1,NJ_GLO
    lon_glo(j) = -180.+360./NJ_GLO*(j-0.5)
    do i=1,NI_GLO
      field_glo(i,j)=(i-1)+(j-1)*NI_GLO
    enddo
  enddo
  end subroutine init_global

  ! This subroutine decomposes global domain.
  subroutine decompose_domain(mpisize,rank,ni,ibegin,nj,jbegin)
    integer,intent(in) :: mpisize,rank
    integer,intent(out) :: ni,ibegin,nj,jbegin
    integer :: n
    ni=NI_GLO ; ibegin=1
    jbegin=0
    do n=0,mpisize-1
      nj=NJ_GLO/mpisize
      if (n<MOD(NJ_GLO,mpisize)) nj=nj+1
      if (n==rank) exit
      jbegin=jbegin+nj
    enddo
    jbegin=jbegin+1
  end subroutine decompose_domain

end program hands_on_par_write
