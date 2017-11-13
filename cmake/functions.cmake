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
# functions.cmake
#
# This file is part of dep-graphV - An useful tool to analize header
# dependendencies via graphs.
#
# This software is distributed under the MIT License:
#
# Copyright (c) 2013 - 2017 Francesco Guastella aka romeoxbm
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


################################################################################
# varHasChanged
################################################################################
function( varHasChanged var value unsetVars )
	if( NOT DEFINED ${var}_LAST )
		set( ${var}_LAST ${value} CACHE STRING "" )
		mark_as_advanced( ${var}_LAST )
	endif()
	
	if( NOT "${${var}}" STREQUAL "${${var}_LAST}" )
		foreach( uVar ${unsetVars} )
			unset( ${uVar} CACHE )
		endforeach()
		set( ${var}_LAST ${${var}} CACHE STRING "" FORCE )
	endif()
endfunction()

################################################################################
# getPathFromPattern
################################################################################
function( getPathFromPattern result pattern desc varList )
	file( GLOB _iPaths ${pattern} )
	file( TO_CMAKE_PATH "${_iPaths}" paths )
	list( LENGTH paths len )
	if( ${len} GREATER 1 )
		#In event of multiple matches, this function create a drop-down list
		#containing matching paths.
		list( GET paths 0 firstElem )
		set( ${result} ${firstElem} CACHE STRING ${desc} )
		set_property( CACHE ${result} PROPERTY STRINGS ${paths} )
		
		varHasChanged( ${result} ${firstElem} "${varList}" )
	else()
		set( ${result} ${paths} PARENT_SCOPE )
	endif()
endfunction()
