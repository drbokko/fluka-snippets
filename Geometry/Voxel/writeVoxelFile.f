      PROGRAM WRITEVOXEL

      INCLUDE '(DBLPRC)'
      INCLUDE '(DIMPAR)'
      INCLUDE '(IOUNIT)'
*  COLUMNS: FROM LEFT TO RIGHT
*  ROWS: FROM BACK TO FRONT
*  SLICES: FROM TOP TO BOTTOM
* Voxel Mesh Parameters
* - bin size [cm] along X => DX
* - bin size [cm] along Y => DY
* - bin size [cm] along Z => DZ
      PARAMETER ( DX = 0.1D-01 )
      PARAMETER ( DY = 0.1D-01 )
      PARAMETER ( DZ = 0.1D-01 )
* Voxel Mesh Parameters
* - number of bin along X => NX
* - number of bin along Y => NY
* - number of bin along Z => NZ
      PARAMETER ( NX = 100 )
      PARAMETER ( NY = 100 )
      PARAMETER ( NZ = 100 )

      DIMENSION MYARRAY(NX,NY,NZ)
      INTEGER*2 MYARRAY
      DIMENSION VXL(NX,NY,NZ)
      INTEGER*2 VXL
      CHARACTER TITLE*80
      DIMENSION IREG(1000), KREG(1000)
      INTEGER*2 IREG, KREG
*
      CALL CMSPPR
      DO IC = 1, 1000
         KREG(IC) = 0
      END DO
      OPEN(UNIT=30,FILE='ascii_file_name.extension',STATUS='OLD')
      READ(30,*) MYARRAY
      NO=0
      MO=0
      DO IZ=1,NZ
         DO IY=1,NY
            DO IX=1,NX
               IF (MYARRAY(IX,IY,IZ) .GT. 0) THEN
                  IO = MYARRAY(IX,IY,IZ)
                  VXL(IX,IY,IZ) = IO
                  MO = MAX (MO,IO)
                  DO IR=1,NO
                     IF (IREG(IR) .EQ. IO) GO TO 1000
                  END DO
                  NO=NO+1
                  IREG(NO)=IO
                  KREG(IO)=NO
                  WRITE(*,'(A,2I10)')' New number, old number: ', NO, IO
 1000             CONTINUE
               END IF
            END DO
         END DO
      END DO
*     NO = number of different organs
*     MO = max. organ number before compacting
      WRITE(*,*) ' NO,MO',NO,MO
      OPEN(UNIT=31,FILE='target.vxl',STATUS='UNKNOWN',
     c     FORM='UNFORMATTED')
      TITLE = 'Dummy Voxel'
      WRITE(31) TITLE
      WRITE(31) NX,NY,NZ,NO,MO
      WRITE(31) DX,DY,DZ
      WRITE(31) VXL
      WRITE(31) (KREG(IC),IC=1,MO)
      STOP
      END
