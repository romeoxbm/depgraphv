#####################################################################################
# packaging.cmake
#
# This file is part of dep-graphV - An useful tool to analize header
# dependendencies via graphs.
#
# This software is distributed under the MIT License:
#
# Copyright (c) 2013 - 2014 Francesco Guastella aka romeoxbm
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
set( VENDOR_EMAIL "guastella.francesco@gmail.com" )
set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "${ProjectName} - Header dependencies analizer\n" )
set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "${CPACK_PACKAGE_DESCRIPTION_SUMMARY} ${ProjectName} is an useful tool to analize header dependendencies via graphs. This is the runtime package of the tool, built against" )
set( CPACK_PACKAGE_VERSION ${ProjectVersion} )
set( CPACK_PACKAGE_INSTALL_DIRECTORY ${ProjectName} )

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

if( WIN32 )
	set( CPACK_MODULE_PATH "${CMAKE_TEMPLATES_PATH}" )
	set( CPACK_GENERATOR "NSIS" )

	set( CPACK_NSIS_PROJECT_NAME ${ProjectName} )
	set( CPACK_NSIS_BIN_PATH ${BIN_INSTALL_PATH} )
	set( CPACK_NSIS_MODIFY_PATH ON )
	set( CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON )
	set( CPACK_NSIS_MUI_FINISHPAGE_RUN "${ProjectName}.exe" )
	set( CPACK_NSIS_CONTACT ${VENDOR_EMAIL} )
	set( CPACK_NSIS_HELP_LINK "http:\\\\\\\\sourceforge.net\\\\projects\\\\depgraph\\\\support" )
	set( CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\sourceforge.net\\\\projects\\\\depgraph" )
	set( CPACK_NSIS_URL_LINK_NAME "${CPACK_NSIS_PROJECT_NAME}'s project page at SourceForge.net" )
	
	set( CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}\\\\res\\\\NSIS_Theme\\\\orange-install.ico" )
	set( CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}\\\\res\\\\NSIS_Theme\\\\orange-uninstall.ico" )
	set( CPACK_NSIS_MUI_HEADER "${CMAKE_CURRENT_SOURCE_DIR}\\\\res\\\\NSIS_Theme\\\\orange_header.bmp" )
	set( CPACK_NSIS_MUI_UNIHEADER "${CMAKE_CURRENT_SOURCE_DIR}\\\\res\\\\NSIS_Theme\\\\orange-uninstall_header.bmp" )
	set( CPACK_NSIS_MUI_WIZARD "${CMAKE_CURRENT_SOURCE_DIR}\\\\res\\\\NSIS_Theme\\\\orange_wizard.bmp" )
	set( CPACK_NSIS_MUI_UNIWIZARD "${CMAKE_CURRENT_SOURCE_DIR}\\\\res\\\\NSIS_Theme\\\\orange-uninstall_wizard.bmp" )
	
	if( MSVC_IDE )
		set( CPACK_NSIS_MSVC_REDIST ON )

		if( "${CMAKE_SIZEOF_VOID_P}" EQUAL "8" )
			set( REDIS_ARCH "x64" )
		
			if( MSVC11 )
				set( CPACK_NSIS_MSVC_REDIST_LINK "http://download.microsoft.com/download/1/3/2/13297EFB-ED35-46B5-BD9A-F32CF7CC2CFF/VSU3/vcredist_x64.exe" )
				set( MSC_VER "2012" )
			elseif( MSVC10 )
				set( CPACK_NSIS_MSVC_REDIST_LINK "http://download.microsoft.com/download/3/2/2/3224B87F-CFA0-4E70-BDA3-3DE650EFEBA5/vcredist_x64.exe" )
				set( MSC_VER "2010" )
			elseif( MSVC90 )
				set( CPACK_NSIS_MSVC_REDIST_LINK "http://download.microsoft.com/download/a/9/8/a989ab72-1d46-4e59-9515-7970f9c07e7a/vcredist_x64.exe" )
				set( MSC_VER "2008" )
			elseif( MSVC80 )
				set( CPACK_NSIS_MSVC_REDIST_LINK "http://download.microsoft.com/download/1/b/b/1bbd848c-8e66-49ab-8592-aebad761d8f0/vcredist_x64.exe" )
				set( MSC_VER "2005" )
			endif()
			
		else( "${CMAKE_SIZEOF_VOID_P}" EQUAL "8" )
			set( REDIS_ARCH "x86" )
		
			if( MSVC11 )
				set( CPACK_NSIS_MSVC_REDIST_LINK "http://download.microsoft.com/download/1/3/2/13297EFB-ED35-46B5-BD9A-F32CF7CC2CFF/VSU3/vcredist_x86.exe" )
				set( MSC_VER "2012" )
			elseif( MSVC10 )
				set( CPACK_NSIS_MSVC_REDIST_LINK "http://download.microsoft.com/download/5/B/C/5BC5DBB3-652D-4DCE-B14A-475AB85EEF6E/vcredist_x86.exe" )
				set( MSC_VER "2010" )
			elseif( MSVC90 )
				set( CPACK_NSIS_MSVC_REDIST_LINK "http://download.microsoft.com/download/f/8/7/f874831f-2dbf-45cf-8d8c-c1a442b2fbbb/vcredist_x86.exe" )
				set( MSC_VER "2008" )
			elseif( MSVC80 )
				set( CPACK_NSIS_MSVC_REDIST_LINK "http://download.microsoft.com/download/7/4/5/74560780-2c53-4d08-b746-a85090201d33/vcredist_x86.exe" )
				set( MSC_VER "2005" )
			endif()
		endif()

		set( CPACK_NSIS_MSVC_REDIST_MSG "Microsoft Visual C++ ${MSC_VER} Redistributable package (${REDIS_ARCH}) is required. Installer will now download and install it. If you already installed it, just skip the redistributable package installation." )

	endif( MSVC_IDE )
	
	set( CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "DeleteRegKey HKCU \\\"Software\\\\${CPACK_PACKAGE_VENDOR}\\\"" )
	
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
	set( CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_VENDOR} <${VENDOR_EMAIL}>" )
	set( CPACK_DEBIAN_PACKAGE_HOMEPAGE "http://sourceforge.net/projects/depgraph/" )
	set( CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON ) # dpkg-shlibdeps must be installed on build system (package: dpkg-dev)
endif( WIN32 )

#Resources
set( CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README" )
set( CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README" )
set( CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING" )

include( CPack )

#Components
cpack_add_component( runtime DISPLAY_NAME "Binaries" DESCRIPTION "Required binaries and libraries" REQUIRED INSTALL_TYPES "Full" "Minimal" )
#cpack_add_component( comp_i18n_en DISPLAY_NAME "English" GROUP "Translations" REQUIRED ) #The ui is already in english...
foreach( t ${AvailableLanguages} )
	cpack_add_component( comp_i18n_${t} DISPLAY_NAME "${t}" GROUP "Translations" INSTALL_TYPES "Full" )
endforeach()
cpack_add_component( docs DISPLAY_NAME "Documentation" DESCRIPTION "Readme, Todo and other information files." REQUIRED INSTALL_TYPES "Full" )

if( WIN32 )
	cpack_add_component( sources DISPLAY_NAME "Sources" DESCRIPTION "Include all ${ProjectName} sources" INSTALL_TYPES "Full" )
endif()
