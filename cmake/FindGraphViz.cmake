# Copyright (C) 2007-2009 LuaDist.
# Created by Peter Kapec <kapecp@gmail.com>
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
# Note:
# Searching headers and libraries is very simple and is NOT as powerful as scripts
# distributed with CMake, because LuaDist defines directories to search for.
# Everyone is encouraged to contact the author with improvements. Maybe this file
# becomes part of CMake distribution sometimes.

# - Find GraphViz
# Find the native GraphViz headers and libraries.
#
# GraphViz_INCLUDE_DIRS - where to find m_apm.h, etc.
# GraphViz_LIBRARIES - List of libraries when using GraphViz.
# GRAPHVIZ_FOUND - True if GraphViz found.

#Libs this script is looking for...
set( libs
	gvc
	graph
	cgraph
	cdt
	pathplan
)

if( WIN32 )
	set( variables GraphViz_INCLUDE_DIR )
	foreach( lib ${libs} )
		list( APPEND variables "GraphViz_LIB_${lib}" )
	endforeach()
		
	getPathFromPattern( 
		GraphViz_INSTALL_DIR
		"$ENV{PROGRAMFILES}/Graphviz*"
		"Installation paths found for Graphviz"
		"${variables}"
	)
		
	#TODO This always link using release libraries as when
	#configuring for Visual Studio, there's no CMAKE_BUILD_TYPE
	#(but CMAKE_CONFIGURATION_TYPES)
	if( CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" )
		set( GRAPHVIZ_LIB_PATH_SUFFIX "lib/debug/lib" )
	else()
		set( GRAPHVIZ_LIB_PATH_SUFFIX "lib/release/lib" )
	endif()

else()
	set( GraphViz_INSTALL_DIR "$ENV{GRAPHVIZ_DIR}" )
	set( GRAPHVIZ_LIB_PATH_SUFFIX "lib" )
endif()

# Look for the include path.
find_path( GraphViz_INCLUDE_DIR
	NAMES graph.h cgraph.h
	HINTS ${GraphViz_INSTALL_DIR}
	PATH_SUFFIXES "include/graphviz"
)

#Look for libraries
foreach( lib ${libs} )
	find_library( GraphViz_LIB_${lib}
		NAMES ${lib}
		HINTS ${GraphViz_INSTALL_DIR}
		PATH_SUFFIXES ${GRAPHVIZ_LIB_PATH_SUFFIX}
	)
	list( APPEND libraries_found ${GraphViz_LIB_${lib}} )
endforeach()

# Handle the QUIETLY and REQUIRED arguments and set GraphViz_FOUND to TRUE if all listed variables are TRUE.
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( GraphViz DEFAULT_MSG libraries_found GraphViz_INCLUDE_DIR )

# Copy the results to the output variables.
if( GRAPHVIZ_FOUND )
	set( GraphViz_LIBRARIES ${libraries_found} )
	set( GraphViz_INCLUDE_DIRS ${GraphViz_INCLUDE_DIR} )
	
	#Getting GraphViz version
	set( versionFile "${GraphViz_INCLUDE_DIR}/graphviz_version.h" )
	if( EXISTS ${versionFile} )
		#from graphviz version 2.36, there isn't VERSION define anymore, so in that case, we look for PACKAGE_VERSION define.
		file( STRINGS ${versionFile} defineVer REGEX "#define VERSION" )
		if( defineVer STREQUAL "" )
			file( STRINGS ${versionFile} defineVer REGEX "#define PACKAGE_VERSION" )
			string( SUBSTRING ${defineVer} 25 4 ver )
		else()
			string( SUBSTRING ${defineVer} 17 4 ver )
		endif()
		if( ${ver} VERSION_GREATER "2.28" OR ${ver} VERSION_EQUAL "2.28" )
			set( GraphViz_USE_CGRAPH ON )
		else()
			set( GraphViz_USE_CGRAPH 0 )
		endif()
	else()
		#TODO Should we always use cgraph when graphviz_version.h does not exists?
		set( GraphViz_USE_CGRAPH ON )
	endif()

else( GRAPHVIZ_FOUND )
	set( GraphViz_LIBRARIES )
	set( GraphViz_INCLUDE_DIRS )
	set( GraphViz_USE_CGRAPH 0 )
endif( GRAPHVIZ_FOUND )

mark_as_advanced( GraphViz_INCLUDE_DIRS GraphViz_LIBRARIES GraphViz_USE_CGRAPH )
