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
!   Author(s): Christoph Kolodziejski, 11/2004  <mailto:>
!   Author(s): Thomas Bretz, 11/2004  <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2004-2005
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
// MHexagonalFT
//
// This is a class representating a (fast) fourier transformation explicitly
// for hexagonal geometries as described in astro-ph/0409388.
//
//
// WARNING:
// ========
// Be carefull using the fast transformation (Prepare())! The precalculation
// consumes a lot of memory. fPsi has the size of 2*n^4 (while n is the
// number of rows in fourier space). For the enhanced MAGIC camery fPsi has
// the size 27691682*sizeof(float) = 105.6MB (Std MAGIC: ~12MB)
//
// The runtime is more or less determined by the speed of accessing a
// huge amount of memory (see above) sequentially.
//
//
// Coordinate systems:
// ===================
//
//  original hexagonal structure     enhanced hexagonal structure
//  ----------------------------     ----------------------------
//
//                            structure
//                            ---------
//
//                h h h                  f f h h h f f
//               h h h h                  f h h h h f
//              h h h h h       ----->     h h h h h
//               h h h h                    h h h h
//                h h h                      h h h
//                                            f f
//                                             f
//
//                            numbering
//                            ---------
//                d c b                  m n o p q r s
//               e 4 3 a                  g h i j k l
//              f 5 1 2 9       ----->     b c d e f
//               g 6 7 8                    7 8 9 a
//                h i j                      4 5 6
//                                            2 3
//                                             1
//
// In reality the fourier space looks like because of symmetries:
//
//            real part                 imaginary part
//            ---------                 --------------
//          m n o p o n m          m   n   o   0  -o  -n  -m
//           g h i i h g             g   h   i  -i  -h  -g
//            b c d c b                b   c   0  -c  -b
//             7 8 8 7                   7   8  -8  -7
//              4 5 4                      4   0  -4
//               2 2                         2  -2
//                1                            0
//
//         column: GetK()                  row: GetM()
//         --------------                  -----------
//          6 5 4 3 2 1 0                 0 1 2 3 4 5 6
//           5 4 3 2 1 0                   0 1 2 3 4 5
//            4 3 2 1 0                     0 1 2 3 4
//             3 2 1 0                       0 1 2 3
//              2 1 0                         0 1 2
//               1 0                           0 1
//                0                             0
//
//       row: GetRow() (m+k)        column: GetCol() (m-k)
//       -------------------        ----------------------
//          6 6 6 6 6 6 6         -6  -4  -2   0   2   4   6
//           5 5 5 5 5 5            -5  -3  -1   1   3   5
//            4 4 4 4 4               -4  -2   0   2   4
//             3 3 3 3                  -3  -1   1   3
//              2 2 2                     -2   0   2
//               1 1                        -1   1
//                0                            0
//
//
// The coordinates of the pixels in the triangle are:
//
//    Double_t dx; // Distance of too pixels in x
//    Double_t dy; // Distance of to pixel rows in y
//    Int_t idx;   // Index of pixel in triangle (see above)
//
//    const Float_t x = dx*GetCol(idx);
//    const Float_t y = dy*Int_t(GetRow(idx)-2*GetNumRows()/3);
//
// You can use MGeomCam::GetPixelIdxXY(x, y) to get the corresponding index
// in space space.
//
//////////////////////////////////////////////////////////////////////////////
#include "MHexagonalFT.h"

#include <TMath.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MArrayD.h"

ClassImp(MHexagonalFT);

using namespace std;

// ---------------------------------------------------------------------------
//
//  Default Constructor - empty
//
MHexagonalFT::MHexagonalFT()
{
}

// ---------------------------------------------------------------------------
//
//  Default Constructor - num is the number of lines the fourier space has.
//  It calls Prepare to fill the arrays with the necessary coefficients.
//
//  Here are some simple rules to calculate parameters in a hexagonal space:
//
// Number of Rings (r) ---> Number of Pixels (p)
// p = 3*r*(r-1)+1
//
// Number of Pixels (p) ---> Number of Rings (r)
// p = (sqrt(9+12*(p-1))+3)/6
//
// Number of pixels at one border == number of rings (r)
// Row of border                  == number of rings (r)
//
// Number of rows to get a triangle: 3r-2
//
MHexagonalFT::MHexagonalFT(Int_t num)
{
    Prepare(num);
}

// ---------------------------------------------------------------------------
//
// Calculate the contents of: fM, fK, fP, fIdx and fPsi.
//
// While fPsi are the fourier coefficients, fM and fK are the hexagonal x and
// y coordinates of the pixel corresponding to the index i which is the
// common index of all arrays. fP is P(i,j) for all pixels.
//
// fIdx is also filled and used for reverse mapping. Due to the geometry
// the right and left side of the fourier space triangle has identical
// values. fIdx 'maps' the indices from the right to the left side.
//
void MHexagonalFT::Prepare(Int_t num)
{
    fNumRows = num;

    fPsi.Set(num*num*num*num*2);

    Int_t lim = num*(num+1)/2;

    fM.Set(lim);
    fK.Set(lim);
    fP.Set(lim);
    fIdx.Set(lim);

    for(int j=0; j<num; j++)
    {
        for(int n=0; n+j<num; n++)
        {
            int idx1 = (j+n)*(j+n+1)/2 + j;

            fM[idx1]=n;
            fK[idx1]=j;

            fP[idx1]=P(j,n);

            for(int k=0; k<num; k++)
            {
                for(int m=0; m+k<num; m++)
                {
                    const Double_t dx = TMath::Pi()*(m-k)/(num-1)/3;
                    const Double_t dy = TMath::Pi()*(m+k)/(num-1);

                    const Double_t cos1 = TMath::Cos(dy*(j+n));
                    const Double_t cos2 = TMath::Cos(dy*j);
                    const Double_t cos3 = TMath::Cos(dy*n);

                    const Double_t psire = 2*(
                        +cos1*TMath::Cos(dx*(j-n))
                        +cos2*TMath::Cos(dx*(j+2*n))
                        +cos3*TMath::Cos(dx*(2*j+n)));

                    const Double_t psiim = 2*(
                        +cos1*TMath::Sin(dx*(j-n))
                        +cos2*TMath::Sin(dx*(j+2*n))
                        -cos3*TMath::Sin(dx*(2*j+n)));

                    const Int_t idx3 = (k+m)*(k+m+1)/2 + k;
                    const Int_t id1  = idx1*lim + idx3;

                    fPsi[id1*2]   = psire;
                    fPsi[id1*2+1] = psiim;
                }
            }
        }
    }

    for (int idx1=0; idx1<lim; idx1++)
    {
        int n = fM[idx1];
        int j = fK[idx1];

        int idx0;
        for (idx0=0; idx0<lim; idx0++)
            if (fM[idx0]==j && fK[idx0]==n)
                break;

        fIdx[idx1]=idx0;
    }

}

// ---------------------------------------------------------------------------
//
// Do a fast forward tranformation. Because all coefficients are
// precalculated, the tranformation is reduced to a simple pointer based
// loop over the coeffiecients multiplied with the corresponding input
// values.
//
// Parameters:
//  inre: array storing the real part of the input (eg. pixel contents)
// outre: array storing the real part of the output
// outim: array storing the imaginary part of the output
//
// inre must be of the size of the fourier space triangle. The pixel
// contents must have been mapped into this new space with the proper
// pixel indices. The size of outre and outim is set accordingly.
//
// After initialization (Prepare()) you can get the size of the arrays with
// GetNumKnots()
//
// For the definition of the coordinate system see class description
//
void MHexagonalFT::TransformFastFWD(const MArrayD &inre, MArrayD &outre,
                                    MArrayD &outim) const
{
    const UInt_t num = fP.GetSize();

    if (inre.GetSize()!=num)
    {
        cout << "ERROR - MHexagonalFT prepared for different size." << endl;
        return;
    }

    outre.Set(num);
    outim.Set(num);

    const Int_t cnt = 108*(fNumRows-1)*(fNumRows-1);

    const Float_t *endp = fP.GetArray()+num;

    for (UInt_t idx1=0; idx1<num; idx1++)
    {
        if (fK[idx1]>fM[idx1])
            continue;

        Double_t sumre=0;
        Double_t sumim=0;

        Float_t  *psi = fPsi.GetArray() + idx1*num*2;
        Float_t  *p   = fP.GetArray();
        Double_t *re  = inre.GetArray();

        // 1st access to psi: const Float_t psire = *psi++;
        // 2nd access to psi: const Float_t psiim = *psi++;
        // sumre += f * *psire;
        // sumim += f * *psiim;
        while (p<endp)
        {
            const Double_t f = *p++ * *re++;

            sumre += f * *psi++;
            sumim += f * *psi++;
        }

        const Double_t factor2 = fP[idx1]/cnt;

        outre[fIdx[idx1]] =  (outre[idx1] =  factor2 * sumre);
        outim[fIdx[idx1]] = -(outim[idx1] = -factor2 * sumim);
    }
}

// ---------------------------------------------------------------------------
//
// Do a fast backward tranformation. Because all coefficients are
// precalculated, the tranformation is reduced to a simple pointer based
// loop over the coeffiecients multiplied with the corresponding input
// values.
//
// Parameters:
//  inre: outre of TransformFastBwd
//  inim: outim of TransformFastBwd
// outre: backward tranformed real part of the resulting
//
// inre and inim must be of the size of the fourier space triangle. The
// pixel contents must have been mapped into this new space with the proper
// pixel indices. The size of outre is set accordingly.
//
// After initialization (Prepare()) you can get the size of the arrays with
// GetNumKnots()
//
// For the definition of the coordinate system see class description
//
void MHexagonalFT::TransformFastBWD(const MArrayD &inre, const MArrayD &inim,
                                    MArrayD &outre) const
{
    const UInt_t num = fP.GetSize();

    // Sanity check: check size of arrays
    if (inre.GetSize()!=num)
    {
        cout << "ERROR - MHexagonalFT prepared for different size." << endl;
        return;
    }
    if (inim.GetSize()!=num)
    {
        cout << "ERROR - MHexagonalFT prepared for different size." << endl;
        return;
    }
    outre.Set(num);

    const Double_t *endre = inre.GetArray()+num;

    for (UInt_t idx1=0; idx1<num; idx1++)
    {
        Float_t  *psi = fPsi.GetArray() + idx1*num*2;
        Double_t *im  = inim.GetArray();
        Double_t *re  = inre.GetArray();

        Double_t sumre=0;
        while (re<endre)
        {
            const Float_t psire = *psi++;
            const Float_t psiim = *psi++;

            sumre += *re++ * psire - *im++ * psiim;
        }

        outre[idx1] = sumre;
    }
}

// ---------------------------------------------------------------------------
//
// This is a slow (direct) version of the tranformation. It is identical
// for forward and backward tranformation.
//
// The whole calculation is done straight forward without any precalculation.
//
// Parameters:
//  inre:  real part of input
//  inim:  imaginary part of input
//  outre: real part of output
//  outim: imaginary part of output
//  fwd:   kTRUE for forward, kFALSE for backward transformations
//
// After initialization (Prepare()) you can get the size of the arrays with
// GetNumKnots()
//
// For the definition of the coordinate system see class description
//
// It is currently not tested!
//
void MHexagonalFT::TransformSlow(const MArrayD &inre, const MArrayD &inim,
                                 MArrayD &outre,      MArrayD &outim,
                                 Bool_t fwd)
{
    static const Double_t fgSqrt3 = TMath::Sqrt(3.);
    static const Double_t fgTan30 = TMath::Tan(30*TMath::DegToRad())*3;

    Int_t num = (Int_t)TMath::Sqrt((Float_t)inim.GetSize());
    Int_t cnt = 108*(num-1)*(num-1);
    Int_t inv = fwd?-1:1;

    // FIXME: For p(j,n)
    fNumRows = num;

    for(int j=0; j<num; j++)
    {
        for(int n=0; n+j<num; n++)
        {
            if (j-n>0 && fwd)
                continue;

            Double_t sumre=0;
            Double_t sumim=0;

            for(int k=0; k<num; k++)
            {
                for(int m=0; m+k<num; m++)
                {
                    Double_t dx = 0.5*(m-k)/num;
                    Double_t dy = 0.5*(m+k)/num*fgTan30;

                    dx *= TMath::TwoPi()/3;
                    dy *= TMath::TwoPi()/fgSqrt3;

                    const Double_t cos1 = TMath::Cos(dy*(j+n));
                    const Double_t cos2 = TMath::Cos(dy*j);
                    const Double_t cos3 = TMath::Cos(dy*n);

                    //Alternatie nach Paper:
                    const Double_t psire = 2*(
                        +cos1*TMath::Cos(dx*(j-n))
                        +cos2*TMath::Cos(dx*(j+2*n))
                        +cos3*TMath::Cos(dx*(2*j+n)));

                    const Double_t psiim = 2*inv*(
                        +cos1*TMath::Sin(dx*(j-n))
                        +cos2*TMath::Sin(dx*(j+2*n))
                        -cos3*TMath::Sin(dx*(2*j+n)));

                    const Double_t factor = (fwd==1?P(k,m):1.);

                    sumre += factor * (inre[k*num+m]*psire - inim[k*num+m]*psiim);
                    sumim += factor * (inre[k*num+m]*psiim + inim[k*num+m]*psire);
                }
            }

            const Double_t factor = (fwd==1?P(j,n)/cnt:1.);

            outre[j*num+n] = factor * sumre;
            outim[j*num+n] = factor * sumim;

            if (fwd)
            {
                outre[n*num+j] =  factor * sumre;
                outim[n*num+j] = -factor * sumim;
            }
        }
    }
}


