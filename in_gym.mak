# Microsoft Developer Studio Generated NMAKE File, Format Version 40001
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=in_gym - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to in_gym - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "in_gym - Win32 Release" && "$(CFG)" != "in_gym - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "in_gym.mak" CFG="in_gym - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "in_gym - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "in_gym - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "in_gym - Win32 Debug"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "in_gym - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\in_gym.dll"

CLEAN : 
	-@erase ".\debug\plugins\in_gym.dll"
	-@erase ".\Release\Main.obj"
	-@erase ".\Release\Script1.res"
	-@erase ".\Release\in_gym.lib"
	-@erase ".\Release\in_gym.exp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/in_gym.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Script1.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/in_gym.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /machine:I386 /out:"d:\!Hmmmm!\in_gym(vc6)\debug\plugins\in_gym.dll"
# SUBTRACT LINK32 /profile /nodefaultlib
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo\
 /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)/in_gym.pdb"\
 /machine:I386 /out:"d:\!Hmmmm!\in_gym(vc6)\debug\plugins\in_gym.dll"\
 /implib:"$(OUTDIR)/in_gym.lib" 
LINK32_OBJS= \
	"$(INTDIR)/Main.obj" \
	"$(INTDIR)/Script1.res"

"$(OUTDIR)\in_gym.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "in_gym - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\in_gym.dll"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\debug\plugins\in_gym.dll"
	-@erase ".\Debug\Main.obj"
	-@erase ".\Debug\Script1.res"
	-@erase ".\Debug\in_gym.lib"
	-@erase ".\Debug\in_gym.exp"
	-@erase ".\Debug\in_gym.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /ML /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /ML /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/in_gym.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Script1.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/in_gym.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /incremental:no /debug /machine:I386 /out:"d:\!Hmmmm!\in_gym(vc6)\debug\plugins\in_gym.dll"
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo\
 /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)/in_gym.pdb" /debug\
 /machine:I386 /out:"$(OUTDIR)/plugins\in_gym.dll"\
 /implib:"$(OUTDIR)/in_gym.lib" 
LINK32_OBJS= \
	"$(INTDIR)/Main.obj" \
	"$(INTDIR)/Script1.res"

"$(OUTDIR)\in_gym.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "in_gym - Win32 Release"
# Name "in_gym - Win32 Debug"

!IF  "$(CFG)" == "in_gym - Win32 Release"

!ELSEIF  "$(CFG)" == "in_gym - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Main.c

!IF  "$(CFG)" == "in_gym - Win32 Release"

DEP_CPP_MAIN_=\
	".\In2.h"\
	".\Neonpsg.c"\
	".\Out.h"\
	".\Ay8910.c"\
	".\SNDPlayer.c"\
	".\Ay8910.h"\
	".\Ay8910u.c"\
	
NODEP_CPP_MAIN_=\
	".\PSG"\
	

"$(INTDIR)\Main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "in_gym - Win32 Debug"

DEP_CPP_MAIN_=\
	".\In2.h"\
	".\Neonpsg.c"\
	".\Out.h"\
	".\Ay8910.c"\
	".\SNDPlayer.c"\
	".\Ay8910.h"\
	".\Ay8910u.c"\
	

"$(INTDIR)\Main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Script1.rc

"$(INTDIR)\Script1.res" : $(SOURCE) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
# End Target
# End Project
################################################################################
