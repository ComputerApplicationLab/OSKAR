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

#include <oskar_convert_healpix_ring_to_theta_phi.h>
#include <oskar_healpix_npix_to_nside.h>
#include <oskar_healpix_nside_to_npix.h>
#include <oskar_convert_galactic_to_fk5.h>
#include <oskar_sky.h>
#include <oskar_getline.h>
#include <oskar_string_to_array.h>
#include <math.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifdef __cplusplus
extern "C" {
#endif

static const double boltzmann = 1.3806488e-23; /* Boltzmann constant in J/K. */

void oskar_sky_load_gsm(oskar_Sky* sky, const char* filename,
        int* status)
{
    int i, n = 0, nside, type;
    FILE* file;
    char* line = 0;
    size_t bufsize = 0;
    oskar_Sky* temp_sky;
    double *temp = 0;

    /* Check all inputs. */
    if (!sky || !filename || !status)
    {
        oskar_set_invalid_argument(status);
        return;
    }

    /* Check if safe to proceed. */
    if (*status) return;

    /* Get the data type. */
    type = oskar_sky_type(sky);
    if (type != OSKAR_SINGLE && type != OSKAR_DOUBLE)
    {
        *status = OSKAR_ERR_BAD_DATA_TYPE;
        return;
    }

    /* Open the file. */
    file = fopen(filename, "r");
    if (!file)
    {
        *status = OSKAR_ERR_FILE_IO;
        return;
    }

    /* Loop over lines in file. */
    while (oskar_getline(&line, &bufsize, file) != OSKAR_ERR_EOF)
    {
        /* Ensure enough space in array. */
        if (n % 100000 == 0)
        {
            temp = (double*) realloc(temp, (n + 100000) * sizeof(double));
            if (!temp)
            {
                *status = OSKAR_ERR_MEMORY_ALLOC_FAILURE;
                break;
            }
        }

        /* Load pixel value. */
        if (oskar_string_to_array_d(line, 1, &temp[n]) < 1) continue;
        ++n;
    }

    /* Free the line buffer and close the file. */
    if (line) free(line);
    fclose(file);

    /* Check if safe to proceed. */
    if (*status)
    {
        if (temp) free(temp);
        return;
    }

    /* Compute nside from npix. */
    nside = oskar_healpix_npix_to_nside(n);
    if (oskar_healpix_nside_to_npix(nside) != n)
    {
        if (temp) free(temp);
        *status = OSKAR_ERR_BAD_GSM_FILE;
        return;
    }

    /* Initialise the temporary sky model. */
    temp_sky = oskar_sky_create(type, OSKAR_LOCATION_CPU, n, status);
    if (*status)
    {
        if (temp) free(temp);
        oskar_sky_free(temp_sky, status);
        return;
    }

    /* Loop over pixels. */
    for (i = 0; i < n; ++i)
    {
        double l, b, ra, dec;

        /* Assume that input map is in Kelvin per steradian. */
        /* Convert temperature per steradian to temperature per pixel. */
        /* Divide by number of pixels per steradian. */
        temp[i] = temp[i] / (n / (4 * M_PI));

        /* Convert temperature per pixel to Jansky per pixel. */
        /* Multiply by 2.0 * k_B * 10^26. */
        /* Assume that any wavelength dependence is already
         * in the input data! */
        temp[i] = temp[i] * 2.0 * boltzmann * 1e26;

        /* Compute Galactic longitude and latitude from pixel index. */
        oskar_convert_healpix_ring_to_theta_phi(nside, i, &b, &l);
        b = (M_PI / 2.0) - b; /* Colatitude to latitude. */

        /* Compute RA and Dec. */
        oskar_convert_galactic_to_fk5_d(1, &l, &b, &ra, &dec);

        /* Store pixel data. */
        oskar_sky_set_source(temp_sky, i, ra, dec,
                temp[i], 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, status);
    }

    /* Append data to model and free temporary storage. */
    free(temp);
    oskar_sky_append(sky, temp_sky, status);
    oskar_sky_free(temp_sky, status);
}

#ifdef __cplusplus
}
#endif