# - Enable precompiled headers support for MSVC and for GCC (3.4 and 4.x)
# Once done this will define:
#
# Variable:
#   PCHSupport_FOUND
#
# Macro:
#   ADD_PRECOMPILED_HEADER

IF( CMAKE_COMPILER_IS_GNUCXX )

	execute_process( COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION )
	if( GCC_VERSION VERSION_GREATER 3.4 )
		SET( PCHSupport_FOUND TRUE )
	else()
		SET( PCHSupport_FOUND FALSE )
	endif()

elseif( MSVC_IDE )

	SET( PCHSupport_FOUND TRUE )

endif( CMAKE_COMPILER_IS_GNUCXX )

MACRO( ADD_PRECOMPILED_HEADER _targetName _input _src_file )

	get_filename_component( _name ${_input} NAME )

	if( MSVC_IDE )

		add_definitions( /Yu"${_name}" )
		add_definitions( /Zm${DEPGRAPHV_PCH_FACTOR} )
		set_source_files_properties( ${_src_file} PROPERTIES COMPILE_FLAGS /Yc"${_name}" )

	elseif( CMAKE_COMPILER_IS_GNUCXX )

		SET( _outdir "${CMAKE_CURRENT_BINARY_DIR}/${_name}.gch" )
		MAKE_DIRECTORY( ${_outdir} )
		SET( _output "${_outdir}/${CMAKE_BUILD_TYPE}.c++" )
		STRING( TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" _flags_var_name )
		SET( _compiler_FLAGS ${${_flags_var_name}} )
		
		GET_DIRECTORY_PROPERTY( _directory_flags INCLUDE_DIRECTORIES )
		FOREACH( item ${_directory_flags} )
			LIST( APPEND _compiler_FLAGS "-I${item}" )
		ENDFOREACH( item )

		GET_DIRECTORY_PROPERTY( _directory_flags DEFINITIONS )
		LIST( APPEND _compiler_FLAGS ${_directory_flags} )

		SEPARATE_ARGUMENTS( _compiler_FLAGS )
		#MESSAGE("_compiler_FLAGS: ${_compiler_FLAGS}")
		#message( "${CMAKE_CXX_COMPILER} ${_compiler_FLAGS} -x c++-header -o ${_output} ${_input}" )
		ADD_CUSTOM_COMMAND(
			OUTPUT ${_output}
			COMMAND ${CMAKE_CXX_COMPILER}
					${_compiler_FLAGS}
					-x c++-header
					-o ${_output} ${_input}
			DEPENDS ${_input}
		)
		ADD_CUSTOM_TARGET( ${_targetName}_gch DEPENDS ${_output} )
		ADD_DEPENDENCIES( ${_targetName} ${_targetName}_gch )
		#SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-include ${_name} -Winvalid-pch -H")
		#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -include ${_name} -Winvalid-pch")
		SET_TARGET_PROPERTIES( ${_targetName} PROPERTIES
			# -Winvalid-pch" this has been removed due to lot of warning messages when building shared libraries..
			COMPILE_FLAGS "-include ${_name}"
		)
	
	endif()
	
ENDMACRO( ADD_PRECOMPILED_HEADER )
