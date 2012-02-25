/*****************************************************************************
 *   Preliminary Qt test code for v3dfxbase (GraphicsScene based)
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

static int curritercount = 0;
static bool app_initialised = false;

extern int qt_program_setup(int testID);
extern void qt_program_cleanup(int testID);
extern int allocate_v3dfx_imgstream_bufs(int numbufs);
extern void deallocate_v3dfx_imgstream_bufs();
extern void test8();
extern void test20_init_1();
extern void test20_init_2();
extern void test20_process_one_prepare(int currIteration);
extern void test20_process_one_draw(int curriteration);
extern void test20_deinit();

extern void test3_init();
extern void test3_process_one(int currIteration);
extern void test3_deinit();

extern void set_mvp(int);

imgstream_device_attributes tempAttrib = {
			256, 
			256, 
			2, 
			256*256*2,
			PVRSRV_PIXEL_FORMAT_UYVY, 
			2}; // 2 buffers
int lastDeviceClass = 0;
unsigned long paArray[] = {0, 0};
unsigned long freeArray[] = {0, 0, 0, 0};  




VideoTestScene::VideoTestScene(QGraphicsScene *parent)
:V3dfxGLScene(parent)
{
	qWarning() << __func__ << "VideoTestScene constructor called";

	currTimer = new QTimer(this);
	connect(currTimer, SIGNAL(timeout()), this, SLOT(updatePicture()));
	currTimer->start(1000);
}

VideoTestScene::~VideoTestScene()
{
	qWarning() << __func__ << "5 deinit";

	test20_deinit();
	destroy();

	deallocate_v3dfx_imgstream_bufs();
	qt_program_cleanup(8);
}

//NOTE - this HAS to be called from within a GL context
//TODO separate application code and base code from init
int VideoTestScene::init()
{
	int err;

	qWarning() << __func__ << "VideoWidget called";
	qWarning() << curritercount;

	err = qt_program_setup(8);
	if(err) 
	{
		qt_program_cleanup(8);		
		goto completed;
	}
	qWarning() << __func__ << "1";

	//GL_IMG_texture_stream - via v3dfxbase
	allocate_v3dfx_imgstream_bufs(2); //2 buffers
	test20_init_1();

	//create imgstream type for testing
	initV3dFx(0, &tempAttrib, 0, paArray);
	load_v_shader(NULL);
	load_f_shader(NULL);
	load_program();
	set_mvp(get_uniform_location("MVPMatrix"));

	//continue rest of the initialisation
	test20_init_2();

	release_program();

	qWarning() << __func__ << "init complete";

completed:
	return 0;
}


void VideoTestScene::drawBackground ( QPainter * painter, const QRectF & rect )
{
	qWarning() << __func__ << "VideoTestScene called";


	if(!app_initialised)
	{
		init();
		app_initialised = true;
	}

	use_program(); 
	test20_process_one_prepare(curritercount);
	qTexImage2DBuf(paArray);

	test20_process_one_draw(curritercount);
	release_program(); 

	V3dfxGLScene::drawBackground(painter, rect);
}

void VideoTestScene::updatePicture()
{
	qWarning() << __func__ << "VideoTestScene called";
	update();
	if(curritercount ++ > MAX_ITER_COUNT) QApplication::quit();
}

QRectF VideoTestScene::boundingRect() const
{
    return QRectF(-150, -150, 600, 600);
}

