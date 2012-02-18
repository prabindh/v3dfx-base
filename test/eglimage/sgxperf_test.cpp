/*****************************************************************************
 *   Preliminary test code for v3dfxbase - reused from sgxperf project
 * This reuses sgxperf code - to provide reference for USERPTR
 * approach for allocation
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


#ifdef _ENABLE_KNOWN_TEXTURES
#include "sgxperf.h"
extern unsigned int _photoframe_pvr_1024x1024[];
extern unsigned int _photoframe_565_1024x1024[];
extern unsigned char yukio_160x128yuv[];
#endif

#define SGX_PERF_printf printf
#define SGX_PERF_ERR_printf printf

// Index to bind the attributes to vertex shaders
#define VERTEX_ARRAY 0
#define TEXCOORD_ARRAY 1

//Bit types
#define SGXPERF_RGB565 0
#define SGXPERF_ARGB8888 2
#define SGXPERF_BYTE8 1 /* Luminance only */
//surfaceType
#define SGXPERF_SURFACE_TYPE_WINDOW 0
#define SGXPERF_SURFACE_TYPE_PIXMAP_16 1
#define SGXPERF_SURFACE_TYPE_PIXMAP_32 2

// dynamically allocated data FOR variable size texture
unsigned int *textureData = 0;
void calculate_rotation_z(float	*mOut,	const float fAngle);
void calculate_rotation_y(float	*mOut,	const float fAngle);
void calculate_rotation_x(float	*mOut,	const float fAngle);
void matrix_mult(float *mA, float *mB, float *mRet);


//common to all test cases (with or without textures)
typedef struct _NATIVE_PIXMAP_STRUCT
{
    long ePixelFormat;
    long eRotation;
    long lWidth;
    long lHeight;
    long lStride;
    long lSizeInBytes;
    long pvAddress;
    long lAddress;
}NATIVE_PIXMAP_STRUCT;

/** GLOBALS **/
EGLConfig eglConfig	= 0;
EGLDisplay eglDisplay = 0;
EGLSurface eglSurface = 0;
EGLSurface eglSurface2 = 0;
EGLContext eglContext = 0;
GLuint textureId0;
int matrixLocation;
float mat_x[16], mat_y[16], mat_z[16], mat_temp[16], mat_final[16];
int inTextureWidth = 256;
int inTextureHeight = 256;
int inRotationEnabled = 0;
int inPixelFormat = SGXPERF_RGB565;
char *inSvgFileName;
int inNumberOfObjectsPerSide = 1;
int inSurfaceType = SGXPERF_SURFACE_TYPE_WINDOW;
int windowWidth, windowHeight;
int quitSignal = 0;
int numTestIterations = 10, inFPS=1;
unsigned int msToSleep = 0;
char* cookie;

int common_init_gl_vertices(int numObjectsPerSide, GLfloat **vertexArray);
int common_init_gl_texcoords(int numObjectsPerSide, GLfloat **textureCoordArray);


/*********************** TEST @ SGXPERF ****************/
#include <sys/mman.h>       // mmap()
#if defined __linux__
#define LINUX //needed for imgdefs
#endif
#include <img_types.h>
#include <servicesext.h>
#include <fcntl.h>
#include "sys/ioctl.h"
#include <unistd.h>

#define MAX_BUFFERS 1

char *buf_paddr[MAX_BUFFERS];
char *buf_vaddr[MAX_BUFFERS] = { (char *) MAP_FAILED };


extern void common_deinit_gl_vertices(GLfloat *vertexArray);
extern void common_eglswapbuffers(
						   EGLDisplay eglDisplay, 
						   EGLSurface eglSurface
						   );


void set_mvp(int matrixLocation)
{
	//set rotation variables to init
	calculate_rotation_z(mat_z, 0);
	calculate_rotation_y(mat_y, 0);
	calculate_rotation_x(mat_x, 0);
	matrix_mult(mat_z, mat_y, mat_temp);
	matrix_mult(mat_temp, mat_x, mat_final);
	glUniformMatrix4fv( matrixLocation, 1, GL_FALSE, mat_final);
}

//Common function to draw the previously input vertices
//Texture mapping if needed, has to be done before
void common_gl_draw(int numObjects)
{
	int startIndex = 0;
	static int alreadyDone = 0;
	
	for(int vertical = 0;vertical < numObjects;vertical ++)
		for(int horizontal = 0;horizontal < numObjects;horizontal ++)
		{
			{
				// Create and Bind texture
				//glGenTextures(1, &textureId0);
				//glBindTexture(GL_TEXTURE_2D, textureId0);
				//add_texture(inTextureWidth, inTextureHeight, textureData, inPixelFormat);
				alreadyDone = 1;
			}
			glDrawArrays(GL_TRIANGLE_STRIP, startIndex, 4);
			{
				glDeleteTextures(1, &textureId0);
			}
			startIndex += 4;
		}
}

CMEM_AllocParams cmemParams = { CMEM_POOL, CMEM_NONCACHED, 4096 };
void* virtualAddress; 
int physicalAddress;

void eglimage_gl_draw(int numObjects)
{
	int startIndex = 0;

	for(int vertical = 0;vertical < numObjects;vertical ++)
		for(int horizontal = 0;horizontal < numObjects;horizontal ++)
		{
			//update texture here if needed
			//memset(textureData);
			glDrawArrays(GL_TRIANGLE_STRIP, startIndex, 4);			
			startIndex += 4;
		}
}





/* Signal handler for clean closure of GL */
void sgxperf_signal_handler(int reason) 
{ 
  SGX_PERF_ERR_printf("\nGot quit signal - Results will be inaccurate!\n"); 
  quitSignal = 1; 
}

bool TestEGLError(const char* pszLocation)
{
	EGLint iErr = eglGetError();
	if (iErr != EGL_SUCCESS)
	{
		SGX_PERF_ERR_printf("%s failed (%d).\n", pszLocation, iErr);
		return false;
	}

	return true;
}

/******************************************************
* Function to create an ARGB texture
* pixelformat can be different from the display format
*******************************************************/
void set_texture(int width, int height, unsigned char* pTex,
					int pixelFormat)
{
	int numbytes=0;
	if(pixelFormat == SGXPERF_ARGB8888) numbytes = 4; //ARGB888
	if(pixelFormat == SGXPERF_RGB565) numbytes = 2; //RGB565
	if(pixelFormat == SGXPERF_BYTE8) numbytes = 1; //LUMINANCE

#ifdef _ENABLE_KNOWN_TEXTURES
	// _photoframe_pvr_1024x1024 is in ARGB format
	if(width == 1024 && height == 1024 && numbytes == 4)
		memcpy(pTex, _photoframe_pvr_1024x1024, width*height*numbytes);
	else if(width == 1024 && height == 1024 && numbytes == 2)
		memcpy(pTex, _photoframe_565_1024x1024, width*height*numbytes);		
	else if(width == 256 && height == 256 && numbytes == 4)
		memcpy(pTex, _fan256x256_argb, width*height*numbytes);	
	else if(width == 256 && height == 256 && numbytes == 2)
		memcpy(pTex, _fan256x256_rgb565, width*height*numbytes);
	else if(width == 256 && height == 256 && numbytes == 1)
		memcpy(pTex, lena_256x256_8bit, width*height*numbytes);
	else
#endif
	{
		for(int i = 0;i < width*height*numbytes;i ++)
			pTex[i] = i;
	}
}

//egl init
int common_eglinit(int testID, int surfaceType, NATIVE_PIXMAP_STRUCT** pNativePixmapPtr)
{
	EGLint iMajorVersion, iMinorVersion;
	EGLint ai32ContextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
	eglDisplay = eglGetDisplay((int)0);

	if (!eglInitialize(eglDisplay, &iMajorVersion, &iMinorVersion))
		return 1;

	if(testID == 9)
		eglBindAPI(EGL_OPENVG_API);
	else
		eglBindAPI(EGL_OPENGL_ES_API);
	if (!TestEGLError("eglBindAPI"))
		return 1;

	EGLint pi32ConfigAttribs[5];
	pi32ConfigAttribs[0] = EGL_SURFACE_TYPE;
	pi32ConfigAttribs[1] = EGL_WINDOW_BIT | EGL_PIXMAP_BIT;
	pi32ConfigAttribs[2] = EGL_RENDERABLE_TYPE;
	if(testID == 9)
		pi32ConfigAttribs[3] = EGL_OPENVG_BIT;
	else
		pi32ConfigAttribs[3] = EGL_OPENGL_ES2_BIT;
	pi32ConfigAttribs[4] = EGL_NONE;

	int iConfigs;
	if (!eglChooseConfig(eglDisplay, pi32ConfigAttribs, &eglConfig, 1, &iConfigs) || (iConfigs != 1))
	{
		SGX_PERF_ERR_printf("Error: eglChooseConfig() failed.\n");
		return 1;
	}
	if(surfaceType == SGXPERF_SURFACE_TYPE_WINDOW)
		eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, (void*) NULL, NULL);
	else //No pixmap in this
	    return 999;

	if (!TestEGLError("eglCreateSurface"))
		return 1;
		
	if(testID == 9)
		eglContext = eglCreateContext(eglDisplay, eglConfig, NULL, NULL);
	else
		eglContext = eglCreateContext(eglDisplay, eglConfig, NULL, ai32ContextAttribs);
	if (!TestEGLError("eglCreateContext"))
		return 1;

	eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
	if (!TestEGLError("eglMakeCurrent"))
		return 1;

	eglSwapInterval(eglDisplay, 1);
	if (!TestEGLError("eglSwapInterval"))
		return 1;

	eglQuerySurface(eglDisplay, eglSurface, EGL_WIDTH, &windowWidth);
	eglQuerySurface(eglDisplay, eglSurface, EGL_HEIGHT, &windowHeight);
	
	fprintf(stderr,"Window width=%d, Height=%d\n", windowWidth, windowHeight);

	return 0;
}
void common_egldeinit(int testID, NATIVE_PIXMAP_STRUCT* pNativePixmap)
{
	eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT) ;
	eglDestroyContext(eglDisplay, eglContext);
	eglDestroySurface(eglDisplay, eglSurface);
	if(testID == 14)
	  eglDestroySurface(eglDisplay, eglSurface2);	
	eglTerminate(eglDisplay);
}

//swapping buffers
void common_eglswapbuffers(
						   EGLDisplay eglDisplay, 
						   EGLSurface eglSurface
						   )
{
	if(inSurfaceType == SGXPERF_SURFACE_TYPE_WINDOW)
		eglSwapBuffers(eglDisplay, eglSurface);
	else if(inSurfaceType == SGXPERF_SURFACE_TYPE_PIXMAP_16 || inSurfaceType == SGXPERF_SURFACE_TYPE_PIXMAP_32)
		eglWaitGL();
}
//Vertices init
int common_init_gl_vertices(int numObjectsPerSide, GLfloat **vertexArray)
{
	GLfloat currX = -1, currY = 1, currZ = -1; //set to top left
	GLfloat objWidth = 2/(float)numObjectsPerSide, objHeight = 2/(float)numObjectsPerSide;

	GLfloat *afVertices = (GLfloat*)malloc(4*3*sizeof(GLfloat)*numObjectsPerSide*numObjectsPerSide);
	*vertexArray = NULL;
	if(afVertices == NULL)
		return 1;
	*vertexArray = afVertices;	
	for(int vertical = 0;vertical < numObjectsPerSide; vertical ++)
	{
		for(int horizontal = 0;horizontal < numObjectsPerSide; horizontal ++)
		{
			//p0
			afVertices[vertical*4*3*numObjectsPerSide + horizontal*4*3] = currX;
			afVertices[vertical*4*3*numObjectsPerSide + horizontal*4*3 + 1] = currY;
			afVertices[vertical*4*3*numObjectsPerSide + horizontal*4*3 + 2] = currZ;
			//p1
			afVertices[vertical*4*3*numObjectsPerSide + horizontal*4*3 + 3] = currX;
			afVertices[vertical*4*3*numObjectsPerSide + horizontal*4*3 + 4] = currY - objHeight;
			afVertices[vertical*4*3*numObjectsPerSide + horizontal*4*3 + 5] = currZ;
			//p2
			afVertices[vertical*4*3*numObjectsPerSide + horizontal*4*3 + 6] = currX + objWidth;
			afVertices[vertical*4*3*numObjectsPerSide + horizontal*4*3 + 7] = currY;
			afVertices[vertical*4*3*numObjectsPerSide + horizontal*4*3 + 8] = currZ;
			//p3
			afVertices[vertical*4*3*numObjectsPerSide + horizontal*4*3 + 9] = currX + objWidth;
			afVertices[vertical*4*3*numObjectsPerSide + horizontal*4*3 + 10] = currY - objHeight;
			afVertices[vertical*4*3*numObjectsPerSide + horizontal*4*3 + 11] = currZ;

			currX += objWidth;
		}
		currX = -1; //reset
		currY -= objHeight;
	}
	//for(int temp = 0;temp < 4*numObjectsPerSide*2;temp ++)
	//	printf("vertex=%f %f %f\n", afVertices[temp], afVertices[temp + 1], afVertices[temp + 2] );

	glEnableVertexAttribArray(VERTEX_ARRAY);
	glVertexAttribPointer(VERTEX_ARRAY, 3, GL_FLOAT, GL_FALSE, 0, (const void*)afVertices);

	return 0;
}

void common_deinit_gl_vertices(GLfloat *vertexArray)
{
	glDisableVertexAttribArray(VERTEX_ARRAY);
	if(vertexArray)
		free(vertexArray);
}

void common_deinit_gl_texcoords(GLfloat *pTexCoordArray)
{
	glDisableVertexAttribArray(TEXCOORD_ARRAY);
	if(pTexCoordArray)
		free(pTexCoordArray);
}

//Texture Coordinates
int common_init_gl_texcoords(int numObjectsPerSide, GLfloat **textureCoordArray)
{//full span
	GLfloat currX = -1, currY = 1; //set to top left
	GLfloat objWidth = 2/(float)numObjectsPerSide, objHeight = 2/(float)numObjectsPerSide;

	GLfloat *afVertices = (GLfloat*)malloc(4*2*sizeof(GLfloat)*numObjectsPerSide*numObjectsPerSide);
	*textureCoordArray = NULL;
	if(afVertices == NULL)
		return 1;
	*textureCoordArray = afVertices;	
	for(int vertical = 0;vertical < numObjectsPerSide; vertical ++)
	{
		for(int horizontal = 0;horizontal < numObjectsPerSide; horizontal ++)
		{
			//p0
			afVertices[vertical*4*2*numObjectsPerSide + horizontal*4*2] = 0;
			afVertices[vertical*4*2*numObjectsPerSide + horizontal*4*2 + 1] = 1;
			//p1
			afVertices[vertical*4*2*numObjectsPerSide + horizontal*4*2 + 2] = 0;
			afVertices[vertical*4*2*numObjectsPerSide + horizontal*4*2 + 3] = 0;
			//p2
			afVertices[vertical*4*2*numObjectsPerSide + horizontal*4*2 + 4] = 1;
			afVertices[vertical*4*2*numObjectsPerSide + horizontal*4*2 + 5] = 1;
			//p3
			afVertices[vertical*4*2*numObjectsPerSide + horizontal*4*2 + 6] = 1;
			afVertices[vertical*4*2*numObjectsPerSide + horizontal*4*2 + 7] = 0;

			currX += objWidth;
		}
		currX = -1; //reset
		currY -= objHeight;
	}
	//for(int temp = 0;temp < 4*numObjectsPerSide*2;temp ++)
	//	printf("vertex=%f %f\n", afVertices[temp], afVertices[temp + 1]);

	glEnableVertexAttribArray(TEXCOORD_ARRAY);
	glVertexAttribPointer(TEXCOORD_ARRAY, 2, GL_FLOAT, GL_FALSE, 0, (const void*)afVertices);

	return 0;
}

//matrix mult
void calculate_rotation_y(
	float	*mOut,
	const float fAngle)
{
	float		fCosine, fSine;
	fCosine =	(float)cos(fAngle);
    fSine =		(float)sin(fAngle);
	mOut[ 0]=fCosine;		mOut[ 4]=0.f;	mOut[ 8]=fSine;	mOut[12]=0.0f;
	mOut[ 1]=0.f;		mOut[ 5]=1.0f;	mOut[ 9]=0.0f;	mOut[13]=0.0f;
	mOut[ 2]=-fSine;		mOut[ 6]=0.0f;	mOut[10]=fCosine;	mOut[14]=0.0f;
	mOut[ 3]=0.0f;		mOut[ 7]=0.0f;	mOut[11]=0.0f;	mOut[15]=1.0f;
}


void calculate_rotation_x(
	float	*mOut,
	const float fAngle)
{
	float		fCosine, fSine;
	fCosine =	(float)cos(fAngle);
    fSine =		(float)sin(fAngle);
	/* Create the trigonometric matrix corresponding to about x */
	mOut[ 0]=1.0f;		mOut[ 4]=0.0f;	mOut[ 8]=0.0f;	mOut[12]=0.0f;
	mOut[ 1]=0.0f;		mOut[ 5]=fCosine;	mOut[ 9]=-fSine;	mOut[13]=0.0f;
	mOut[ 2]=0.0f;		mOut[ 6]=fSine;	mOut[10]=fCosine;	mOut[14]=0.0f;
	mOut[ 3]=0.0f;		mOut[ 7]=0.0f;	mOut[11]=0.0f;	mOut[15]=1.0f;
}



void calculate_rotation_z(
	float	*mOut,
	const float fAngle)
{
	float		fCosine, fSine;
	fCosine =	(float)cos(fAngle);
    fSine =		(float)sin(fAngle);
	/* Create the trigonometric matrix corresponding to Rotation about z */
	mOut[ 0]=fCosine;		mOut[ 4]=-fSine;	mOut[ 8]=0.0f;	mOut[12]=0.0f;
	mOut[ 1]=fSine;		mOut[ 5]=fCosine;	mOut[ 9]=0.0f;	mOut[13]=0.0f;
	mOut[ 2]=0.0f;		mOut[ 6]=0.0f;	mOut[10]=1.0f;	mOut[14]=0.0f;
	mOut[ 3]=0.0f;		mOut[ 7]=0.0f;	mOut[11]=0.0f;	mOut[15]=1.0f;
}

void matrix_mult(
				 float *mA,
				 float *mB,
				 float *mRet
				 )
{
	/* Perform calculation on a dummy matrix (mRet) */
	mRet[ 0] = mA[ 0]*mB[ 0] + mA[ 1]*mB[ 4] + mA[ 2]*mB[ 8] + mA[ 3]*mB[12];
	mRet[ 1] = mA[ 0]*mB[ 1] + mA[ 1]*mB[ 5] + mA[ 2]*mB[ 9] + mA[ 3]*mB[13];
	mRet[ 2] = mA[ 0]*mB[ 2] + mA[ 1]*mB[ 6] + mA[ 2]*mB[10] + mA[ 3]*mB[14];
	mRet[ 3] = mA[ 0]*mB[ 3] + mA[ 1]*mB[ 7] + mA[ 2]*mB[11] + mA[ 3]*mB[15];

	mRet[ 4] = mA[ 4]*mB[ 0] + mA[ 5]*mB[ 4] + mA[ 6]*mB[ 8] + mA[ 7]*mB[12];
	mRet[ 5] = mA[ 4]*mB[ 1] + mA[ 5]*mB[ 5] + mA[ 6]*mB[ 9] + mA[ 7]*mB[13];
	mRet[ 6] = mA[ 4]*mB[ 2] + mA[ 5]*mB[ 6] + mA[ 6]*mB[10] + mA[ 7]*mB[14];
	mRet[ 7] = mA[ 4]*mB[ 3] + mA[ 5]*mB[ 7] + mA[ 6]*mB[11] + mA[ 7]*mB[15];

	mRet[ 8] = mA[ 8]*mB[ 0] + mA[ 9]*mB[ 4] + mA[10]*mB[ 8] + mA[11]*mB[12];
	mRet[ 9] = mA[ 8]*mB[ 1] + mA[ 9]*mB[ 5] + mA[10]*mB[ 9] + mA[11]*mB[13];
	mRet[10] = mA[ 8]*mB[ 2] + mA[ 9]*mB[ 6] + mA[10]*mB[10] + mA[11]*mB[14];
	mRet[11] = mA[ 8]*mB[ 3] + mA[ 9]*mB[ 7] + mA[10]*mB[11] + mA[11]*mB[15];

	mRet[12] = mA[12]*mB[ 0] + mA[13]*mB[ 4] + mA[14]*mB[ 8] + mA[15]*mB[12];
	mRet[13] = mA[12]*mB[ 1] + mA[13]*mB[ 5] + mA[14]*mB[ 9] + mA[15]*mB[13];
	mRet[14] = mA[12]*mB[ 2] + mA[13]*mB[ 6] + mA[14]*mB[10] + mA[15]*mB[14];
	mRet[15] = mA[12]*mB[ 3] + mA[13]*mB[ 7] + mA[14]*mB[11] + mA[15]*mB[15];
}


int test_eglimagekhr_init_texture_streaming()
{

    //setup CMEM pointers  
    virtualAddress = CMEM_alloc(inTextureWidth*inTextureHeight*4, &cmemParams);
    if(!virtualAddress)                                      
    {
	  SGX_PERF_printf("Error in CMEM_alloc\n");
	  return 1;
    }
    physicalAddress = CMEM_getPhys(virtualAddress);
    if(!physicalAddress)                                      
    {
	  SGX_PERF_printf("Error in CMEM_getPhys\n");
	  return 2;
    }
	return 0;
}
void test_eglimagekhr_deinit_texture_streaming()
{
    if(virtualAddress)
      CMEM_free(virtualAddress, &cmemParams);
}



int program_setup(int testID)
{
	int err;

	signal(SIGINT, sgxperf_signal_handler);

	//Allocate texture for use in GL texturing modes(can also be done from CMEM if memory permits
	textureData = (unsigned int*)malloc(inTextureWidth*inTextureHeight*4);
	if(!textureData)
		SGX_PERF_ERR_printf("ERROR: No malloc memory for allocating texture!\n");
	set_texture(inTextureWidth, inTextureHeight, (unsigned char*)textureData, inPixelFormat);

	//initialise egl
	err = common_eglinit(testID, inSurfaceType, NULL);
	if(err)
	{
		SGX_PERF_ERR_printf("ERROR: eglinit - err = %d\n", err);
		return 1;
	}

	/* Set clear */
	glClearColor(0.2f, 0.8f, 1.0f, 1.0f); 
	glClear(GL_COLOR_BUFFER_BIT);
	
#ifdef _ENABLE_CMEM
	//Initialise the CMEM module. 
	//CMEM ko should be inserted before this point
	SGX_PERF_printf("Configuring CMEM\n");
	CMEM_init();
#endif

	return 0;
}


void program_cleanup(int testID)
{
	if(textureData) 
		free(textureData);
	common_egldeinit(testID, NULL);
#ifdef _ENABLE_CMEM
	CMEM_exit();
#endif
}
