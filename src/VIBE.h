/*
 * UFO Detector | www.UFOID.net
 *
 * Copyright (C) 2016 UFOID
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/*
* Author(s): BeS 2013
*/

#ifndef __CV_VIBE_H__
#define __CV_VIBE_H__

#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <omp.h>
#include <iostream>

typedef struct
{
	unsigned char *samples;
	unsigned int numberOfSamples;
	unsigned int sizeOfSample;
}pixel;

typedef struct
{
	pixel *pixels;
	unsigned int width;
	unsigned int height;
	unsigned int stride;
	unsigned int numberOfSamples;
	unsigned int matchingThreshold;
	unsigned int matchingNumber;
	unsigned int updateFactor;
}vibeModel;

typedef vibeModel vibeModel_t;

static unsigned int *rnd;
static unsigned int rndSize;
static unsigned int rndPos;

vibeModel *libvibeModelNew();
unsigned char getRandPixel(const unsigned char *image_data, const unsigned int width, const unsigned int height, const unsigned int stride, const unsigned int x, const unsigned int y);
int32_t libvibeModelInit(vibeModel *model, const unsigned char *image_data, const unsigned int width, const unsigned int height, const unsigned int stride);
int32_t libvibeModelUpdate(vibeModel *model, const unsigned char *image_data, unsigned char *segmentation_map);
int32_t libvibeModelFree(vibeModel *model);

void initRnd(unsigned int size);
unsigned int freeRnd();
#endif /*__CV_VIBE_H__*/
