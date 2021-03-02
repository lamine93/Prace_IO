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
  call check(...)
  
  ! Define three dimensions lat (size NI_GLO), lon (size NJ_GLO) and time. 
  call check(...)
  call check(...)
  call check(...) 

  ! Initialize global arrays.
  call init_global(lon_glo,lat_glo,field_glo)

  ! Decompose global domain. Returned values: ni,nj,ibegin,jbegin.
  call decompose_domain(mpisize,rank,ni,ibegin,nj,jbegin)

  ! Allocate and fill in local arrays. 
  allocate(lat(ni),lon(nj),field(ni,nj))
  lat(:)=lat_glo(ibegin:ibegin-1+ni)
  lon(:)=lon_glo(jbegin:jbegin-1+nj)
  field(:,:) = field_glo(ibegin:ibegin-1+ni,jbegin:jbegin-1+nj)

  ! Define variable "lat" containing latitude.
  call check(...)
  ! Add attribute "standart_name" holding the value "latitude"
  call check(...)
  ! Add attribute "units" holding the value "degrees_north"
  call check(...)

  ! Define variable "lon" containing longitude.
  call check(...)
  ! Add attribute "standart_name" holding the value "longitude"
  call check(...)
  ! Add attribute "units" holding the value "degrees_east"
  call check(...)

  ! Define variable "var_2d_temp".
  call check(...)
  ! Add attribute "standart_name" holding the value "dummy_temporal_variable"
  call check(...)

  ! End define mode. This tells netCDF we are done defining metadata.
  call check(...)

  ! Write latitude
  call check(...)
 ! Write longitude
  call check(...)

  ! Set mode to collective for the temporal variable.
  call check(...)) 

  ! Write the temporal variable.
  do t=1,NTIME
    call check(...)
  enddo

  ! Close file.
  call check(...)

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
