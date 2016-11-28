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
!   Author(s): Javier Lopez 5/2004 <mailto:jlopez@ifae.es>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//   MFHVNotNominal
//
//   Filter to look if the HV monitored value is in agreement with the
//   expected nominal value. If this is not true it sets a flag in MBadPixelsCam
//   to not use this pixel in the analysis.
//   Moreover if more than a certain number of pixels (set by defauld to 230) is
//   deviated from the nominal value the event is rejected.
//
//  Input Containers:
//   MCameraHV
//
//  Output Containers:
//   MBadPixelsCam
//
//////////////////////////////////////////////////////////////////////////////
#include "MFHVNotNominal.h"

#include <fstream>
#include <stdlib.h>

#include <TMath.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MCameraHV.h"

#include "MBadPixelsPix.h"
#include "MBadPixelsCam.h"

ClassImp(MFHVNotNominal);

using namespace std;
// --------------------------------------------------------------------------
//
// Default constructor. 
//
MFHVNotNominal::MFHVNotNominal(const char *name, const char *title)
  : fHV(NULL), fBadPixels(NULL), fHVConfFile("NULL"), fMaxHVDeviation(0.03), fMaxNumPixelsDeviated(230)
{
    fName  = name  ? name  : "MFHVNotNominal";
    fTitle = title ? title : "Filter to reject events with too many pixels out of nominal HV";

    fCut[0] = 0;
    fCut[1] = 0;
}

// --------------------------------------------------------------------------
//
// The PreProcess searches for the following input containers:
//  - MCameraHV
//
// The following containers are searched and created if they were not found:
//  - MBadPixelsCam
//
Int_t MFHVNotNominal::PreProcess(MParList *pList)
{
  // 
  // Containers that have to be there.
  //
    fHV = (MCameraHV*)pList->FindObject("MCameraHV");
    if (!fHV)
    {
      *fLog << err << "MCameraHV not found... aborting." << endl;
      return kFALSE;
    }

    // 
    // Containers that are created in case that they are not there.
    //
    fBadPixels = (MBadPixelsCam*)pList->FindObject("MBadPixelsCam");
    if (!fBadPixels)
      {
	*fLog << err << "Cannot find nor create MBadPixelsCam... aborting" << endl;
	return kFALSE;
      }

    if (fHVConfFile != "NULL")
      {
        
	const UInt_t npix = 577;
	fHVNominal.Set(npix);

	// Here we get the nominal HV values directitly from a CC 
	// HV configuration file.
	// Those files have the following structure:
	//  [HV]
	//  pixel_'hwnumber'='hvvalue'
    
	ifstream fin(fHVConfFile);
    
	TString str;
	
	if(!fin)
	  {
	    *fLog << err << "Imposible to open CC HV configuration file " << fHVConfFile << endl;
	    return kFALSE;
	  }
	else
	    *fLog << dbg << "Opened CC HV configuration file " << fHVConfFile << endl;

	
	fin >> str;
	if(str != "[HV]")
	  {
	    *fLog << err << fHVConfFile << " file is not a CC HV configuration file" << endl;
	    return kFALSE;
	  }
        else
	    *fLog << dbg << fHVConfFile << " file is a good CC HV configuration file" << endl;

	UInt_t npixinfile=0;
	while(1)
	  {
	    fin >> str;
	    if (fin.eof())
	      break;

	    UInt_t equalpos = str.Index("=");
	    UInt_t underpos = str.Index("_");
	    UInt_t length   = str.Length();

	    TString tmp = str(underpos+1,equalpos-(underpos+1));
	    UInt_t   hwpix = atoi((const char*)tmp);
	    tmp = str(equalpos+1,length-(equalpos+1));
	    Double_t value = atof((const char*)tmp);
				  
	    fHVNominal[hwpix-1]=value;
	    npixinfile++;
	  }
	
	fin.close();

	if (npixinfile!=npix)
	  {
	    *fLog << err << fHVConfFile << " CC HV configuration file contain " << npixinfile << " pixels while the camera have " << npix << " pixels." << endl;
	    return kFALSE;
	  }	    
      }

    return kTRUE;
}


// ---------------------------------------------------------
//
// HVNotNominal rejection: 
// 
// Requiring less than fMaxHVDeviation deviation of HV monitored values
// from nominal ones.
// 
// fMaxNumPixelsDeviated is set to 230 by default which is slightly higher 
// than the number of outer pixels in MAGIC (for the case that 
// the outer pixels have some defect).
//
Bool_t MFHVNotNominal::HVNotNominalRejection()
{
  UInt_t npix = fHVNominal.GetSize();

  UInt_t notnominalpix = 0;

  for (UInt_t idx=1; idx<npix; idx++)
    {
      Double_t hv = (*fHV)[idx];
      Double_t nominal = fHVNominal[idx];
      Double_t dev = 2.*TMath::Abs(nominal-hv)/(nominal+hv);
      
      if (dev > fMaxHVDeviation)
	{
	  MBadPixelsPix &bad = (*fBadPixels)[idx];
	  bad.SetUnsuitable(MBadPixelsPix::kUnsuitableEvt);
	  bad.SetHardware(MBadPixelsPix::kHVNotNominal);
	  notnominalpix++;
	}
    }

    if (notnominalpix!=0)
      fCut[0]++;

    //
    // If the camera contains more than fMaxNumPixelsDeviated
    // pixels with deviated HV, then the event is discarted.
    //

    return notnominalpix > fMaxNumPixelsDeviated;
}

// -----------------------------------------------------------
//
//  Compare the HV monitored valued with the HV nominal ones.
//
Int_t MFHVNotNominal::Process()
{
    fResult = HVNotNominalRejection();

    if (fResult)
      fCut[1]++;

    return kTRUE;
}

Int_t MFHVNotNominal::PostProcess()
{
    if (GetNumExecutions()==0)
        return kTRUE;

    *fLog << inf << endl;
    *fLog << GetDescriptor() << " execution statistics:" << endl;
    *fLog << dec << setfill(' ');

    *fLog << " " << setw(7) << fCut[0] << " (" << setw(3) ;
    *fLog << (Int_t)(fCut[0]*100/GetNumExecutions()) ;
    *fLog << "%) 'some not nominal pixel' events" << endl;

    *fLog << " " << setw(7) << fCut[1] << " (" << setw(3) ;
    *fLog << (Int_t)(fCut[1]*100/GetNumExecutions()) ;
    *fLog << "%) rejected events" ;
    *fLog << " (with fMaxNumPixelsDeviated = " << fMaxNumPixelsDeviated << ")" << endl;

    return kTRUE;
}

