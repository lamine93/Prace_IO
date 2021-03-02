
! ###################################################################
! #                       HDF5 hands on                             #
! #  Created : April 2011                                           # 
! #                                                                 #
! #  Author:                                                        #
! #     Matthieu Haefele                                            #
! #     matthieu.haefele@ipp.mpg.de                                 #
! #     High Level Support Team (HLST)                              #
! #     Max-Planck Institut fuer Plasmaphysik                       #
! #                                                                 #
! ###################################################################

PROGRAM HDF5_EX
     USE HDF5

     IMPLICIT NONE

     CHARACTER(LEN=10), PARAMETER :: filename = "example.h5" ! File name
     CHARACTER(LEN=8), PARAMETER :: dsetname = "IntArray"     ! Dataset name
     INTEGER, PARAMETER     ::   rank = 3                        
     INTEGER, PARAMETER     ::   NX = 10                        
     INTEGER, PARAMETER     ::   NY = 6                        
     INTEGER, PARAMETER     ::   NZ = 4
     INTEGER(HID_T) :: file, dataset, dataspace
     INTEGER(HSIZE_T), DIMENSION(3) :: dims
     INTEGER     ::   status
     INTEGER, DIMENSION(NX,NY,NZ) :: data
     INTEGER     :: i,j,k
     
     DO k=1,NZ
        DO j=1,NY
           DO i=1,NX
              data(i,j,k) = (i-1) + (j-1)*NX + (k-1)*NX*NY;
           ENDDO
        ENDDO
     ENDDO
        
     !
     ! Initialize FORTRAN predefined datatypes.
     !
     CALL h5open_f(status)

     !
     ! Create a new file using default properties.
     ! 
     CALL h5fcreate_f(filename, H5F_ACC_TRUNC_F, file, status)
     
     
     ! 
     ! Create the dataspace.
     !
     dims(1) = NX
     dims(2) = NY
     dims(3) = NZ
     CALL h5screate_simple_f(rank, dims, dataspace, status)
     
     
     !
     ! Create the dataset default properties.
     !
     CALL h5dcreate_f(file, dsetname, H5T_NATIVE_INTEGER, dataspace, &
                      dataset, status)
      
     !
     ! Write the data actually
     !
     CALL h5dwrite_f(dataset, H5T_NATIVE_INTEGER, data, dims, status)


     !   
     ! End access to the dataset and release resources used by it.
     ! 
     CALL h5dclose_f(dataset, status)

     !
     ! Terminate access to the data space.
     !
     CALL h5sclose_f(dataspace, status)

     ! 
     ! Close the file.
     !
     CALL h5fclose_f(file, status)

     !
     ! Close FORTRAN predefined datatypes.
     !
     CALL h5close_f(status)

     END PROGRAM HDF5_EX
     
