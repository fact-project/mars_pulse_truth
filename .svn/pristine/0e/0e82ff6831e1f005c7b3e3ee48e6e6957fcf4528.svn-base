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
!   Author(s): Wolfgang Wittek, 09/2004 <mailto:wittek@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
//  MMakePadHistograms
//
//     reads calibrated data of a particular type (ON, OFF or MC)
//     and produces the histograms for the padding
//
//     these histograms are further treated in MPad
//
/////////////////////////////////////////////////////////////////////////////
#include "MMakePadHistograms.h"

#include <math.h>
#include <stdio.h>

#include <TH2.h>
#include <TH3.h>
#include <TCanvas.h>
#include <TFile.h>

#include "MBadPixelsCalc.h"
#include "MBadPixelsTreat.h"
#include "MContinue.h"
#include "MEvtLoop.h"
#include "MFillH.h"
#include "MFSelBasic.h"
#include "MGeomApply.h"
#include "MHBadPixels.h"
#include "MHSigmaTheta.h"
#include "MLog.h"
#include "MLogManip.h"
#include "MParList.h"
#include "MProgressBar.h"
#include "MReadMarsFile.h"
#include "MReadReports.h"
#include "MTaskList.h"
//#include "MExtrapolatePointingPos.h"
#include "MPointingPosCalc.h"

ClassImp(MMakePadHistograms);

using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor. 
//
MMakePadHistograms::MMakePadHistograms(const char *name, const char *title) 
{
  fName  = name  ? name  : "MMakePadHistograms";
  fTitle = title ? title : "Class to make Pad histograms";

  fHSigmaTheta      = NULL;
  fHSigmaThetaOuter = NULL;
  fHDiffPixTheta    = NULL;
  fHSigmaPixTheta   = NULL;

  fHBadIdTheta      = NULL;
  fHBadNTheta       = NULL;

  // default values
  fType             = "";
  fMaxEvents        = -1;
  fNamePedPhotCam   = "MPedPhotCamFromData";
  fNameInputFile    = "";
  fNameOutputFile   = "";
  fPedestalLevel    = 3.0;
  fUseInterpolation = kTRUE;
  fProcessPedestal  = kTRUE;
  fProcessTime      = kFALSE;
}


// --------------------------------------------------------------------------
//
// SetDataType. 
//
//    the type may be  ON, OFF or MC
//
void MMakePadHistograms::SetDataType(const char *type) 
{
  fType = type;
}

// --------------------------------------------------------------------------
//
// SetMaxEvents. 
//
//    set the maximum number of events to be read
//
void MMakePadHistograms::SetMaxEvents(Int_t maxev) 
{
  fMaxEvents = maxev;
}

// --------------------------------------------------------------------------
//
// SetNameInputFile. 
//
//    the input file contains the calibrated data
//
void MMakePadHistograms::SetNameInputFile(const char *input) 
{
  fNameInputFile = input;
}

// --------------------------------------------------------------------------
//
// SetNameOutputFile. 
//
//    the output file contains the histgrams for the padding
//
void MMakePadHistograms::SetNameOutputFile(const char *output) 
{
  fNameOutputFile = output;
}

// --------------------------------------------------------------------------
//
// SetNamePedPhotCam. 
//
//    set the name of the MPedPhotCam container;
//    it is used in : MBadPixelsCalc, MBadPixelsTreat, MHSigmaTheta
//
void MMakePadHistograms::SetNamePedPhotCam(const char *name) 
{
  fNamePedPhotCam = name;
}

// --------------------------------------------------------------------------
//
// SetPedestalLevel. 
//
//    set the pedestal level;
//    it is used in : MBadPixelsCalc
//
void MMakePadHistograms::SetPedestalLevel(Float_t pedlevel) 
{
  fPedestalLevel = pedlevel;
}

// --------------------------------------------------------------------------
//
// SetUseInterpolation. 
//
//    set the option "UseInterpolation";
//    it is used in : MBadPixelsTreat
//
void MMakePadHistograms::SetUseInterpolation(Bool_t useinter) 
{
  fUseInterpolation = useinter;
}

// --------------------------------------------------------------------------
//
// SetProcessPedestal. 
//
//    set the option "ProcessPedestal";
//    it is used in : MBadPixelsTreat
//
void MMakePadHistograms::SetProcessPedestal(Bool_t procped) 
{
  fProcessPedestal = procped;
}

// --------------------------------------------------------------------------
//
// SetProcessTime. 
//
//    set the option "ProcessTime";
//    it is used in : MBadPixelsTreat
//
void MMakePadHistograms::SetProcessTime(Bool_t proctim) 
{
  fProcessTime = proctim;
}

// --------------------------------------------------------------------------
//
//
Bool_t MMakePadHistograms::MakeHistograms()
{
      if (fType == "")
      {
        *fLog << err << "MMakePadHistograms::MakeHistograms;   type of input data is not defined"
              << endl;
        return kFALSE;
      }

      if (fNameInputFile == "")
      {
        *fLog << err << "MMakePadHistograms::MakeHistograms;   name of input file is not defined"
              << endl;
        return kFALSE;
      }

      if (fNameOutputFile == "")
      {
        *fLog << err << "MMakePadHistograms::MakeHistograms;   name of output file is not defined"
              << endl;
        return kFALSE;
      }

      *fLog << "------------" << endl;
      *fLog << fType << " events :" << endl;
      *fLog << "------------" << endl;
      *fLog << "MMakePadHistograms : calibrated " << fType
            << "  data are read from file " << fNameInputFile << endl;


      MTaskList tlist;
      MParList  plist;

      MReadReports read;
      read.AddTree("Drive");
      read.AddTree("Events","MTime.",kTRUE);
      static_cast<MRead&>(read).AddFile(fNameInputFile);

      read.AddToBranchList("MReportDrive.*");
      read.AddToBranchList("MRawEvtHeader.*");

//      MReadMarsFile  read("Events", fNameInputFile);
//      read.DisableAutoScheme();


      MGeomApply apply;

//      MExtrapolatePointingPos extraPointing(fNameInputFile);
      MPointingPosCalc pointposcalc;

      MBadPixelsCalc badcalc;
      badcalc.SetNamePedPhotCam(fNamePedPhotCam);
      badcalc.SetPedestalLevel(fPedestalLevel);

      MBadPixelsTreat badtreat;
      badtreat.SetNamePedPhotCam(fNamePedPhotCam);
      badtreat.SetUseInterpolation(fUseInterpolation);
      badtreat.SetProcessPedestal(fProcessPedestal);
      badtreat.SetProcessTimes(fProcessTime);

      MFSelBasic selbasic;
      MContinue contbasic(&selbasic);
      contbasic.SetName("SelBasic");

      MHBadPixels bad("BadPixels");
      bad.SetNamePedPhotCam(fNamePedPhotCam);
      MFillH fillbad("BadPixels[MHBadPixels]", "MBadPixelsCam");
      fillbad.SetName("FillBad");

      MHSigmaTheta sigth("SigmaTheta");
      sigth.SetNamePedPhotCam(fNamePedPhotCam);
      MFillH fillsigtheta ("SigmaTheta[MHSigmaTheta]", "");
      fillsigtheta.SetName("FillSigTheta");    
 
      //*****************************
      // entries in MParList
    
      plist.AddToList(&tlist);
      plist.AddToList(&bad);
      plist.AddToList(&sigth);

      //*****************************
      // entries in MTaskList
    
      tlist.AddToList(&read);
      tlist.AddToList(&apply);
      tlist.AddToList(&pointposcalc, "Events");

//      tlist.AddToList(&badcalc);  done in callisto
//      tlist.AddToList(&badtreat); done in callisto

      tlist.AddToList(&contbasic, "Events");
      tlist.AddToList(&fillbad, "Events");
      tlist.AddToList(&fillsigtheta, "Events");
      //*****************************

      MProgressBar bar;
      MEvtLoop evtloop;
      evtloop.SetParList(&plist);
      evtloop.SetProgressBar(&bar);

      //      if ( !evtloop.Eventloop(fMaxEvents) )
      //    return kFALSE;
      evtloop.Eventloop(fMaxEvents);

      tlist.PrintStatistics(0, kTRUE);

      bad.DrawClone();
      sigth.DrawClone();

      // save the histograms for the padding
      fHSigmaTheta     = (TH2D*)sigth.GetSigmaTheta();
      fHSigmaThetaOuter= (TH2D*)sigth.GetSigmaThetaOuter();
      fHSigmaPixTheta  = (TH3D*)sigth.GetSigmaPixTheta();
      fHDiffPixTheta   = (TH3D*)sigth.GetDiffPixTheta();

      fHBadIdTheta = (TH2D*)bad.GetBadId();
      fHBadNTheta  = (TH2D*)bad.GetBadN();

      TFile filewrite(fNameOutputFile, "RECREATE", "");
      fHSigmaTheta->Write();
      fHSigmaThetaOuter->Write();
      //fHSigmaPixTheta->Write();
      fHDiffPixTheta->Write();
      //fHBadIdTheta->Write();
      //fHBadNTheta->Write();
      filewrite.Close();

      *fLog << "" << endl;
      *fLog << "MMakePadHistograms : padding plots for " << fType
            << "  data written onto file " << fNameOutputFile << endl;

  return kTRUE;
}

// --------------------------------------------------------------------------










