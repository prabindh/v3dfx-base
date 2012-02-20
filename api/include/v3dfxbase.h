/*****************************************************************************
 *   Base classes for v3dfx
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



#ifndef __V3DFX_BASE_H
#define __V3DFX_BASE_H

#define LINUX

#include "GLES2/gl2.h"
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include "img_defs.h"
#include "img_types.h"
#include "servicesext.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define LOG_V3DFX_BASE printf

// Index to bind the attributes to vertex shaders
#define VERTEX_ARRAY_DEFAULT 0
#define TEXCOORD_ARRAY_DEFAULT 1


#ifdef __cplusplus
extern "C" {
#endif 




/* 
Expected flow:
egl init by app
gl init by app
new TISGXStreamDeviceBase - for # of channels in platform
new TISGXStreamTexBase - que buffers in each channel
TISGXStreamTexBase->qTexImage2DBuf / dq loop by app
gl vertex scene load by app
gl draw by app
TISGXStreamTexBase->signal_draw by app
*/

/**
This class creates and manages one streaming device
- will NOT manage shaders and program specific to imgstream or eglimage
- will NOT manage data input, que management (done by TISGXStreamTexBase)
- will NOT manage EGL / contexts
- will NOT do vertex loading
- will NOT do draw calls.
*/
#define MAX_BUFFERS_PER_STREAM 10

typedef struct __bufferstate
{
	unsigned long pa;
	unsigned int state;
}bufferstate;

class TISGXStreamDeviceBase
{
public:
	TISGXStreamDeviceBase();
	~TISGXStreamDeviceBase();
	int deviceId; /*! current device Id */
	bufferstate bufferState[MAX_BUFFERS_PER_STREAM];
	unsigned int texIdArray[MAX_BUFFERS_PER_STREAM];
	int currBufferIndex;
	int maxBufferCount;

	virtual int init(void* attrib, int deviceId, unsigned long *paArray);
	/*! Indicate to object that a draw has happened with current
	bound texture to target display - enabling dq later */	
	void signal_draw(int texIndex);
	virtual int qTexImage2DBuf(void* fullBufPhyAddr);
	virtual int dqTexImage2DBuf(void* freeBufPhyAddr);
	int get_device_id(); 
	virtual int destroy();
};


/**
Base class for handling texturing.
Allows to que n streams for rendering onto ONE display device using SGX 
- will manage shaders and program specific to imgstream or eglimage
- will manage data input, que management
- will do texture binding
- will NOT do draw calls
- Application informs draw through signal_draw() before dqTexImage2DBuf()
*/
class TISGXStreamTexBase
{
private:
	int currStreamDeviceId;
public:
	TISGXStreamTexBase();
	~TISGXStreamTexBase();
	GLuint uiVertShader;
	GLuint uiFragShader;
	GLuint uiProgramObject;
	virtual int init(int streamDeviceId);
	int get_attrib_location(char const* attribname);
	int get_uniform_location(char const* uniformname);
	virtual int load_v_shader(char const* vshader);
	virtual int load_f_shader(char const* fshader);
	virtual int load_program();
	int use_program(); /*! Call when mixing programs */
	int unload_shader_program();
};


#ifdef __cplusplus
}
#endif 



#endif //#define __V3DFX_BASE_H
