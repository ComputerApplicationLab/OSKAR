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

#include "station/oskar_element_model_compute_splines.h"
#include "math/oskar_spline_data_compute.h"
#include "math/oskar_SplineData.h"

#ifdef __cplusplus
extern "C" {
#endif

int oskar_element_model_compute_splines(oskar_ElementModel* e)
{
    int err;
    err = oskar_spline_data_compute(&e->spline_phi_im, e->num_points_phi,
            e->num_points_theta, e->min_phi, e->min_theta, e->max_phi,
            e->max_theta, &e->phi_im);
    if (err) return err;
    err = oskar_spline_data_compute(&e->spline_phi_re, e->num_points_phi,
            e->num_points_theta, e->min_phi, e->min_theta, e->max_phi,
            e->max_theta, &e->phi_re);
    if (err) return err;
    err = oskar_spline_data_compute(&e->spline_theta_im, e->num_points_phi,
            e->num_points_theta, e->min_phi, e->min_theta, e->max_phi,
            e->max_theta, &e->theta_im);
    if (err) return err;
    err = oskar_spline_data_compute(&e->spline_theta_re, e->num_points_phi,
            e->num_points_theta, e->min_phi, e->min_theta, e->max_phi,
            e->max_theta, &e->theta_re);
    if (err) return err;

    return OSKAR_ERR_UNKNOWN;
}

#ifdef __cplusplus
}
#endif