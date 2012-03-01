/*****************************************************************************
 *   
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

#ifndef _US_RENDERER_H
#define _US_RENDERER_H

#define ULTRASOUND_WIDTH 128
#define ULTRASOUND_HEIGHT 128
#define ULTRASOUND_LAYERS 128 /* NOTE - MAX_BUFFERS_PER_STREAM is limited to 128 in v3dfxbase.h */


int us_program_setup();
void us_program_cleanup();
int us_get_data_size_bytes();
int us_allocate_v3dfx_imgstream_bufs(int allocBytes, void** virtualAddress, void** physicalAddress);
void us_deallocate_v3dfx_imgstream_bufs(void *virtualAddress);
int load_original_data(void* virtualAddress);
void us_render_init_1(void* virtualAddress, int numbytes);
void us_render_init_2();
void us_render_process_one_prepare(int layerId, int currIteration);
void us_render_process_one_draw(int curriteration);
void us_render_deinit();

void us_set_mvp(int mvplocation, float yangle);


#endif //
