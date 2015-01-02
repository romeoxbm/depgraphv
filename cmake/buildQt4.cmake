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
# buildQt4.cmake
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
if( NOT DEFINED qt4Modules )
	set( qt4Modules "QtCore" )
endif()

macro( CUSTOM_QT4_ADD_RESOURCES outfiles )
	QT4_EXTRACT_OPTIONS( rcc_files rcc_options rcc_target ${ARGN} )

	foreach( it ${rcc_files} )
		get_filename_component( outfilename ${it} NAME_WE )
		get_filename_component( infile ${it} ABSOLUTE )
		get_filename_component( rc_path ${infile} PATH )
		set( outfile ${CMAKE_CURRENT_BINARY_DIR}/src/qrc_${outfilename}.cxx )

		set( _RC_DEPENDS)
		if( EXISTS "${infile}" )
			#  parse file for dependencies
			#  all files are absolute paths or relative to the location of the qrc file
			file( READ "${infile}" _RC_FILE_CONTENTS )
			string( REGEX MATCHALL "<file[^<]+" _RC_FILES "${_RC_FILE_CONTENTS}" )
			foreach( _RC_FILE ${_RC_FILES} )
				string( REGEX REPLACE "^<file[^>]*>" "" _RC_FILE "${_RC_FILE}" )
				if( NOT IS_ABSOLUTE "${_RC_FILE}" )
					set( _RC_FILE "${rc_path}/${_RC_FILE}" )
				endif()
				set( _RC_DEPENDS ${_RC_DEPENDS} "${_RC_FILE}" )
			endforeach()
			unset( _RC_FILES )
			unset( _RC_FILE_CONTENTS )
			# Since this cmake macro is doing the dependency scanning for these files,
			# let's make a configured file and add it as a dependency so cmake is run
			# again when dependencies need to be recomputed.
			QT4_MAKE_OUTPUT_FILE( "${infile}" "" "qrc.depends" out_depends )
			configure_file( "${infile}" "${out_depends}" COPY_ONLY )
		else()
			# The .qrc file does not exist (yet). Let's add a dependency and hope
			# that it will be generated later
			set( out_depends )
		endif()

		add_custom_command( OUTPUT ${outfile}
			COMMAND ${CMAKE_COMMAND}
			ARGS -E echo "#include \"depgraphv_pch.h\"" > ${outfile}
			COMMAND ${QT_RCC_EXECUTABLE}
			ARGS ${rcc_options} -name ${outfilename} ${infile} >> ${outfile}
			MAIN_DEPENDENCY ${infile}
			DEPENDS ${_RC_DEPENDS} "${out_depends}" VERBATIM
		)
		set( ${outfiles} ${${outfiles}} ${outfile} )
	endforeach()
endmacro()

macro( CUSTOM_QT4_WRAP_UI outfiles )
	QT4_EXTRACT_OPTIONS( ui_files ui_options ui_target ${ARGN} )

	foreach( it ${ui_files} )
		get_filename_component(outfile ${it} NAME_WE)
		get_filename_component(infile ${it} ABSOLUTE)
		set( outfile ${CMAKE_CURRENT_BINARY_DIR}/src/ui_${outfile}.h )
		add_custom_command( OUTPUT ${outfile}
			COMMAND ${QT_UIC_EXECUTABLE}
			ARGS ${ui_options} -o ${outfile} ${infile}
			MAIN_DEPENDENCY ${infile} VERBATIM
		)
		set( ${outfiles} ${${outfiles}} ${outfile} )
	endforeach()
endmacro()

macro( CUSTOM_QT4_WRAP_CPP outfiles )
	# get include dirs
	QT4_GET_MOC_FLAGS( moc_flags )
	QT4_EXTRACT_OPTIONS( moc_files moc_options moc_target ${ARGN} )

	foreach( it ${moc_files} )
		get_filename_component( it ${it} ABSOLUTE )
		QT4_MAKE_OUTPUT_FILE( ${it} moc_ cxx outfile )
		#message( STATUS "${it} ${outfile} ${moc_flags} ${moc_options} ${moc_target}" )
		#CUSTOM_QT4_CREATE_MOC_COMMAND( ${it} ${outfile} "${moc_flags}" "${moc_options}" "${moc_target}" )
		add_custom_command( OUTPUT ${outfile}
			COMMAND ${CMAKE_COMMAND}
			ARGS -E echo "#include \"depgraphv_pch.h\"" > ${outfile}
			COMMAND ${QT_MOC_EXECUTABLE}
			ARGS ${moc_flags} ${moc_options} ${moc_target} ${it} >> ${outfile}
			DEPENDS ${it} VERBATIM
		)
		set( ${outfiles} ${${outfiles}} ${outfile} )
	endforeach()
endmacro ()

if( WIN32 )
	set( QT_INSTALL_PATH "C:/Qt/4.8.6" CACHE PATH "Installation path of Qt libraries." FORCE )
	set( CMAKE_PREFIX_PATH "${QT_INSTALL_PATH}" )
endif( WIN32 )

#Looking for Qt4 modules
find_package( Qt4 REQUIRED ${qt4Modules} )
if( NOT Qt4_FOUND )
	message( SEND_ERROR "* Failed to find Qt4." )
else()
	message( STATUS "* Found ${QTVERSION}" )
endif()

include( ${QT_USE_FILE} )

if( DEFINED Sources_moc )
        custom_qt4_wrap_cpp( Mocs ${Sources_moc} )
endif()

if( DEFINED Sources_ui )
	custom_qt4_wrap_ui( UISrcs ${Sources_ui} )
endif()

if( DEFINED Resources )
	# generate rules for building source files from the resources
	if( ${DEPGRAPHV_ENABLE_PCH_SUPPORT} )
		custom_qt4_add_resources( RCCSrcs ${Resources} )
	else()
		qt4_add_resources( RCCSrcs ${Resources} )
	endif()
endif()

foreach( t ${AvailableLanguages} )
    if( DEPGRAPHV_UPDATE_QT_TRANSLATIONS )
	    qt4_create_translation( i18nSrcs ${FilesToTranslate} ${i18n_${t}} )
    else()
	    qt4_add_translation( i18nSrcs ${i18n_${t}} )
    endif( DEPGRAPHV_UPDATE_QT_TRANSLATIONS )
endforeach()

list( APPEND includes ${QT_INCLUDES} )
list( APPEND libraries ${QT_LIBRARIES} )
