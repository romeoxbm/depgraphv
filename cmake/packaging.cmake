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
set( CPACK_PACKAGE_INSTALL_DIRECTORY "dep-graphV" )

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
set( CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README" ) #TODO Not visualized by NSIS
set( CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING" )

set( CPACK_COMPONENTS_ALL runtime translations sources )
set( CPACK_COMPONENT_RUNTIME_DISPLAY_NAME "dep-graphV application" ) #TODO
set( CPACK_COMPONENT_RUNTIME_DESCRIPTION "dep-graphV application description goes here" ) #TODO

set( CPACK_COMPONENT_TRANSLATIONS_DISPLAY_NAME "Translations" ) #TODO
set( CPACK_COMPONENT_TRANSLATIONS_DESCRIPTION "Translations description goes here" ) #TODO

#Dependencies
set( CPACK_COMPONENT_TRANSLATIONS_DEPENDS runtime )

if( WIN32 )
	set( CPACK_GENERATOR "NSIS" )
	
	set( CPACK_COMPONENT_SOURCES_DISPLAY_NAME "Sources" ) #TODO
	set( CPACK_COMPONENT_SOURCES_DESCRIPTION "Sources description goes here" ) #TODO
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