/*****************************************************************************
 *   Preliminary Qt test code for v3dfxbase - Main window
 *
 * Copyright (C) 2012 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *   
 *   Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the
 *   distribution.
 *   
 *   Neither the name of Texas Instruments Incorporated nor the names of
 *   its contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contact: prabu@ti.com
 ****************************************************************************/

#include "v3dfxtest_window.h"
#include "videowidget.h"

#include <QApplication>
#include <QMenuBar>
#include <QGroupBox>
#include <QGridLayout>


V3dfxTestWindow::V3dfxTestWindow()
{
    videoWidget = new VideoWidget();

    QGroupBox * groupBox = new QGroupBox(this);
    setCentralWidget(groupBox);
	groupBox->setTitle("V3dfx - QtQGL Test");

    QGridLayout *layout = new QGridLayout(groupBox);

    layout->addWidget(videoWidget,1,0,8,1);

    groupBox->setLayout(layout);

    QMenu *fileMenu = new QMenu("File");
    QAction *exit = new QAction("Exit", fileMenu);
    fileMenu->addAction(exit);

    menuBar()->addMenu(fileMenu);

    QObject::connect(exit, SIGNAL(triggered(bool)), this, SLOT(close()));
}
