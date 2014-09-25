/**
 * main.cpp
 *
 * This source file is part of dep-graphV - An useful tool to analize header
 * dependendencies via graphs.
 *
 * This software is distributed under the MIT License:
 *
 * Copyright (c) 2013 - 2014 Francesco Guastella aka romeoxbm
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "mainwindow.h"
#include "buildsettings.h"
#include <QApplication>
#include <stdio.h>

#ifdef WIN32
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#endif

namespace depgraphV
{
#ifdef WIN32
	/**
	 * @brief toggleConsole function allocate/free a log console. Only available on Windows.
	 * @param enabled True when a console allocation is required, false when the already allocated console should be freed.
	 */
	void toggleConsole( bool enabled );
#endif

	/**
	 * @brief Print the application version and quit.
	 */
	void printVersion()
	{
#ifdef WIN32
		toggleConsole( true );
#endif
		printf( "%s version %s\n", APP_NAME, APP_VER );
	}

	/**
	 * @brief This function prints out an help message showing all the command line arguments available and their usage.
	 */
	void printHelp()
	{
		printVersion();
		printf( "Usage\n\n" );
		printf( "\t%s [options]\n\n", APP_NAME );
		printf( "Options\n" );
		printf( "\t-h (--help) \t\t= Print this help message and quit.\n" );
		printf( "\t-V (--version) \t\t= Print %s version and quit.\n", APP_NAME );
		printf( "\t-l (--with-log)\t\t= Enable log messages (disabled by default).\n" );
	}

	/**
	 * @brief This function is being used when it is necessary to disable log messages sent by this application
	 *		  via qDebug(), qWarning(), etc.
	 */
#ifdef DEPGRAPHV_USE_QT5
	void noMessageOutput( QtMsgType, const QMessageLogContext&, const QString& ) {}
#else
	void noMessageOutput( QtMsgType, const char* ) {}
#endif

#ifdef WIN32
	void toggleConsole( bool enabled )
	{
		bool initially = GetConsoleWindow() != NULL;
		bool success = true;

		if( enabled )
		{
			if( initially )
				return;

			if( AllocConsole() )
			{
				SetConsoleTitle( APP_NAME );
				//TODO WARNING C4996 here (freopen may be unsafe)
				freopen( "CONOUT$", "w", stdout );
				freopen( "CONOUT$", "w", stderr );
			}
			else
				success = false;
		}
		else if( initially )
		{
			system( "pause" );
			success = FreeConsole();
		}

		if( !success )
			throw new std::exception();
	}

	/**
	 * @brief Function called when quitting this application.
	 */
	void _atExitFunc()
	{
		toggleConsole( false );
	}
} //end of depgraphV namespace

INT WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, INT )
{
	atexit( depgraphV::_atExitFunc );
	QApplication app( __argc, __argv );
#else
} //end of depgraphV namespace

int main( int argc, char* argv[] )
{
	QApplication app( argc, argv );

#ifndef DEPGRAPHV_USE_QT5
	//Required to show menu icons with Qt4
	app.setAttribute( Qt::AA_DontShowIconsInMenus, false );
#endif

#endif

	bool logEnabled = false;
	if( app.arguments().count() > 1 )
	{
		//First of all, check for valid option..
		QStringList validOptions;
		validOptions << "-h" << "--help" << "-V" << "--version" << "-l" << "--with-log";
		for( unsigned short i = 1; i < app.arguments().count(); i++ )
		{
			QString current = app.arguments()[ i ];
			if( !validOptions.contains( current ) )
			{
				//Wrong option. Print help and quit
				printf( "WRONG USAGE: Unknown option \"%s\"\n\n", current.toStdString().c_str() );
				depgraphV::printHelp();
				return 0;
			}

			if( current == "--version" || current == "-V" )
			{
				depgraphV::printVersion();
				return 0;
			}

			else if( current == "--help" || current == "-h" )
			{
				depgraphV::printHelp();
				return 0;
			}

			else if( current == "--with-log" || current == "-l" )
				logEnabled = true;
		}
	}

	if( logEnabled )
	{
#ifdef WIN32
		depgraphV::toggleConsole( true );
#endif
	}
	else
	{
#ifdef DEPGRAPHV_USE_QT5
		qInstallMessageHandler( &depgraphV::noMessageOutput );
#else
		qInstallMsgHandler( &depgraphV::noMessageOutput );
#endif
	}

	app.setApplicationName( APP_NAME );
	app.setApplicationVersion( APP_VER );

	depgraphV::MainWindow w;
	w.show();

	return app.exec();
}
