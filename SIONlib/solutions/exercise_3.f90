program exercise_3
    use sion_f90

    implicit none
    integer :: task, ntasks, nfiles
    integer :: i, eof
    integer(kind=8) :: chunksizes
    integer :: globalranks
    integer ::sid, ierr
    integer(kind=4) :: fsblksize
    integer,dimension(:),allocatable :: data
    character(len=2) :: filemode = "r"
    integer(kind=8) :: datasize_in_block
    integer(kind=8) :: bytes_read

    ! open file
    call fsion_open("parfile.sion",filemode, ntasks, nfiles, &
                    chunksizes,fsblksize,globalranks,&
                    sid)

    do task = 0, ntasks - 1
        ! switch to task
        call fsion_seek(sid,task,SION_CURRENT_BLK,int(SION_CURRENT_POS,8),ierr)
        write(*,*) "Task:", task
        ! read all chunks
        call fsion_feof(sid, eof)
        do while (eof == 0)
            ! check size of current data in chunk
            datasize_in_block = fsion_bytes_avail_in_block(sid)
            allocate(data(datasize_in_block/kind(i)))
            call fsion_read(data(1),1_8,datasize_in_block,sid,bytes_read)
            write(*,*) data
            deallocate(data)
            call fsion_feof(sid, eof)
        end do
    end do

    ! close file 
    call fsion_close(sid,ierr)
end
