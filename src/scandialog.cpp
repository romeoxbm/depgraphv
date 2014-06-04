/**
 * scandialog.cpp
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
#include "scandialog.h"
#include "ui_scandialog.h"
#include "mainwindow.h"
#include "backgroundworker.h"
#include <QMessageBox>

namespace depgraphV
{
	ScanDialog::ScanDialog( MainWindow* win )
		: QDialog( static_cast<QWidget*>( win ), Qt::FramelessWindowHint ),
		_ui( new Ui::ScanDialog ),
		_worker( new BackgroundWorker( win, this ) ),
		_detailsShown( true ),
		_failed( false ),
		_foldersCount( 0 ),
		_filesCount( 0 ),
		_analizedFilesCount( 0 )
	{
		_ui->setupUi( this );

		//Connect signals/slots
		connect( _worker, SIGNAL( folderFound( QString ) ), this, SLOT( onFolderFound( QString ) ) );
		connect( _worker, SIGNAL( fileFound( QString ) ), this, SLOT( onFileFound( QString ) ) );
		connect( _worker, SIGNAL( scanFinished() ), this, SLOT( onScanFinished() ) );
		connect( _worker, SIGNAL( graphCreated() ), this, SLOT( onGraphCreated() ) );
		connect( _worker, SIGNAL( fileAnalized( QString ) ), this, SLOT( onFileAnalized( QString ) ) );
		connect( _worker, SIGNAL( fail( QString ) ), this, SLOT( onFail( QString ) ) );
		connect( _worker, SIGNAL( fail( QString ) ), _ui->elapsedTime, SLOT( stopTimer() ) );
		connect( _ui->cancelButton, SIGNAL( clicked() ), _worker, SLOT( on_abort() ) );
		connect( _ui->cancelButton, SIGNAL( clicked() ), this, SLOT( onAbortClicked() ) );

		if( win )
		{
			connect( win->graph(), SIGNAL( layoutApplied() ), this, SLOT( onLayoutFinished() ) );
			connect( win->graph(), SIGNAL( layoutApplied() ), _ui->elapsedTime, SLOT( stopTimer() ) );
		}

		onToggleDetails();
	}
	//--------------------------------------------------------------------------------------------------------------------------
	ScanDialog::~ScanDialog()
	{
		_freeWorker();
		delete _ui;
	}
	//--------------------------------------------------------------------------------------------------------------------------
    void ScanDialog::onFolderFound( QString )
	{
		_foldersCount++;
		_ui->foldersCount->setText( QString::number( _foldersCount ) );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void ScanDialog::onFileFound( QString )
	{
		_filesCount++;
		_ui->filesCount->setText( QString::number( _filesCount ) );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void ScanDialog::onScanFinished()
	{
		_ui->progressBar->setMaximum( _filesCount );
		_ui->currentOperation->setText( tr( "Creating graph..." ) );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void ScanDialog::onGraphCreated()
	{
		_ui->currentOperation->setText( tr( "Layouting..." ) );
		_ui->progressBar->setValue( 0 );
		_ui->progressBar->setMaximum( 0 );

		//From now on, layouting cannot be stopped.
		_ui->cancelButton->setEnabled( false );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void ScanDialog::onFileAnalized( QString file )
	{
		_analizedFilesCount++;
		_ui->progressBar->setValue( _analizedFilesCount ); 
		_ui->analizedFilesCount->setText( QString::number( _analizedFilesCount ) );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void ScanDialog::onLayoutFinished()
	{
		_freeWorker();
		_ui->progressBar->setMaximum( 1 );
		_ui->progressBar->setValue( 1 );
		_ui->currentOperation->setText( tr( "Done" ) );
		_ui->cancelButton->setText( tr( "Close" ) );
		_ui->cancelButton->setEnabled( true );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void ScanDialog::onFail( QString message )
	{
		_freeWorker();
		_failed = true;
		QMessageBox::critical( this, tr( "Error" ), message );
		_ui->cancelButton->setText( tr( "Close" ) );
		_ui->cancelButton->setEnabled( true );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void ScanDialog::onToggleDetails()
	{
		_detailsShown = !_detailsShown;
		QString suffix = " <<<";

		if( _detailsShown )
		{
			_ui->detailsGroupBox->show();
			this->resize( 0, this->maximumHeight() );
		}
		else
		{
			_ui->detailsGroupBox->hide();
			this->resize( 0, this->minimumHeight() );
			suffix = " >>>";
		}

		_ui->detailsButton->setText( tr( "Details" ) + suffix );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void ScanDialog::onAbortClicked()
	{
		if( _ui->cancelButton->text() != tr( "Close" ) )
			return;

		if( _failed )
			reject();
		else
			accept();
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void ScanDialog::showEvent( QShowEvent* evt )
	{
		QDialog::showEvent( evt );
		_ui->elapsedTime->startTimer();
		_worker->start();
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void ScanDialog::_freeWorker()
	{
		if( _worker )
		{
			_worker->wait();
			delete _worker;
		}

		_worker = 0;
	}
}
