/*
 * Copyright (c) 2013, The University of Oxford
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

#include "sky/oskar_evaluate_jones_Z.h"

#include "station/oskar_evaluate_source_horizontal_lmn.h"
#include "interferometry/oskar_offset_geocentric_cartesian_to_geocentric_cartesian.h"
#include "utility/oskar_vector_types.h"
#include "station/oskar_evaluate_pierce_points.h"
#include "sky/oskar_evaluate_TEC_TID.h"

#include <oskar_telescope.h>
#include <oskar_sky.h>
#include <oskar_jones.h>
#include <oskar_mem.h>

#include <math.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

static void oskar_evaluate_TEC(oskar_WorkJonesZ* work, int num_pp,
        const oskar_SettingsIonosphere* settings,
        double gast, int* status);

static void evaluate_station_ECEF_coords(
        double* station_x, double* station_y, double* station_z,
        int stationID, const oskar_Telescope* telescope);

static void evaluate_jones_Z_station(oskar_Mem* Z_station,
        double wavelength, const oskar_Mem* TEC, const oskar_Mem* hor_z,
        double min_elevation, int num_pp, int* status);


void oskar_evaluate_jones_Z(oskar_Jones* Z, const oskar_Sky* sky,
        const oskar_Telescope* telescope,
        const oskar_SettingsIonosphere* settings, double gast,
        double frequency_hz, oskar_WorkJonesZ* work, int* status)
{
    int i, num_sources, num_stations;
    /* Station position in ECEF frame */
    double station_x, station_y, station_z, wavelength;
    oskar_Mem Z_station;
    int type;
    oskar_Sky* sky_cpu; /* Copy of the sky model on the CPU */

    /* Check all inputs. */
    if (!Z || !sky || !telescope || !settings || !work || !status)
    {
        oskar_set_invalid_argument(status);
        return;
    }

    /* Check if safe to proceed. */
    if (*status) return;

    /* Check data types. */
    type = oskar_sky_type(sky);
    if (oskar_telescope_type(telescope) != type ||
            oskar_jones_type(Z) != (type | OSKAR_COMPLEX) ||
            oskar_work_jones_z_type(work) != type)
    {
        *status = OSKAR_ERR_BAD_DATA_TYPE;
        return;
    }

    /* For now, this function requires data is on the CPU .. check this. */

    /* Resize the work array (if needed) */
    num_stations = oskar_telescope_num_stations(telescope);
    num_sources = oskar_sky_num_sources(sky);
    oskar_work_jones_z_resize(work, num_sources, status);

    /* Copy the sky model to the CPU. */
    sky_cpu = oskar_sky_create_copy(sky, OSKAR_LOCATION_CPU, status);

    oskar_mem_init(&Z_station, oskar_jones_type(Z), OSKAR_LOCATION_CPU,
            num_sources, OSKAR_FALSE, status);
    wavelength = 299792458.0 / frequency_hz;

    /* Evaluate the ionospheric phase screen for each station at each
     * source pierce point. */
    for (i = 0; i < num_stations; ++i)
    {
        double last, lon, lat, alt;
        const oskar_Station* station;
        station = oskar_telescope_station_const(telescope, i);
        lon = oskar_station_longitude_rad(station);
        lat = oskar_station_latitude_rad(station);
        alt = oskar_station_altitude_m(station);
        last = gast + lon;

        /* Evaluate horizontal x,y,z source positions (for which to evaluate
         * pierce points) */
        oskar_evaluate_source_horizontal_lmn(num_sources, &work->hor_x,
                &work->hor_y, &work->hor_z, oskar_sky_ra_const(sky_cpu),
                oskar_sky_dec_const(sky_cpu), last, lat, status);

        /* Obtain station coordinates in the ECEF frame. */
        evaluate_station_ECEF_coords(&station_x, &station_y, &station_z, i,
                telescope);

        /* Obtain the pierce points. */
        /* FIXME this is current hard-coded to TID height screen 0 */
        oskar_evaluate_pierce_points(&work->pp_lon, &work->pp_lat,
                &work->pp_rel_path, lon, lat, alt, station_x, station_y,
                station_z, settings->TID[0].height_km * 1000., num_sources,
                &work->hor_x, &work->hor_y, &work->hor_z, status);

        /* Evaluate TEC values for the pierce points */
        oskar_evaluate_TEC(work, num_sources, settings, gast, status);

        /* Get a pointer to the Jones matrices for the station */
        oskar_jones_get_station_pointer(&Z_station, Z, i, status);

        /* Populate the Jones matrix with ionospheric phase */
        evaluate_jones_Z_station(&Z_station, wavelength,
                &work->total_TEC, &work->hor_z, settings->min_elevation,
                num_sources, status);
    }

    oskar_sky_free(sky_cpu, status);
}


/* Evaluate the TEC value for each pierce point - note: at the moment this is
 * just the accumulation of one or more TID screens.
 * TODO convert this to a stand-alone function.
 */
static void oskar_evaluate_TEC(oskar_WorkJonesZ* work, int num_pp,
        const oskar_SettingsIonosphere* settings,
        double gast, int* status)
{
    int i;

    /* FIXME For now limit number of screens to 1, this can be removed
     * if a TEC model which is valid for multiple screens is implemented
     */
    if (settings->num_TID_screens > 1)
        *status = OSKAR_ERR_SETTINGS_IONOSPHERE;

    oskar_mem_set_value_real(&work->total_TEC, 0.0, 0, 0, status);

    /* Loop over TID screens to evaluate TEC values */
    for (i = 0; i < settings->num_TID_screens; ++i)
    {
        oskar_mem_set_value_real(&work->screen_TEC, 0.0, 0, 0, status);

        /* Evaluate TEC values for the screen */
        oskar_evaluate_TEC_TID(&work->screen_TEC, num_pp, &work->pp_lon,
                &work->pp_lat, &work->pp_rel_path, settings->TEC0,
                &settings->TID[i], gast);

        /* Accumulate into total TEC */
        /* FIXME addition is not physical for more than one TEC screen in the
         * way TIDs are currently evaluated as TEC0 is added into both screens.
         */
        oskar_mem_add(&work->total_TEC, &work->total_TEC, &work->screen_TEC,
                status);
    }
}


static void evaluate_station_ECEF_coords(
        double* station_x, double* station_y, double* station_z,
        int stationID, const oskar_Telescope* telescope)
{
    double st_x, st_y, st_z;
    double lon, lat, alt;
    const oskar_Station* station;
    const void *x_, *y_, *z_;

    x_ = oskar_mem_void_const(oskar_telescope_station_x_const(telescope));
    y_ = oskar_mem_void_const(oskar_telescope_station_y_const(telescope));
    z_ = oskar_mem_void_const(oskar_telescope_station_z_const(telescope));
    station = oskar_telescope_station_const(telescope, stationID);
    lon = oskar_station_longitude_rad(station);
    lat = oskar_station_latitude_rad(station);
    alt = oskar_station_altitude_m(station);

    if (oskar_mem_type(oskar_telescope_station_x_const(telescope)) ==
            OSKAR_DOUBLE)
    {
        st_x = ((const double*)x_)[stationID];
        st_y = ((const double*)y_)[stationID];
        st_z = ((const double*)z_)[stationID];
    }
    else
    {
        st_x = (double)((const float*)x_)[stationID];
        st_y = (double)((const float*)y_)[stationID];
        st_z = (double)((const float*)z_)[stationID];
    }

    oskar_offset_geocentric_cartesian_to_geocentric_cartesian(1,
            &st_x, &st_y, &st_z, lon, lat, alt,
            station_x, station_y, station_z);
}

static void evaluate_jones_Z_station(oskar_Mem* Z_station,
        double wavelength, const oskar_Mem* TEC, const oskar_Mem* hor_z,
        double min_elevation, int num_pp, int* status)
{
    int i, type;
    double arg;

    /* Check all inputs. */
    if (!Z_station || !TEC || !hor_z || !status)
    {
        oskar_set_invalid_argument(status);
        return;
    }

    /* Check if safe to proceed. */
    if (*status) return;

    type = oskar_mem_type(Z_station);
    if (type == OSKAR_DOUBLE_COMPLEX)
    {
        double2* Z = (double2*)Z_station->data;
        for (i = 0; i < num_pp; ++i)
        {
            /* Initialise as an unit scalar Z = (1 + 0i) i.e. no phase change */
            Z[i].x = 1.0;
            Z[i].y = 0.0;

            /* If the pierce point is below the minimum specified elevation
             * don't evaluate a phase */
            if (asin(((double*)hor_z->data)[i]) < min_elevation)
                continue;

            arg = wavelength * 25. * ((double*)TEC->data)[i];

            /* Z phase == exp(i * lambda * 25 * tec) */
            Z[i].x = cos(arg);
            Z[i].y = sin(arg);
        }
    }
    else if (type == OSKAR_SINGLE_COMPLEX)
    {
        float2* Z = (float2*)Z_station->data;
        for (i = 0; i < num_pp; ++i)
        {
            /* Initialise as an unit scalar Z = (1 + 0i) i.e. no phase change */
            Z[i].x = 1.0;
            Z[i].y = 0.0;

            /* If the pierce point is below the minimum specified elevation
             * don't evaluate a phase */
            if (asin(((float*)hor_z->data)[i]) < min_elevation)
                continue;

            arg = wavelength * 25. * ((float*)TEC->data)[i];

            /* Z phase == exp(i * lambda * 25 * tec) */
            Z[i].x = cos(arg);
            Z[i].y = sin(arg);
        }
    }
    else
    {
        *status = OSKAR_ERR_BAD_JONES_TYPE;
    }
}

#ifdef __cplusplus
}
#endif
