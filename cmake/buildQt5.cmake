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
# buildQt5.cmake
#
# This file is part of dep-graphV - An useful tool to analize header
# dependendencies via graphs.
#
# This software is distributed under the MIT License:
#
# Copyright (c) 2013 - 2015 Francesco Guastella aka romeoxbm
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
if( NOT DEFINED qt5Modules )
	set( qt5Modules "Widgets" )
endif()

include( functions-qt5 )
varHasChanged( DEPGRAPHV_USE_QT5 ${DEPGRAPHV_USE_QT5} "QT_QMAKE_EXECUTABLE" )

if( WIN32 )
	set( QT_INSTALL_DIR_PREFIX "C:/Qt/Qt*/*.*" )
	
	if( MSVC12 )
		set( PATTERN "${QT_INSTALL_DIR_PREFIX}/msvc2013_opengl" )
	elseif( MSVC11 )
		if( "${CMAKE_SIZEOF_VOID_P}" EQUAL "8" )
			set( PATTERN "${QT_INSTALL_DIR_PREFIX}/msvc2012_64" )
		else()
			set( PATTERN "${QT_INSTALL_DIR_PREFIX}/msvc2012" )
		endif()
	elseif( MSVC10 )
		set( PATTERN "${QT_INSTALL_DIR_PREFIX}/msvc2010" )
	else()
		set( PATTERN "${QT_INSTALL_DIR_PREFIX}/mingw48_32/" )
	endif( MSVC12 )
	
	foreach( mod ${qt5Modules} )
		list( APPEND variables "Qt5${mod}_DIR" ) 
	endforeach()
	
	getPathFromPattern(
		QT_INSTALL_DIR
		${PATTERN}
		"Installation paths found for Qt5"
		"${variables}"
	)
	
	set( CMAKE_PREFIX_PATH ${QT_INSTALL_DIR} )
endif( WIN32 )

#Looking for Qt5 modules
foreach( mod ${qt5Modules} )
	message( STATUS "* Looking for Qt5 ${mod}..." )
	find_package( Qt5${mod} REQUIRED )
	if( NOT ${Qt5${mod}_FOUND} )
		message( SEND_ERROR "* Failed to find Qt5 ${mod}." )
	else()
		message( STATUS "* Found ${Qt5${mod}_VERSION}" )
		if( NOT mod STREQUAL "LinguistTools" )
			list( APPEND includes ${Qt5${mod}_INCLUDE_DIRS} )
			list( APPEND libraries "Qt5::${mod}" )
		endif()
	endif()
endforeach()

if( DEFINED Sources_moc )
	qt5_wrap_cpp( Mocs ${Sources_moc} OPTIONS -bdepgraphv_pch.h )
endif()

if( DEFINED Sources_ui )
	custom_qt5_wrap_ui( UISrcs ${Sources_ui} )
endif()

# generate rules for building source files from the resources
if( DEFINED Resources )
	# generate rules for building source files from the resources
	if( ${DEPGRAPHV_ENABLE_PCH_SUPPORT} )
		custom_qt5_add_resources( RCCSrcs ${Resources} )
	else()
		qt5_add_resources( RCCSrcs ${Resources} )
	endif()
endif()

foreach( t ${AvailableLanguages} )
	if( DEPGRAPHV_UPDATE_QT_TRANSLATIONS )
		qt5_create_translation( i18nSrcs ${FilesToTranslate} ${i18n_${t}} )
	else()
		qt5_add_translation( i18nSrcs ${i18n_${t}} )
	endif( DEPGRAPHV_UPDATE_QT_TRANSLATIONS )
endforeach()
