################################################################################
#                _                                        _
#             __| | ___ _ __         __ _ _ __ __ _ _ __ | |__/\   /\
#            / _` |/ _ \ '_ \ _____ / _` | '__/ _` | '_ \| '_ \ \ / /
#           | (_| |  __/ |_) |_____| (_| | | | (_| | |_) | | | \ V /
#            \__,_|\___| .__/       \__, |_|  \__,_| .__/|_| |_|\_/
#                      |_|          |___/          |_|
#
################################################################################
#
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
################################################################################
#CPack settings

#Checking prerequisites for UNIX systems
if( UNIX )
    #First of all, we need to check for valid DEPGRAPHV_PREFERRED_PACKAGE value
    if( ${DEPGRAPHV_PREFERRED_PACKAGE} STREQUAL "DEB" )
	   find_program( DPKG_SHLIBDEPS_FOUND NAMES dpkg-shlibdeps DOC "path to the dpkg-shlibdeps script." )
	   if( NOT DPKG_SHLIBDEPS_FOUND )
		  message( WARNING "dpkg-shlibdeps must be installed in order to create a .deb package! (package name: dpkg-dev)" )
		  return()
	   endif()

    elseif( ${DEPGRAPHV_PREFERRED_PACKAGE} STREQUAL "RPM" )
	   #TODO Check for rpmbuild for rpm package
	   message( WARNING "At this time, it is not possibile to build an .rpm package.\nThis feature will be soon available." )
	   return()

    else()
	   message( SEND_ERROR "${DEPGRAPHV_PREFERRED_PACKAGE} is an invalid value for DEPGRAPHV_PREFERRED_PACKAGE option!" )
    endif()
endif( UNIX )

set( CPACK_PACKAGE_VENDOR "Guastella Francesco" )
set( VENDOR_EMAIL "guastella.francesco@gmail.com" )
set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "${ProjectName} - Header dependencies analizer\n" )
set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "${CPACK_PACKAGE_DESCRIPTION_SUMMARY} ${ProjectName} is an useful tool to analize header dependendencies via graphs. This is the runtime package of the tool, built against" )
set( CPACK_PACKAGE_VERSION ${ProjectVersion} )
set( CPACK_PACKAGE_INSTALL_DIRECTORY ${ProjectName} )

#Qt version
if( DEPGRAPHV_USE_QT5 )
	set( CPACK_PACKAGE_NAME "${ProjectName}-qt5" )
	set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "${CPACK_PACKAGE_DESCRIPTION_SUMMARY} Qt5," )
else()
	set( CPACK_PACKAGE_NAME "${ProjectName}-qt4" )
	set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "${CPACK_PACKAGE_DESCRIPTION_SUMMARY} Qt4," )
endif( DEPGRAPHV_USE_QT5 )

#OpenGL Support?
if( DEPGRAPHV_USE_OPENGL )
	set( CPACK_PACKAGE_NAME "${CPACK_PACKAGE_NAME}-gl" )
	set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "${CPACK_PACKAGE_DESCRIPTION_SUMMARY} with OpenGL support." )
else()
	set( CPACK_PACKAGE_NAME "${CPACK_PACKAGE_NAME}-nogl" )
	set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "${CPACK_PACKAGE_DESCRIPTION_SUMMARY} without OpenGL support." )
endif( DEPGRAPHV_USE_OPENGL )

if( WIN32 )
    include( pack_nsis )
else()
    if( ${DEPGRAPHV_PREFERRED_PACKAGE} STREQUAL "DEB" )
	   include( pack_deb )

    elseif( ${DEPGRAPHV_PREFERRED_PACKAGE} STREQUAL "RPM" )
	   include( pack_rpm )

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
