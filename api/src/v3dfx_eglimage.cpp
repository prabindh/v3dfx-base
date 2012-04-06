/*****************************************************************************
 *   eglimage classes for v3dfx-base
 * Lot of it influenced by eglimageoes in sgxperf, and in RobClark's
 * eglimage work at 
 * https://github.com/robclark/xbmc/commits/gstreamer-eglimg
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
#include "../include/v3dfx_eglimage.h"


#ifdef __cplusplus
extern "C" {
#endif 

TISGXStreamEGLIMAGEDevice::TISGXStreamEGLIMAGEDevice() {}
TISGXStreamEGLIMAGEDevice::~TISGXStreamEGLIMAGEDevice() {}
/**
Unlike imgstream, eglimage based texturing has no concept of a 
device. Just start texturing. Hence no device gets created here.
*/
int TISGXStreamEGLIMAGEDevice::init(
	void* attrib, 
	int inDeviceId,
	unsigned long *paArray)
{
	int err;

	attributes = *((eglimage_device_attributes*)attrib);
	LOG_V3DFX_BASE("Init attributes:\nwidthpixels=%d,heightpixels=%d,bpp=%d, \
		stride=%d,colourformat=%d\n", attributes.widthPixels, 
			attributes.heightPixels, 
		attributes.bytesPerPixel, attributes.strideBytes,
		attributes.pvrPixelFormat);

	deviceId = inDeviceId;

	err = initialise_eglimage_gl(paArray);
	return err;
}

int TISGXStreamEGLIMAGEDevice::destroy()
{
	//TODO
#ifdef _ENABLE_EGLIMAGE //Use new header
	for(int bcount = 0;bcount < maxBufferCount;bcount ++)
	{
		glDeleteTextures(1, &eglImageTextureObjectIds[bcount]);
	}
	glBindTexture(GL_TEXTURE_EXTERNAL_OES, 0);
#endif
	return 0;
}

int TISGXStreamEGLIMAGEDevice::initialise_eglimage_gl(unsigned long *paArray)
{
	int paCount = 0;

#ifdef _ENABLE_EGLIMAGE //TODO - use new header in SDK
	unsigned int newTexObjectId = 0;

	PFNEGLCREATEIMAGEKHRPROC eglCreateImageKHR =
            (PFNEGLCREATEIMAGEKHRPROC)eglGetProcAddress("eglCreateImageKHR");
	PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES =
            (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)
		eglGetProcAddress("glEGLImageTargetTexture2DOES");
	if(!eglCreateImageKHR || !glEGLImageTargetTexture2DOES)
	{
		LOG_V3DFX_BASE("%s Error getting proc addresses for eglImage\n", __func__);
		return 1;
	}

	//create required eglimage targets
	while(paArray[paCount])
	{
		EGLImageKHR eglImage = eglCreateImageKHR(
					attributes.egldisplay, 
					EGL_NO_CONTEXT, 
					EGL_RAW_VIDEO_TI, 
					(void*)paArray[paCount], 
					(int*)attributes.eglAttribArray
					);
		if (eglImage == EGL_NO_IMAGE_KHR) 
		{
			LOG_V3DFX_BASE("%s Error in eglCreateImageKHR\n", __func__);
			return 2;
		}
		glGenTextures(1, &newTexObjectId);
		glBindTexture(GL_TEXTURE_EXTERNAL_OES, newTexObjectId);
		glTexParameteri(GL_TEXTURE_EXTERNAL_OES, 
					GL_TEXTURE_MIN_FILTER, 
					GL_NEAREST);
		glTexParameteri(GL_TEXTURE_EXTERNAL_OES, 
					GL_TEXTURE_MAG_FILTER, 
					GL_NEAREST);
		glEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES, 
				(GLeglImageOES)eglImage);
		//store the texture ids for later use
		eglImageTextureObjectIds[paCount] = newTexObjectId;
		paCount ++;
	}
#endif
	maxBufferCount = paCount ? paCount-1 : 0;
	return 0;
}

int TISGXStreamEGLIMAGEDevice::qTexImage2DBuf(void* fullBufPhyAddr)
{
#ifdef _ENABLE_EGLIMAGE //TODO - use new header in SDK
	//Get bufferindex from PhyAddr
	//bufferIndex = get_buffer_index_from_pa(fullBufPhyAddr);
	glBindTexture(GL_TEXTURE_EXTERNAL_OES, eglImageTextureObjectIds[currBufferIndex]);
#endif
	return 0;
}
int TISGXStreamEGLIMAGEDevice::dqTexImage2DBuf(void* freeBufPhyAddr)
{
	//Nothing to see here
	//Do we return physical addresses here ?
	//TODO:
	return 0;
}


TISGXStreamTexEGLIMAGE::TISGXStreamTexEGLIMAGE() {}
TISGXStreamTexEGLIMAGE::~TISGXStreamTexEGLIMAGE() {}
int TISGXStreamTexEGLIMAGE::init(int streamDeviceId)
{
	LOG_V3DFX_BASE("%s initialised\n", __func__);
	return 0;
}
int TISGXStreamTexEGLIMAGE::load_v_shader(char const* vshader)
{
	int err = 0;

	const char* pszVertTextureShader = "\
		attribute highp   vec4  inVertex;\
		uniform mediump mat4  MVPMatrix;\
		attribute mediump vec2  inTexCoord;\
		varying mediump vec2  TexCoord;\
		void main()\
		{\
			gl_Position = MVPMatrix * inVertex;\
			TexCoord = inTexCoord;\
		}";


	if(vshader == NULL)
	{
		err = TISGXStreamTexBase::load_v_shader(pszVertTextureShader);
	}
	else
	{
		LOG_V3DFX_BASE("Cannot handle new v shader\n");
	}
	return err;
}
int TISGXStreamTexEGLIMAGE::load_f_shader(char const* fshader)
{
	int err = 0;

	const char* pszFragEGLTextureStreamShader = "\
	        #extension GL_OES_EGL_image_external : require\n \
			 varying mediump vec2 TexCoord; \
			uniform samplerExternalOES sTexture;\n \
			 void main(void) \
			 {	\
				gl_FragColor = texture2D(sTexture, TexCoord); \
			 }";

	if(fshader == NULL)
	{
		err = TISGXStreamTexBase::load_f_shader(pszFragEGLTextureStreamShader);
	}
	else
	{
		LOG_V3DFX_BASE("Cannot handle new f shader\n");
	}
	return err;
		
}
int TISGXStreamTexEGLIMAGE::load_program()
{
	return TISGXStreamTexBase::load_program();
}



#ifdef __cplusplus
}
#endif 

