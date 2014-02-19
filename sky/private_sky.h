/*
 * Copyright (c) 2011-2014, The University of Oxford
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

#ifndef OSKAR_PRIVATE_SKY_H_
#define OSKAR_PRIVATE_SKY_H_

/**
 * @file private_sky.h
 */

#include <oskar_mem.h>

/**
 * @struct oskar_Sky
 *
 * @brief Structure to hold a sky model.
 *
 * @details
 * The structure holds source parameters for a sky model.
 */
struct oskar_Sky
{
    int precision;
    int location;

    int num_sources;           /**< Number of sources in the sky model. */
    oskar_Mem* RA;             /**< Right ascension, in radians. */
    oskar_Mem* Dec;            /**< Declination, in radians. */
    oskar_Mem* I;              /**< Stokes-I, in Jy. */
    oskar_Mem* Q;              /**< Stokes-Q, in Jy. */
    oskar_Mem* U;              /**< Stokes-U, in Jy. */
    oskar_Mem* V;              /**< Stokes-V, in Jy. */
    oskar_Mem* reference_freq; /**< Reference frequency for the source flux, in Hz. */
    oskar_Mem* spectral_index; /**< Spectral index. */
    oskar_Mem* RM;             /**< Rotation measure, in radians / m^2. */

    double ra0;                /**< Reference right ascension, in radians. */
    double dec0;               /**< Reference declination, in radians. */
    oskar_Mem* l;              /**< Phase centre relative l-direction cosines. */
    oskar_Mem* m;              /**< Phase centre relative m-direction cosines. */
    oskar_Mem* n;              /**< Phase centre relative n-direction cosines. */

    int use_extended;          /**< Enable use of extended sources */
    oskar_Mem* FWHM_major;     /**< Major axis FWHM for Gaussian sources, in radians. */
    oskar_Mem* FWHM_minor;     /**< Minor axis FWHM for Gaussian sources, in radians. */
    oskar_Mem* position_angle; /**< Position angle for Gaussian sources, in radians. */
    oskar_Mem* gaussian_a;     /**< Gaussian source width parameter */
    oskar_Mem* gaussian_b;     /**< Gaussian source width parameter */
    oskar_Mem* gaussian_c;     /**< Gaussian source width parameter */
};

#ifndef OSKAR_SKY_TYPEDEF_
#define OSKAR_SKY_TYPEDEF_
typedef struct oskar_Sky oskar_Sky;
#endif /* OSKAR_SKY_TYPEDEF_ */

#endif /* OSKAR_PRIVATE_SKY_H_ */
