! This program reads data in parallel from a NetCDF file. 
! It is inspired by examples provided by NetCDF.
! Olga Abramkina (olga.abramkina@idris.fr), 2021

program hands_on_par_read 
  use netcdf
  implicit none
  include 'mpif.h'

  integer :: rank
  integer :: mpisize
  integer :: ierr
  
  character (len = *), parameter :: FILE_NAME = "par_write.nc"
  integer,parameter :: NDIMS=2
  integer,parameter :: NTIME=2

  integer :: ni_glo, nj_glo
  real, allocatable :: lon_glo(:),lat_glo(:)
  integer, allocatable :: field(:,:) 
  integer :: ni,ibegin,nj,jbegin
  integer :: i, j, t
  integer :: ncid, lat_dimid, lon_dimid, timedimid, varid, lonid, latid
  integer :: starts(NDIMS), counts(NDIMS)

  call MPI_Init(ierr)
  call MPI_Comm_rank(MPI_COMM_WORLD,rank,ierr)
  call MPI_Comm_size(MPI_COMM_WORLD,mpisize,ierr)

  ! Create the netcdf file.
  call check(nf90_open(FILE_NAME,NF90_NOWRITE,ncid,&
                         comm=MPI_COMM_WORLD,info=MPI_INFO_NULL))
 
  ! Get IDs of the two spacial dimensions given their names
  call check( nf90_inq_dimid(ncid, "lat", lat_dimid) )
  call check( nf90_inq_dimid(ncid, "lon", lon_dimid) )

  ! Get two spatial dimensions given their ids
  call check( nf90_inquire_dimension(ncid, lat_dimid, len = ni_glo) )
  call check( nf90_inquire_dimension(ncid, lon_dimid, len = nj_glo) ) 

  ! Assume that each MPI process reads the entirety of lat and lon
  allocate(lat_glo(ni_glo),lon_glo(nj_glo))

  ! Decompose global domain.
  ! Returned values: ni,nj,ibegin,jbegin.
  call decompose_domain(mpisize,rank,ni_glo,nj_glo,ni,ibegin,nj,jbegin)

  ! Contrary to lon, lat, assume that the varible is read in parallel
  ! Allocate local data array 
  allocate(field(ni,nj))

  ! Get IDs of two varialbes, latitude and longitude
  call check( nf90_inq_varid(ncid, "lat", latid) )
  call check( nf90_inq_varid(ncid, "lon", lonid) )

  ! Read latitude and longitude
  call check( nf90_get_var(ncid, latid, lat_glo) )
  call check( nf90_get_var(ncid, lonid, lon_glo) )

  ! Get ID of the temporal variable
  call check( nf90_inq_varid(ncid, "var_2d_temp", varid) )

  ! Set mode to collective for the temporal variable
  ! Not needed for reading
  !call check(nf90_var_par_access(ncid, varid, nf90_collective)) 

  ! Read the temporal variable
  do t=1,NTIME
    call check(nf90_get_var(ncid, varid, field, start=(/ibegin,jbegin,t/), count=(/ni,nj,1/)))

    ! Verify the read data
    do j = 1, nj
      do i = 1, ni
        if (field(i, j) /= (j-1+jbegin-1)*ni + (i-1+ibegin-1)) then
          print *, "Error: something went wrong; data read from the file do not correspond to the expected."
          stop 2
        endif
      end do
    end do

  enddo

  print *, "Parallel read is done successfully."

  ! Close file
  call check(nf90_close(ncid))

  deallocate(lat_glo, lon_glo, field)
  
  call MPI_FINALIZE(ierr)

  contains
  
  ! This subroutine part of the netCDF package.
  ! It handles errors by printing an error message
  ! and exiting with a non-zero status.
  subroutine check(status)
    integer,intent(in) :: status
    if(status /= nf90_noerr) then 
  !    print *, trim(nf90_strerror(status))
   print *, nf90_strerror(status)
      stop "Stopped"
    end if
  end subroutine check

  ! This subroutine decomposes global domain.
  subroutine decompose_domain(mpisize,rank,ni_glo,nj_glo,ni,ibegin,nj,jbegin)
    integer,intent(in) :: mpisize, rank, ni_glo, nj_glo
    integer,intent(out) :: ni, ibegin, nj, jbegin
    integer :: n
    ni=ni_glo ; ibegin=1
    jbegin=0
    do n=0,mpisize-1
      nj=nj_glo/mpisize
      if (n<MOD(nj_glo,mpisize)) nj=nj+1
      if (n==rank) exit
      jbegin=jbegin+nj
    enddo
    jbegin=jbegin+1
  end subroutine decompose_domain

END PROGRAM hands_on_par_read





