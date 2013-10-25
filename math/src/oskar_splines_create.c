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

#include <private_splines.h>
#include <oskar_splines.h>
#include <oskar_mem.h>

#ifdef __cplusplus
extern "C" {
#endif

oskar_Splines* oskar_splines_create(int type, int location, int* status)
{
    oskar_Splines* data = 0;

    /* Check all inputs. */
    if (!status)
    {
        oskar_set_invalid_argument(status);
        return 0;
    }

    /* Allocate and initialise the structure. */
    data = (oskar_Splines*) malloc(sizeof(oskar_Splines));
    if (!data)
    {
        *status = OSKAR_ERR_MEMORY_ALLOC_FAILURE;
        return 0;
    }

    data->type = type;
    data->location = location;
    data->num_knots_x = 0;
    data->num_knots_y = 0;
    oskar_mem_init(&data->knots_x, type, location, 0, OSKAR_TRUE, status);
    oskar_mem_init(&data->knots_y, type, location, 0, OSKAR_TRUE, status);
    oskar_mem_init(&data->coeff, type, location, 0, OSKAR_TRUE, status);

    /* Return pointer to the structure. */
    return data;
}

#ifdef __cplusplus
}
#endif