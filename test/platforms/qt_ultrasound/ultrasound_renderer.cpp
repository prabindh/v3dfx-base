/*****************************************************************************
 *   Ultrasound rendering for v3dfx - imgstream based
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

#include "ultrasound_renderer.h"

#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "GLES2/gl2.h"
#include "cmem.h"

/**** SGXPERF variables ******/
#define VERTEX_ARRAY 0 //valid only for the shader defined here
#define TEXCOORD_ARRAY 1
float *pVertexArray, *pTexCoordArray;
int inNumberOfObjectsPerSide = 1;
void us_set_mvp(int matrixLocation, float yAngle);
int common_init_gl_texcoords(int numObjectsPerSide, GLfloat **textureCoordArray);
void common_deinit_gl_texcoords(GLfloat *pTexCoordArray);
void common_deinit_gl_vertices(GLfloat *vertexArray);
int common_init_gl_vertices(int numObjectsPerSide, GLfloat **vertexArray);
int common_init_gl_texcoords(int numObjectsPerSide, GLfloat **textureCoordArray);
void img_stream_gl_draw(int numObjects);
/**** SGXPERF  ******/


int us_program_setup()
{
	int err;

	//Initialise the CMEM module. 
	//CMEM ko should be inserted before this point
	printf("Configuring CMEM\n");
	CMEM_init();
	return 0;

}
void us_program_cleanup()
{
	CMEM_exit();
}

//Retrieve size of data file input.img
int us_get_data_size_bytes()
{
	return ULTRASOUND_WIDTH * ULTRASOUND_HEIGHT * ULTRASOUND_LAYERS;
}


CMEM_AllocParams cmemParams = { CMEM_POOL, CMEM_NONCACHED, 4096 };
int us_allocate_v3dfx_imgstream_bufs(int allocBytes, void** virtualAddress, void** physicalAddress)
{
	//setup CMEM pointers  
	*virtualAddress = CMEM_alloc(allocBytes, &cmemParams);
	if(!*virtualAddress)                                      
	{
		printf("Error in CMEM_alloc\n");
		return 1;
	}
	*physicalAddress = (void*) CMEM_getPhys(*virtualAddress);
	if(!*physicalAddress)
	{
		printf("Error in CMEM_getPhys\n");
		return 2;
	}
	return 0;
}
void us_deallocate_v3dfx_imgstream_bufs(void *virtualAddress)
{
	if(virtualAddress)
		CMEM_free(virtualAddress, &cmemParams);
}

//Assumes input.img is GL_RGBA data
int load_original_data(void* virtualAddress, int numbytes)
{
	FILE *file;
	unsigned char *textureData3D = (unsigned char*)virtualAddress;

	int numread;

	if(!textureData3D)
	{
		printf("Invalid input pointer for texture data!\n");
		return 1;
	}
	
	unsigned char* originaldata = (unsigned char*)malloc(numbytes *sizeof(unsigned char));
	if(!originaldata)
	{
		printf("Could not allocate temporary memory of %d bytes!\n", numbytes);
		return 2;
	}
	if ((file = fopen("input.img", "rb")) == NULL) 
	{
		printf("Ultrasound Data File input.img not found in current directory!\n");
		free(originaldata);
		return 3;
	}
	numread = fread(originaldata, sizeof( unsigned char ), numbytes, file);
	fclose(file);

	for(int i = 0; i < numbytes; i++)
	{
		textureData3D[i *4 ] = originaldata[i];
		textureData3D[i *4 +1] = originaldata[i];
		textureData3D[i *4 +2] = originaldata[i];
		textureData3D[i *4 +3] = originaldata[i]; 
	}
	free(originaldata);

	return 0;
}



//Load texture data
void us_render_init_1(void* virtualAddress, int numbytes)
{
	load_original_data(virtualAddress, numbytes);
}

void us_render_init_2()
{
	static GLfloat texcoord_img[] = 
        {0,0, 1,0, 0,1, 1,1};

	common_init_gl_vertices(1, &pVertexArray);
	common_init_gl_texcoords(1, &pTexCoordArray);

	//override the texturecoords for this extension only
	glDisableVertexAttribArray(TEXCOORD_ARRAY);
	glEnableVertexAttribArray(TEXCOORD_ARRAY);
	glVertexAttribPointer(TEXCOORD_ARRAY, 2, GL_FLOAT, GL_FALSE, 
		0, (const void*)texcoord_img);

	//MUST
	glDisableVertexAttribArray(VERTEX_ARRAY);
	glDisableVertexAttribArray(TEXCOORD_ARRAY);
}
void us_render_process_one_prepare(int layerId, int currIteration)
{
	//MUST
	glEnableVertexAttribArray(VERTEX_ARRAY);
	glEnableVertexAttribArray(TEXCOORD_ARRAY);

	common_init_gl_vertices(inNumberOfObjectsPerSide, &pVertexArray);
	common_init_gl_texcoords(inNumberOfObjectsPerSide, &pTexCoordArray);
}
void us_render_process_one_draw(int curriteration)
{
	img_stream_gl_draw(inNumberOfObjectsPerSide);
	//deviceClass->signal_draw(0); //only one buffer
	//deviceClass->dqTexImage2DBuf(freeArray);

	common_deinit_gl_vertices(pVertexArray);
	common_deinit_gl_texcoords(pTexCoordArray);

	printf("glgeterror after draw = %x\n", glGetError());

	//MUST
	glDisableVertexAttribArray(VERTEX_ARRAY);
	glDisableVertexAttribArray(TEXCOORD_ARRAY);


}
void us_render_deinit()
{
	common_deinit_gl_vertices(pVertexArray);
	common_deinit_gl_texcoords(pTexCoordArray);
}


/********* SGXPERF functions ************/
float mat_x[16], mat_y[16], mat_z[16], mat_temp[16], mat_final[16];
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

void us_set_mvp(int matrixLocation, float yAngle)
{
	//set rotation variables to init
	calculate_rotation_z(mat_z, 0);
	calculate_rotation_y(mat_y, yAngle);
	calculate_rotation_x(mat_x, 0);
	matrix_mult(mat_z, mat_y, mat_temp);
	matrix_mult(mat_temp, mat_x, mat_final);
	glUniformMatrix4fv( matrixLocation, 1, GL_FALSE, mat_final);
}

void img_stream_gl_draw(int numObjects)
{
	int startIndex = 0;

	for(int vertical = 0;vertical < numObjects;vertical ++)
		for(int horizontal = 0;horizontal < numObjects;horizontal ++)
		{
			glDrawArrays(GL_TRIANGLE_STRIP, startIndex, 4);			
			startIndex += 4;
		}
}

//Vertices init
int common_init_gl_vertices(int numObjectsPerSide, GLfloat **vertexArray, int id)
{
	GLfloat currX = -1, currY = 1, currZ = -1 + id/ULTRASOUND_LAYERS; //set to top left
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

/****************** SGXPERF functions **********************/


