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
 * projectinfodialog.cpp
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
#include "projectinfodialog.h"
#include "ui_projectinfodialog.h"
#include "project.h"
#include "helpers.h"

namespace depgraphV
{
	ProjectInfoDialog::ProjectInfoDialog( QWidget* parent )
		: QDialog( parent ),
		_ui( new Ui::ProjectInfoDialog )
	{
		_ui->setupUi( this );
	}
	//-------------------------------------------------------------------------
	ProjectInfoDialog::~ProjectInfoDialog()
	{
		delete _ui;
	}
	//-------------------------------------------------------------------------
	int ProjectInfoDialog::exec()
	{
		Project* p = Singleton<Project>::instancePtr();
		_ui->name->setText( QString( "%1%2" ).arg(
								p->name(),
								Project::defaultExtension )
		);
		_ui->version->setText( QString::number( p->version() ) );
		_ui->path->setText( Helpers::pathShortener( p->path(), 44 ) );
		_ui->size->setText( tr( "%1 bytes" ).arg(
								QString::number( p->size() ) )
		);

		return QDialog::exec();
	}
	//-------------------------------------------------------------------------
	void ProjectInfoDialog::_openContainingFolder()
	{
		Project* p = Singleton<Project>::instancePtr();
		Helpers::showInExplorer( p->path() );
	}
}
