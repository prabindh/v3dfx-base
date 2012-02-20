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
#include "../include/v3dfxbase.h"


TISGXStreamDeviceBase::TISGXStreamDeviceBase() {}
TISGXStreamDeviceBase::~TISGXStreamDeviceBase() {}
int TISGXStreamDeviceBase::init(
	void* attrib, 
	int deviceId, 
	unsigned long *paArray)
{ 
	return 0;
}
int TISGXStreamDeviceBase::get_device_id() {return 0;}
int TISGXStreamDeviceBase::qTexImage2DBuf(void* fullBufPhyAddr){return 0;}
int TISGXStreamDeviceBase::dqTexImage2DBuf(void* freeBufPhyAddr){return 0;}

void TISGXStreamDeviceBase::signal_draw(int texIndex)
{
	//TODO - handle in-the-middle-of-draw cases
	LOG_V3DFX_BASE("INFO: setting draw-complete on texIndex %d\n", texIndex);
	bufferState[texIndex].state = 0;
}
int TISGXStreamDeviceBase::destroy() 
{
	return 0;
}

TISGXStreamTexBase::TISGXStreamTexBase() {}
TISGXStreamTexBase::~TISGXStreamTexBase() {}
int TISGXStreamTexBase::init(int streamDeviceId){return 0;}
int TISGXStreamTexBase::get_uniform_location(char const* uniformname)
{
	return(glGetUniformLocation(uiProgramObject, uniformname));
}
int TISGXStreamTexBase::load_v_shader(char const* vshader)
{
	bool bShaderCompiled;
	if(vshader == NULL)
		return 1;

	uiVertShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(uiVertShader, 1, (const char**)&vshader, NULL);

	glCompileShader(uiVertShader);
    	glGetShaderiv(uiVertShader, GL_COMPILE_STATUS, (int*)&bShaderCompiled);

	if (!bShaderCompiled)
	{
		int i32InfoLogLength, i32CharsWritten;
		glGetShaderiv(uiVertShader, GL_INFO_LOG_LENGTH, &i32InfoLogLength);
		char* pszInfoLog = new char[i32InfoLogLength];
		glGetShaderInfoLog(uiVertShader, i32InfoLogLength, &i32CharsWritten, pszInfoLog);
		LOG_V3DFX_BASE("Failed to compile vertex shader: %s\n", pszInfoLog);
		delete [] pszInfoLog;
		return 2;
	}
	LOG_V3DFX_BASE("successfully loaded default v shader\n");
	return 0;
}
int TISGXStreamTexBase::load_f_shader(char const* fshader)
{
	bool bShaderCompiled;
	if(fshader == NULL)
		return 1;
	// Create the fragment shader object
	uiFragShader = glCreateShader(GL_FRAGMENT_SHADER);
	// Load the source code into it
	glShaderSource(uiFragShader, 1, (const char**)&fshader, NULL);

	// Compile the source code
	glCompileShader(uiFragShader);

	// Check if compilation succeeded
	glGetShaderiv(uiFragShader, GL_COMPILE_STATUS, (int*)&bShaderCompiled);

	if (!bShaderCompiled)
	{
		// An error happened, first retrieve the length of the log message
		int i32InfoLogLength, i32CharsWritten;
		glGetShaderiv(uiFragShader, GL_INFO_LOG_LENGTH, &i32InfoLogLength);

		// Allocate enough space for the message and retrieve it
		char* pszInfoLog = new char[i32InfoLogLength];
		glGetShaderInfoLog(uiFragShader, i32InfoLogLength, &i32CharsWritten, pszInfoLog);

		// Displays the error
		LOG_V3DFX_BASE("Failed to compile fragment shader: %s\n", pszInfoLog);
		delete [] pszInfoLog;
		return 2;
	}
	LOG_V3DFX_BASE("successfully loaded default f shader\n");
	return 0;
}
int TISGXStreamTexBase::load_program()
{
	// Create the shader program
	uiProgramObject = glCreateProgram();

	// Attach the fragment and vertex shaders to it
	glAttachShader(uiProgramObject, uiFragShader);
	glAttachShader(uiProgramObject, uiVertShader);

	// Bind the custom vertex attribute "myVertex" to location VERTEX_ARRAY
	glBindAttribLocation(uiProgramObject, VERTEX_ARRAY_DEFAULT, "inVertex");
	glBindAttribLocation(uiProgramObject, TEXCOORD_ARRAY_DEFAULT, "inTexCoord");

	// Link the program
	glLinkProgram(uiProgramObject);

	// Check if linking succeeded
    	GLint bLinked;
	glGetProgramiv(uiProgramObject, GL_LINK_STATUS, &bLinked);

	if (!bLinked)
	{
		int ui32InfoLogLength, ui32CharsWritten;
		glGetProgramiv(uiProgramObject, GL_INFO_LOG_LENGTH, &ui32InfoLogLength);
		char* pszInfoLog = new char[ui32InfoLogLength];
		glGetProgramInfoLog(uiProgramObject, ui32InfoLogLength, &ui32CharsWritten, pszInfoLog);
		LOG_V3DFX_BASE("Failed to link program: %s\n", pszInfoLog);
		delete [] pszInfoLog;
		return 1;
	}

	// Actually use the created program
 	glUseProgram(uiProgramObject);
	LOG_V3DFX_BASE("successfully loaded default program\n");
	return 0;
}

int TISGXStreamTexBase::use_program()
{
 	glUseProgram(uiProgramObject);
}

int TISGXStreamTexBase::unload_shader_program()
{
	// Frees the OpenGL handles for the program and the 2 shaders
	glDeleteProgram(uiProgramObject);
	glDeleteShader(uiFragShader);
	glDeleteShader(uiVertShader);
	return 0;
}


