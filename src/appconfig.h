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
 * appconfig.h
 *
 * This source file is part of dep-graphV - An useful tool to analize header
 * dependendencies via graphs.
 *
 * This software is distributed under the MIT License:
 *
 * Copyright (c) 2013 - 2015 Francesco Guastella aka romeoxbm
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

#ifndef ISERIALIZABLEOBJECT_H
#	include "iserializableobject.h"
#endif

#ifndef SINGLETON_H
#	include "singleton.h"
#endif

#ifndef GRAPH_H
#	include "graph.h"
#endif

namespace depgraphV
{
	/**
	 * @brief The AppConfig class
	 */
	class AppConfig : public QObject, public Singleton<AppConfig>, public ISerializableObject
	{
		Q_OBJECT
		Q_PROPERTY( QStringList recentDocuments READ recentDocuments WRITE setRecentDocuments )
		Q_PROPERTY( QString language READ language WRITE setLanguage )
		Q_PROPERTY( bool showDonateOnExit READ showDonateOnExit WRITE setShowDonateOnExit )
		Q_PROPERTY( bool warnOnGraphRemoval READ warnOnGraphRemoval WRITE setWarnOnGraphRemoval )
		Q_PROPERTY( bool warnOnGraphClearing READ warnOnGraphClearing WRITE setWarnOnGraphClearing )
		Q_PROPERTY( bool autoApplySettingChanges READ autoApplySettingChanges WRITE setAutoApplySettingChanges )

	public:
		/**
		 * @brief \a AppConfig constructor.
		 * @param parent The parent widget (default \a NULL)
		 */
		explicit AppConfig( QWidget* parent = 0 );

		/**
		 * @brief \a AppConfig destructor.
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
		 * @brief Save default settings, only the first time this application
		 * run, so when there's still no default settings saved.
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
		 * @return On Linux, when building with Debug symbols, it returns ".",
		 * otherwise the installation prefix.
		 */
		const QString& installPrefix();

		/**
		 * @brief Return the path where translation files are being installed.
		 */
		const QString& translationsPath();

		void lookForTranslations();

		const QStringList& recentDocuments() const { return _recentDocs; }

		/**
		 * @return Return a string containing the language currently in use
		 * (for instance, "en").
		 */
		const QString& language() const { return _language; }

		/**
		 * @return True if the donation tab page will be shown when closing this
		 * application, false otherwise.
		 */
		bool showDonateOnExit() const { return _showDonateOnExit; }

		bool warnOnGraphRemoval() const { return _warnOnGraphRemoval; }

		bool warnOnGraphClearing() const { return _warnOnGraphClearing; }

		bool autoApplySettingChanges() const { return _autoApplySettingChanges; }

	public slots:
		void setRecentDocuments( const QStringList& value ) { _recentDocs = value; }
		void setLanguage( const QString& value );

		/**
		 * @brief This method change the on-closing policy concerning donation
		 * tab page visibility.
		 * @param value The new "visibility" value used by _showDonateOnExit field.
		 */
		void setShowDonateOnExit( bool value ) { _showDonateOnExit = value; }

		void setWarnOnGraphRemoval( bool value ) { _warnOnGraphRemoval = value; }

		void setWarnOnGraphClearing( bool value ) { _warnOnGraphClearing = value; }

		void setAutoApplySettingChanges( bool value ) { _autoApplySettingChanges = value; }

	signals:
		void configSaved();
		void configRestored();
		void translationFound( QString lang, QString path );

	private:
		/**
		 * @brief It does the save/restore job.
		 * @param save True when saving, false when restoring.
		 * @param def True if saving default settings, false if saving current
		 * settings.
		 */
		void _doSaveRestore( bool save, bool def );

		void _lookForTranslationsByPath( const QString& path );
		void _switchTranslator( QTranslator* t, const QString& fileName,
								const QString& directory = "",
								bool justRemoveTranslator = false );

		QSettings _settings;
		QList<ISerializableObject*> _serializableObjects;

		QString _instPrefix;
		QString _trPath;

		QStringList _recentDocs;

		QString _language;
		QTranslator _qtTranslator;
		QTranslator _appTranslator;

		//lang, path
		QMap<QString, QString> _availableTranslations;

		bool _showDonateOnExit;
		bool _warnOnGraphRemoval;
		bool _warnOnGraphClearing;
		bool _autoApplySettingChanges;
	};
}

#endif // APPCONFIG_H
