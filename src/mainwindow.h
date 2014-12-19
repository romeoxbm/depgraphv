/**
 * mainwindow.h
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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#ifndef ABOUTDIALOG_H
#	include "aboutdialog.h"
#endif

#ifndef HANDLEROOTSDIALOG_H
#	include "handlerootsdialog.h"
#endif

#ifndef SELECTFILESDIALOG_H
#	include "selectfilesdialog.h"
#endif

#ifndef PROJECT_H
#	include "project.h"
#endif

namespace depgraphV
{
	namespace Ui
	{
		class MainWindow;
	}

	class SettingsDialog;

	class MainWindow : public QMainWindow, public ISerializableObject
	{
		Q_OBJECT
		Q_PROPERTY( QByteArray windowState READ windowState WRITE setWindowState )
		Q_PROPERTY( QByteArray geometryState READ geometryState WRITE setGeometryState )

	public:
		/**
		 * @brief MainWindow constructor.
		 * @param parent The parent widget (default NULL)
		 */
		explicit MainWindow( QWidget* parent = 0 );

		/**
		 * @brief MainWindow destructor.
		 */
		~MainWindow();

		void show( const QString& fileName = "" );

		virtual QList<const char*> propList() const;

		QByteArray windowState() const;
		QByteArray geometryState() const;

	public slots:
		void setWindowState( const QByteArray& value );
		void setGeometryState( const QByteArray& value );

	signals:
		void projectOpened();
		void projectLoaded();
		void projectClosed();

	protected:
		virtual void changeEvent( QEvent* );
		virtual void closeEvent( QCloseEvent* );

	private slots:
		//File Menu
		void newProject();
		void openProject();
		void saveProject();
		void saveAsProject();
		void closeProject();

		void saveAsDot();
		void saveAsImage();

		void clearRecentDocs();

		void onDraw();
		void onClear();
		void onConfigRestored();

		//Tabwidget slots
		void onCurrentTabChanged( int = -1 );
		void onGraphCountChanged( int count );

		void about();
		void settings();
		void restoreDefaultSettings();

		void checkForUpdates() const;
		void onUpdateReplyProgress( qint64, qint64 );
		void onUpdateReply( QNetworkReply* );

		void onSelectFilesOrFolders();
		void onLanguageActionTriggered( QAction* action );
		void onTranslationFound( QString lang, QString path );

		void onRecentDocumentTriggered();

	private:
		Ui::MainWindow* _ui;
		Project* _project;

		QProgressBar* _progressBar;
		AppConfig* _config;

		QNetworkAccessManager* _netManager;

		//Dialogs
		AboutDialog* _aboutDlg;
		SettingsDialog* _settingsDlg;
		HandleRootsDialog* _rootsDlg;
		SelectFilesDialog* _filesDlg;

		QActionGroup* _langGroup;

		QAction* _actionClearRecentList;
		QAction* _currentRecentDocument;

		bool _imageFiltersUpdated;
		QString _imageFilters;
		QMap<QString, QAction*> _langActions;
		QMap<QString, QString> _imageFiltersByExt;

		void _doSaveProject( bool saveAs );
		void _scanFolder( const QFlags<QDir::Filter>& flags, QStringList* filesList, QFileInfo& dirInfo );
		void _scanFolders() const;
		void _scanFiles( const QStringList& files ) const;
		bool _applyGraphLayout() const;
		void _doClearGraph() const;
		void _onProjectOpened( const QString& statusBarMessage );
		bool _lookForRequiredImageFormats();

		QAction* _newRecentDocument( const QString& filePath );
		void _updateRecentDocumentsList();

		void _updateWindowTitle( bool closingProject = true );

		bool _discardProjectChanges();

		void _startSlowOperation( const QString& message, int maxValue ) const;

		void _showAboutDialog( bool showDonations );

		//Post request data
		static QByteArray _postData();
	};
}

#endif // MAINWINDOW_H
