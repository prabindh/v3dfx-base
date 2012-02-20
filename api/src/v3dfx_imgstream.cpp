/*****************************************************************************
 *   Base classes for v3dfx - imgstream implementation
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
#include "../include/v3dfx_imgstream.h"


#ifdef __cplusplus
extern "C" {
#endif 

TISGXStreamIMGSTREAMDevice::TISGXStreamIMGSTREAMDevice() {}
TISGXStreamIMGSTREAMDevice::~TISGXStreamIMGSTREAMDevice() {}
int TISGXStreamIMGSTREAMDevice::init(
	void* attrib, 
	int inDeviceId,
	unsigned long *paArray)
{
	int err;

	attributes = *((imgstream_device_attributes*)attrib);
	LOG_V3DFX_BASE("Init attributes:\nwidthpixels=%d,heightpixels=%d\n,bpp=%d, \
		stride=%d,colourformat=%d,ch=%d\n", attributes.widthPixels, 
			attributes.heightPixels, 
		attributes.bytesPerPixel, attributes.strideBytes,
		attributes.pvrPixelFormat, attributes.numBuffers);

	deviceId = inDeviceId;
	maxBufferCount = attributes.numBuffers;

	bcfd = -1;
	err = initialise_imgstream_km(paArray);
	if(err) return err;

	err = initialise_imgstream_gl();
	return err;
}

int TISGXStreamIMGSTREAMDevice::destroy()
{
	if(bcfd != -1)
	        close(bcfd);
	return 0;
}

int TISGXStreamIMGSTREAMDevice::set_tex_buf_addr(
		int idx, 
		unsigned long buf_paddr)
{
        bc_buf_ptr_t buf_pa;
        buf_pa.pa = buf_paddr;
        buf_pa.index = idx;
        if (ioctl(bcfd, BCIOSET_BUFFERPHYADDR, &buf_pa) != 0) {
            LOG_V3DFX_BASE("ERROR: BCIOSET_BUFFERADDR[%d]: failed (0x%lx)\n",
                    buf_pa.index, buf_pa.pa);
            return 1;
        }
        return 0;
}

int TISGXStreamIMGSTREAMDevice::convert_pvr_to_bc_pixel_format(int pvr_format)
{
	int bc_pix_format;
	if      (pvr_format == PVRSRV_PIXEL_FORMAT_YUYV)   bc_pix_format = BC_PIX_FMT_YUYV;
	else if (pvr_format == PVRSRV_PIXEL_FORMAT_RGB565) bc_pix_format = BC_PIX_FMT_RGB565;
	else if (pvr_format == PVRSRV_PIXEL_FORMAT_ARGB8888)   bc_pix_format = BC_PIX_FMT_ARGB;
	else if (pvr_format == PVRSRV_PIXEL_FORMAT_UYVY)   bc_pix_format = BC_PIX_FMT_UYVY;
	else 
	{
		LOG_V3DFX_BASE(" Unsupported pixel format \n");
		return -1;
	}
	return bc_pix_format;
}



int TISGXStreamIMGSTREAMDevice::initialise_imgstream_km(
	unsigned long *paArray)
{
	int bc_pixel_format = convert_pvr_to_bc_pixel_format(attributes.pvrPixelFormat);
	bool retval = init_bcdev_without_pa(
			deviceId, 
			bc_pixel_format, 
			attributes.widthPixels,
			attributes.heightPixels,
			attributes.numBuffers);
	if(!retval)
	{
		return 1;
	}
	//If a paArray is passed, initialise km buffers right away
	for(int bcount = 0;bcount < attributes.numBuffers;bcount ++)
	{
		if(paArray[bcount] == 0) 
		{		
			continue;
		}

		//For USERPTR mode, set the buffer pointer
		LOG_V3DFX_BASE("About to BCIOSET_BUFFERADDR 0x%x \n", (unsigned int)paArray[bcount]);            
		if(set_tex_buf_addr(bcount, paArray[bcount]))
		{
			LOG_V3DFX_BASE("Error BCIOSET_BUFFERADDR failed\n");
			return 2;
	    	}
	}
	return 0;	
}

/* Initialize the texture streaming extension - without actually
allocating the buffers, only pass the count */
bool TISGXStreamIMGSTREAMDevice::init_bcdev_without_pa(
		int bcdev_id, 
		unsigned int pix_frmt, 
		int width, 
		int height,
		int num_channels)
{
	char bcdev_name[] = "/dev/bccatX";
	BCIO_package ioctl_var;
	bc_buf_params_t buf_param;


	buf_param.width  = width;
	buf_param.height = height;
	buf_param.count  = num_channels; 
	buf_param.fourcc = pix_frmt;
	buf_param.type   = BC_MEMORY_USERPTR;

	bcdev_name[strlen(bcdev_name)-1] = '0' + bcdev_id;
	bcfd = open(bcdev_name, O_RDWR|O_NDELAY);
	if (bcfd == -1) {
        	LOG_V3DFX_BASE("ERROR: open %s failed\n", bcdev_name);
	        return false;
	}	

	if (ioctl(bcfd, BCIOREQ_BUFFERS, &buf_param) != 0) {
		LOG_V3DFX_BASE("ERROR: BCIOREQ_BUFFERS failed\n");
		return false;
	}

	if (ioctl(bcfd, BCIOGET_BUFFERCOUNT, &ioctl_var) != 0) {
		LOG_V3DFX_BASE("ERROR: BCIOGET_BUFFERCOUNT failed\n");
		return false;
	}

	if (ioctl_var.output == 0) {
		LOG_V3DFX_BASE("ERROR: no texture buffer available\n");
		return false;
	}
	LOG_V3DFX_BASE("INFO: MAX Number of Texture Buffers: %d\n", ioctl_var.output);

	return true;
}


int TISGXStreamIMGSTREAMDevice::initialise_imgstream_gl()
{
	myglTexBindStreamIMG = (PFNGLTEXBINDSTREAMIMGPROC)
		eglGetProcAddress("glTexBindStreamIMG");
	if(!myglTexBindStreamIMG)
	{
		LOG_V3DFX_BASE("TexBindStreamIMG eglGetProcAddress failed\n");
		return 2;
	}
	glTexParameteri(GL_TEXTURE_STREAM_IMG, 
			GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_STREAM_IMG, 
			GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenTextures(maxBufferCount, texIdArray);
	return 0;
}

int TISGXStreamIMGSTREAMDevice::qTexImage2DBuf(void* fullBufPhyAddrArray)
{
	unsigned long * paArray = (unsigned long*)fullBufPhyAddrArray;

	//If a paArray is passed, initialise km buffers right away
	for(int paCount = 0;paCount < maxBufferCount;paCount ++)
	{
		unsigned long currPa = paArray[paCount];
		if(currPa == 0) // No need to update device here
		{
			continue;
		}
		//For USERPTR mode, set the buffer pointer
		LOG_V3DFX_BASE("About to BCIOSET_BUFFERADDR %x\n", (unsigned int)currPa);            
		if(set_tex_buf_addr(paCount, currPa))
		{
			LOG_V3DFX_BASE("Error BCIOSET_BUFFERADDR failed\n");
			return 2;
	    	}
		LOG_V3DFX_BASE("Completed BCIOSET_BUFFERADDR %x\n", (unsigned int)currPa);
		//TODO - store texobjects	
		//TODO: Avoid deletion first time
		glDeleteTextures(1, &texIdArray[paCount]);
		glGenTextures(1, &texIdArray[paCount]);
		glBindTexture(GL_TEXTURE_STREAM_IMG, texIdArray[paCount]);
		glTexParameterf(GL_TEXTURE_STREAM_IMG, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_STREAM_IMG, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		myglTexBindStreamIMG(deviceId, paCount);

		//Remember, and set this buffer as in use
		bufferState[paCount].pa = currPa;
		bufferState[paCount].state = 1;
	}

	return 0;
}
int TISGXStreamIMGSTREAMDevice::dqTexImage2DBuf(void* freeBufPhyAddr)
{
	//TODO:
	//*((int*)freeBufPhyAddr) = next_indicated_by_signal_draw;
	
	unsigned long * paArray = (unsigned long*)freeBufPhyAddr;
	for(int bcount = 0;bcount < maxBufferCount;bcount ++)
	{
		if(bufferState[bcount].state == 0)
		{
			paArray[bcount] = bufferState[bcount].pa;
			bufferState[bcount].pa = NULL;
		}
	}
	return 0;
}



TISGXStreamTexIMGSTREAM::TISGXStreamTexIMGSTREAM() {}
TISGXStreamTexIMGSTREAM::~TISGXStreamTexIMGSTREAM() {}
int TISGXStreamTexIMGSTREAM::init(int streamDeviceId)
{
	LOG_V3DFX_BASE("%s initialised\n", __func__);
	return 0;
}
int TISGXStreamTexIMGSTREAM::load_v_shader(char const* vshader)
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
int TISGXStreamTexIMGSTREAM::load_f_shader(char const* fshader)
{
	int err = 0;

	const char* pszFragIMGTextureStreamShader = "\
			 #ifdef GL_IMG_texture_stream2\n \
			 #extension GL_IMG_texture_stream2 : enable \n \
			 #endif \n \
			 varying mediump vec2 TexCoord; \
			 uniform samplerStreamIMG sTexture; \
			 void main(void) \
			 {	\
				gl_FragColor = textureStreamIMG(sTexture, TexCoord); \
			 }";

	if(fshader == NULL)
	{
		err = TISGXStreamTexBase::load_f_shader(pszFragIMGTextureStreamShader);
	}
	else
	{
		LOG_V3DFX_BASE("Cannot handle new f shader\n");
	}
	return err;
		
}
int TISGXStreamTexIMGSTREAM::load_program()
{
	return TISGXStreamTexBase::load_program();
}



#ifdef __cplusplus
}
#endif 

