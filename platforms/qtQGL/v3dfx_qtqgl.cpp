/*****************************************************************************
 *   Implementation for v3dfxbase
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

#include "v3dfx_qtqgl.h"
#include <QtCore/QtDebug>
#include <QtOpenGL>

extern int qt_program_setup(int testID);
extern void qt_program_cleanup(int testID);
extern int allocate_v3dfx_imgstream_bufs(int numbufs);
extern void deallocate_v3dfx_imgstream_bufs();
extern void test8();
extern void test20_init();
extern void test20_process_one(int currIteration);
extern void test20_deinit();

extern void test3_init();
extern void test3_process_one(int currIteration);
extern void test3_deinit();



static int curritercount = 0;

#define MAX_ITER_COUNT 50

V3dfxGLWidget::V3dfxGLWidget(QGLWidget *parent)
    : QGLWidget(parent)
{
	qWarning() << __func__ << "constructor called";
	currColor = 0;
	initialised = 0;

    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);
    setAutoBufferSwap(false);

}

V3dfxGLWidget::~V3dfxGLWidget()
{
	qWarning() << __func__ << " destructor called";
}

//TODO separate application code and base code from init
int V3dfxGLWidget::init()
{
	int err;

	qWarning() << __func__ << "V3dfxGLWidget called";
	qWarning() << curritercount;

	if(curritercount >= MAX_ITER_COUNT) goto completed; //do it only for MAX_ITER_COUNT times

	if(initialised == 0) 
	{
#if 1
		err = qt_program_setup(8);
		qWarning() << __func__ << "1";
		if(err) 
		{
			qt_program_cleanup(8);		
			goto completed;
		}
		/* GL_IMG_texture_stream - via v3dfxbase */
		allocate_v3dfx_imgstream_bufs(2); //2 buffers
		qWarning() << __func__ << "2";
		test20_init();
		qWarning() << __func__ << "3";
		initialised = 1;
#else
		test3_init();
		initialised = 1;
#endif
		qWarning() << __func__ << "init complete, going to complete";
		goto completed; //do not do rendering in init (initializeGL), only inside paintGL
	}

	if(curritercount < MAX_ITER_COUNT)
	{
		qWarning() << __func__ << "4, with iteration #" << curritercount;

#if 1
		test20_process_one(curritercount);

#else
		test3_process_one(curritercount);
#endif

		curritercount ++;
		//swap buffers will be done by framework itself after paint call
	}
	else
	{
		qWarning() << __func__ << "5 deinit";
#if 1
		test20_deinit();
		deallocate_v3dfx_imgstream_bufs();
		qt_program_cleanup(8);
#else
		test3_deinit();
#endif
		return 0;
	}
completed:
	return 0;
}

void V3dfxGLWidget::initializeGL()
{
	qWarning() << __func__ << "V3dfxGLWidget called";
	glClearColor(currColor, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	init();
}

void V3dfxGLWidget::paintGL()
{
	qWarning() << __func__ << "V3dfxGLWidget called";

	QString frameString;
	QTextStream(&frameString) << "frame count = " << curritercount << " Red = " << currColor;

	QPainter painter;
	painter.begin(this);

	painter.beginNativePainting();


	currColor += 0.01f;
	if(currColor > 1.0f) currColor = 0;

	glClearColor(currColor, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//TODO - remove this init - which also does process !!
	//separate application code and base code from init
	init();

    painter.drawText(20, 40, frameString);


    painter.end();
    
    swapBuffers();

}


