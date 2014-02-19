/*
 * Copyright (c) 2012-2014, The University of Oxford
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

#ifdef OSKAR_HAVE_CUDA
#include <cuda_runtime_api.h>
#define H2D cudaMemcpyHostToDevice
#endif

#include <private_sky.h>
#include <oskar_sky.h>

#ifdef __cplusplus
extern "C" {
#endif

void oskar_sky_set_spectral_index(oskar_Sky* sky, int index,
        double ref_frequency, double spectral_index, int* status)
{
    int type, location;
    char *ref_, *spix_;

    /* Check all inputs. */
    if (!sky || !status)
    {
        oskar_set_invalid_argument(status);
        return;
    }

    /* Check if safe to proceed. */
    if (*status) return;

    /* Get the data location and type. */
    location = oskar_sky_location(sky);
    type = oskar_sky_precision(sky);

    if (index >= sky->num_sources)
    {
        *status = OSKAR_ERR_OUT_OF_RANGE;
        return;
    }

    /* Get byte pointers. */
    ref_ = oskar_mem_char(sky->reference_freq);
    spix_ = oskar_mem_char(sky->spectral_index);

    if (location == OSKAR_LOCATION_GPU)
    {
#ifdef OSKAR_HAVE_CUDA
        size_t size, offset_bytes;
        size = oskar_mem_element_size(type);
        offset_bytes = index * size;
        if (type == OSKAR_DOUBLE)
        {
            cudaMemcpy(ref_ + offset_bytes, &ref_frequency, size, H2D);
            cudaMemcpy(spix_ + offset_bytes, &spectral_index, size, H2D);
        }
        else if (type == OSKAR_SINGLE)
        {
            float t_ref_freq = (float)ref_frequency;
            float t_spectral_index = (float)spectral_index;
            cudaMemcpy(ref_ + offset_bytes, &t_ref_freq, size, H2D);
            cudaMemcpy(spix_ + offset_bytes, &t_spectral_index, size, H2D);
        }
#else
        *status = OSKAR_ERR_CUDA_NOT_AVAILABLE;
#endif
    }
    else
    {
        if (type == OSKAR_DOUBLE)
        {
            ((double*)ref_)[index] = ref_frequency;
            ((double*)spix_)[index] = spectral_index;

        }
        else if (type == OSKAR_SINGLE)
        {
            ((float*)ref_)[index] = (float)ref_frequency;
            ((float*)spix_)[index] = (float)spectral_index;
        }
        else
        {
            *status = OSKAR_ERR_BAD_DATA_TYPE;
        }
    }
}

#ifdef __cplusplus
}
#endif
