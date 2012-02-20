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

void V3dfxGLWidget::initializeGL()
{
	qWarning() << __func__ << "V3dfxGLWidget called";
}

void V3dfxGLWidget::paintGL()
{
	qWarning() << __func__ << "V3dfxGLWidget called";

	swapBuffers();

}

/************* Function Implementations ******************/
int V3dfxGLWidget::initV3dFx(
		int streamingType,/*! IMGSTREAM, EGLIMAGE */
		void* attrib, /* Attribute structure based on type */
		int deviceId, /* Device # applicable to IMGSTREAM */
		unsigned long *paArray /*! Array of buffers */
		)
{
	int err = 0;
	//TODO define enums
	if(streamingType == 0) //imgstream
	{
		deviceClass = new TISGXStreamIMGSTREAMDevice();
		texClass = new TISGXStreamTexIMGSTREAM();		
	}
	else if (streamingType == 1)
	{
		deviceClass = new TISGXStreamEGLIMAGEDevice();
		texClass = new TISGXStreamTexEGLIMAGE();	
	}
	else 
	{
		return 1;
	}
	if(!deviceClass || !texClass)
	{
		return 2;
	}

	currDeviceType = streamingType;
	currDeviceId = deviceId;

	err = deviceClass->init(attrib, deviceId, paArray);
	if(err)
	{
		return err;
	}
	err = texClass->init(deviceId);
	if(err)
	{
		return err;
	}
}

int V3dfxGLWidget::qTexImage2DBuf(void* fullBufPhyAddrArray)
{
	return deviceClass->qTexImage2DBuf(fullBufPhyAddrArray);
}
void V3dfxGLWidget::signal_draw(int texIndex)
{
	return deviceClass->signal_draw(texIndex);
}

int V3dfxGLWidget::dqTexImage2DBuf(void* freeBufPhyAddrArray)
{
	return deviceClass->dqTexImage2DBuf(freeBufPhyAddrArray);
}
int V3dfxGLWidget::load_v_shader(char const* vshader)
{ 
	return texClass->load_v_shader(vshader);
}
int V3dfxGLWidget::load_f_shader(char const* fshader)
{ 
	return texClass->load_f_shader(fshader);
}
int V3dfxGLWidget::load_program()
{
	return texClass->load_program(); 
}
int V3dfxGLWidget::use_program()
{ 
	texClass->use_program();
}

int V3dfxGLWidget::release_program()
{ 
	return texClass->release_program();
}

int V3dfxGLWidget::get_attrib_location(char const* attribname)
{ 
	return texClass->get_attrib_location(attribname);
}
int V3dfxGLWidget::get_uniform_location(char const* uniformname)
{ 
	return texClass->get_uniform_location(uniformname);
}
int V3dfxGLWidget::destroy()
{ 
	return deviceClass->destroy();
}





