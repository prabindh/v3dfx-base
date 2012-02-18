/*****************************************************************************
 *   Preliminary test code for v3dfxbase - reused from sgxperf project
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

#include "v3dfx_imgstream.h"

#include <EGL/egl.h>

extern float mat_x[], mat_y[], mat_z[], mat_temp[], mat_final[];
extern void calculate_rotation_z(float	*mOut,	const float fAngle);
extern void calculate_rotation_y(float	*mOut,	const float fAngle);
extern void calculate_rotation_x(float	*mOut,	const float fAngle);
extern void matrix_mult(float *mA, float *mB, float *mRet);


extern int common_init_gl_vertices(int numObjectsPerSide, GLfloat **vertexArray);
extern int common_init_gl_texcoords(int numObjectsPerSide, GLfloat **textureCoordArray);

extern void common_deinit_gl_vertices(GLfloat *vertexArray);
extern void common_eglswapbuffers(
						   EGLDisplay eglDisplay, 
						   EGLSurface eglSurface
						   );
extern void common_deinit_gl_texcoords(GLfloat *pTexCoordArray);
extern void img_stream_gl_draw(int inNumberOfObjectsPerSide);

extern int inTextureWidth;
extern int inTextureHeight;
extern int inNumberOfObjectsPerSide;
extern int quitSignal;
extern EGLDisplay eglDisplay ;
extern EGLSurface eglSurface;
extern int numTestIterations;
extern int matrixLocation;
extern void set_mvp(int);
extern int physicalAddress;
extern void* virtualAddress; 

#define TEXCOORD_ARRAY 1
/********************************************************************
TEST20 - with v3dfxbase classes
********************************************************************/
TISGXStreamTexIMGSTREAM* texClass;
TISGXStreamIMGSTREAMDevice* deviceClass;
imgstream_device_attributes tempAttrib = {inTextureWidth, inTextureHeight, 2, 
				inTextureWidth*inTextureHeight*2,
				PVRSRV_PIXEL_FORMAT_UYVY, 
				2	//2 //2 buffers used
				};
int lastDeviceClass = 0;

//FOR TEST ONLY - will really come from CMEM_getPhysAddr or similar
unsigned long paArray[] = {0, 0};
unsigned long freeArray[] = {0, 0, 0, 0};  

void test20()
{
	int matrixLocation;
	//initialise gl vertices
	static GLfloat texcoord_img[] = 
        {0,0, 1,0, 0,1, 1,1};
	float *pVertexArray, *pTexCoordArray;
	char* currAddress = (char*)virtualAddress;

	common_init_gl_vertices(inNumberOfObjectsPerSide, &pVertexArray);
	common_init_gl_texcoords(inNumberOfObjectsPerSide, &pTexCoordArray);
	//Buff 0
	for(int i = 0;i < inTextureWidth*inTextureHeight*4;i ++)
		currAddress[i] = i & 0xF;
	//Buff 1
	for(int i = 0;i < inTextureWidth*inTextureHeight*4;i ++)
		currAddress[i+inTextureWidth*inTextureHeight*4] = 0xFF;


	deviceClass = new TISGXStreamIMGSTREAMDevice();
	texClass = new TISGXStreamTexIMGSTREAM();

	paArray[0] = physicalAddress;
	paArray[1] = physicalAddress + (inTextureWidth*inTextureHeight*4);

	deviceClass->init(&tempAttrib, lastDeviceClass, paArray);
	texClass->init(lastDeviceClass);
	texClass->load_v_shader(NULL);
	texClass->load_f_shader(NULL);
	texClass->load_program();

	matrixLocation = texClass->get_uniform_location("MVPMatrix");
	set_mvp(matrixLocation);

	//override the texturecoords for this extension only
	glDisableVertexAttribArray(TEXCOORD_ARRAY);
	glEnableVertexAttribArray(TEXCOORD_ARRAY);
	glVertexAttribPointer(TEXCOORD_ARRAY, 2, GL_FLOAT, GL_FALSE, 
		0, (const void*)texcoord_img);


	for(int numiter = 0;(numiter < 100) && (!quitSignal);numiter ++)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		paArray[0] = physicalAddress;
		paArray[1] = physicalAddress + (inTextureWidth*inTextureHeight*4);
		deviceClass->qTexImage2DBuf(paArray);

		img_stream_gl_draw(inNumberOfObjectsPerSide);
		//deviceClass->signal_draw(0); //only one buffer
		//deviceClass->dqTexImage2DBuf(freeArray);

		common_eglswapbuffers (eglDisplay, eglSurface);

		//Try changing data
		for(int i = 0;i < inTextureWidth*inTextureHeight*4;i ++)
			currAddress[i] = (numiter*i) & 0xFF;
		for(int i = 0;i < inTextureWidth*inTextureHeight*4;i ++)
			currAddress[i + inTextureWidth*inTextureHeight*4] = 
					(numiter*i *5) & 0xFF;				

		paArray[0] = physicalAddress;
		paArray[1] = physicalAddress + (inTextureWidth*inTextureHeight*4);
		deviceClass->qTexImage2DBuf(paArray);

		img_stream_gl_draw(inNumberOfObjectsPerSide);
		//deviceClass->signal_draw(1); //only one buffer
		//deviceClass->dqTexImage2DBuf(freeArray);

		common_eglswapbuffers (eglDisplay, eglSurface);
	}

	common_deinit_gl_vertices(pVertexArray);
	common_deinit_gl_texcoords(pTexCoordArray);

	deviceClass->destroy();
}


extern int program_setup(int testID);
extern void program_cleanup(int testID);
extern int allocate_v3dfx_imgstream_bufs(int numbufs);
extern void deallocate_v3dfx_imgstream_bufs();
extern void test8();
/******************************************************************
Main program - exercises streaming extension - IMGSTREAM
******************************************************************/
int main()
{
	int err = program_setup(8);
	if(err) goto cleanup;

#if 0
	/* Regular imgstream */
	test8();	
#else
	/* GL_IMG_texture_stream - via v3dfxbase */
	allocate_v3dfx_imgstream_bufs(2); //2 buffers
	test20();
	deallocate_v3dfx_imgstream_bufs();
#endif

cleanup:
	program_cleanup(8);

	return 0;
}
