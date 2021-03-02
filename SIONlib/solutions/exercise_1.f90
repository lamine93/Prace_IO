program exercise_1
    use mpi
    use sion_f90
    use sion_f90_mpi

    implicit none
    integer(kind=8) :: chunksize
    integer :: sid,globalrank,ierr
    integer :: lComm = MPI_COMM_NULL
    integer(kind=4) :: fsblksize
    integer :: nfiles
    character(len=255) :: newfname

    ! MPI inititalization
    call MPI_Init(ierr)
    call MPI_Comm_rank(MPI_COMM_WORLD,globalrank,ierr)

    ! create a new file
    fsblksize=-1
    chunksize=1000*kind(nfiles)
    nfiles=1
    call fsion_paropen_mpi("parfile.sion","w",nfiles, MPI_COMM_WORLD, &
                           lComm,chunksize,fsblksize,globalrank,&
                           newfname,sid)
    ! close file 
    call fsion_parclose_mpi(sid,ierr)

    ! MPI finalization
    call MPI_Finalize(ierr)
end
