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
set( CMAKE_TEMPLATES_PATH "${CMAKE_SCRIPTS_PATH}/templates" )
set( CMAKE_MODULE_PATH "${CMAKE_SCRIPTS_PATH};${CMAKE_MODULE_PATH}" )
set( INSTALL_PATH "bin" )

if( WIN32 )
	set( TRANSLATION_INSTALL_PATH "translations" )
	set( DOCS_INSTALL_PATH "docs" )
	set( SRC_INSTALL_PATH "src" )
else()
	set( TRANSLATION_INSTALL_PATH "share/${ProjectName}/translations" )
	set( DOCS_INSTALL_PATH "share/doc/${ProjectName}" )
endif()

project( ${ProjectName} )

set( Headers
	src/aboutdialog.h
	src/appconfig.h
	src/graph.h
	src/mainwindow.h
)

set( Sources
	src/aboutdialog.cpp
	src/appconfig.cpp
	src/graph.cpp
	src/main.cpp
	src/mainwindow.cpp
)

set( Sources_ui
	src/aboutdialog.ui
	src/mainwindow.ui
)

set( Resources
	res/resources.qrc
)

set( FilesToTranslate
	${Sources_ui}
	${Sources}
)

set( AvailableLanguages
	"Italiano"
)

set( i18n_Italiano "i18n/${ProjectName}_it.ts" )

set( DOCS
	AUTHORS
	ChangeLog
	COPYING
	README
	TODO
)

foreach( t ${AvailableLanguages} )
	string( REPLACE "i18n" ${CMAKE_CURRENT_BINARY_DIR} newT ${i18n_${t}} )
	string( REPLACE ".ts" ".qm" newT ${newT} )
	list( APPEND Translations_rel ${newT} )
endforeach()

option( QT_USE_QT5 "Build against Qt5 libraries (instead of Qt4)." FALSE )
option( QT_USE_OPENGL FALSE )
option( QT_UPDATE_TRANSLATIONS false )
if( CMAKE_COMPILER_IS_GNUCXX )
	option( BUILD_32 "Build 32bit executable (when false, 64bit will be selected)" TRUE )
endif()

#Looking for Graphviz
message( STATUS "* Looking for Graphviz..." )
find_package( GraphViz REQUIRED )
if( NOT GRAPHVIZ_FOUND )
	message( SEND_ERROR "* Failed to find Graphviz." )
else()
	message( STATUS "* Found" )
endif()

#Include right file depending on selected Qt version.
if( QT_USE_QT5 )
	include( buildQt5 )
else()
	include( buildQt4 )
endif( QT_USE_QT5 )

if( GraphViz_USE_CGRAPH )
	list( APPEND libraries "${GraphViz_cgraph_LIBRARY}" )
else()
	list( APPEND libraries "${GraphViz_graph_LIBRARY}" )
endif()

if( CMAKE_COMPILER_IS_GNUCXX )
	set( definitions "-Wall" )
	
	if( BUILD_32 )
		set( definitions "${definitions} -m32" )
	else()
		set( definitions "${definitions} -m64" )
	endif( BUILD_32 )
	
elseif( MSVC_IDE )
	set( definitions "/W4 /MP" )
endif()

message( STATUS "* Using definitions: ${definitions}" )
add_definitions( ${definitions} )

include_directories( ${includes} )

add_executable( ${ProjectName} WIN32
	${Headers}
	${Sources}
	${UISrcs}
	${RCCSrcs}
	${i18nSrcs}
)

if( QT_USE_QT5 )
	qt5_use_modules( ${ProjectName} ${qt5Modules} )
endif( QT_USE_QT5 )

target_link_libraries( ${ProjectName} ${libraries} )

install( TARGETS ${ProjectName} RUNTIME DESTINATION ${INSTALL_PATH} COMPONENT runtime )

file( WRITE "${CMAKE_CURRENT_BINARY_DIR}/tPath" "${CMAKE_INSTALL_PREFIX}/${TRANSLATION_INSTALL_PATH}" )
install( FILES "${CMAKE_CURRENT_BINARY_DIR}/tPath" DESTINATION ${INSTALL_PATH} COMPONENT runtime )

foreach( t ${AvailableLanguages} )
	#install( FILES ${Translations_rel} DESTINATION ${TRANSLATION_INSTALL_PATH} COMPONENT comp_i18n_ita )

	string( REPLACE "i18n" ${CMAKE_CURRENT_BINARY_DIR} newT ${i18n_${t}} )
	string( REPLACE ".ts" ".qm" newT ${newT} )
	install( FILES ${newT} DESTINATION ${TRANSLATION_INSTALL_PATH} COMPONENT comp_i18n_${t} )
endforeach()
install( FILES ${DOCS} DESTINATION ${DOCS_INSTALL_PATH} COMPONENT docs )

if( WIN32 )
	set( RUNTIME_LIBS_PATH "${CMAKE_PREFIX_PATH}bin;${GraphViz_INSTALL_DIRS}/bin" )
	
	if( MSVC_IDE )
		include( CreateLaunchers )
		create_target_launcher( ${ProjectName} WORKING_DIRECTORY "$(OutDir)" RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBS_PATH} )

		source_group( "Qrc Resources" FILES ${Resources} ${RCCSrcs} )
		source_group( "Ui" FILES ${Sources_ui} ${UISrcs} )
		source_group( "Compiled Translations" FILES ${Translations_rel} )
	endif( MSVC_IDE )
			
	#Configure installDeps.cmake
	configure_file(
		"${CMAKE_TEMPLATES_PATH}/installDeps.cmake.in"
		"${CMAKE_CURRENT_BINARY_DIR}/installDeps.cmake"
		IMMEDIATE @ONLY
	)
	
	#Configure installConfig6.cmake
	configure_file(
		"${CMAKE_TEMPLATES_PATH}/installConfig6.cmake.in"
		"${CMAKE_CURRENT_BINARY_DIR}/installConfig6.cmake"
		IMMEDIATE @ONLY
	)

	install( FILES ${Headers} ${Sources} ${Sources_ui} ${Resources} ${Translations} ${CMAKE_CURRENT_BINARY_DIR}/buildsettings.h
		 DESTINATION ${SRC_INSTALL_PATH} COMPONENT sources )

	install( SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/installDeps.cmake" COMPONENT runtime )
	install( SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/installConfig6.cmake" COMPONENT runtime )
	
else()	
	#TODO .desktop file
	#See http://techbase.kde.org/Development/Tutorials/Desktop_File
	
	#Configure .desktop file
	configure_file(
		"${CMAKE_TEMPLATES_PATH}/${ProjectName}.desktop.in"
		"${CMAKE_CURRENT_BINARY_DIR}/${ProjectName}.desktop"
		IMMEDIATE @ONLY
	)
	
	#install( PROGRAMS "${ProjectName}.desktop" DESTINATION ${XDG_APPS_INSTALL_DIR} )
	install( PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/${ProjectName}.desktop" DESTINATION "share/applications/kde4" )
endif( WIN32 )

include( packaging )

#Configure buildsettings file
configure_file(
	"${CMAKE_TEMPLATES_PATH}/buildsettings.h.in"
	"${CMAKE_CURRENT_BINARY_DIR}/buildsettings.h"
	IMMEDIATE @ONLY
)

# uninstall target
configure_file(
	"${CMAKE_TEMPLATES_PATH}/cmake_uninstall.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
	IMMEDIATE @ONLY
)

add_custom_target( uninstall COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" )
