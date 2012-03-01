/*****************************************************************************
 *   Preliminary Qt test code for v3dfxbase (GraphicsScene based, Ultrasound)
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
#include "ultrasound_renderer.h"

#include <QtGui>
#include <QGraphicsScene>
#include <QPainter>
#include <QtCore/QtDebug>
#include <QtCore/QTimer>

#define MAX_ITER_COUNT 100

static int curritercount = 0;
static float currRotationAngle = 0.0;
int mvpMatrixLoc = 0;

static bool app_initialised = false;

imgstream_device_attributes tempAttrib = {
			ULTRASOUND_WIDTH, //width
			ULTRASOUND_HEIGHT, //height
			4, //bpp
			ULTRASOUND_WIDTH * 4, //stride
			PVRSRV_PIXEL_FORMAT_ARGB8888, //pixel format
			ULTRASOUND_LAYERS}; // 128 buffers - stacked images
int lastDeviceClass = 0;
unsigned long vaArray[ULTRASOUND_LAYERS];
unsigned long paArray[ULTRASOUND_LAYERS];
unsigned char *pVContigMem = 0;
unsigned char *pPContigMem = 0;



VideoTestScene::VideoTestScene(QGraphicsScene *parent)
:V3dfxGLScene(parent)
{
	qWarning() << __func__ << "VideoTestScene constructor called";

	currTimer = new QTimer(this);
	connect(currTimer, SIGNAL(timeout()), this, SLOT(updatePicture()));
	currTimer->start(500);
}

VideoTestScene::~VideoTestScene()
{
	qWarning() << __func__ << "5 deinit";

	us_render_deinit();
	destroy();

	us_deallocate_v3dfx_imgstream_bufs(pVContigMem);
	us_program_cleanup();
}

//NOTE - this HAS to be called from within a GL context
//TODO separate application code and base code from init
int VideoTestScene::init()
{
	int err;

	qWarning() << __func__ << "VideoWidget called";
	qWarning() << curritercount;

	err = us_program_setup();
	if(err) 
	{
		us_program_cleanup();		
		goto completed;
	}
	qWarning() << __func__ << "1";

	//GL_IMG_texture_stream - via v3dfxbase
	err = us_allocate_v3dfx_imgstream_bufs( 
		ULTRASOUND_WIDTH * ULTRASOUND_HEIGHT * ULTRASOUND_LAYERS * 4,
		(void**)&pVContigMem,
		(void**)&pPContigMem
		);
	qWarning() << __func__ << "2";
	if(err) 
	{
		us_program_cleanup();		
		goto completed;
	}
	for(int i = 0;i < ULTRASOUND_LAYERS;i ++)
	{
		paArray[i] = (unsigned long)(pPContigMem + i*ULTRASOUND_WIDTH*ULTRASOUND_HEIGHT*4);
	}
	qWarning() << __func__ << "3";
	//This function will convert the 1BPP LUMINANCE image to 4bpp - TODO - use original LUMINANCE AS-IS
	us_render_init_1(pVContigMem, ULTRASOUND_WIDTH * ULTRASOUND_HEIGHT * ULTRASOUND_LAYERS);
	qWarning() << __func__ << "4";
	//create imgstream type for testing
	initV3dFx(0, &tempAttrib, 0, paArray);
	qWarning() << __func__ << "5";
	load_v_shader(NULL);
	load_f_shader(NULL);
	load_program();
	mvpMatrixLoc = get_uniform_location("MVPMatrix");
	us_set_mvp(mvpMatrixLoc, currRotationAngle);

	//continue rest of the initialisation
	 us_render_init_2();
	qWarning() << __func__ << "6";
	release_program();

	qWarning() << __func__ << "init complete";

	err = 0;
completed:
	return err;
}


void VideoTestScene::drawBackground ( QPainter * painter, const QRectF & rect )
{
	qWarning() << __func__ << "VideoTestScene called";


	if(!app_initialised)
	{
		int err = init();
		if(!err) app_initialised = true;
	}

	use_program(); 

	//clear previous screen
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	us_set_mvp(mvpMatrixLoc, currRotationAngle);
	currRotationAngle += ((float)22/(float)(7*8));
	//TODO - clarify this textureID vs texture issue
	for (int id = 0;id < ULTRASOUND_LAYERS; id ++)
	{
		//Do only one at a time - so reset all to zero first
		for (int initid = 0;initid < ULTRASOUND_LAYERS; initid ++) paArray[initid] = 0;
		//Set PHYSICAL address only for current id HERE - assuming 4 bytespp
		paArray[id] = (unsigned long)&pPContigMem[id*ULTRASOUND_WIDTH*ULTRASOUND_HEIGHT*4];

		us_render_process_one_prepare(id, curritercount);
		qTexImage2DBuf(paArray);
		us_render_process_one_draw(curritercount);
	}
	release_program(); 
	glDisable(GL_BLEND);

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

