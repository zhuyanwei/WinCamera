/*
 * Copyright (c) 2014 Andy Huang <andyspider@126.com>
 *
 * This file is part of Camkit.
 *
 * Camkit is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * Camkit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Camkit; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef COMDEF_H
#define COMDEF_H
#include <string.h>

typedef unsigned int U32;
#define CLEAR(x) memset (&(x), 0, sizeof (x))
#define UNUSED(expr) do { (void)(expr); } while (0)

#define  WIDTH 320
#define  HEIGHT 240
#define FRAMERATE 15

#define MAXDATASIZE 1420
#define H264 96
#define AAC 97
#define SSRC 10

#define BASE_PORT 8090

/* #define SAMPLE_RATE  (17932) // Test failure to open with this value. */
#define SAMPLE_RATE  (8000)
#define FRAMES_PER_BUFFER (1024)
//#define NUM_SECONDS     (5)
#define NUM_CHANNELS    (2)
/* #define DITHER_FLAG     (paDitherOff) */
#define DITHER_FLAG     (0) /**/

#endif /* COMDEF_H */
