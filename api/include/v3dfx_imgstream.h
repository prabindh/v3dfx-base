/*****************************************************************************
 *   Base classes for v3dfx - imgstream based
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



#ifndef __V3DFX_IMGSTREAM_BASE_H
#define __V3DFX_IMGSTREAM_BASE_H

#include "v3dfxbase.h"

#include <img_types.h>
#include <servicesext.h>
#include "bc_cat.h"
#include <fcntl.h>
#include "sys/ioctl.h"
#include <unistd.h>


#ifdef __cplusplus
extern "C" {
#endif 

typedef void (GL_APIENTRYP PFNGLTEXBINDSTREAMIMGPROC) (GLint device, GLint deviceoffset);
typedef const GLubyte *(GL_APIENTRYP PFNGLGETTEXSTREAMDEVICENAMEIMGPROC) (GLenum target);
typedef void (GL_APIENTRYP 
	PFNGLGETTEXSTREAMDEVICEATTRIBUTEIVIMGPROC) 
		(GLenum target, GLenum pname, GLint *params);

#define GL_TEXTURE_STREAM_IMG                                   0x8C0D     
#define GL_TEXTURE_NUM_STREAM_DEVICES_IMG                       0x8C0E     
#define GL_TEXTURE_STREAM_DEVICE_WIDTH_IMG                      0x8C0F
#define GL_TEXTURE_STREAM_DEVICE_HEIGHT_IMG                     0x8EA0     
#define GL_TEXTURE_STREAM_DEVICE_FORMAT_IMG                     0x8EA1      
#define GL_TEXTURE_STREAM_DEVICE_NUM_BUFFERS_IMG                0x8EA2     


#define BC_CAT_DRV "/dev/bccat0"


typedef struct __imgstream_device_attributes
{
	int widthPixels;
	int heightPixels;	
	int bytesPerPixel;
	int strideBytes;
	PVRSRV_PIXEL_FORMAT pvrPixelFormat; /*! Refer pvr_services.h */
	int numBuffers;
}imgstream_device_attributes;


class TISGXStreamIMGSTREAMDevice : public TISGXStreamDeviceBase
{
	imgstream_device_attributes attributes;
	int initialise_imgstream_km(unsigned long *paArray);
	int initialise_imgstream_gl();
	int bcfd;
	PFNGLTEXBINDSTREAMIMGPROC myglTexBindStreamIMG;
	int convert_pvr_to_bc_pixel_format(int pvr_format);
	int set_tex_buf_addr(int idx, unsigned long buf_paddr);
	bool init_bcdev_without_pa(
		int bcdev_id, 
		unsigned int pix_frmt, 
		int width, 
		int height,
		int num_channels);
public:
	TISGXStreamIMGSTREAMDevice();
	~TISGXStreamIMGSTREAMDevice();
	int init(void* attrib, int deviceId, unsigned long *paArray);
	int destroy();
	int qTexImage2DBuf(void* fullBufPhyAddrArray);
	int dqTexImage2DBuf(void* freeBufPhyAddrArray);
};


/**
Class for handling IMGSTREAM texturing.
*/
class TISGXStreamTexIMGSTREAM : public TISGXStreamTexBase
{
public:
	TISGXStreamTexIMGSTREAM();
	~TISGXStreamTexIMGSTREAM();
	int init(int streamDeviceId);
	int load_v_shader(char const* vshader);
	int load_f_shader(char const* fshader);
	int load_program();
};


#ifdef __cplusplus
}
#endif 


#endif //#define __V3DFX_IMGSTREAM_BASE_H


