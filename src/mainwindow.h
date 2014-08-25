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

#include "graph.h"
#include "aboutdialog.h"
#include "settingsdialog.h"
#include "scandialog.h"
#include "handlerootsdialog.h"
#include "selectfilesdialog.h"

#include <QMainWindow>
#include <QActionGroup>

namespace depgraphV
{
	namespace Ui
	{
		class MainWindow;
	}

	class AppConfig;

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

		Graph* graph() const;

		virtual QList<const char*> propList() const;

		QByteArray windowState() const;
		QByteArray geometryState() const;

	public slots:
		void setWindowState( const QByteArray& value );
		void setGeometryState( const QByteArray& value );

	protected:
		virtual void changeEvent( QEvent* );
		virtual void closeEvent( QCloseEvent* );

	private slots:
		void onDraw();
		void onClear();
		void onConfigRestored();
		void changeToolbarOrientation();
		void about();
		void settings();
		void restoreDefaultSettings();
		void rendererTypeChanged( QAction* );
		void parseOptionsChanged();
		void saveAsImage();
		void saveAsDot() const;
		void exitApp();
		void on_actionSelect_Files_triggered();
		void onLanguageActionTriggered( QAction* action );
		void onTranslationFound( QString lang, QString path );

	private:
		Ui::MainWindow* _ui;
		AppConfig* _config;

		//Dialogs
		AboutDialog* _aboutDlg;
		SettingsDialog* _settingsDlg;
		ScanDialog* _scanDlg;
		HandleRootsDialog* _rootsDlg;
		SelectFilesDialog* _filesDlg;

		QActionGroup* _langGroup;

		bool _imageFiltersUpdated;
		QString _imageFilters;
		QMap<QString, QAction*> _langActions;
		QMap<QString, QString> _imageFiltersByExt;

		void _doClearGraph() const;
		void _setGraphAttributes() const;
		void _setGroupBoxesEnabled( bool value ) const;
		void _setButtonsAndActionsEnabled( bool value ) const;
		bool _lookForRequiredImageFormats();

		void _showAboutDialog( bool showDonations );
		QString _ucFirst( const QString& value ) const;
	};
}

#endif // MAINWINDOW_H
