# - Enable precompiled headers support for MSVC and for GCC (3.4 and 4.x)
# Once done this will define:
#
# Variable:
#   PCHSupport_FOUND
#
# Macro:
#   ADD_PRECOMPILED_HEADER

IF( CMAKE_COMPILER_IS_GNUCXX )
	#Check for gcc version
	execute_process( COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION )
	if( GCC_VERSION VERSION_GREATER 3.4 )
		set( PCHSupport_FOUND TRUE )
	else()
		set( PCHSupport_FOUND FALSE )
	endif()

elseif( MSVC_IDE )

	set( PCHSupport_FOUND TRUE )

endif( CMAKE_COMPILER_IS_GNUCXX )

macro( ADD_PRECOMPILED_HEADER _targetName _input _src_file )

	get_filename_component( _name ${_input} NAME )

	if( MSVC_IDE )

		add_definitions( /Yu"${_name}" )
		add_definitions( /FI"${_name}" )
		add_definitions( /Zm${DEPGRAPHV_PCH_FACTOR} )
		set_source_files_properties( ${_src_file} PROPERTIES COMPILE_FLAGS /Yc"${_name}" )

	elseif( CMAKE_COMPILER_IS_GNUCXX )

		set( _outdir "${CMAKE_CURRENT_BINARY_DIR}/${_name}.gch" )
		make_directory( ${_outdir} )
		set( _output "${_outdir}/${CMAKE_BUILD_TYPE}.c++" )
		string( TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" _flags_var_name )
		set( _compiler_FLAGS ${${_flags_var_name}} )

		get_directory_property( _directory_flags INCLUDE_DIRECTORIES )
		foreach( item ${_directory_flags} )
			list( APPEND _compiler_FLAGS "-I${item}" )
		endforeach( item )

		get_directory_property( _directory_flags DEFINITIONS )
		list( APPEND _compiler_FLAGS ${_directory_flags} )

		separate_arguments( _compiler_FLAGS )
		add_custom_command(
			OUTPUT ${_output}
			COMMAND ${CMAKE_CXX_COMPILER}
					${_compiler_FLAGS}
					-x c++-header
					-o ${_output} ${_input}
			DEPENDS ${_input}
		)

		add_custom_target( ${_targetName}_gch DEPENDS ${_output} )
		add_dependencies( ${_targetName} ${_targetName}_gch )
		set_target_properties( ${_targetName} PROPERTIES
			COMPILE_FLAGS "-include ${_name} -Winvalid-pch"
		)

	endif()

endmacro( ADD_PRECOMPILED_HEADER )
