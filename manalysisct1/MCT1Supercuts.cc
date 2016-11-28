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
!   Author(s): Wolfgang Wittek, 08/2003 <mailto:wittek@mppmu.mpg.de>
!   Author(s): Thomas Bretz, 08/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//   MCT1Supercuts                                                         //
//                                                                         //
//   this is the container for the parameters of the supercuts             //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
#include "MCT1Supercuts.h"

#include <math.h>
#include <fstream>

#include "MParList.h"

#include "MLog.h"
#include "MLogManip.h"

ClassImp(MCT1Supercuts);

using namespace std;

// --------------------------------------------------------------------------
//
// constructor
//
MCT1Supercuts::MCT1Supercuts(const char *name, const char *title)
  : fParameters(104), fStepsizes(104),
    fLengthUp(fParameters.GetArray()),   fLengthLo(fParameters.GetArray()+8),
    fWidthUp(fParameters.GetArray()+16), fWidthLo(fParameters.GetArray()+24),
    fDistUp(fParameters.GetArray()+32),  fDistLo(fParameters.GetArray()+40),
    fAsymUp(fParameters.GetArray()+48),  fAsymLo(fParameters.GetArray()+56),
    fConcUp(fParameters.GetArray()+64),  fConcLo(fParameters.GetArray()+72),
    fLeakage1Up(fParameters.GetArray()+80), fLeakage1Lo(fParameters.GetArray()+88),
    fAlphaUp(fParameters.GetArray()+96)
{
    fName  = name  ? name  : "MCT1Supercuts";
    fTitle = title ? title : "Container for the supercut parameters";

    // set supercut parameters to their default values
    InitParameters();
}


// --------------------------------------------------------------------------
//
// set default values for the supercut parameters
//
void MCT1Supercuts::InitParameters()
{
    //---------------------------------------------------
    //  these are Daniel's original values for Mkn 421

    fLengthUp[0] =  0.315585;
    fLengthUp[1] =  0.001455;
    fLengthUp[2] =  0.203198;
    fLengthUp[3] =  0.005532;
    fLengthUp[4] = -0.001670;
    fLengthUp[5] = -0.020362;
    fLengthUp[6] =  0.007388;
    fLengthUp[7] = -0.013463;

    fLengthLo[0] =  0.151530;
    fLengthLo[1] =  0.028323;
    fLengthLo[2] =  0.510707;
    fLengthLo[3] =  0.053089;
    fLengthLo[4] =  0.013708;
    fLengthLo[5] =  2.357993;
    fLengthLo[6] =  0.000080;
    fLengthLo[7] = -0.007157;

    fWidthUp[0] =  0.145412;
    fWidthUp[1] = -0.001771;
    fWidthUp[2] =  0.054462;
    fWidthUp[3] =  0.022280;
    fWidthUp[4] = -0.009893;
    fWidthUp[5] =  0.056353;
    fWidthUp[6] =  0.020711;
    fWidthUp[7] = -0.016703;

    fWidthLo[0] =  0.089187;
    fWidthLo[1] = -0.006430;
    fWidthLo[2] =  0.074442;
    fWidthLo[3] =  0.003738;
    fWidthLo[4] = -0.004256;
    fWidthLo[5] = -0.014101;
    fWidthLo[6] =  0.006126;
    fWidthLo[7] = -0.002849;

    fDistUp[0] =  1.787943;
    fDistUp[1] =  0;
    fDistUp[2] =  2.942310;
    fDistUp[3] =  0.199815;
    fDistUp[4] =  0;
    fDistUp[5] =  0.249909;
    fDistUp[6] =  0.189697;
    fDistUp[7] =  0;

    fDistLo[0] =  0.589406;
    fDistLo[1] =  0;
    fDistLo[2] = -0.083964;
    fDistLo[3] = -0.007975;
    fDistLo[4] =  0;
    fDistLo[5] =  0.045374;
    fDistLo[6] = -0.001750;
    fDistLo[7] =  0;
    

    // dummy values

    fAsymUp[0] =  1.e10;
    fAsymUp[1] =  0.0;
    fAsymUp[2] =  0.0;
    fAsymUp[3] =  0.0;
    fAsymUp[4] =  0.0;
    fAsymUp[5] =  0.0;
    fAsymUp[6] =  0.0;
    fAsymUp[7] =  0.0;

    fAsymLo[0] = -1.e10;
    fAsymLo[1] =  0.0;
    fAsymLo[2] =  0.0;
    fAsymLo[3] =  0.0;
    fAsymLo[4] =  0.0;
    fAsymLo[5] =  0.0;
    fAsymLo[6] =  0.0;
    fAsymLo[7] =  0.0;

    fConcUp[0] =  1.e10;
    fConcUp[1] =  0.0;
    fConcUp[2] =  0.0;
    fConcUp[3] =  0.0;
    fConcUp[4] =  0.0;
    fConcUp[5] =  0.0;
    fConcUp[6] =  0.0;
    fConcUp[7] =  0.0;

    fConcLo[0] = -1.e10;
    fConcLo[1] =  0.0;
    fConcLo[2] =  0.0;
    fConcLo[3] =  0.0;
    fConcLo[4] =  0.0;
    fConcLo[5] =  0.0;
    fConcLo[6] =  0.0;
    fConcLo[7] =  0.0;

    fLeakage1Up[0] =  1.e10;
    fLeakage1Up[1] =  0.0;
    fLeakage1Up[2] =  0.0;
    fLeakage1Up[3] =  0.0;
    fLeakage1Up[4] =  0.0;
    fLeakage1Up[5] =  0.0;
    fLeakage1Up[6] =  0.0;
    fLeakage1Up[7] =  0.0;

    fLeakage1Lo[0] = -1.e10;
    fLeakage1Lo[1] =  0.0;
    fLeakage1Lo[2] =  0.0;
    fLeakage1Lo[3] =  0.0;
    fLeakage1Lo[4] =  0.0;
    fLeakage1Lo[5] =  0.0;
    fLeakage1Lo[6] =  0.0;
    fLeakage1Lo[7] =  0.0;

    fAlphaUp[0] = 13.123440; 
    fAlphaUp[1] = 0;
    fAlphaUp[2] = 0;
    fAlphaUp[3] = 0;
    fAlphaUp[4] = 0;
    fAlphaUp[5] = 0;
    fAlphaUp[6] = 0;
    fAlphaUp[7] = 0;

    //---------------------------------------------------
    // fStepsizes 
    // if == 0.0    the parameter will be fixed in the minimization
    //    != 0.0    initial step sizes for the parameters

    // LengthUp
    fStepsizes[0] = 0.03;
    fStepsizes[1] = 0.0002;
    fStepsizes[2] = 0.02;
    fStepsizes[3] = 0.0006;
    fStepsizes[4] = 0.0002;
    fStepsizes[5] = 0.002;
    fStepsizes[6] = 0.0008;
    fStepsizes[7] = 0.002;

    // LengthLo
    fStepsizes[8]  = 0.02;
    fStepsizes[9]  = 0.003;
    fStepsizes[10] = 0.05;
    fStepsizes[11] = 0.006;
    fStepsizes[12] = 0.002;
    fStepsizes[13] = 0.3;
    fStepsizes[14] = 0.0001;
    fStepsizes[15] = 0.0008;

    // WidthUp
    fStepsizes[16] = 0.02;
    fStepsizes[17] = 0.0002;
    fStepsizes[18] = 0.006;
    fStepsizes[19] = 0.003;
    fStepsizes[20] = 0.002;
    fStepsizes[21] = 0.006;
    fStepsizes[22] = 0.002;
    fStepsizes[23] = 0.002;

    // WidthLo
    fStepsizes[24] = 0.009;
    fStepsizes[25] = 0.0007;
    fStepsizes[26] = 0.008;
    fStepsizes[27] = 0.0004;
    fStepsizes[28] = 0.0005;
    fStepsizes[29] = 0.002;
    fStepsizes[30] = 0.0007;
    fStepsizes[31] = 0.003;

    // DistUp
    fStepsizes[32] = 0.2;
    fStepsizes[33] = 0.0;
    fStepsizes[34] = 0.3;
    fStepsizes[35] = 0.02;
    fStepsizes[36] = 0.0;
    fStepsizes[37] = 0.03;
    fStepsizes[38] = 0.02;
    fStepsizes[39] = 0.0;

    // DistLo
    fStepsizes[40] = 0.06;
    fStepsizes[41] = 0.0;
    fStepsizes[42] = 0.009;
    fStepsizes[43] = 0.0008;
    fStepsizes[44] = 0.0;
    fStepsizes[45] = 0.005;
    fStepsizes[46] = 0.0002;
    fStepsizes[47] = 0.0;

    // AsymUp
    fStepsizes[48] = 0.0;
    fStepsizes[49] = 0.0;
    fStepsizes[50] = 0.0;
    fStepsizes[51] = 0.0;
    fStepsizes[52] = 0.0;
    fStepsizes[53] = 0.0;
    fStepsizes[54] = 0.0;
    fStepsizes[55] = 0.0;

    // AsymLo
    fStepsizes[56] = 0.0;
    fStepsizes[57] = 0.0;
    fStepsizes[58] = 0.0;
    fStepsizes[59] = 0.0;
    fStepsizes[60] = 0.0;
    fStepsizes[61] = 0.0;
    fStepsizes[62] = 0.0;
    fStepsizes[63] = 0.0;

    // ConcUp
    fStepsizes[64] = 0.0;
    fStepsizes[65] = 0.0;
    fStepsizes[66] = 0.0;
    fStepsizes[67] = 0.0;
    fStepsizes[68] = 0.0;
    fStepsizes[69] = 0.0;
    fStepsizes[70] = 0.0;
    fStepsizes[71] = 0.0;

    // ConcLo
    fStepsizes[72] = 0.0;
    fStepsizes[73] = 0.0;
    fStepsizes[74] = 0.0;
    fStepsizes[75] = 0.0;
    fStepsizes[76] = 0.0;
    fStepsizes[77] = 0.0;
    fStepsizes[78] = 0.0;
    fStepsizes[79] = 0.0;

    // Leakage1Up
    fStepsizes[80] = 0.0;
    fStepsizes[81] = 0.0;
    fStepsizes[82] = 0.0;
    fStepsizes[83] = 0.0;
    fStepsizes[84] = 0.0;
    fStepsizes[85] = 0.0;
    fStepsizes[86] = 0.0;
    fStepsizes[87] = 0.0;

    // Leakage1Lo
    fStepsizes[88] = 0.0;
    fStepsizes[89] = 0.0;
    fStepsizes[90] = 0.0;
    fStepsizes[91] = 0.0;
    fStepsizes[92] = 0.0;
    fStepsizes[93] = 0.0;
    fStepsizes[94] = 0.0;
    fStepsizes[95] = 0.0;

    // AlphaUp
    fStepsizes[96]  = 0.0;
    fStepsizes[97]  = 0.0;
    fStepsizes[98]  = 0.0;
    fStepsizes[99]  = 0.0;
    fStepsizes[100] = 0.0;
    fStepsizes[101] = 0.0;
    fStepsizes[102] = 0.0;
    fStepsizes[103] = 0.0;
}


// --------------------------------------------------------------------------
//
// Set the parameter values from the array 'd'
//
//
Bool_t MCT1Supercuts::SetParameters(const TArrayD &d)
{
    if (d.GetSize() != fParameters.GetSize())
    {
        *fLog << err << "Sizes of d and of fParameters are different : "
              << d.GetSize() << ",  " << fParameters.GetSize() << endl;
        return kFALSE;
    }

    fParameters = d;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Set the step sizes from the array 'd'
//
//
Bool_t MCT1Supercuts::SetStepsizes(const TArrayD &d)
{
    if (d.GetSize() != fStepsizes.GetSize())
    {
        *fLog << err << "Sizes of d and of fStepsizes are different : "
              << d.GetSize() << ",  " << fStepsizes.GetSize() << endl;
        return kFALSE;
    }

    fStepsizes = d;

    return kTRUE;
}

















