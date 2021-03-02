program example_par_read
  use netcdf
  implicit none
  include 'mpif.h'

  integer :: mpirank
  integer :: mpisize
  integer :: ierr

  character (len = *), parameter :: FILE_NAME = "par_write.nc"
  integer,parameter :: NDIMS=2
  integer :: ni_glo, nj_glo
  integer,allocatable :: field(:,:)
  integer :: ni,ibegin,nj,jbegin,i,j
  integer :: ncid,nidimid,njdimid,varid

  ! MPI initialization
  call MPI_Init(ierr)
  call MPI_Comm_rank(MPI_COMM_WORLD,mpirank,ierr)
  call MPI_Comm_size(MPI_COMM_WORLD,mpisize,ierr)

  ! Open the file
  call check( nf90_open(FILE_NAME, NF90_NOWRITE, ncid,&
                    comm=MPI_COMM_WORLD, info=MPI_INFO_NULL))

  ! Get IDs of the two spatial dimensions given their names
  call check( nf90_inq_dimid(ncid, "ni", nidimid) )
  call check( nf90_inq_dimid(ncid, "nj", njdimid) )

  ! Get two spatial dimensions
  call check( nf90_inquire_dimension(ncid, nidimid, len = ni_glo) )
  call check( nf90_inquire_dimension(ncid, njdimid, len = nj_glo) )

  ! Get ID of the spacial variable given its name
  call check( nf90_inq_varid(ncid, "toto", varid) )

  ! Decompose global domain.
  ! Returned values: ni,nj,ibegin,jbegin.
  call decompose_domain(mpisize,mpirank,ni_glo,nj_glo,ni,ibegin,nj,jbegin)

  ! Allocate local data array
  allocate(field(ni,nj))

  ! Read data
  call check( nf90_get_var(ncid, varid, field, start=(/ibegin,jbegin/), count=(/ni,nj/)) )

  ! Close the file
  call check( nf90_close(ncid) )

  ! Verify the read data
  do j = 1, nj
    do i = 1, ni
      if (field(i, j) /= (j-1+jbegin-1)*ni + (i-1+ibegin-1)) then
        print *, "Error: something went wrong; data read from the file do not correspond to the expected."
        stop 2 
      endif
    end do
  end do
  
  print *, "Parallel read is done successfully."

  deallocate(field)

  call MPI_FINALIZE(ierr)

  !--------------------------------------
  ! Functions and subroutines
  !--------------------------------------

  contains 

  subroutine check(status)
    integer, intent ( in) :: status

    if(status /= nf90_noerr) then
      print *, trim(nf90_strerror(status))
      stop 2
    end if
  end subroutine check

  !--------------------------------------
  ! This subroutine decomposes global domain
  !--------------------------------------
  subroutine decompose_domain(mpisize,rank,ni_glo,nj_glo,ni,ibegin,nj,jbegin)
    integer,intent(in) :: mpisize,rank,ni_glo,nj_glo
    integer,intent(out) :: ni,ibegin,nj,jbegin
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

end program example_par_read

