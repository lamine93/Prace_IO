program example_par_write
  use netcdf
  implicit none
  include 'mpif.h'

  integer :: mpirank
  integer :: mpisize
  integer :: ierr

  character (len = *), parameter :: FILE_NAME = "par_write.nc"
  integer,parameter :: NDIMS=2
  integer,parameter :: NI_GLO=5
  integer,parameter :: NJ_GLO=10

  integer :: field_glo(NI_GLO,NJ_GLO)
  integer,allocatable :: field(:,:)
  integer :: ni,ibegin,nj,jbegin
  integer :: ncid,idimid,jdimid,varid

  !--------------------------------------
  ! MPI initialization
  !--------------------------------------

  call MPI_Init(ierr)
  call MPI_Comm_rank(MPI_COMM_WORLD,mpirank,ierr)
  call MPI_Comm_size(MPI_COMM_WORLD,mpisize,ierr)

  !--------------------------------------
  !  NetCDF metadata definitions
  !--------------------------------------

  ! Create a file
  ierr = nf90_create(FILE_NAME, IOR(NF90_NETCDF4,NF90_MPIIO), ncid,&
                    comm=MPI_COMM_WORLD, info=MPI_INFO_NULL)

  ! Define two spatial dimensions
  ierr = nf90_def_dim(ncid, "ni", NI_GLO, idimid)
  ierr = nf90_def_dim(ncid, "nj", NJ_GLO, jdimid)

  ! Define a 2D variable and add an attribute
  ierr = nf90_def_var(ncid, "toto", NF90_INT, (/idimid, jdimid/), varid)
  ierr = nf90_put_att(ncid, varid, "standart_name", "Dummy variable")

  ! Close metadata definition
  ierr = nf90_enddef(ncid)

  !--------------------------------------
  !  Global and local data initialization
  !--------------------------------------

  ! Initialize global data array
  !
  call init_global(field_glo)

  ! Decompose global domain.
  ! Returned values: ni,nj,ibegin,jbegin.
  !
  call decompose_domain(mpisize,mpirank,ni,ibegin,nj,jbegin)

  ! Allocate and fill in local array
  !
  allocate(field(ni,nj))
  field(:,:) = field_glo(ibegin:ibegin-1+ni,jbegin:jbegin-1+nj)
  
  !--------------------------------------
  ! Write data and close the file  
  !--------------------------------------

  ! Write data
  ierr = nf90_put_var(ncid, varid, field, start=(/ibegin,jbegin/), count=(/ni,nj/))
  ! Close the file
  ierr = nf90_close(ncid)
 
  deallocate(field)

  call MPI_FINALIZE(ierr)

  !--------------------------------------
  ! Functions and subroutines
  !--------------------------------------

  contains 

  !--------------------------------------
  ! This subroutine fills in global data array
  !--------------------------------------

  subroutine init_global(field_glo)
  integer,intent(inout) :: field_glo(NI_GLO,NJ_GLO)
  integer i,j

  do j=1,NJ_GLO
    do i=1,NI_GLO
      field_glo(i,j)=(i-1)+(j-1)*NI_GLO
    enddo
  enddo
  end subroutine init_global

  !--------------------------------------
  ! This subroutine decomposes global domain
  !--------------------------------------
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

end program example_par_write

