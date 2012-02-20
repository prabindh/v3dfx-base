#include "v3dfx_imgstream.h"

#include <EGL/egl.h>
#include "cmem.h"


// Index to bind the attributes to vertex shaders
#define VERTEX_ARRAY 0
#define TEXCOORD_ARRAY 1


extern float mat_x[], mat_y[], mat_z[], mat_temp[], mat_final[];
extern void calculate_rotation_z(float	*mOut,	const float fAngle);
extern void calculate_rotation_y(float	*mOut,	const float fAngle);
extern void calculate_rotation_x(float	*mOut,	const float fAngle);
extern void matrix_mult(float *mA, float *mB, float *mRet);

extern void set_texture(int width, int height, unsigned char* pTex,
					int pixelFormat);


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

extern unsigned int *textureData;
extern int inPixelFormat;

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
char* currAddress = 0;
float *pVertexArray, *pTexCoordArray;

void test20_init()
{
	int matrixLocation;
	//initialise gl vertices
	static GLfloat texcoord_img[] = 
        {0,0, 1,0, 0,1, 1,1};

	currAddress = (char*)virtualAddress;
	//Buff 0
	for(int i = 0;i < inTextureWidth*inTextureHeight*4;i ++)
		currAddress[i] = i & 0xF;
	//Buff 1
	for(int i = 0;i < inTextureWidth*inTextureHeight*4;i ++)
		currAddress[i+inTextureWidth*inTextureHeight*4] = 0xFF;

	paArray[0] = physicalAddress;
	paArray[1] = physicalAddress + (inTextureWidth*inTextureHeight*4);

	deviceClass = new TISGXStreamIMGSTREAMDevice();
	texClass = new TISGXStreamTexIMGSTREAM();
	deviceClass->init(&tempAttrib, lastDeviceClass, paArray);
	texClass->init(lastDeviceClass);
	texClass->load_v_shader(NULL);
	texClass->load_f_shader(NULL);
	texClass->load_program();
	matrixLocation = texClass->get_uniform_location("MVPMatrix");
	set_mvp(matrixLocation);





	common_init_gl_vertices(inNumberOfObjectsPerSide, &pVertexArray);
	common_init_gl_texcoords(inNumberOfObjectsPerSide, &pTexCoordArray);

	//override the texturecoords for this extension only
	glDisableVertexAttribArray(TEXCOORD_ARRAY);
	glEnableVertexAttribArray(TEXCOORD_ARRAY);
	glVertexAttribPointer(TEXCOORD_ARRAY, 2, GL_FLOAT, GL_FALSE, 
		0, (const void*)texcoord_img);

	//MUST
	glDisableVertexAttribArray(VERTEX_ARRAY);
	glDisableVertexAttribArray(TEXCOORD_ARRAY);


	//check
	currAddress = (char*)virtualAddress;
	printf("currAddress in %s = %x\n", __func__, currAddress);

	texClass->release_program();
}

void test20_process_one(int currIteration)
{
	currAddress = (char*)virtualAddress;

	texClass->use_program(); 
	//MUST
	glEnableVertexAttribArray(VERTEX_ARRAY);
	glEnableVertexAttribArray(TEXCOORD_ARRAY);


	//printf("currAddress in %s = %x\n", __func__, currAddress);
	{
		glClear(GL_COLOR_BUFFER_BIT);
	common_init_gl_vertices(inNumberOfObjectsPerSide, &pVertexArray);
	common_init_gl_texcoords(inNumberOfObjectsPerSide, &pTexCoordArray);

		paArray[0] = physicalAddress;
		paArray[1] = physicalAddress + (inTextureWidth*inTextureHeight*4);
		deviceClass->qTexImage2DBuf(paArray);

		printf("glgeterror after q = %x\n", glGetError());

		img_stream_gl_draw(inNumberOfObjectsPerSide);
		//deviceClass->signal_draw(0); //only one buffer
		//deviceClass->dqTexImage2DBuf(freeArray);

	common_deinit_gl_vertices(pVertexArray);
	common_deinit_gl_texcoords(pTexCoordArray);



		printf("glgeterror after draw = %x\n", glGetError());

		//Try changing data
		for(int i = 0;i < inTextureWidth*inTextureHeight*4;i ++)
			currAddress[i] = (currIteration*i) & 0xFF;
		for(int i = 0;i < inTextureWidth*inTextureHeight*4;i ++)
			currAddress[i + inTextureWidth*inTextureHeight*4] = 
					(currIteration*i *5) & 0xFF;					
	}

	texClass->release_program();
	//MUST
	glDisableVertexAttribArray(VERTEX_ARRAY);
	glDisableVertexAttribArray(TEXCOORD_ARRAY);



}

void test20_deinit()
{
	common_deinit_gl_vertices(pVertexArray);
	common_deinit_gl_texcoords(pTexCoordArray);

	deviceClass->destroy();
}

int qt_program_setup(int testID)
{
	int err;

	//no need to initialise egl
#ifdef _ENABLE_CMEM
	//Initialise the CMEM module. 
	//CMEM ko should be inserted before this point
	printf("Configuring CMEM\n");
	CMEM_init();
#endif

	return 0;
}

void qt_program_cleanup(int testID)
{
	if(textureData) 
		free(textureData);
#ifdef _ENABLE_CMEM
	CMEM_exit();
#endif
}


///////////////TEST3 - SIMPLE TEXIMAGE2D ////////////////
static GLfloat texcoord_img[] = 
      {0,0, 1,0, 0,1, 1,1};

extern void common_gl_draw(int numObjects);
static GLuint uiProgramObject;					// Used to hold the program handle (made out of the two previous shaders
void test3_init()
{
	int matrixLocation;
	//initialise gl vertices

	//Allocate texture for use in GL texturing modes(can also be done from CMEM if memory permits
	textureData = (unsigned int*)malloc(inTextureWidth*inTextureHeight*4);
	if(!textureData)
		printf("ERROR: No malloc memory for allocating texture!\n");
	set_texture(inTextureWidth, inTextureHeight, (unsigned char*)textureData, inPixelFormat);

	//load shaders
	GLuint uiFragShader, uiVertShader;		// Used to hold the fragment and vertex shader handles
	const char* pszFragTextureShader = "\
		uniform sampler2D  sTexture; \
		varying mediump vec2  TexCoord; \
    mediump vec3 tempColor;\
		void main (void)\
		{\
        tempColor = vec3(texture2D(sTexture, TexCoord));\
				gl_FragColor.r = tempColor.r;\
				gl_FragColor.g = tempColor.g;\
				gl_FragColor.b = tempColor.b;\
				gl_FragColor.a = 0.0; \
		}";
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

	printf("glgeterror 201 = %x\n", glGetError());
	// Create the fragment shader object
	uiFragShader = glCreateShader(GL_FRAGMENT_SHADER);

	// Load the source code into it
	glShaderSource(uiFragShader, 1, (const char**)&pszFragTextureShader, NULL);

	// Compile the source code
	glCompileShader(uiFragShader);

	// Check if compilation succeeded
	GLint bShaderCompiled;
    	glGetShaderiv(uiFragShader, GL_COMPILE_STATUS, &bShaderCompiled);

	if (!bShaderCompiled)
	{
		// An error happened, first retrieve the length of the log message
		int i32InfoLogLength, i32CharsWritten;
		glGetShaderiv(uiFragShader, GL_INFO_LOG_LENGTH, &i32InfoLogLength);

		// Allocate enough space for the message and retrieve it
		char* pszInfoLog = new char[i32InfoLogLength];
	        glGetShaderInfoLog(uiFragShader, i32InfoLogLength, &i32CharsWritten, pszInfoLog);

		// Displays the error
		printf("Failed to compile fragment shader: %s\n", pszInfoLog);
		delete [] pszInfoLog;
		goto cleanup;
	}
	printf("glgeterror 202 = %x\n", glGetError());
	// Loads the vertex shader in the same way
	uiVertShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(uiVertShader, 1, (const char**)&pszVertTextureShader, NULL);

	glCompileShader(uiVertShader);
	glGetShaderiv(uiVertShader, GL_COMPILE_STATUS, &bShaderCompiled);

	if (!bShaderCompiled)
	{
		int i32InfoLogLength, i32CharsWritten;
		glGetShaderiv(uiVertShader, GL_INFO_LOG_LENGTH, &i32InfoLogLength);
		char* pszInfoLog = new char[i32InfoLogLength];
	        glGetShaderInfoLog(uiVertShader, i32InfoLogLength, &i32CharsWritten, pszInfoLog);
		printf("Failed to compile vertex shader: %s\n", pszInfoLog);
		delete [] pszInfoLog;
		goto cleanup;
	}


	printf("glgeterror 202 = %x\n", glGetError());
	// Create the shader program
    uiProgramObject = glCreateProgram();

	// Attach the fragment and vertex shaders to it
	glAttachShader(uiProgramObject, uiFragShader);
	glAttachShader(uiProgramObject, uiVertShader);

	// Bind the custom vertex attribute "myVertex" to location VERTEX_ARRAY
	glBindAttribLocation(uiProgramObject, VERTEX_ARRAY, "inVertex");
	glBindAttribLocation(uiProgramObject, TEXCOORD_ARRAY, "inTexCoord");

	// Link the program
    glLinkProgram(uiProgramObject);

	// Check if linking succeeded in the same way we checked for compilation success
    GLint bLinked;
    glGetProgramiv(uiProgramObject, GL_LINK_STATUS, &bLinked);

	if (!bLinked)
	{
		int ui32InfoLogLength, ui32CharsWritten;
		glGetProgramiv(uiProgramObject, GL_INFO_LOG_LENGTH, &ui32InfoLogLength);
		char* pszInfoLog = new char[ui32InfoLogLength];
		glGetProgramInfoLog(uiProgramObject, ui32InfoLogLength, &ui32CharsWritten, pszInfoLog);
		printf("Failed to link program: %s\n", pszInfoLog);
		delete [] pszInfoLog;
		goto cleanup;
	}

	printf("glgeterror 203 = %x\n", glGetError());

	// Actually use the created program
	glUseProgram(uiProgramObject);

	//set rotation variables to init
	matrixLocation = glGetUniformLocation(uiProgramObject, "MVPMatrix");

	printf("glgeterror 204 = %x\n", glGetError());
	calculate_rotation_z(mat_z, 0);
	calculate_rotation_y(mat_y, 0);
	calculate_rotation_x(mat_x, 0);
	matrix_mult(mat_z, mat_y, mat_temp);
	matrix_mult(mat_temp, mat_x, mat_final);
	glUniformMatrix4fv( matrixLocation, 1, GL_FALSE, mat_final);


	//MUST
	glDisableVertexAttribArray(VERTEX_ARRAY);
	glDisableVertexAttribArray(TEXCOORD_ARRAY);

	printf("glgeterror 205 = %x\n", glGetError());

cleanup:
	return;
}

void test3_process_one(int currIteration)
{
	float *pVertexArray, *pTexCoordArray;

	// Actually use the created program
	glUseProgram(uiProgramObject);

	common_init_gl_vertices(inNumberOfObjectsPerSide, &pVertexArray);
	common_init_gl_texcoords(inNumberOfObjectsPerSide, &pTexCoordArray);


	glClear(GL_COLOR_BUFFER_BIT);

	common_gl_draw(inNumberOfObjectsPerSide);

	printf("glgeterror after draw = %x\n", glGetError());

	common_deinit_gl_vertices(pVertexArray);
	common_deinit_gl_texcoords(pTexCoordArray);

	// unbind program
	glUseProgram(0);
	//MUST
	glDisableVertexAttribArray(VERTEX_ARRAY);
	glDisableVertexAttribArray(TEXCOORD_ARRAY);


}

void test3_deinit()
{
}
