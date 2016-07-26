 program toq_profiles_driver
   use toq_profiles_mod
   implicit none
   real :: wid_E1, p_E1
   real :: nped13, tpedEV
   real :: ncore13, tcoreEV
   real :: nedge13, tedgeEV

   integer :: npsi, i
   real, dimension (101) :: psin, p_0, n13
   real :: nexpin=1.1, nexpout=1.1, texpin=1.2, texpout=1.4

   npsi=101

   wid_E1 = 0.04
   p_E1   = 0.01

   tcoreEV = 2000.
   nped13 = 3.6

   tpedEV = 300
   ncore13 = nped13 * 1.5
   nedge13 = nped13 * 0.25
   tedgeEV = 75.

   do i=0,npsi-1
      psin(i+1)=i/(npsi-1.)
   enddo

   call toq_profiles( &
        psin, npsi, wid_E1, &
        nped13, tpedEV, &
        ncore13, tcoreEV, &
        nedge13, tedgeEV, &
        nexpin, nexpout, texpin, texpout, &
        p_0, n13)

   do i=1,npsi
      write(*,*)psin(i),p_0(i),n13(i),p_0(i)/n13(i)/1.6022/20.
   enddo

 end program toq_profiles_driver
