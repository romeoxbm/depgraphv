#####################################################################################
# packaging.cmake
#
# This file is part of dep-graphV - An useful tool to analize header
# dependendencies via graphs.
#
# This software is distributed under the MIT License:
#
# Copyright (c) 2013 Francesco Guastella aka romeoxbm
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#####################################################################################
#CPack settings

set( CPACK_PACKAGE_VENDOR "Guastella Francesco" )
set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "dep-graphV is an useful tool to analize header dependendencies via graphs. This is the runtime package of the tool, built against" )
set( CPACK_PACKAGE_VERSION ${ProjectVersion} )
set( CPACK_PACKAGE_INSTALL_DIRECTORY ${ProjectName} )
set( CPACK_CREATE_DESKTOP_LINKS "${ProjectName}" ${CPACK_CREATE_DESKTOP_LINKS} ) #TODO It doesn't create any desktop shortcut

#Qt version
if( QT_USE_QT5 )
	set( CPACK_PACKAGE_NAME "${ProjectName}-qt5" )
	set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "${CPACK_PACKAGE_DESCRIPTION_SUMMARY} Qt5," )
else()
	set( CPACK_PACKAGE_NAME "${ProjectName}-qt4" )
	set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "${CPACK_PACKAGE_DESCRIPTION_SUMMARY} Qt4," )
endif( QT_USE_QT5 )

#OpenGL Support?
if( QT_USE_OPENGL )
	set( CPACK_PACKAGE_NAME "${CPACK_PACKAGE_NAME}-gl" )
	set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "${CPACK_PACKAGE_DESCRIPTION_SUMMARY} with OpenGL support." )
else()
	set( CPACK_PACKAGE_NAME "${CPACK_PACKAGE_NAME}-nogl" )
	set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "${CPACK_PACKAGE_DESCRIPTION_SUMMARY} without OpenGL support." )
endif( QT_USE_OPENGL )

#Resources
set( CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README" ) #TODO Not tested
set( CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README" ) #TODO Not visualized by NSIS
set( CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING" )

set( CPACK_COMPONENTS_ALL runtime translations sources )
set( CPACK_COMPONENT_RUNTIME_DISPLAY_NAME "dep-graphV" ) #TODO
set( CPACK_COMPONENT_RUNTIME_DESCRIPTION "dep-graphV binaries" ) #TODO
set( CPACK_COMPONENT_RUNTIME_REQUIRED ON )

set( CPACK_COMPONENT_TRANSLATIONS_DISPLAY_NAME "Translations" ) #TODO
set( CPACK_COMPONENT_TRANSLATIONS_DESCRIPTION "Translations description goes here" ) #TODO
#TODO ?
#set( CPACK_COMPONENT_TRANSLATIONS_REQUIRED ON )

#Dependencies
set( CPACK_COMPONENT_TRANSLATIONS_DEPENDS runtime )

if( WIN32 )
	set( CPACK_GENERATOR "NSIS" )
	
	set( CPACK_COMPONENT_SOURCES_DISPLAY_NAME "Sources" ) #TODO
	set( CPACK_COMPONENT_SOURCES_DESCRIPTION "Sources description goes here" ) #TODO
	
	set( CPACK_NSIS_MODIFY_PATH ON )
	set( CPACK_NSIS_MUI_FINISHPAGE_RUN "${ProjectName}.exe" )
	set( CPACK_NSIS_CONTACT "guastella.francesco@gmail.com" )
	set( CPACK_NSIS_HELP_LINK "http:\\\\\\\\sourceforge.net\\\\projects\\\\depgraph\\\\support" )
	set( CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\sourceforge.net\\\\projects\\\\depgraph" )
	
	if( MSVC_IDE )
		if( "${CMAKE_SIZEOF_VOID_P}" EQUAL "8" )
			set( REDIS_ARCH "x64" )
		
			if( MSVC11 )
				set( REDIS_LINK "http://download.microsoft.com/download/1/3/2/13297EFB-ED35-46B5-BD9A-F32CF7CC2CFF/VSU3/vcredist_x64.exe" )
				set( MSC_VER "2012" )
			elseif( MSVC10 )
				set( REDIS_LINK "http://download.microsoft.com/download/3/2/2/3224B87F-CFA0-4E70-BDA3-3DE650EFEBA5/vcredist_x64.exe" )
				set( MSC_VER "2010" )
			elseif( MSVC90 )
				set( REDIS_LINK "http://download.microsoft.com/download/a/9/8/a989ab72-1d46-4e59-9515-7970f9c07e7a/vcredist_x64.exe" )
				set( MSC_VER "2008" )
			elseif( MSVC80 )
				set( REDIS_LINK "http://download.microsoft.com/download/1/b/b/1bbd848c-8e66-49ab-8592-aebad761d8f0/vcredist_x64.exe" )
				set( MSC_VER "2005" )
			endif()
		else( "${CMAKE_SIZEOF_VOID_P}" EQUAL "8" )
			set( REDIS_ARCH "x86" )
		
			if( MSVC11 )
				set( REDIS_LINK "http://download.microsoft.com/download/1/3/2/13297EFB-ED35-46B5-BD9A-F32CF7CC2CFF/VSU3/vcredist_x86.exe" )
				set( MSC_VER "2012" )
			elseif( MSVC10 )
				set( REDIS_LINK "http://download.microsoft.com/download/5/B/C/5BC5DBB3-652D-4DCE-B14A-475AB85EEF6E/vcredist_x86.exe" )
				set( MSC_VER "2010" )
			elseif( MSVC90 )
				set( REDIS_LINK "http://download.microsoft.com/download/f/8/7/f874831f-2dbf-45cf-8d8c-c1a442b2fbbb/vcredist_x86.exe" )
				set( MSC_VER "2008" )
			elseif( MSVC80 )
				set( REDIS_LINK "http://download.microsoft.com/download/7/4/5/74560780-2c53-4d08-b746-a85090201d33/vcredist_x86.exe" )
				set( MSC_VER "2005" )
			endif()
		endif()

		#TODO Should it be better to use DownloadFile function?
		set( CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
			MessageBox MB_OK \\\"Microsoft Visual C++ ${MSC_VER} Redistributable package (${REDIS_ARCH}) is required. Installer will now download and install it\\\"
			StrCpy $2 \\\"$INSTDIR\\\\vcredist.exe\\\"
			NSISdl::download \\\"${REDIS_LINK}\\\" $2
			Pop $R0 ; Get the return value
			StrCmp $R0 \\\"success\\\" +3
				MessageBox MB_OK \\\"Download failed: $R0\\\"
				Quit
			success:
				ExecWait '\\\"$2\\\" /q:a'
				Delete $2
		")
	endif( MSVC_IDE )
	
	set( CPACK_NSIS_CREATE_ICONS "
		CreateShortCut \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\${ProjectName}.lnk\\\" \\\"$INSTDIR\\\\${INSTALL_PATH}\\\\${ProjectName}.exe\\\"
	" )
	
	set( CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "DeleteRegKey HKCU \\\"Software\\\\Guastella Francesco\\\"" )
	set( CPACK_NSIS_DELETE_ICONS "Delete \\\"$SMPROGRAMS\\\\$MUI_TEMP\\\\${ProjectName}.lnk\\\"" )
	
else()
	set( CPACK_GENERATOR "DEB" )
	
	#Architecture suffix
	if( BUILD_32 )
		set( CPACK_SYSTEM_NAME "${CPACK_SYSTEM_NAME}i386" )
	else()
		set( CPACK_SYSTEM_NAME "${CPACK_SYSTEM_NAME}amd64" )
	endif( BUILD_32 )

	#Conflicts
	if( QT_USE_QT5 )
		if( QT_USE_OPENGL )
			set( CPACK_DEBIAN_PACKAGE_CONFLICTS "dep-graphv-qt5-nogl, dep-graphv-qt4-gl, dep-graphv-qt4-nogl" )
		else()
			set( CPACK_DEBIAN_PACKAGE_CONFLICTS "dep-graphv-qt5-gl, dep-graphv-qt4-gl, dep-graphv-qt4-nogl" )
		endif( QT_USE_OPENGL )
	else()
		if( QT_USE_OPENGL )
			set( CPACK_DEBIAN_PACKAGE_CONFLICTS "dep-graphv-qt4-nogl, dep-graphv-qt5-gl, dep-graphv-qt5-nogl" )
		else()
			set( CPACK_DEBIAN_PACKAGE_CONFLICTS "dep-graphv-qt4-gl, dep-graphv-qt5-gl, dep-graphv-qt5-nogl" )
		endif( QT_USE_OPENGL )
	endif( QT_USE_QT5 )
	
	#deb settings
	set( CPACK_DEBIAN_PACKAGE_MAINTAINER "Francesco Guastella <guastella.francesco@gmail.com>" )
	set( CPACK_DEBIAN_PACKAGE_HOMEPAGE "http://sourceforge.net/projects/depgraph/" )
	set( CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON ) # dpkg-shlibdeps must be installed on build system (package: dpkg-dev)
endif( WIN32 )

include( CPack )
