###############################################################################
#####               This makefile generates ELWANNO.DLL                   #####
###############################################################################
!include $(WGDEV)\src\gcrules.mak

VIRDIR = .\GCVIRDAT

DLLLIB =            \
     $(DLLLIB)      \
     wgserver.lib   \
     galgsbl.lib

OBJFILES =          \
     elwanno.obj     

VIRFILES =          \
     GCVIRDAT\elwanno.vir  \
     GCVIRDAT\elwannos.vir \

elwanno.dll:        \
     $(GCBUILD)     \
     elwanno.h      \
     $(OBJFILES)    \
     $(DLLLIB)      \
     $(DEFILE)      \
     $(VIRFILES) 
     $(LINK) $(LIBCMD) @&&|
$(PRODLL) $(OBJFILES)
$(RUNDIR)\$&
$(RUNDIR)\$&
$(DLLLIB) $(LINKDLL)
$(DEFILE)
|
