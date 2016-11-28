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
!   Copyright: MAGIC Software Development, 2004
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////

#include "MHexagonFFT.h"

#include <TMath.h>

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MHexagonFFT);

using namespace std;

// ---------------------------------------------------------------------------
//
//  Default Constructor
//  Initializes random number generator and default variables
//
MHexagonFFT::MHexagonFFT()
{
}

/*
void MHexagonFFT::Prepare(Int_t num, Float_t scale)
{
    //Int_t    num   = 34;
    //Double_t scale = dist_y/dist_x;

    Int_t    cnt   = 108*num*num;

    psire.Set(num*num);
    psiim.Set(num*num);

//    for(int j=0; j<num; j++)
//    {
//        for(int n=0; n<num; n++)
//        {
//            if (arr_k_m_id[j][n]<0)
//                continue;
//
//            Double_t sumre=0;
//            Double_t sumim=0;

            for(int k=0; k<num; k++)
            {
                for(int m=0; m<34-k; m++)
                {
                    //Int_t new_ID=arr_k_m_id[k][m];
                    //if (new_ID<0)
                    //    continue;

                    Double_t dx = 0.5*(m-k)/num;
                    Double_t dy = 0.5*(m+k)/num*scale;

                    dx *= TMath::TwoPi()/3;
                    dy *= TMath::TwoPi()/fgSqrt3;

                    const Double_t cos1 = cos(dy*(j+n));
                    const Double_t cos2 = cos(dy*j);
                    const Double_t cos3 = cos(dy*n);

                    //Alternatie nach Paper:
                    psire[m*num+k] = 0.5*(
                        +cos1*cos(dx*(j-n))
                        +cos2*cos(dx*(j+2*n))
                        +cos3*cos(dx*(2*j+n)));

                    psiim[m*num+k] = 0.5*(
                        +cos1*sin(dx*(j-n))
                        +cos2*sin(dx*(j+2*n))
                        -cos3*sin(dx*(2*j+n)));


//                    psi_im *= i_inv;
//
//                    Double_t factor = (i_inv==1?1.:P_j_n(k,m));
//
//                    sumre += factor * (inre[new_ID]*psi_re - inim[new_ID]*psi_im);
//                    sumim += factor * (inre[new_ID]*psi_im + inim[new_ID]*psi_re);
}
            }

//            Double_t factor = (i_inv==1?1.:P_j_n(j,n)/cnt);
//
//            outre[arr_k_m_id[j][n]] = factor * sumre;
//            outim[arr_k_m_id[j][n]] = factor * sumim;
//        }
//    }
}
*/

void MHexagonFFT::Prepare(Float_t scale, Int_t num)
{
    static const Double_t fgSqrt3 = TMath::Sqrt(3.);

    fNum = num;

    MArrayD fCosX(num*num*num*3);
    MArrayD fCosY(num*num*num*3);
    MArrayD fSin(num*num*num*3);

    for(int j=0; j<3*num; j++)
    {
        for(int k=0; k<num; k++)
        {
            for(int m=0; m<num-k; m++)
            {
                Double_t dx = 0.5*(m-k)/num;
                Double_t dy = 0.5*(m+k)/num*scale;

                dx *= TMath::TwoPi()/3;
                dy *= TMath::TwoPi()/fgSqrt3;

                const Int_t idx = (m*num + k)*3*num;

                fCosX[idx+j] = TMath::Cos(dx*j);
                fCosY[idx+j] = TMath::Cos(dy*j);
                fSin [idx+j] = TMath::Sin(dx*j);
            }
        }
    }

    //fPsiRe.Set(num*num*num*num);
    //fPsiIm.Set(num*num*num*num);
    fPsi.Set(num*num*num*num*2);

    Int_t lim = num*(num+1)/2;

    fM.Set(lim);
    fK.Set(lim);
    fP.Set(lim);
    fIdx.Set(lim);

    for(int j=0; j<num; j++)
    {
        for(int n=0; n<num-j; n++)
        {
            int idx0 = num-n-1;
            int idx1 = idx0*(idx0+1)/2 + j;

            fM[idx1]=n;
            fK[idx1]=j;

            fP[idx1]=P(j,n);

            for(int k=0; k<num; k++)
            {
                for(int m=0; m<num-k; m++)
                {
                    const Int_t idx = (m*num + k)*3*num;

                    const Float_t cos1 = fCosY[idx+j+n];
                    const Float_t cos2 = fCosY[idx+j];
                    const Float_t cos3 = fCosY[idx+n];

                    int idx2 = num-m-1;
                    int idx3 = idx2*(idx2+1)/2 + k;
                    const Int_t id1 = idx1*lim + idx3;

                    //fPsiRe[id1]
                    Double_t fPsiRe
                        = 2*(
                          +cos1*fCosX[idx+TMath::Abs(j-n)]
                          +cos2*fCosX[idx+j+2*n]
                          +cos3*fCosX[idx+2*j+n]);

                    //fPsiIm[id1] = 2*(
                    Double_t fPsiIm = 2*(
                          +cos1*fSin[idx+TMath::Abs(j-n)]*TMath::Sign(1, j-n)
                          +cos2*fSin[idx+j+2*n]
                          -cos3*fSin[idx+2*j+n]);

                    fPsi[id1*2]   = fPsiRe;//fPsiRe[id1];
                    fPsi[id1*2+1] = fPsiIm;//fPsiIm[id1];
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
/*
void MHexagonFFT::DGT4(const MArrayD &inre,
                       const MArrayD &inim,
                       MArrayD &outre,
                       MArrayD &outim,
                       Float_t scale,
                       Bool_t fwd)
{
    Int_t num = (Int_t)TMath::Sqrt((Float_t)inim.GetSize());
    Int_t cnt = 108*num*num;
    Int_t lim = num*(num+1)/2;

    Float_t *endp = fP.GetArray()+lim;

    for (int idx1=0; idx1<lim; idx1++)
    {
        if (fK[idx1]>fM[idx1] && fwd)
            continue;

        Double_t sumre=0;
        Double_t sumim=0;

        Float_t  *psi = fPsi.GetArray() + idx1*lim*2;
        Float_t  *p   = fP.GetArray();
        Double_t *im  = inim.GetArray();
        Double_t *re  = inre.GetArray();

        while (p<endp)
        {
            const Float_t factor1 = (fwd?*p:1.);

            const Float_t psire = *psi++;
            const Float_t psiim = *psi++;

            sumre     += factor1 * (*re * psire - *im * psiim);
            if (fwd)
                sumim += factor1 * (*re * psiim + *im * psire);

            im++;
            re++;
            p++;
        }

        const Double_t factor2 = fwd?fP[idx1]/cnt:1.;

        outre[idx1] =  factor2 * sumre;
        outim[idx1] = -factor2 * sumim;
    }

    if (!fwd)
        return;

    for (int idx1=0; idx1<lim; idx1++)
    {
        if (fK[idx1]<fM[idx1])
            continue;

        outre[idx1] =  outre[fIdx[idx1]];
        outim[idx1] = -outim[fIdx[idx1]];
    }
    }*/

void MHexagonFFT::TransformFastFWD(const MArrayD &inre,
                                   MArrayD &outre,
                                   MArrayD &outim) const
{
    const UInt_t num = fP.GetSize();

    if (inre.GetSize()!=num)
    {
        cout << "ERROR - MHexagonFFT prepared for different size." << endl;
        return;
    }

    outre.Set(num);
    outim.Set(num);

    const Int_t cnt = 108*fNum*fNum;

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

        outre[fIdx[idx1]] =   outre[idx1] =  factor2 * sumre;
        outim[fIdx[idx1]] = -(outim[idx1] = -factor2 * sumim);

        /*
         outre[idx1] =  factor2 * sumre;
         outim[idx1] = -factor2 * sumim;

         outre[fIdx[idx1]] =  outre[idx1];
         outim[fIdx[idx1]] = -outim[idx1];
         */
    }
    /*
     for (UInt_t idx1=0; idx1<num; idx1++)
     {
        if (fK[idx1]<fM[idx1])
           continue;

        outre[idx1] =  outre[fIdx[idx1]];
        outim[idx1] = -outim[fIdx[idx1]];
     }
     */
}

void MHexagonFFT::TransformFastBWD(const MArrayD &inre,
                                   const MArrayD &inim,
                                   MArrayD &outre) const
{
    const UInt_t num = fP.GetSize();

    if (inre.GetSize()!=num)
    {
        cout << "ERROR - MHexagonFFT prepared for different size." << endl;
        return;
    }
    if (inim.GetSize()!=num)
    {
        cout << "ERROR - MHexagonFFT prepared for different size." << endl;
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
/*
void MHexagonFFT::DGT3(const MArrayD &inre,
                       const MArrayD &inim,
                       MArrayD &outre,
                       MArrayD &outim,
                       Float_t scale,
                       Bool_t fwd)
{
    Int_t num = (Int_t)TMath::Sqrt((Float_t)inim.GetSize());
    Int_t cnt = 108*num*num;

    for(int j=0; j<num; j++)
    {
        for(int n=0; n<num-j; n++)
        {
            if (j-n>0 && fwd)
                continue;

            Double_t sumre=0;
            Double_t sumim=0;

            Int_t lim = num*(num+1)/2;
            for (int idx0=0; idx0<lim; idx0++)

//            for(int k=0; k<num; k++)
            {
                int m = fM[idx0];
                int k = fK[idx0];
            //                for(int m=0; m<num-k; m++)

                {
                    const Int_t id = k*num + m;
                    const Int_t id1 = (((id*num)+n)*num+j)*2;

                    //Alternatie nach Paper:
                    const Float_t psire = fPsi[id1];   //fPsiRe[(id*num+n)*num+j];
                    const Float_t psiim = fPsi[id1+1]; //fPsiIm[(id*num+n)*num+j]*inv;

                    const Float_t factor1 = fwd==1?P(k,m):1.;

                    sumre += factor1 * (inre[id]*psire - inim[id]*psiim);
                    if (fwd)
                        sumim += factor1 * (inre[id]*psiim + inim[id]*psire);
                }
            }

            const Double_t factor2 = fwd==1?P(j,n)/cnt:1.;

            outre[j*num+n] =  factor2 * sumre;
            outim[j*num+n] = -factor2 * sumim;

            if (!fwd)
                continue;

            outre[n*num+j] =  factor2 * sumre;
            outim[n*num+j] =  factor2 * sumim;
        }
    }
}

void MHexagonFFT::DGT2(const MArrayD &inre,
                       const MArrayD &inim,
                       MArrayD &outre,
                       MArrayD &outim,
                       Float_t scale,
                       Bool_t fwd)
{
    Int_t num = (Int_t)TMath::Sqrt((Float_t)inim.GetSize());
    Int_t cnt = 108*num*num;
    Int_t inv = fwd?-1:1;

    for(int j=0; j<num; j++)
    {
        for(int n=0; n<num-j; n++)
        {
            if (j-n>0 && fwd)
                continue;

            Double_t sumre=0;
            Double_t sumim=0;

            for(int k=0; k<num; k++)
            {
                for(int m=0; m<num-k; m++)
                {
                    const Int_t idx = (m*num + k)*3*num;

                    const Float_t cos1 = fCosY[idx+j+n];
                    const Float_t cos2 = fCosY[idx+j];
                    const Float_t cos3 = fCosY[idx+n];

                    //Alternatie nach Paper:
                    const Float_t psire = 2*(
                          +cos1*fCosX[idx+TMath::Abs(j-n)]
                          +cos2*fCosX[idx+j+2*n]
                          +cos3*fCosX[idx+2*j+n]);

                    const Float_t psiim = 2*inv*(
                          +cos1*fSin[idx+TMath::Abs(j-n)]*TMath::Sign(1, j-n)
                          +cos2*fSin[idx+j+2*n]
                          -cos3*fSin[idx+2*j+n]);

                    const Float_t factor = (fwd==1?P(k,m):1.);

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
*/
void MHexagonFFT::TransformSlow(const MArrayD &inre, const MArrayD &inim,
                                MArrayD &outre,      MArrayD &outim,
                                Float_t scale,       Bool_t fwd)
{
    static const Double_t fgSqrt3 = TMath::Sqrt(3.);

    Int_t num = (Int_t)TMath::Sqrt((Float_t)inim.GetSize());
    Int_t cnt = 108*num*num;
    Int_t inv = fwd?-1:1;

    for(int j=0; j<num; j++)
    {
        for(int n=0; n<num-j; n++)
        {
            if (j-n>0 && fwd)
                continue;

            Double_t sumre=0;
            Double_t sumim=0;

            for(int k=0; k<num; k++)
            {
                for(int m=0; m<num-k; m++)
                {
                    Double_t dx = 0.5*(m-k)/num;
                    Double_t dy = 0.5*(m+k)/num*scale;

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
