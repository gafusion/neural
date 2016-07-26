module toq_profiles_mod
   implicit none
   private
   public :: toq_profiles
 contains
   subroutine toq_profiles( &
        psin, npsi, widthp, &
        nped13, tpedEV, &
        ncore13, tcoreEV, &
        nedge13, tedgeEV, &
        nexpin, nexpout, texpin, texpout, &
        p_0, n13)
     implicit none
     real, dimension (:) :: psin, p_0, n13
     real :: widthp, nped13, tpedEV, ncore13, tcoreEV, nedge13, tedgeEV, nexpin, nexpout, texpin, texpout
     real :: xphalf, pconst, a_n, a_t, xped, ncoretanh, tcoretanh, xpsi, nval, nvalp, tval, tvalp, xtoped
     integer :: npsi, i

     !from model 127 of psetup.f in TOQ code

     xphalf=1.-widthp
     pconst=1.-tanh((1.0-xphalf)/widthp)
     a_n=2.*(nped13-nedge13)/(1.+tanh(1.)-pconst)
     a_t=2.*(tpedEV-tedgeEV)/(1.+tanh(1.)-pconst)
     xped=xphalf-widthp
     ncoretanh=0.5*a_n*(1.-tanh(-xphalf/widthp)-pconst)+nedge13
     tcoretanh=0.5*a_t*(1.-tanh(-xphalf/widthp)-pconst)+tedgeEV
     do i=1,npsi
         xpsi=psin(i)
         write(*,*)xpsi
         nval=0.5*a_n*(1.-tanh((xpsi-xphalf)/widthp)-pconst)+nedge13
         nvalp=-0.5*a_n/widthp*(1./cosh((xpsi-xphalf)/widthp)**2)
         tval=0.5*a_t*(1.-tanh((xpsi-xphalf)/widthp)-pconst)+tedgeEV
         tvalp=-0.5*a_t/widthp*(1./cosh((xpsi-xphalf)/widthp)**2)
         if (ncore13.gt.0. .and. xpsi.lt.xped) then
            xtoped=xpsi/xped
            nval=nval+(ncore13-ncoretanh)*(1.-xtoped**nexpin)**nexpout
            nvalp=nvalp-(ncore13-ncoretanh)*nexpin*nexpout*xtoped**(nexpin-1.)*(1.-xtoped**nexpin)**(nexpout-1.)
         endif
         if (tcoreEV.gt.0. .and. xpsi.lt.xped) then
            xtoped=xpsi/xped
            tval=tval+(tcoreEV-tcoretanh)*(1.-xtoped**texpin)**texpout
            tvalp=tvalp-(tcoreEV-tcoretanh)*texpin*texpout*xtoped**(texpin-1.)*(1.-xtoped**texpin)**(texpout-1.)
         endif
         p_0(i)=2.*(nval*1.e13)*(tval*1.6022e-12)
         n13(i)=nval
     enddo
   end subroutine toq_profiles
 end module toq_profiles_mod
