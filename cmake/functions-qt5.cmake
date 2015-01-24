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
# functions-qt5.cmake
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

################################################################################
# CUSTOM_QT5_ADD_RESOURCES
################################################################################
function( CUSTOM_QT5_ADD_RESOURCES outfiles )
	set( options )
	set( oneValueArgs )
	set( multiValueArgs OPTIONS )

	cmake_parse_arguments( _RCC "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

	set( rcc_files ${_RCC_UNPARSED_ARGUMENTS} )
	set( rcc_options ${_RCC_OPTIONS} )

	foreach( it ${rcc_files} )
		get_filename_component( outfilename ${it} NAME_WE )
		get_filename_component( infile ${it} ABSOLUTE )
		get_filename_component( rc_path ${infile} PATH )
		set( outfile ${CMAKE_CURRENT_BINARY_DIR}/src/qrc_${outfilename}.cpp )

		set( _RC_DEPENDS )
		if( EXISTS "${infile}" )
			#  parse file for dependencies
			#  all files are absolute paths or relative to the location of the qrc file
			file( READ "${infile}" _RC_FILE_CONTENTS )
			string( REGEX MATCHALL "<file[^<]+" _RC_FILES "${_RC_FILE_CONTENTS}" )
			foreach( _RC_FILE ${_RC_FILES})
				string( REGEX REPLACE "^<file[^>]*>" "" _RC_FILE "${_RC_FILE}" )
				if( NOT IS_ABSOLUTE "${_RC_FILE}" )
					set( _RC_FILE "${rc_path}/${_RC_FILE}" )
				endif()
				set( _RC_DEPENDS ${_RC_DEPENDS} "${_RC_FILE}" )
			endforeach()
			# Since this cmake macro is doing the dependency scanning for these files,
			# let's make a configured file and add it as a dependency so cmake is run
			# again when dependencies need to be recomputed.
			qt5_make_output_file( "${infile}" "" "qrc.depends" out_depends )
			configure_file( "${infile}" "${out_depends}" COPY_ONLY )
		else()
			# The .qrc file does not exist (yet). Let's add a dependency and hope
			# that it will be generated later
			set( out_depends )
		endif()

		add_custom_command( OUTPUT ${outfile}
							COMMAND ${CMAKE_COMMAND}
							ARGS -E echo "#include \"depgraphv_pch.h\"" > ${outfile}
							COMMAND ${Qt5Core_RCC_EXECUTABLE}
							ARGS ${rcc_options} -name ${outfilename} ${infile} >> ${outfile}
							MAIN_DEPENDENCY ${infile}
							DEPENDS ${_RC_DEPENDS} "${out_depends}" VERBATIM 
		)
		list( APPEND ${outfiles} ${outfile} )
	endforeach()
	set( ${outfiles} ${${outfiles}} PARENT_SCOPE )
endfunction()

################################################################################
# CUSTOM_QT5_WRAP_UI
################################################################################
function( CUSTOM_QT5_WRAP_UI outfiles )
	set( options )
	set( oneValueArgs )
	set( multiValueArgs OPTIONS )

	cmake_parse_arguments( _WRAP_UI "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

	set( ui_files ${_WRAP_UI_UNPARSED_ARGUMENTS} )
	set( ui_options ${_WRAP_UI_OPTIONS} )

	foreach( it ${ui_files} )
		get_filename_component( outfile ${it} NAME_WE )
		get_filename_component( infile ${it} ABSOLUTE )
		set( outfile ${CMAKE_CURRENT_BINARY_DIR}/src/ui_${outfile}.h )
		add_custom_command( OUTPUT ${outfile}
			COMMAND ${Qt5Widgets_UIC_EXECUTABLE}
			ARGS ${ui_options} -o ${outfile} ${infile}
			MAIN_DEPENDENCY ${infile} VERBATIM
		)
		list( APPEND ${outfiles} ${outfile} )
	endforeach()
	set( ${outfiles} ${${outfiles}} PARENT_SCOPE )
endfunction()