/*****************************************************************************
 *   Preliminary Qt test code for v3dfxbase
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

#include "qt-v3dfx-test.h"

#include <QtGui>
#include <QGraphicsScene>
#include <QPainter>
#include <QtCore/QtDebug>
#include <QtCore/QTimer>

#define MAX_ITER_COUNT 11

static int numTimes = 0;

VideoTestItem::VideoTestItem(QGraphicsItem *parent)
:V3dfxGLItem(parent)
{
	qWarning() << __func__ << "VideoTestItem constructor called";

	//For testing, comment out
	//V3dfxGLItem::init();

	currTimer = new QTimer(this);
	connect(currTimer, SIGNAL(timeout()), this, SLOT(updatePicture()));
	currTimer->start(1000);
}
void VideoTestItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *wdg)
{
	qWarning() << __func__ << "VideoTestItem called";
	V3dfxGLItem::paint(painter, item, wdg);
}

void VideoTestItem::updatePicture()
{
	qWarning() << __func__ << "VideoTestItem called";
	update();
	if(numTimes ++ > MAX_ITER_COUNT) QApplication::quit();
}
