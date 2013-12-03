/**
 * mainwindow.h
 *
 * This source file is part of dep-graphV - An useful tool to analize header
 * dependendencies via graphs.
 *
 * This software is distributed under the MIT License:
 *
 * Copyright (c) 2013 Francesco Guastella aka romeoxbm
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

#include <QMainWindow>
#include <QActionGroup>
#include <QTranslator>
#include "graph.h"
#include "aboutdialog.h"

namespace depgraphV
{
	namespace Ui
	{
		class MainWindow;
	}

	class AppConfig;

	class MainWindow : public QMainWindow
	{
		Q_OBJECT
		
	public:
		explicit MainWindow( QWidget* parent = 0 );
		~MainWindow();

		void translateUi( const QString& locale );

		QString rootPath() const;
		void setRootPath( const QString& value );

		bool isRecursiveScanEnabled() const;
		void setRecursiveScanEnabled( bool value );

		const QVariant& selectedLocaleData() const;

		void setRendererActionCheckedByType( Graph::RendererType type, bool checked );

		void setHighQualityAAChecked( bool value );

		void headersInfo( bool* parseHdr, bool* hCustomFiltersEnabled, int* selectedHdrFilter, QString* customHdrFilter ) const;
		void sourcesInfo( bool* parseSrc, bool* sCustomFiltersEnabled, int* selectedSrcFilter, QString* customSrcFilter ) const;

		void setHeadersInfo( bool parseHdr, bool hCustomFiltersEnabled, int selectedHdrFilter, const QString& customHdrFilter );
		void setSourcesInfo( bool parseSrc, bool sCustomFiltersEnabled, int selectedSrcFilter, const QString& customSrcFilter );

	protected:
		virtual void changeEvent( QEvent* );
		virtual void closeEvent( QCloseEvent* );

	private slots:
		void on_selectedRootFolder_textChanged( const QString& );
		void selectRootFolder();
		void on_drawButton_clicked();
		void on_clearButton_clicked();
		void about();
		void restoreDefaultSettings();
		void rendererTypeChanged( QAction* );
		void languageChanged( QAction* );
		void parseOptionsChanged();
		void saveAsImage();
		void saveAsDot() const;
		void exitApp();

	private:
		Ui::MainWindow* _ui;
		AboutDialog* _aboutDlg;
		QActionGroup* _langGroup;
		AppConfig* _config;
		QTranslator _qtTranslator;
		QTranslator _appTranslator;
		QString _currentLocale;
		QMap<QString, QAction*> _availableLanguages;
		bool _isValidDirSelected;

		bool _imageFiltersUpdated;
		QString _imageFilters;
		QMap<QString, QString> _imageFiltersByExt;

		void _doClearGraph() const;
		void _setGraphAttributes() const;
		void _setGroupBoxesEnabled( bool value ) const;
		void _setButtonsAndActionsEnabled( bool value ) const;
		uint _scanFolder( const QString& dirName ) const;
		QStringList _getNameFilters() const;

		void _lookForTranslations( const QString& path );
		bool _createLanguageAction( const QString& locale , const QString& langFilePath );
		void _switchTranslator( QTranslator* t, const QString& fileName, const QString& directory = "", bool justRemoveTranslator = false );
	};
}

#endif // MAINWINDOW_H
