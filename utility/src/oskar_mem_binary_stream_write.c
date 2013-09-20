/*
 * Copyright (c) 2012-2013, The University of Oxford
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the University of Oxford nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <private_mem.h>
#include <oskar_mem.h>

#include <oskar_binary_stream_write.h>
#include <oskar_mem_binary_stream_write.h>

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void oskar_mem_binary_stream_write(const oskar_Mem* mem, FILE* stream,
        unsigned char id_group, unsigned char id_tag, int user_index,
        size_t num_to_write, int* status)
{
    int type;
    oskar_Mem temp;
    size_t size_bytes;
    const oskar_Mem* data = NULL;

    /* Check all inputs. */
    if (!mem || !stream || !status)
    {
        oskar_set_invalid_argument(status);
        return;
    }

    /* Check if safe to proceed. */
    if (*status) return;

    /* Get the data type. */
    type = mem->type;

    /* Initialise temporary (to zero length). */
    oskar_mem_init(&temp, type, OSKAR_LOCATION_CPU, 0, OSKAR_TRUE, status);

    /* Get the total number of bytes to write. */
    if (num_to_write <= 0)
        num_to_write = mem->num_elements;
    size_bytes = num_to_write * oskar_mem_element_size(type);

    /* Check if data is in CPU or GPU memory. */
    data = mem;
    if (mem->location == OSKAR_LOCATION_GPU)
    {
        /* Copy to temporary. */
        oskar_mem_copy(&temp, mem, status);
        data = &temp;
    }

    /* Save the memory to a binary stream. */
    oskar_binary_stream_write(stream, (unsigned char)type,
            id_group, id_tag, user_index, size_bytes, data->data, status);

    /* Free the temporary. */
    oskar_mem_free(&temp, status);
}

void oskar_mem_binary_stream_write_ext(const oskar_Mem* mem, FILE* stream,
        const char* name_group, const char* name_tag, int user_index,
        size_t num_to_write, int* status)
{
    int type;
    oskar_Mem temp;
    size_t size_bytes;
    const oskar_Mem* data = NULL;

    /* Check all inputs. */
    if (!mem || !stream || !name_group || !name_tag || !status)
    {
        oskar_set_invalid_argument(status);
        return;
    }

    /* Check if safe to proceed. */
    if (*status) return;

    /* Get the data type. */
    type = mem->type;

    /* Initialise temporary (to zero length). */
    oskar_mem_init(&temp, type, OSKAR_LOCATION_CPU, 0, OSKAR_TRUE, status);

    /* Get the total number of bytes to write. */
    if (num_to_write <= 0)
        num_to_write = mem->num_elements;
    size_bytes = num_to_write * oskar_mem_element_size(type);

    /* Check if data is in CPU or GPU memory. */
    data = mem;
    if (mem->location == OSKAR_LOCATION_GPU)
    {
        /* Copy to temporary. */
        oskar_mem_copy(&temp, mem, status);
        data = &temp;
    }

    /* Save the memory to a binary stream. */
    oskar_binary_stream_write_ext(stream, (unsigned char)type,
            name_group, name_tag, user_index, size_bytes, data->data, status);

    /* Free the temporary. */
    oskar_mem_free(&temp, status);
}

#ifdef __cplusplus
}
#endif
