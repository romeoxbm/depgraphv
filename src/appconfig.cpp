/**
 ******************************************************************************
 *                _                                        _
 *             __| | ___ _ __         __ _ _ __ __ _ _ __ | |__/\   /\
 *            / _` |/ _ \ '_ \ _____ / _` | '__/ _` | '_ \| '_ \ \ / /
 *           | (_| |  __/ |_) |_____| (_| | | | (_| | |_) | | | \ V /
 *            \__,_|\___| .__/       \__, |_|  \__,_| .__/|_| |_|\_/
 *                      |_|          |___/          |_|
 *
 ******************************************************************************
 *
 * appconfig.cpp
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
#include "depgraphv_pch.h"
#include "appconfig.h"

namespace depgraphV
{
	AppConfig::AppConfig( QWidget* parent )
		: QObject( parent ),
		  Singleton<AppConfig>(),
		  _settings( APP_VENDOR, APP_NAME ),
		  _language( "en" ),
		  _showDonateOnExit( true ),
		  _warnOnGraphRemoval( true )
	{
		registerSerializable( this );
		_availableTranslations.insert( "en", "" );
	}
	//-------------------------------------------------------------------------
	AppConfig::~AppConfig()
	{
	}
	//-------------------------------------------------------------------------
	void AppConfig::save()
	{
		_doSaveRestore( true, false );
	}
	//-------------------------------------------------------------------------
	void AppConfig::restore()
	{
		_doSaveRestore( false, false );
	}
	//-------------------------------------------------------------------------
	void AppConfig::saveDefault()
	{
		if( _settings.childGroups().contains( "default" ) )
			return;

		//Move the main window to its default position (center of the screen)
		//before saving default settings
		QMainWindow* w = static_cast<QMainWindow*>( this->parent() );
		QPoint desktopCenter( QApplication::desktop()->screenGeometry().center() );
		w->move( desktopCenter - w->rect().center() );

		_doSaveRestore( true, true );
	}
	//-------------------------------------------------------------------------
	void AppConfig::restoreDefault()
	{
		if( !_settings.childGroups().contains( "default" ) )
		{
			qCritical() << qPrintable( tr( "Missing default settings" ) );
			return;
		}

		_doSaveRestore( false, true );
	}
	//-------------------------------------------------------------------------
	const QString& AppConfig::installPrefix()
	{
		if( _instPrefix.isEmpty() )
		{
#ifdef WIN32
			QString regKey = QString(
								 "HKEY_LOCAL_MACHINE\\Software\\Microsoft"
								 "\\Windows\\CurrentVersion\\Uninstall\\%1"
			).arg( APP_NAME );
			QSettings s( regKey, QSettings::NativeFormat );
			QFileInfo info( s.value( "UninstallString" ).toString() );
			_instPrefix = info.absolutePath();
#else
#	ifndef NDEBUG
			_instPrefix = ".";
#	else
			QFileInfo f( QApplication::applicationDirPath() );
			_instPrefix = f.absolutePath();
			_instPrefix.replace( APP_BIN_PATH, "" );
#	endif //NDEBUG
#endif
		}

		return _instPrefix;
	}
	//-------------------------------------------------------------------------
	const QString& AppConfig::translationsPath()
	{
		if( _trPath.isEmpty() )
			_trPath = QString( "%1/%2" ).arg( installPrefix(), APP_TR_PATH );

		return _trPath;
	}
	//-------------------------------------------------------------------------
	void AppConfig::lookForTranslations()
	{
		_lookForTranslationsByPath( QApplication::applicationDirPath() );
		_lookForTranslationsByPath( this->translationsPath() );
	}
	//-------------------------------------------------------------------------
	void AppConfig::registerSerializable( ISerializableObject* obj )
	{
		if( !_serializableObjects.contains( obj ) )
			_serializableObjects << obj;
		else
			qWarning() << qPrintable( tr( "Object already registered." ) );
	}
	//-------------------------------------------------------------------------
	QList<const char*> AppConfig::propList() const
	{
		QList<const char*> props;
		props 	 << "recentDocuments"
				 << "language"
				 << "showDonateOnExit"
				 << "warnOnGraphRemoval";

		return props;
	}
	//-------------------------------------------------------------------------
	void AppConfig::setLanguage( const QString& value )
	{
		if( _language == value || !_availableTranslations.contains( value ) )
			return;

		_language = value;

		QString appFileName = QString( "%1_%2.qm" ).arg( APP_NAME, _language );
		QString qtFileName = QString( "qt_%1.qm" ).arg( _language );

		//Changing translators
		_switchTranslator(
					&_appTranslator,
					appFileName,
					_availableTranslations[ _language ],
					_language == "en"
		);

		_switchTranslator(
					&_qtTranslator,
					qtFileName,
					QLibraryInfo::location( QLibraryInfo::TranslationsPath )
		);

		//Changing locale
		QLocale::setDefault( QLocale( _language ) );
	}
	//-------------------------------------------------------------------------
	void AppConfig::_doSaveRestore( bool save, bool def )
	{
		QString group;
		QString message;
		if( def )
		{
			group = "default/";
			if( save )
				message = tr( "Saving default settings..." );
			else
				message = tr( "Restoring default settings..." );
		}
		else
		{
			group = "current/";
			if( save )
				message = tr( "Saving settings..." );
			else
				message = tr( "Restoring settings..." );
		}
		qDebug() << qPrintable( message );

		this->blockSignals( true );
		foreach( ISerializableObject* sObj, _serializableObjects )
		{
			QObject* obj = dynamic_cast<QObject*>( sObj );
			if( !obj )
				continue;

			const QMetaObject* metaObj = obj->metaObject();
			QString className = metaObj->className();

			_settings.beginGroup( group + className );
			foreach( const char* propName, sObj->propList() )
			{
				int propIdx = metaObj->indexOfProperty( propName );
				if( propIdx == -1 )
				{
					qDebug() << qPrintable(
						tr( "Unable to find property \"" ) + propName +
						tr( "\". Please review propList implementation for \"" )
						+ className + "\""
					);
					continue;
				}

				QMetaProperty p = metaObj->property( propIdx );
				QString key = QString( "%1_%2" ).arg( obj->objectName(), p.name() );
				QVariant qv = save ? p.read( obj ) : _settings.value( key );

				if( qv.isNull() || !qv.isValid() )
				{
					qDebug() << qPrintable( tr( "Skipping property \"" )
											+ className + "::" + p.name() +
											tr( "\" because has either null or invalid value." )
					);
					continue;
				}

				if( save )
					_settings.setValue( key, qv );
				else
				{
					if( p.isWritable() )
						p.write( obj, p.isEnumType() ? qv.toInt() : qv );
					else
					{
						qDebug() << qPrintable(
							tr( "Property \"" ) + className + "::" + p.name() +
							tr( "\" doesn't use the WRITE keyword. "
								"Please check its Q_PROPERTY declaration." )
						);
					}
				}
			}
			_settings.endGroup();
		} //foreach ISerializableObject*
		this->blockSignals( false );

		if( save )
			emit configSaved();
		else
			emit configRestored();
	}
	//-------------------------------------------------------------------------
	void AppConfig::_lookForTranslationsByPath( const QString& path )
	{
		QDir dir( path );
		QString appName( QApplication::applicationName() );
		dir.setNameFilters( QStringList( appName + "*.qm" ) );
		foreach( QFileInfo entry, dir.entryInfoList( QDir::NoDotAndDotDot | QDir::Files ) )
		{
			int startPos = appName.length() + 1;
			QString translation( entry.fileName().mid( startPos, 2 ) );

			if( _availableTranslations.contains( translation ) )
				continue;

			_availableTranslations.insert( translation, path );
			qDebug() << qPrintable(
							tr( "Found translation \"%1\" in %2" ).arg( translation, path )
			);
			emit translationFound( translation, path );
		}
	}
	//-------------------------------------------------------------------------
	void AppConfig::_switchTranslator( QTranslator* t, const QString& fileName,
										const QString& directory, bool justRemoveTranslator )
	{
		//remove the old one
		QApplication::removeTranslator( t );

		if( justRemoveTranslator )
			return;

		QString fName = QString( "%1/%2" ).arg( directory, fileName );
		QFile file( fName );
		if( !file.exists() )
		{
			qDebug() << qPrintable( tr( "Translation \"%1\" does not exists" ).arg( fName ) );
			return;
		}

		//and then..load the new one
		if( t->load( fileName, directory ) )
		{
			QApplication::installTranslator( t );
			qDebug() << qPrintable(
							tr( "Now using translation \"%1\" (%2)" ).arg( fileName, directory )
			);
		}
	}
} // end of depgraphV namespace
