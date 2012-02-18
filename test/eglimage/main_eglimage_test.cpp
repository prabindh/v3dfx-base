/*****************************************************************************
 *   Preliminary test code for v3dfxbase - reused from sgxperf project
 * Main test code for eglimage - derived from sgxperf, and 
 * Rob Clark's work at  
 * https://github.com/robclark/xbmc/commits/gstreamer-eglimg
 * Allows calling into v3dfx-test-base
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

#include <stdio.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <sys/time.h> //for profiling
#include <stdlib.h> //for profiling
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <math.h>
#ifdef _ENABLE_CMEM
#include <cmem.h> //for contiguous physical memory allocation. Always used.
#endif //CMEM

#include "v3dfx_eglimage.h"

extern void set_mvp(int);
extern int test_eglimagekhr_init_texture_streaming();
extern int test_eglimagekhr_deinit_texture_streaming();
extern int common_init_gl_vertices(int numObjectsPerSide, GLfloat **vertexArray);
extern int common_init_gl_texcoords(int numObjectsPerSide, GLfloat **textureCoordArray);
extern void common_gl_draw(int numObjects);
extern int program_setup(int testID);
extern int program_cleanup(int testID);
extern void common_eglswapbuffers(
						   EGLDisplay eglDisplay, 
						   EGLSurface eglSurface
						   );


extern EGLDisplay eglDisplay;
extern int inTextureWidth;
extern int inTextureHeight;
extern int inNumberOfObjectsPerSide;
extern int physicalAddress;
extern EGLSurface eglSurface;
/********************************************************************
TEST21 - with v3dfxbase classes
********************************************************************/
TISGXStreamTexEGLIMAGE* texClass;
TISGXStreamEGLIMAGEDevice* deviceClass;

    EGLint eglAttributes[] = {
            EGL_GL_VIDEO_FOURCC_TI,      FOURCC_STR("UYVY"),
            EGL_GL_VIDEO_WIDTH_TI,       inTextureWidth,
            EGL_GL_VIDEO_HEIGHT_TI,      inTextureHeight,
            EGL_GL_VIDEO_BYTE_STRIDE_TI, inTextureWidth,
            EGL_GL_VIDEO_BYTE_SIZE_TI,   inTextureWidth*inTextureHeight*2,
            // TODO: pick proper YUV flags..
            EGL_GL_VIDEO_YUV_FLAGS_TI,   EGLIMAGE_FLAGS_YUV_CONFORMANT_RANGE |
            EGLIMAGE_FLAGS_YUV_BT601,
            EGL_NONE
    };

eglimage_device_attributes tempAttrib = {inTextureWidth, inTextureHeight, 2, 
					inTextureWidth*inTextureHeight*2, 
				PVRSRV_PIXEL_FORMAT_YUV420,
					eglDisplay,
					eglAttributes, 1};
int lastDeviceClass = 0;

//FOR TEST ONLY - will really come from CMEM_getPhysAddr or similar
unsigned long paArray[] = {0,0}; 
unsigned long freeArray[] = {0, 0, 0, 0};  

/******************************************************************
Simple test code for eglimageKHR based streaming.
Note the similarity with imgstream based streaming due to the 
usage of v3dfx-base
******************************************************************/

void test21_eglimagekhr()
{
	int matrixLocation;
	deviceClass = new TISGXStreamEGLIMAGEDevice();
	texClass = new TISGXStreamTexEGLIMAGE();


	test_eglimagekhr_init_texture_streaming();

	paArray[0] = physicalAddress;
	deviceClass->init(&tempAttrib, lastDeviceClass, paArray);
	texClass->init(lastDeviceClass);
	texClass->load_v_shader(NULL);
	texClass->load_f_shader(NULL);
	texClass->load_program();
	matrixLocation = texClass->get_uniform_location("MVPMatrix");

	set_mvp(matrixLocation);

	//initialise gl vertices
	float *pVertexArray, *pTexCoordArray;
	common_init_gl_vertices(inNumberOfObjectsPerSide, &pVertexArray);
	common_init_gl_texcoords(inNumberOfObjectsPerSide, &pTexCoordArray);

	//Rendering loop
	glClear(GL_COLOR_BUFFER_BIT);
	deviceClass->qTexImage2DBuf(&paArray);

	common_gl_draw(inNumberOfObjectsPerSide);
	common_eglswapbuffers (eglDisplay, eglSurface);

	deviceClass->signal_draw(0); //only 1 buffer
	deviceClass->dqTexImage2DBuf(&freeArray);

	deviceClass->destroy();

	test_eglimagekhr_deinit_texture_streaming();
}


int main()
{
	program_setup(3);
	test21_eglimagekhr();
	program_cleanup(3);
	return 0;
}
