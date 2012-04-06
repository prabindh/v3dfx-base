/*****************************************************************************
 *   Base classes for v3dfx - eglimage based
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


#ifndef __V3DFX_BASE_EGLIMAGE_H
#define __V3DFX_BASE_EGLIMAGE_H

#define FOURCC(a, b, c, d) ((uint32_t)(uint8_t)(a) | ((uint32_t)(uint8_t)(b) << 8) | ((uint32_t)(uint8_t)(c) << 16) | ((uint32_t)(uint8_t)(d) << 24 ))
#define FOURCC_STR(str)    FOURCC(str[0], str[1], str[2], str[3])


#ifndef EGL_TI_raw_video
#  define EGL_TI_raw_video 1
#  define EGL_RAW_VIDEO_TI					0x333A	/* eglCreateImageKHR target */
#  define EGL_GL_VIDEO_FOURCC_TI				0x3331	/* eglCreateImageKHR attribute */
#  define EGL_GL_VIDEO_WIDTH_TI					0x3332	/* eglCreateImageKHR attribute */
#  define EGL_GL_VIDEO_HEIGHT_TI				0x3333	/* eglCreateImageKHR attribute */
#  define EGL_GL_VIDEO_BYTE_STRIDE_TI			0x3334	/* eglCreateImageKHR attribute */
#  define EGL_GL_VIDEO_BYTE_SIZE_TI				0x3335	/* eglCreateImageKHR attribute */
#  define EGL_GL_VIDEO_YUV_FLAGS_TI				0x3336	/* eglCreateImageKHR attribute */
#endif

#ifndef EGLIMAGE_FLAGS_YUV_CONFORMANT_RANGE
#  define EGLIMAGE_FLAGS_YUV_CONFORMANT_RANGE (0 << 0)
#  define EGLIMAGE_FLAGS_YUV_FULL_RANGE       (1 << 0)
#  define EGLIMAGE_FLAGS_YUV_BT601            (0 << 1)
#  define EGLIMAGE_FLAGS_YUV_BT709            (1 << 1)
#endif




#include "v3dfxbase.h"

#include <img_types.h>
#include <servicesext.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <fcntl.h>
#include "sys/ioctl.h"
#include <unistd.h>


#ifdef __cplusplus
extern "C" {
#endif 



typedef struct __eglimage_device_attributes
{
	int widthPixels;
	int heightPixels;	
	int bytesPerPixel;
	int strideBytes;
	PVRSRV_PIXEL_FORMAT pvrPixelFormat; /*! Refer pvr_services.h */
	EGLDisplay egldisplay;
	int *eglAttribArray;
	int numBuffers;
}eglimage_device_attributes;


class TISGXStreamEGLIMAGEDevice : public TISGXStreamDeviceBase
{
	eglimage_device_attributes attributes;
	unsigned int eglImageTextureObjectIds[MAX_BUFFERS_PER_STREAM];
	int initialise_eglimage_gl(unsigned long *paArray);
public:
	TISGXStreamEGLIMAGEDevice();
	~TISGXStreamEGLIMAGEDevice();
	/*! deviceId is immaterial, but paArray is needed. 
	paArray - Array of physical addresses NULL terminated
	*/
	int init(void* attrib, int deviceId, unsigned long *paArray);
	/*! Send a buffer for texturing.
	NOTE - this buffer is not released immediately.
	Use dq to get next free buffer. */
	int qTexImage2DBuf(void* fullBufPhyAddr);
	/*! Get next free buffer for loading new data */
	int dqTexImage2DBuf(void* freeBufPhyAddr);
	/*! Tear down */
	int destroy();
};

/**
Class for handling EGLIMAGE texturing.
*/
class TISGXStreamTexEGLIMAGE : public TISGXStreamTexBase
{
public:
	TISGXStreamTexEGLIMAGE();
	~TISGXStreamTexEGLIMAGE();
	/*! streamId is immaterial for eglimage at the moment */
	int init(int streamDeviceId);
	/*! Pass NULL for loading default eglimage shader */
	int load_v_shader(char const* vshader);
	/*! Pass NULL for loading default eglimage shader */
	int load_f_shader(char const* fshader);
	/*! Compiles and makes program current with v&f shaders */
	int load_program();
};

#ifdef __cplusplus
}
#endif 



#endif //#define __V3DFX_BASE_H
