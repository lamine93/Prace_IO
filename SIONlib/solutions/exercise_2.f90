program exercise_2
    use mpi
    use sion_f90
    use sion_f90_mpi

    implicit none
    integer(kind=8) :: chunksize, bwrote
    integer :: sid,globalrank,ierr
    integer :: lComm = MPI_COMM_NULL
    integer(kind=4) :: fsblksize
    integer :: nfiles
    character(len=255) :: newfname

    integer, parameter :: ARRAY_SIZE = 1000
    integer, dimension(:), allocatable :: data

    ! MPI inititalization
    call MPI_Init(ierr)
    call MPI_Comm_rank(MPI_COMM_WORLD,globalrank,ierr)

    ! create data
    allocate(data(ARRAY_SIZE))
    data = globalrank

    ! create a new file
    fsblksize=-1
    chunksize=ARRAY_SIZE*kind(data(1))
    nfiles=1
    call fsion_paropen_mpi("parfile.sion","w",nfiles, MPI_COMM_WORLD, &
                           lComm,chunksize,fsblksize,globalrank,&
                           newfname,sid)

    call fsion_write(data(1),int(kind(data(1)),8),int(ARRAY_SIZE,8),sid,bwrote)

    ! close file 
    call fsion_parclose_mpi(sid,ierr)

    deallocate(data)

    ! MPI finalization
    call MPI_Finalize(ierr)
end
