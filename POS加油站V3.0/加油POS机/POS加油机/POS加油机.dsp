# Microsoft Developer Studio Project File - Name="POS���ͻ�" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=POS���ͻ� - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "POS���ͻ�.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "POS���ͻ�.mak" CFG="POS���ͻ� - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "POS���ͻ� - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "POS���ͻ� - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "POS���ͻ� - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "POS���ͻ� - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "POS���ͻ� - Win32 Release"
# Name "POS���ͻ� - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Project\DB.C
# End Source File
# Begin Source File

SOURCE=.\Project\Function.c
# End Source File
# Begin Source File

SOURCE=.\Project\M1card.c
# End Source File
# Begin Source File

SOURCE=.\Project\main.c
# End Source File
# Begin Source File

SOURCE=.\Project\Menu.c
# End Source File
# Begin Source File

SOURCE=.\Project\NormalData.c
# End Source File
# Begin Source File

SOURCE=.\Project\opercard.c
# End Source File
# Begin Source File

SOURCE=.\Project\sam_card.c
# End Source File
# Begin Source File

SOURCE=.\Project\SIM200.c
# End Source File
# Begin Source File

SOURCE=.\Project\tcpip.c
# End Source File
# Begin Source File

SOURCE=.\Project\utils.c
# End Source File
# Begin Source File

SOURCE=.\Project\ydt.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Project\data_type.h
# End Source File
# Begin Source File

SOURCE=.\Project\DB.H
# End Source File
# Begin Source File

SOURCE=.\Project\Function.h
# End Source File
# Begin Source File

SOURCE=.\Project\M1card.h
# End Source File
# Begin Source File

SOURCE=.\Project\main.h
# End Source File
# Begin Source File

SOURCE=.\Project\Menu.h
# End Source File
# Begin Source File

SOURCE=.\Project\opercard.h
# End Source File
# Begin Source File

SOURCE=.\Project\sam_card.h
# End Source File
# Begin Source File

SOURCE=.\Project\SIM200.H
# End Source File
# Begin Source File

SOURCE=.\Project\tcpip.h
# End Source File
# Begin Source File

SOURCE=.\Project\utils.h
# End Source File
# Begin Source File

SOURCE=.\Project\ydt.h
# End Source File
# Begin Source File

SOURCE=.\Project\ymodem.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
