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
		/**
		 * @brief MainWindow constructor.
		 * @param parent The parent widget (default NULL)
		 */
		explicit MainWindow( QWidget* parent = 0 );

		/**
		 * @brief MainWindow destructor.
		 */
		~MainWindow();

		/**
		 * @brief Translate user interface by using specified locale.
		 * @param locale The locale we want to use for the ui.
		 */
		void translateUi( const QString& locale );

		/**
		 * @return The root path currently in use.
		 */
		QString rootPath() const;

		/**
		 * @return Return a string containing the locale currently in use (for instance, "en").
		 */
		const QString& currentLocale() const { return _currentLocale; }

		/**
		 * @return True if the donation tab page will be shown when closing this application, false otherwise.
		 */
		bool showDonateOnExit() const { return _showDonateOnExit; }

		/**
		 * @brief This method change the on-closing policy concerning donation tab page visibility.
		 * @param value The new "visibility" value used by _showDonateOnExit field.
		 */
		void setShowDonateOnExit( bool value ) { _showDonateOnExit = value; }

		/**
		 * @brief Set root path.
		 * @param value The new root path to use. If value is empty, QDir::currentPath() value will be used instead.
		 */
		void setRootPath( const QString& value );

		/**
		 * @return True if recursive scan checkbox is being checked, false otherwise.
		 */
		bool isRecursiveScanEnabled() const;

		/**
		 * @param Set recursive scan option checked state to "value".
		 */
		void setRecursiveScanEnabled( bool value );

		/**
		 * @return Return a QVariant containing data from the currently-selected locale.
		 */
		QVariant selectedLocaleData() const;

		/**
		 * @brief Set checked value to the renderer QAction specified by "type".
		 * @param type The renderer type.
		 * @param checked The new checked value.
		 */
		void setRendererActionCheckedByType( Graph::RendererType type, bool checked );

		/**
		 * @brief Change the high quality antialiasing setting.
		 * @param value The new value for high quality antialiasing.
		 */
		void setHighQualityAA( bool value );

		/**
		 * @brief Get all the headers-related informations.
		 * @param parseHdr Parse headers checked value.
		 * @param hCustomFiltersEnabled Return true if custom filters are enabled.
		 * @param selectedHdrFilter The index of the current selected filter.
		 * @param customHdrFilter Custom filters specified by the user.
		 */
		void headersInfo( bool* parseHdr, bool* hCustomFiltersEnabled, int* selectedHdrFilter, QString* customHdrFilter ) const;

		/**
		 * @brief sourcesInfo Get all the sources-related informations.
		 * @param parseSrc Parse sources checked value.
		 * @param sCustomFiltersEnabled Return true if custom filters are enabled.
		 * @param selectedSrcFilter The index of the current selected filter.
		 * @param customSrcFilter Custom filters specified by the user.
		 */
		void sourcesInfo( bool* parseSrc, bool* sCustomFiltersEnabled, int* selectedSrcFilter, QString* customSrcFilter ) const;

		/**
		 * @brief setHeadersInfo Set all headers-related informations.
		 * @param parseHdr Checked value for "Parse headers" checkbox.
		 * @param hCustomFiltersEnabled Enable or disable custom header filters.
		 * @param selectedHdrFilter Set the current selected filter.
		 * @param customHdrFilter Set custom filters string.
		 */
		void setHeadersInfo( bool parseHdr, bool hCustomFiltersEnabled, int selectedHdrFilter, const QString& customHdrFilter );

		/**
		 * @brief setSourcesInfo Set all sources-related informations.
		 * @param parseSrc Checked value for "Parse sources" checkbox.
		 * @param sCustomFiltersEnabled Enable or disable custom sources filters.
		 * @param selectedSrcFilter Set the current selected filter.
		 * @param customSrcFilter Set custom filters string.
		 */
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
		bool _showDonateOnExit;

		bool _imageFiltersUpdated;
		QString _imageFilters;
		QMap<QString, QString> _imageFiltersByExt;

		void _doClearGraph() const;
		void _setGraphAttributes() const;
		void _setGroupBoxesEnabled( bool value ) const;
		void _setButtonsAndActionsEnabled( bool value ) const;
		uint _scanFolder( const QString& dirName ) const;
		QStringList _getNameFilters() const;
		bool _lookForRequiredImageFormats();

		void _showAboutDialog( bool showDonations );

		void _lookForTranslations( const QString& path );
		bool _createLanguageAction( const QString& locale , const QString& langFilePath );
		void _switchTranslator( QTranslator* t, const QString& fileName, const QString& directory = "", bool justRemoveTranslator = false );
	};
}

#endif // MAINWINDOW_H
