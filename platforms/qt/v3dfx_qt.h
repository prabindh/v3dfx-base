/*****************************************************************************
 *   Headers for v3dfxbase (QGraphicsScene)
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
#ifndef V3DFX_QT_H
#define V3DFX_QT_H

#include <QGraphicsScene>
#include "v3dfxbase.h"
#include "v3dfx_imgstream.h"
#include "v3dfx_eglimage.h"



class V3dfxGLScene : public QGraphicsScene
{
Q_OBJECT
private:
	float currColor;
	int initialised;
	int currDeviceType;
	int currDeviceId;
	TISGXStreamDeviceBase* deviceClass;
	TISGXStreamTexBase* texClass;
public:
	V3dfxGLScene(QGraphicsScene *parent = 0);
	~V3dfxGLScene();
	virtual QRectF boundingRect() const;
	int initV3dFx(
		int streamingType,/*! IMGSTREAM, EGLIMAGE */
		void* attrib, /* Attribute structure based on type */
		int deviceId, /* Device # applicable to IMGSTREAM */
		unsigned long *paArray /*! Array of buffers */
		);
	int qTexImage2DBuf(void* fullBufPhyAddrArray);
	void signal_draw(int texIndex);
	int dqTexImage2DBuf(void* freeBufPhyAddrArray);
	int load_v_shader(char const* vshader);
	int load_f_shader(char const* fshader);
	int load_program(); /*! link and load program */
	int use_program(); /*! Binds default program */
	int release_program();/*! Unbinds default program */
	int get_attrib_location(char const* attribname);
	int get_uniform_location(char const* uniformname);
	int destroy();

protected:
	//QGraphicsScene paint
	virtual void drawBackground ( QPainter * painter, const QRectF & rect );
};
#endif
