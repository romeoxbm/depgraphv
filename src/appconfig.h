/**
 * appconfig.h
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
#ifndef APPCONFIG_H
#define APPCONFIG_H

#include "iserializableobject.h"
#include "singleton.h"
#include "memento.h"
#include "buildsettings.h"
#include <QSettings>
#include <QTranslator>
#ifndef QT_USE_QT5
#	include <QStringList>
#endif // QT_USE_QT5

namespace depgraphV
{
	/**
	 * @brief The AppConfig class
	 */
	class AppConfig : public QObject, public Singleton<AppConfig>, public ISerializableObject
	{
		Q_OBJECT
		Q_PROPERTY( QString language READ language WRITE setLanguage )
		Q_PROPERTY( bool scanByFolders READ scanByFolders WRITE setScanByFolders )
		Q_PROPERTY( bool isRecursiveScanEnabled READ isRecursiveScanEnabled WRITE setRecursiveScanEnabled )
		Q_PROPERTY( bool hiddenFoldersIncluded READ hiddenFoldersIncluded WRITE setHiddenFoldersIncluded )
		Q_PROPERTY( QStringList selectedFolders READ selectedFolders WRITE setSelectedFolders )
		Q_PROPERTY( bool showDonateOnExit READ showDonateOnExit WRITE setShowDonateOnExit )

		//Header Filters
		Q_PROPERTY( bool hdr_parseEnabled READ hdr_parseEnabled WRITE hdr_setParseEnabled )
		Q_PROPERTY( bool hdr_standardFiltersEnabled READ hdr_standardFiltersEnabled WRITE hdr_setStandardFiltersEnabled )
		Q_PROPERTY( QString hdr_currentStandardFilter READ hdr_currentStandardFilter WRITE hdr_setCurrentStandardFilter )
		Q_PROPERTY( QString hdr_customFilters READ hdr_customFilters WRITE hdr_setCustomFilters )

		//Source Filters
		Q_PROPERTY( bool src_parseEnabled READ src_parseEnabled WRITE src_setParseEnabled )
		Q_PROPERTY( bool src_standardFiltersEnabled READ src_standardFiltersEnabled WRITE src_setStandardFiltersEnabled )
		Q_PROPERTY( QString src_currentStandardFilter READ src_currentStandardFilter WRITE src_setCurrentStandardFilter )
		Q_PROPERTY( QString src_customFilters READ src_customFilters WRITE src_setCustomFilters )

	public:
		/**
		 * @brief AppConfig constructor.
		 */
		explicit AppConfig( QWidget* parent = 0 );

		/**
		 * @brief AppConfig destructor.
		 */
		~AppConfig();

		/**
		 * @brief Save currently in use settings.
		 */
		void save();

		/**
		 * @brief Restore previously saved settings.
		 */
		void restore();

		/**
		 * @brief Save default settings, only the first time this application run, so when there's
		 *					  still no default settings saved.
		 */
		void saveDefault();

		/**
		 * @brief Restore default settings.
		 */
		void restoreDefault();

		void registerSerializable( ISerializableObject* obj );

		virtual QList<const char*> propList() const;

		/**
		 * @brief Return the installation prefix.
		 * @return On Linux, when building with Debug symbols, it returns ".", otherwise the installation prefix.
		 */
		const QString& installPrefix();

		/**
		 * @brief Return the path where translation files are being installed.
		 */
		const QString& translationsPath();

		void lookForTranslations();

		/**
		 * @return Return a string containing the language currently in use (for instance, "en").
		 */
		const QString& language() const { return _language; }

		bool scanByFolders() const{ return _scanByFolders; }

		QStringList& selectedFolders() const { return _selectedFolders->state(); }
		Memento<QStringList>* selectedFoldersMemento() const { return _selectedFolders; }

		/**
		 * @return True if recursive scan is enabled, false otherwise.
		 */
		bool isRecursiveScanEnabled() const { return _recursiveScan; }

		/**
		 * @brief hiddenFoldersIncluded
		 * @return
		 */
		bool hiddenFoldersIncluded() const { return _hiddenFolders; }

		/**
		 * @return True if the donation tab page will be shown when closing this application, false otherwise.
		 */
		bool showDonateOnExit() const { return _showDonateOnExit; }

		//Header Filters
		bool hdr_parseEnabled() const						{ return _hdrParseEnabled; }
		bool hdr_standardFiltersEnabled() const				{ return _hdrStandardFiltersEnabled; }
		const QString& hdr_currentStandardFilter() const	{ return _hdrCurrentStandardFilter; }
		const QString& hdr_customFilters() const			{ return _hdrCustomFilters; }

		const QStringList& headerNameFilters();

		//Source Filters
		bool src_parseEnabled() const						{ return _srcParseEnabled; }
		bool src_standardFiltersEnabled() const				{ return _srcStandardFiltersEnabled; }
		const QString& src_currentStandardFilter() const	{ return _srcCurrentStandardFilter; }
		const QString& src_customFilters() const			{ return _srcCustomFilters; }

		const QStringList& sourceNameFilters();

	public slots:
		void setLanguage( const QString& value );

		void setScanByFolders( bool value ) { _scanByFolders = value; }

		void setSelectedFolders( const QStringList& folders );

		/**
		 * @param Set recursive scan option checked state to "value".
		 */
		void setRecursiveScanEnabled( bool value ) { _recursiveScan = value; }

		/**
		 * @brief setHiddenFoldersIncluded
		 * @param value
		 */
		void setHiddenFoldersIncluded( bool value ) { _hiddenFolders = value; }

		/**
		 * @brief This method change the on-closing policy concerning donation tab page visibility.
		 * @param value The new "visibility" value used by _showDonateOnExit field.
		 */
		void setShowDonateOnExit( bool value ) { _showDonateOnExit = value; }

		//Header Filters
		void hdr_setParseEnabled( bool value );
		void hdr_setStandardFiltersEnabled( bool value );
		void hdr_setCurrentStandardFilter( const QString& value );
		void hdr_setCustomFilters( const QString& value );

		//Source Filters
		void src_setParseEnabled( bool value );
		void src_setStandardFiltersEnabled( bool value );
		void src_setCurrentStandardFilter( const QString& value );
		void src_setCustomFilters( const QString& value );

	signals:
		void configSaved();
		void configRestored();

		void translationFound( QString lang, QString path );

	private:
		QSettings _settings;
		QList<ISerializableObject*> _serializableObjects;

		/**
		 * @brief It does the save/restore job.
		 * @param save True when saving, false when restoring.
		 * @param def True if saving default settings, false if saving current settings.
		 */
		void _doSaveRestore( bool save, bool def );

		void _lookForTranslationsByPath( const QString& path );
		void _switchTranslator( QTranslator* t, const QString& fileName,
								const QString& directory = "", bool justRemoveTranslator = false );

		QString _instPrefix;
		QString _trPath;

		QString _language;
		QTranslator _qtTranslator;
		QTranslator _appTranslator;

		//lang, path
		QMap<QString, QString> _availableTranslations;

		bool _scanByFolders;
		Memento<QStringList>* _selectedFolders;
		bool _recursiveScan;
		bool _hiddenFolders;
		bool _showDonateOnExit;

		//Header Filters
		bool _hdrParseEnabled;
		bool _hdrStandardFiltersEnabled;
		QString _hdrCurrentStandardFilter;
		QString _hdrCustomFilters;

		//Source Filters
		bool _srcParseEnabled;
		bool _srcStandardFiltersEnabled;
		QString _srcCurrentStandardFilter;
		QString _srcCustomFilters;

		//Name filters
		QStringList _hdrNameFilters;
		bool _hdrNameFiltersDirty;

		QStringList _srcNameFilters;
		bool _srcNameFiltersDirty;
	};
}

#endif // APPCONFIG_H
