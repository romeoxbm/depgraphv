#####################################################################################
# buildQt4.cmake
#
# This file is part of dep-graphV - An useful tool to analize header
# dependendencies via graphs.
#
# This software is distributed under the MIT License:
#
# Copyright (c) 2013 - 2014 Francesco Guastella aka romeoxbm
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
if( NOT DEFINED qt4Modules )
	set( qt4Modules "QtCore" )
endif()

if( WIN32 )
	set( QT_INSTALL_PATH "C:/Qt/4.8.5" CACHE PATH "Installation path of Qt libraries." FORCE )
	set( CMAKE_PREFIX_PATH "${QT_INSTALL_PATH}" )
	endif()
endif( WIN32 )

#Looking for Qt4 modules
find_package( Qt4 REQUIRED ${qt4Modules} )
if( NOT Qt4_FOUND )
	message( SEND_ERROR "* Failed to find Qt4." )
else()
	message( STATUS "* Found ${QTVERSION}" )
endif()

include( ${QT_USE_FILE} )

qt4_wrap_ui( UISrcs ${Sources_ui} )

# generate rules for building source files from the resources
qt4_add_resources( RCCSrcs ${Resources} )

foreach( t ${AvailableLanguages} )
    if( QT_UPDATE_TRANSLATIONS )
	    qt4_create_translation( i18nSrcs ${FilesToTranslate} ${i18n_${t}} )
    else()
	    qt4_add_translation( i18nSrcs ${i18n_${t}} )
    endif( QT_UPDATE_TRANSLATIONS )
endforeach()

set( includes "${CMAKE_SOURCE_DIR}/src;${GraphViz_INCLUDE_DIRS};${QT_INCLUDES}" )
set( libraries "${GraphViz_gvc_LIBRARY};${QT_LIBRARIES}" )
