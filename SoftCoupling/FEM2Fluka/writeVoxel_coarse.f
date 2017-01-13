      PROGRAM WRITEVOXEL

      INCLUDE '(DBLPRC)'
      INCLUDE '(DIMPAR)'
      INCLUDE '(IOUNIT)'
*  COLUMNS: FROM LEFT TO RIGHT
*  ROWS: FROM BACK TO FRONT
*  SLICES: FROM TOP TO BOTTOM
* Fine Mesh
*      PARAMETER ( DX = 0.2D-01 )
*      PARAMETER ( DY = 0.2D-01 )
*      PARAMETER ( DZ = 2.5D-01 )
*      PARAMETER ( NX = 155 )
*      PARAMETER ( NY = 225 )
*      PARAMETER ( NZ = 404 )
* Intermediate Mesh
*      PARAMETER ( DX = 0.5D-01 )
*      PARAMETER ( DY = 0.5D-01 )
*      PARAMETER ( DZ = 2.5D-01 )
*      PARAMETER ( NX = 62 )
*      PARAMETER ( NY = 90 ) 
*      PARAMETER ( NZ = 404 )
* Coarse Mesh
      PARAMETER ( DX = 1.0D-01 )
      PARAMETER ( DY = 1.0D-01 )
      PARAMETER ( DZ = 5.0D-01 )
      PARAMETER ( NX = 31 )
      PARAMETER ( NY = 45 )
      PARAMETER ( NZ = 202 )

      DIMENSION CT(NX,NY,NZ)
      INTEGER*2 CT
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
      OPEN(UNIT=30,FILE='target_block.ascii',STATUS='OLD')
      READ(30,*) CT
      NO=0
      MO=0
      DO IZ=1,NZ
         DO IY=1,NY
            DO IX=1,NX
               IF (CT(IX,IY,IZ) .GT. 0) THEN
                  IO = CT(IX,IY,IZ)
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
      TITLE = 'Tungsten Block'
      WRITE(31) TITLE
      WRITE(31) NX,NY,NZ,NO,MO
      WRITE(31) DX,DY,DZ
      WRITE(31) VXL
      WRITE(31) (KREG(IC),IC=1,MO)
      STOP
      END
