#####################################################################################
# buildQt5.cmake
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
set( definitions "-DQT5" )
set( qt5Modules "Widgets;Svg" )

if( WIN32 )
	#Default installation path of Qt5.1.1
	if( MSVC11 )
		set( CMAKE_PREFIX_PATH "C:/Qt/5.1.1/msvc2012/" )
	elseif( MSVC10 )
		set( CMAKE_PREFIX_PATH "C:/Qt/5.1.1/msvc2010/" )
	else()
		set( CMAKE_PREFIX_PATH "C:/Qt/5.1.1/mingw48_32/" )
	endif( MSVC11 )
endif( WIN32 )

#Looking for Qt5 Widgets
message( STATUS "* Looking for Qt5 Widgets..." )
find_package( Qt5Widgets REQUIRED )
if( NOT Qt5Widgets_FOUND )
	message( SEND_ERROR "* Failed to find Qt5 Widgets." )
else()
	message( STATUS "* Found" )
endif()

#Looking for Qt5 Svg
message( STATUS "* Looking for Qt5 Svg..." )
find_package( Qt5Svg REQUIRED )
if( NOT Qt5Svg_FOUND )
	message( SEND_ERROR "* Failed to find Qt5 Svg." )
else()
	message( STATUS "* Found" )
endif()

#Looking for Qt5 Opengl
if( QT_USE_OPENGL )
	message( STATUS "* Looking for Qt5 OpenGL..." )
	find_package( Qt5OpenGL REQUIRED )
	if( NOT Qt5OpenGL_FOUND )
		message( STATUS "* Failed to find Qt5 OpenGL." )
	else()
		set( qt5Modules "${qt5Modules};OpenGL" )
		message( STATUS "* Found" )
	endif()
endif( QT_USE_OPENGL )

message( STATUS "* Looking for Qt5 Linguist tools..." )
find_package( Qt5LinguistTools REQUIRED )
if( NOT Qt5LinguistTools_FOUND )
	message( SEND_ERROR "* Failed to find Qt5 Linguist tools." )
else()
	message( STATUS "* Found" )
endif()

qt5_wrap_ui( UISrcs ${Sources_ui} )

# generate rules for building source files from the resources
qt5_add_resources( RCCSrcs ${Resources} )

if( QT_UPDATE_TRANSLATIONS )
	qt5_create_translation( i18nSrcs ${FilesToTranslate} ${Translations} )
else()
	qt5_add_translation( i18nSrcs ${Translations} )
endif( QT_UPDATE_TRANSLATIONS )

set( includes "${CMAKE_SOURCE_DIR}/src;${GraphViz_INCLUDE_DIRS}" )
set( libraries "${GraphViz_gvc_LIBRARY};${GraphViz_cgraph_LIBRARY}" )