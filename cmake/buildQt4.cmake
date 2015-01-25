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

include( functions-qt4 )
varHasChanged( DEPGRAPHV_USE_QT5 ${DEPGRAPHV_USE_QT5} "QT_QMAKE_EXECUTABLE" )

if( WIN32 )
	set( QT_INSTALL_DIR "C:/Qt/4.8.6" CACHE PATH "Installation path of Qt4 libraries." )
	set( CMAKE_PREFIX_PATH "${QT_INSTALL_DIR}" )
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
