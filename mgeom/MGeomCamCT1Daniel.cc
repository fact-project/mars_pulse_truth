/* ======================================================================== *\
!
! *
! * This file is part of MARS, the MAGIC Analysis and Reconstruction
! * Software. It is distributed to you in the hope that it can be a useful
! * and timesaving tool in analysing Data of imaging Cerenkov telescopes.
! * It is distributed WITHOUT ANY WARRANTY.
! *
! * Permission to use, copy, modify and distribute this software and its
! * documentation for any purpose is hereby granted without fee,
! * provided that the above copyright notice appear in all copies and
! * that both that copyright notice and this permission notice appear
! * in supporting documentation. It is provided "as is" without express
! * or implied warranty.
! *
!
!
!   Author(s): Thomas Bretz    12/2000 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Harald Kornmayer 1/2001
!   Author(s): Wolfgang Wittek 10/2003 <mailto:wittek@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MGeomCamCT1Daniel
//
// This class stores the geometry information of the CT1 camera.
// The next neighbor information comes from a table, the geometry layout
// is calculated (for Algorithm see CreateCam
//
//
// The only difference to MGeomCamCT1 is the second argument of MGeomCam
// (which is the distance camera-reflector)
//        it is        dist = 4.8129 m    (instead of 4.88  m)
//
// together with the pixel diameter of d = 21 mm this gives      
// d/dist = 0.0043633 = 0.25 deg    (instead of 0.0043033 = 0.2466 deg)
//
////////////////////////////////////////////////////////////////////////////
#include "MGeomCamCT1Daniel.h"

#include <math.h>     // floor

#include "MGeomPix.h"

ClassImp(MGeomCamCT1Daniel);

// --------------------------------------------------------------------------
//
//  CT1 camera has 127 pixels. For geometry and Next Neighbor info see
//  CreateCam and CreateNN
//
MGeomCamCT1Daniel::MGeomCamCT1Daniel(const char *name)
    : MGeomCam(127, 4.8129, name, "Geometry information of CT1 camera")
{
    CreateCam();
    CreateNN();
    InitGeometry();
} 

// --------------------------------------------------------------------------
//
//  Create Next Neighbors: Fill the NN Info into the pixel objects.
//
void MGeomCamCT1Daniel::CreateNN()
{ 
    const Short_t nn[127][6] = {       // Neighbors of #
      {  1,   2,   3,   4,   5,   6},  // 0
      {  0,   2,   6,   7,   8,  18},
      {  0,   1,   3,   8,   9,  10},
      {  0,   2,   4,  10,  11,  12},
      {  0,   3,   5,  12,  13,  14},
      {  0,   4,   6,  14,  15,  16},
      {  0,   1,   5,  16,  17,  18},
      {  1,   8,  18,  19,  20,  36},
      {  1,   2,   7,   9,  20,  21},
      {  2,   8,  10,  21,  22,  23},
      {  2,   3,   9,  11,  23,  24},  // 10
      {  3,  10,  12,  24,  25,  26},
      {  3,   4,  11,  13,  26,  27},
      {  4,  12,  14,  27,  28,  29},
      {  4,   5,  13,  15,  29,  30},
      {  5,  14,  16,  30,  31,  32},
      {  5,   6,  15,  17,  32,  33},
      {  6,  16,  18,  33,  34,  35},
      {  1,   6,   7,  17,  35,  36},
      {  7,  20,  36,  37,  38,  60},
      {  7,   8,  19,  21,  38,  39},  // 20
      {  8,   9,  20,  22,  39,  40},
      {  9,  21,  23,  40,  41,  42},
      {  9,  10,  22,  24,  42,  43},
      { 10,  11,  23,  25,  43,  44},
      { 11,  24,  26,  44,  45,  46},
      { 11,  12,  25,  27,  46,  47},
      { 12,  13,  26,  28,  47,  48},
      { 13,  27,  29,  48,  49,  50},
      { 13,  14,  28,  30,  50,  51},
      { 14,  15,  29,  31,  51,  52},  // 30
      { 15,  30,  32,  52,  53,  54},
      { 15,  16,  31,  33,  54,  55},
      { 16,  17,  32,  34,  55,  56},
      { 17,  33,  35,  56,  57,  58},
      { 17,  18,  34,  36,  58,  59},
      {  7,  18,  19,  35,  59,  60},
      { 19,  38,  60,  61,  62,  90},
      { 19,  20,  37,  39,  62,  63},
      { 20,  21,  38,  40,  63,  64},
      { 21,  22,  39,  41,  64,  65},  // 40
      { 22,  40,  42,  65,  66,  67},
      { 22,  23,  41,  43,  67,  68},
      { 23,  24,  42,  44,  68,  69},
      { 24,  25,  43,  45,  69,  70},
      { 25,  44,  46,  70,  71,  72},
      { 25,  26,  45,  47,  72,  73},
      { 26,  27,  46,  48,  73,  74},
      { 27,  28,  47,  49,  74,  75},
      { 28,  48,  50,  75,  76,  77},
      { 28,  29,  49,  51,  77,  78},  // 50
      { 29,  30,  50,  52,  78,  79},
      { 30,  31,  51,  53,  79,  80},
      { 31,  52,  54,  80,  81,  82},
      { 31,  32,  53,  55,  82,  83},
      { 32,  33,  54,  56,  83,  84},
      { 33,  34,  55,  57,  84,  85},
      { 34,  56,  58,  85,  86,  87},
      { 34,  35,  57,  59,  87,  88},
      { 35,  36,  58,  60,  88,  89},
      { 19,  36,  37,  59,  89,  90},  // 60
      { 37,  62,  90,  91,  92, 126},
      { 37,  38,  61,  63,  92,  93},
      { 38,  39,  62,  64,  93,  94},
      { 39,  40,  63,  65,  94,  95},
      { 40,  41,  64,  66,  95,  96},
      { 41,  65,  67,  96,  97,  98},
      { 41,  42,  66,  68,  98,  99},
      { 42,  43,  67,  69,  99, 100},
      { 43,  44,  68,  70, 100, 101},
      { 44,  45,  69,  71, 101, 102},  // 70
      { 45,  70,  72, 102, 103, 104},
      { 45,  46,  71,  73, 104, 105},
      { 46,  47,  72,  74, 105, 106},
      { 47,  48,  73,  75, 106, 107},
      { 48,  49,  74,  76, 107, 108},
      { 49,  75,  77, 108, 109, 110},
      { 49,  50,  76,  78, 110, 111},
      { 50,  51,  77,  79, 111, 112},
      { 51,  52,  78,  80, 112, 113},
      { 52,  53,  79,  81, 113, 114},  // 80
      { 53,  80,  82, 114, 115, 116},
      { 53,  54,  81,  83, 116, 117},
      { 54,  55,  82,  84, 117, 118},
      { 55,  56,  83,  85, 118, 119},
      { 56,  57,  84,  86, 119, 120},
      { 57,  85,  87, 120, 121, 122},
      { 57,  58,  86,  88, 122, 123},
      { 58,  59,  87,  89, 123, 124},
      { 59,  60,  88,  90, 124, 125},
      { 37,  60,  61,  89, 125, 126},  // 90
      { 61,  92, 126,  -1,  -1,  -1},
      { 61,  62,  91,  93,  -1,  -1},
      { 62,  63,  92,  94,  -1,  -1},
      { 63,  64,  93,  95,  -1,  -1},
      { 64,  65,  94,  96,  -1,  -1},
      { 65,  66,  95,  97,  -1,  -1},
      { 66,  96,  98,  -1,  -1,  -1},
      { 66,  67,  97,  99,  -1,  -1},
      { 67,  68,  98, 100,  -1,  -1},
      { 68,  69,  99, 101,  -1,  -1},  // 100
      { 69,  70, 100, 102,  -1,  -1},
      { 70,  71, 101, 103,  -1,  -1},
      { 71, 102, 104,  -1,  -1,  -1},
      { 71,  72, 103, 105,  -1,  -1},
      { 72,  73, 104, 106,  -1,  -1},
      { 73,  74, 105, 107,  -1,  -1},
      { 74,  75, 106, 108,  -1,  -1},
      { 75,  76, 107, 109,  -1,  -1},
      { 76, 108, 110,  -1,  -1,  -1},
      { 76,  77, 109, 111,  -1,  -1},  // 110
      { 77,  78, 110, 112,  -1,  -1},
      { 78,  79, 111, 113,  -1,  -1},
      { 79,  80, 112, 114,  -1,  -1},
      { 80,  81, 113, 115,  -1,  -1},
      { 81, 114, 116,  -1,  -1,  -1},
      { 81,  82, 115, 117,  -1,  -1},
      { 82,  83, 116, 118,  -1,  -1},
      { 83,  84, 117, 119,  -1,  -1},
      { 84,  85, 118, 120,  -1,  -1},
      { 85,  86, 119, 121,  -1,  -1},  // 120
      { 86, 120, 122,  -1,  -1,  -1},
      { 86,  87, 121, 123,  -1,  -1},
      { 87,  88, 122, 124,  -1,  -1},
      { 88,  89, 123, 125,  -1,  -1},
      { 89,  90, 124, 126,  -1,  -1},
      { 61,  90,  91, 125,  -1,  -1}   // 126
  };

  for (Int_t i=0; i<127; i++)
      (*this)[i].SetNeighbors(nn[i][0], nn[i][1], nn[i][2],
                              nn[i][3], nn[i][4], nn[i][5]);
}

// --------------------------------------------------------------------------
//
//  Calculate the geometry information of CT1 and fill this information
//  into the pixel objects.
//
void MGeomCamCT1Daniel::CreateCam()
{
    //
    // fill the geometry class with the coordinates of the CT1 camera
    //
    //*fLog << inf << " Create CT1 geometry " << endl;

    //
    // this algorithm is from Martin Kestel originally
    // it was punt into a root/C++ context by Harald Kornmayer and Thomas Bretz
   
    const Float_t diameter = 21;    // units are mm
    const Float_t kS32  = sqrt(3)/2;

    //
    //  add the first pixel to the list
    //
    Int_t pixnum = 0;

    (*this)[pixnum++].Set(0, 0, diameter);

    for (Int_t ring=1; ring<7; ring++)
    {
        //
        // calc. coords for this ring counting from the
        // starting number to the ending number
        //
        for (int i=0; i<ring; i++)
            (*this)[pixnum++].Set((ring-i*0.5)*diameter,
                                  i*kS32*diameter,
                                  diameter);

        for (int i=0; i<ring; i++)
            (*this)[pixnum++].Set((ring*0.5-i)*diameter,
                                  ring*kS32 * diameter,
                                  diameter);

        for (int i=0; i<ring; i++)
            (*this)[pixnum++].Set(-(ring+i)*0.5*diameter,
                                  (ring-i)*kS32*diameter,
                                  diameter);

        for (int i=0; i<ring; i++)
            (*this)[pixnum++].Set((0.5*i-ring)*diameter,
                                  -i*kS32*diameter,
                                  diameter);

        for (int i=0; i<ring; i++)
            (*this)[pixnum++].Set((i-ring*0.5)*diameter,
                                  -ring*kS32 * diameter,
                                  diameter);

        for (int i=0; i<ring; i++)
            (*this)[pixnum++].Set((ring+i)*0.5*diameter,
                                  (-ring+i)*kS32*diameter,
                                  diameter);
    }
}

