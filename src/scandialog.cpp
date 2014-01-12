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
		  _worker( new BackgroundWorker( win ) ),
		  _detailsShown( true ),
		  _failed( false ),
		  _foldersCount( 0 ),
		  _filesCount( 0 ),
		  _analizedFilesCount( 0 ),
		  _timer( new QTimer( this ) ),
		  _elapsedSeconds( 0 )
	{
		_ui->setupUi( this );

		//Connect signals/slots
		connect( _timer, SIGNAL( timeout() ), this, SLOT( on_timeout() ) );
		connect( _worker, SIGNAL( directoryFound() ), this, SLOT( on_directoryFound() ) );
		connect( _worker, SIGNAL( fileFound( QString ) ), this, SLOT( on_fileFound( QString ) ) );
		connect( _worker, SIGNAL( scanFinished() ), this, SLOT( on_scanFinished() ) );
		connect( _worker, SIGNAL( graphCreated() ), this, SLOT( on_graphCreated() ) );
		connect( _worker, SIGNAL( fileAnalized( QString ) ), this, SLOT( on_fileAnalized( QString ) ) );
		connect( _worker, SIGNAL( fail( QString ) ), this, SLOT( on_fail( QString ) ) );
		connect( win->graph(), SIGNAL( layoutApplied() ), this, SLOT( on_layoutFinished() ) );
		connect( _ui->cancelButton, SIGNAL( clicked() ), _worker, SLOT( on_abort() ) );
		connect( _ui->cancelButton, SIGNAL( clicked() ), this, SLOT( on_abort_clicked() ) );

		on_toggleDetails();
	}
	//--------------------------------------------------------------------------------------------------------------------------
	ScanDialog::~ScanDialog()
	{
		_freeWorker();

		delete _timer;
		delete _ui;
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void ScanDialog::on_directoryFound()
	{
		_foldersCount++;
		_ui->foldersCount->setText( QString::number( _foldersCount ) );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void ScanDialog::on_fileFound( QString file )
	{
		_filesCount++;
		_ui->filesCount->setText( QString::number( _filesCount ) );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void ScanDialog::on_scanFinished()
	{
		_ui->progressBar->setMaximum( _filesCount );
		_ui->currentOperation->setText( tr( "Creating graph..." ) );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void ScanDialog::on_graphCreated()
	{
		_ui->currentOperation->setText( tr( "Layouting..." ) );
		_ui->progressBar->setValue( 0 );
		_ui->progressBar->setMaximum( 0 );

		//From now on, layouting cannot be stopped.
		_ui->cancelButton->setEnabled( false );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void ScanDialog::on_fileAnalized( QString file )
	{
		_analizedFilesCount++;
		_ui->progressBar->setValue( _analizedFilesCount ); 
		_ui->analizedFilesCount->setText( QString::number( _analizedFilesCount ) );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void ScanDialog::on_layoutFinished()
	{
		_freeWorker();
		_ui->progressBar->setMaximum( 1 );
		_ui->progressBar->setValue( 1 );
		_ui->currentOperation->setText( tr( "Done" ) );
		_timer->stop();
		_ui->cancelButton->setText( tr( "Close" ) );
		_ui->cancelButton->setEnabled( true );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void ScanDialog::on_fail( QString message )
	{
		_freeWorker();
		_failed = true;
		_timer->stop();
		QMessageBox::critical( this, tr( "Error" ), message );
		_ui->cancelButton->setText( tr( "Close" ) );
		_ui->cancelButton->setEnabled( true );
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void ScanDialog::on_timeout()
	{
		_elapsedSeconds++;

		if( _detailsShown && _timer->isActive() )
		{
			int mins = ( _elapsedSeconds / 60 ) % 60;
			int hours = ( _elapsedSeconds / 3600 );
			_elapsedSeconds = _elapsedSeconds % 60;
			_ui->elapsedTime->setText( QString( "%1:%2:%3" )
				.arg( hours, 2, 10, QLatin1Char( '0' ) )
				.arg( mins, 2, 10, QLatin1Char( '0' ) )
				.arg( _elapsedSeconds, 2, 10, QLatin1Char( '0' ) )
			);
		}
	}
	//--------------------------------------------------------------------------------------------------------------------------
	void ScanDialog::on_toggleDetails()
	{
		_detailsShown = !_detailsShown;
		QString suffix = " <<<";

		if( _detailsShown )
		{
			_ui->detailsGroupBox->show();
			this->resize( 0, this->maximumHeight() );

			//Update time elapsed label, then start timer
			this->on_timeout();
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
	void ScanDialog::on_abort_clicked()
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
		_timer->start( 1000 );
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
