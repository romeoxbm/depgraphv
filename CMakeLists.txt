#####################################################################################
# CMakeLists.txt
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
cmake_minimum_required( VERSION 2.8.11 )

set( ProjectName "dep-graphV" )
set( ProjectVersion "0.2" )
set( CMAKE_AUTOMOC ON )

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set( CMAKE_INCLUDE_CURRENT_DIR ON )
set( CMAKE_SCRIPTS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )
set( CMAKE_MODULE_PATH "${CMAKE_SCRIPTS_PATH};${CMAKE_MODULE_PATH}" )

if( WIN32 )
	#Default installation path of Qt5.1.1
	set( CMAKE_PREFIX_PATH "C:\\Qt\\5.1.1\\mingw48_32\\" )
	set( INSTALL_PATH "${CMAKE_INSTALL_PREFIX}" )
	set( TRANSLATION_INSTALL_PATH "translations" )
else()

	set( INSTALL_PATH "bin" )
	set( TRANSLATION_INSTALL_PATH "share/${ProjectName}/translations" )
endif()

project( ${ProjectName} )

set( Headers
	src/graph.h
	src/mainwindow.h
)

set( Sources
	src/graph.cpp
	src/main.cpp
	src/mainwindow.cpp
)

set( Sources_ui
	src/mainwindow.ui
)

set( Resources
	res/resources.qrc
)

set( FilesToTranslate
    ${Sources_ui}
    ${Sources}
)

set( Translations
	i18n/${ProjectName}_en_US.ts
	i18n/${ProjectName}_it_IT.ts
)

option( QT_USE_QT5 "Build against Qt5 libraries (instead of Qt4)." FALSE )
option( QT_USE_OPENGL FALSE )
option( QT_UPDATE_TRANSLATIONS false )

#Looking for Graphviz
message( STATUS "* Looking for Graphviz..." )
find_package( GraphViz REQUIRED )
if( NOT GRAPHVIZ_FOUND )
	message( SEND_ERROR "* Failed to find Graphviz." )
else()
	message( STATUS "* Found" )
endif()

if( QT_USE_QT5 )

	set( definitions "-DQT5" )
	set( qt5Modules "Widgets;Svg" )

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

else()

	set( definitions "" )

	#Looking for Qt4
	message( STATUS "* Looking for Qt4..." )
	if( QT_USE_OPENGL )
		find_package( Qt4 REQUIRED QtCore QtSvg QtOpenGL )
	else()
		find_package( Qt4 REQUIRED QtCore QtSvg )
	endif( QT_USE_OPENGL )
		
	if( NOT Qt4_FOUND )
		message( SEND_ERROR "* Failed to find Qt4." )
	else()
		message( STATUS "* Found ${QTVERSION}" )
	endif()
	
	include( ${QT_USE_FILE} )

	qt4_wrap_ui( UISrcs ${Sources_ui} )

	# generate rules for building source files from the resources
	qt4_add_resources( RCCSrcs ${Resources} )

	if( QT_UPDATE_TRANSLATIONS )
		qt4_create_translation( i18nSrcs ${FilesToTranslate} ${Translations} )
	else()
		qt4_add_translation( i18nSrcs ${Translations} )
	endif( QT_UPDATE_TRANSLATIONS )

	set( includes "${CMAKE_SOURCE_DIR}/src;${GraphViz_INCLUDE_DIRS};${QT_INCLUDES}" )
	set( libraries "${GraphViz_gvc_LIBRARY};${GraphViz_cgraph_LIBRARY};${QT_LIBRARIES}" )

endif( QT_USE_QT5 )

# enable warnings and other required definitions
if( QT_USE_OPENGL )
	set( definitions "${definitions} -DUSE_OPENGL" )
endif( QT_USE_OPENGL )

if( CMAKE_COMPILER_IS_GNUCXX )
	set( definitions "${definitions} -Wall" )
elseif( MSVC_IDE )
	set( definitions "${definitions} /W4 /MP" )
endif()

message( STATUS "* Using definitions: ${definitions}" )
add_definitions( ${definitions} )

include_directories( ${includes} )

add_executable( ${ProjectName}
	${Headers}
	${Sources}
	${MOCSrcs}
	${UISrcs}
	${RCCSrcs}
	${i18nSrcs}
)

if( QT_USE_QT5 )
	qt5_use_modules( ${ProjectName} ${qt5Modules} )
endif( QT_USE_QT5 )

target_link_libraries( ${ProjectName} ${libraries} )

install( TARGETS ${ProjectName} RUNTIME DESTINATION ${INSTALL_PATH} )

#Install translations
file( GLOB Translations_rel "${CMAKE_CURRENT_BINARY_DIR}/*.qm" )
install( FILES ${Translations_rel} DESTINATION ${TRANSLATION_INSTALL_PATH} )

#Configure buildsettings file
configure_file(
	"${CMAKE_SCRIPTS_PATH}/buildsettings.h.in"
	"${CMAKE_CURRENT_BINARY_DIR}/buildsettings.h"
	IMMEDIATE @ONLY
)

# uninstall target
configure_file(
	"${CMAKE_SCRIPTS_PATH}/cmake_uninstall.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
	IMMEDIATE @ONLY
)

add_custom_target( uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake )
