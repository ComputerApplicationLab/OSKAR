/*
 * Copyright (c) 2011, The University of Oxford
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

#ifndef OSKAR_ELEMENT_MODEL_H_
#define OSKAR_ELEMENT_MODEL_H_

/**
 * @file oskar_ElementModel.h
 */

#include "utility/oskar_vector_types.h"
#include <stdlib.h> // For size_t.

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Structure to hold antenna (embedded element) pattern data.
 *
 * @details
 * This structure holds the complex gain of an antenna as a function of theta
 * and phi. The 2D data can be interpolated easily using the additional
 * meta-data.
 *
 * The theta coordinate is assumed to be the fastest-varying dimension.
 */
struct oskar_ElementModel
{
    int coordsys;       ///< Specifies whether horizontal or wrt phase centre.
    int n_points;       ///< Total number of points in all arrays.
    int n_phi;          ///< Number of points in the phi direction.
    int n_theta;        ///< Number of points in the theta direction.
    float inc_phi;      ///< Increment in the phi direction, in radians.
    float inc_theta;    ///< Increment in the theta direction, in radians.
    float max_phi;      ///< Maximum value of phi, in radians.
    float max_theta;    ///< Maximum value of theta, in radians.
    float min_phi;      ///< Minimum value of phi, in radians.
    float min_theta;    ///< Minimum value of theta, in radians.
    float2* g_phi;      ///< Response in phi direction at coordinates (re,im).
    float2* g_theta;    ///< Response in theta direction at coordinates (re,im).
    size_t pitch_phi;   ///< Memory pitch, as returned by cudaMallocPitch().
    size_t pitch_theta; ///< Memory pitch, as returned by cudaMallocPitch().
};
typedef struct oskar_ElementModel oskar_ElementModel;

#ifdef __cplusplus
}
#endif

#endif // OSKAR_ELEMENT_MODEL_H_