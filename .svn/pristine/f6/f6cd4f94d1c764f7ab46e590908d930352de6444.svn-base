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
!   Author(s): Thomas Bretz, 6/2003 <mailto:tbretz@astro.uni-wuerzburg.de>
!   Author(s): Wolfgang Wittek, 7/2003 <mailto:wittek@mppmu.mpg.de>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MCT1FindSupercuts                                                       //
//                                                                         //
// Class for otimizing the parameters of the supercuts                     //
//                                                                         //
//                                                                         //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
#include "MCT1FindSupercuts.h"

#include <math.h>            // fabs 

#include <TFile.h>
#include <TArrayD.h>
#include <TMinuit.h>
#include <TCanvas.h>
#include <TStopwatch.h>
#include <TVirtualFitter.h>

#include "MBinning.h"
#include "MContinue.h"
#include "MCT1Supercuts.h"
#include "MCT1SupercutsCalc.h"
#include "MDataElement.h"
#include "MDataMember.h"

#include "MEvtLoop.h"
#include "MFCT1SelFinal.h"
#include "MF.h"
#include "MFEventSelector.h"
#include "MFEventSelector2.h"
#include "MFillH.h"
//#include "MGeomCamCT1Daniel.h"
#include "MFEventSelector.h"
#include "MGeomCamCT1.h"
#include "MH3.h"
#include "MHCT1Supercuts.h"
#include "MHFindSignificance.h"
#include "MHMatrix.h"
#include "MHOnSubtraction.h"

#include "MLog.h"
#include "MLogManip.h"
#include "MMatrixLoop.h"
#include "MMinuitInterface.h"
#include "MParList.h"
#include "MProgressBar.h"
#include "MReadMarsFile.h"
#include "MReadTree.h"
#include "MTaskList.h"


ClassImp(MCT1FindSupercuts);

using namespace std;


//------------------------------------------------------------------------
//
// fcnSupercuts 
//
// - calculates the quantity to be minimized (using TMinuit)
//
// - the quantity to be minimized is (-1)*significance of the gamma signal
//   in the alpha distribution (after cuts)
//
// - the parameters to be varied in the minimization are the cut parameters
//   (par)
//
static void fcnSupercuts(Int_t &npar, Double_t *gin, Double_t &f, 
                         Double_t *par, Int_t iflag)
{
    //cout <<  "entry fcnSupercuts" << endl;

    //-------------------------------------------------------------
    // save pointer to the MINUIT object for optimizing the supercuts
    // because it will be overwritten 
    // when fitting the alpha distribution in MHFindSignificance
    TMinuit *savePointer = gMinuit;
    //-------------------------------------------------------------


    MEvtLoop *evtloopfcn = (MEvtLoop*)gMinuit->GetObjectFit();

    MParList *plistfcn   = (MParList*)evtloopfcn->GetParList();
    //MTaskList *tasklistfcn   = (MTaskList*)plistfcn->FindObject("MTaskList");

    MCT1Supercuts *super = (MCT1Supercuts*)plistfcn->FindObject("MCT1Supercuts");
    if (!super)
    {
        gLog << "fcnSupercuts : MCT1Supercuts object '" << "MCT1Supercuts"
            << "' not found... aborting" << endl;
        return;
    }

    //
    // transfer current parameter values to MCT1Supercuts
    //
    // Attention : npar is the number of variable parameters
    //                  not the total number of parameters
    //
    Double_t fMin, fEdm, fErrdef;
    Int_t     fNpari, fNparx, fIstat;
    gMinuit->mnstat(fMin, fEdm, fErrdef, fNpari, fNparx, fIstat);

    super->SetParameters(TArrayD(fNparx, par));

    //$$$$$$$$$$$$$$$$$$$$$
    // for testing
    //TArrayD checkparameters = super->GetParameters();
    //gLog << "fcnsupercuts : fNpari, fNparx =" << fNpari << ",  " 
    //     << fNparx  << endl;
    //gLog << "fcnsupercuts : i, par, checkparameters =" << endl;
    //for (Int_t i=0; i<fNparx; i++)
    //{
    //  gLog << i << ",  " << par[i] << ",  " << checkparameters[i] << endl;
    //}
    //$$$$$$$$$$$$$$$$$$$$$

    //
    // plot alpha with the current cuts
    //
    evtloopfcn->Eventloop();

    //tasklistfcn->PrintStatistics(0, kTRUE);

    MH3* alpha = (MH3*)plistfcn->FindObject("AlphaFcn", "MH3");
    if (!alpha)
        return;

    TH1 &alphaHist = alpha->GetHist();
    alphaHist.SetName("alpha-fcnSupercuts");

    //-------------------------------------------
    // set Minuit pointer to zero in order not to destroy the TMinuit
    // object for optimizing the supercuts
    gMinuit = NULL;

    //=================================================================
    // fit alpha distribution to get the number of excess events and
    // calculate significance of gamma signal in the alpha plot
  
    const Double_t alphasig = 20.0;
    const Double_t alphamin = 30.0;
    const Double_t alphamax = 90.0;
    const Int_t    degree   =    2;

    Bool_t drawpoly;
    Bool_t fitgauss;
    if (iflag == 3)
    {
        drawpoly  = kTRUE;
        fitgauss  = kTRUE;
    }
    else
    {
        drawpoly  = kFALSE;
        fitgauss  = kFALSE;
    }
    //drawpoly  = kFALSE;
    //fitgauss  = kFALSE;

    const Bool_t print = kTRUE;

    MHFindSignificance findsig;
    findsig.SetRebin(kTRUE);
    findsig.SetReduceDegree(kFALSE);
    
    const Bool_t rc = findsig.FindSigma(&alphaHist, alphamin, alphamax, degree,
                                        alphasig, drawpoly, fitgauss, print);

    //=================================================================

    // reset gMinuit to the MINUIT object for optimizing the supercuts 
    gMinuit = savePointer;
    //-------------------------------------------

    if (!rc)
    {
        gLog << "fcnSupercuts : FindSigma() failed" << endl;
        f = 1.e10;
        return;
    }

    // plot some quantities during the optimization
    MHCT1Supercuts *plotsuper = (MHCT1Supercuts*)plistfcn->FindObject("MHCT1Supercuts");
    if (plotsuper)
        plotsuper->Fill(&findsig);

    //------------------------
    // get significance
    const Double_t significance = findsig.GetSignificance();
    f = significance>0 ? -significance : 0;


    //------------------------
    // optimize signal/background ratio
    //Double_t ratio = findsig.GetNbg()>0.0 ? 
    //                 findsig.GetNex()/findsig.GetNbg() : 0.0; 
    //f = -ratio;

    //-------------------------------------------
    // final calculations
    //if (iflag == 3)
    //{
    //
    //}    

    //-------------------------------------------------------------
}


// --------------------------------------------------------------------------
//
// Default constructor.
//
MCT1FindSupercuts::MCT1FindSupercuts(const char *name, const char *title)
: fHowManyTrain(10000), fHowManyTest(10000), fMatrixFilter(NULL)
{
    fName  = name  ? name  : "MCT1FindSupercuts";
    fTitle = title ? title : "Optimizer of the supercuts";

    //---------------------------
    // camera geometry is needed for conversion mm ==> degree
    //fCam = new MGeomCamCT1Daniel; 
    fCam = new MGeomCamCT1; 

    // matrices to contain the training/test samples
    fMatrixTrain = new MHMatrix("MatrixTrain");
    fMatrixTest  = new MHMatrix("MatrixTest");

    // objects of MCT1SupercutsCalc to which these matrices are attached
    fCalcHadTrain = new MCT1SupercutsCalc("SupercutsCalcTrain");
    fCalcHadTest  = new MCT1SupercutsCalc("SupercutsCalcTest");

    // Define columns of matrices
    fCalcHadTrain->InitMapping(fMatrixTrain);
    fCalcHadTest->InitMapping(fMatrixTest);
}

// --------------------------------------------------------------------------
//
// Default destructor.
//
MCT1FindSupercuts::~MCT1FindSupercuts()
{
    delete fCam;
    delete fMatrixTrain;
    delete fMatrixTest;
    delete fCalcHadTrain;
    delete fCalcHadTest;
}

// --------------------------------------------------------------------------
//
// Define the matrix 'fMatrixTrain' for the training sample
//
// alltogether 'howmanytrain' events are read from file 'nametrain';
// the events are selected according to a target distribution 'hreftrain'
//
//
Bool_t MCT1FindSupercuts::DefineTrainMatrix(
			  const TString &nametrain, MH3 &hreftrain,
	                  const Int_t howmanytrain, const TString &filetrain)
{
    if (nametrain.IsNull() || howmanytrain <= 0)
        return kFALSE;

    *fLog << "=============================================" << endl;
    *fLog << "fill training matrix from file '" << nametrain 
          << "',   select " << howmanytrain 
          << " events " << endl;
    if (!hreftrain.GetHist().GetEntries()==0)
    {
      *fLog << "     according to a distribution given by the MH3 object '"
            << hreftrain.GetName() << "'" << endl;
    }
    else
    {
      *fLog << "     randomly" << endl;
    }


    MParList  plist;
    MTaskList tlist;

    MReadMarsFile read("Events", nametrain);
    read.DisableAutoScheme();

    MFEventSelector2 seltrain(hreftrain);
    seltrain.SetNumMax(howmanytrain);
    seltrain.SetName("selectTrain");

    MFillH filltrain(fMatrixTrain);
    filltrain.SetFilter(&seltrain);
    filltrain.SetName("fillMatrixTrain");

    //******************************
    // entries in MParList 
    
    plist.AddToList(&tlist);
    plist.AddToList(fCam);
    plist.AddToList(fMatrixTrain);

    //******************************
    // entries in MTaskList 

    tlist.AddToList(&read);
    tlist.AddToList(&seltrain);
    tlist.AddToList(&filltrain);

    //******************************

    MProgressBar bar;
    MEvtLoop evtloop;
    evtloop.SetParList(&plist);
    evtloop.SetName("EvtLoopMatrixTrain");
    evtloop.SetProgressBar(&bar);

    if (!evtloop.Eventloop())
      return kFALSE;

    tlist.PrintStatistics(0, kTRUE);

    fMatrixTrain->Print("SizeCols");
    Int_t howmanygenerated = fMatrixTrain->GetM().GetNrows();
    if (TMath::Abs(howmanygenerated-howmanytrain) > TMath::Sqrt(9.*howmanytrain))
    {
      *fLog << "MCT1FindSupercuts::DefineTrainMatrix; no.of generated events ("
	    << howmanygenerated 
            << ") is incompatible with the no.of requested events ("
            << howmanytrain << ")" << endl;
    }

    *fLog << "training matrix was filled" << endl;
    *fLog << "=============================================" << endl;

    //--------------------------
    // write out training matrix

    if (filetrain != "")
    {
      TFile filetr(filetrain, "RECREATE");
      fMatrixTrain->Write();
      filetr.Close();

      *fLog << "MCT1FindSupercuts::DefineTrainMatrix; Training matrix was written onto file '"
            << filetrain << "'" << endl;
    }


    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Define the matrix for the test sample
//
// alltogether 'howmanytest' events are read from file 'nametest'
//
// the events are selected according to a target distribution 'hreftest'
//
//
Bool_t MCT1FindSupercuts::DefineTestMatrix(
			  const TString &nametest, MH3 &hreftest,
	                  const Int_t howmanytest, const TString &filetest)
{
    if (nametest.IsNull() || howmanytest<=0)
        return kFALSE;

    *fLog << "=============================================" << endl;
    *fLog << "fill test matrix from file '" << nametest 
          << "',   select " << howmanytest 
          << " events " << endl;
    if (!hreftest.GetHist().GetEntries()==0)
    {
      *fLog << "     according to a distribution given by the MH3 object '"
            << hreftest.GetName() << "'" << endl;
    }
    else
    {
      *fLog << "     randomly" << endl;
    }


    MParList  plist;
    MTaskList tlist;

    MReadMarsFile read("Events", nametest);
    read.DisableAutoScheme();

    MFEventSelector2 seltest(hreftest);
    seltest.SetNumMax(howmanytest);
    seltest.SetName("selectTest");
 
    MFillH filltest(fMatrixTest);
    filltest.SetFilter(&seltest);

    //******************************
    // entries in MParList 
    
    plist.AddToList(&tlist);
    plist.AddToList(fCam);
    plist.AddToList(fMatrixTest);

    //******************************
    // entries in MTaskList 

    tlist.AddToList(&read);
    tlist.AddToList(&seltest);
    tlist.AddToList(&filltest);

    //******************************

    MProgressBar bar;
    MEvtLoop evtloop;
    evtloop.SetParList(&plist);
    evtloop.SetName("EvtLoopMatrixTest");
    evtloop.SetProgressBar(&bar);

    if (!evtloop.Eventloop())
      return kFALSE;

    tlist.PrintStatistics(0, kTRUE);

    fMatrixTest->Print("SizeCols");
    const Int_t howmanygenerated = fMatrixTest->GetM().GetNrows();
    if (TMath::Abs(howmanygenerated-howmanytest) > TMath::Sqrt(9.*howmanytest))
    {
      *fLog << "MCT1FindSupercuts::DefineTestMatrix; no.of generated events ("
	    << howmanygenerated 
            << ") is incompatible with the no.of requested events ("
            << howmanytest << ")" << endl;
    }

    *fLog << "test matrix was filled" << endl;
    *fLog << "=============================================" << endl;

    //--------------------------
    // write out test matrix

    if (filetest != "")
    {
      TFile filete(filetest, "RECREATE", "");
      fMatrixTest->Write();
      filete.Close();

      *fLog << "MCT1FindSupercuts::DefineTestMatrix; Test matrix was written onto file '"
            << filetest << "'" << endl;
    }


  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Define the matrices for the training and test sample respectively
//
//
//
Bool_t MCT1FindSupercuts::DefineTrainTestMatrix(
			  const TString &name, MH3 &href,
	                  const Int_t howmanytrain, const Int_t howmanytest,
                          const TString &filetrain, const TString &filetest)
{
    *fLog << "=============================================" << endl;
    *fLog << "fill training and test matrix from file '" << name 
          << "',   select "   << howmanytrain 
          << " training and " << howmanytest << " test events " << endl;
    if (!href.GetHist().GetEntries()==0)
    {
      *fLog << "     according to a distribution given by the MH3 object '"
            << href.GetName() << "'" << endl;
    }
    else
    {
      *fLog << "     randomly" << endl;
    }


    MParList  plist;
    MTaskList tlist;

    MReadMarsFile read("Events", name);
    read.DisableAutoScheme();

    MFEventSelector2 selector(href);
    selector.SetNumMax(howmanytrain+howmanytest);
    selector.SetName("selectTrainTest");
    selector.SetInverted();

    MContinue cont(&selector);
    cont.SetName("ContTrainTest");

    Double_t prob =  ( (Double_t) howmanytrain )
                   / ( (Double_t)(howmanytrain+howmanytest) );
    MFEventSelector split;
    split.SetSelectionRatio(prob);

    MFillH filltrain(fMatrixTrain);
    filltrain.SetFilter(&split);
    filltrain.SetName("fillMatrixTrain");


    // consider this event as candidate for a test event 
    // only if event was not accepted as a training event

    MContinue conttrain(&split);
    conttrain.SetName("ContTrain");

    MFillH filltest(fMatrixTest);
    filltest.SetName("fillMatrixTest");


    //******************************
    // entries in MParList 
    
    plist.AddToList(&tlist);
    plist.AddToList(fCam);
    plist.AddToList(fMatrixTrain);
    plist.AddToList(fMatrixTest);

    //******************************
    // entries in MTaskList 

    tlist.AddToList(&read);
    tlist.AddToList(&cont);

    tlist.AddToList(&split);
    tlist.AddToList(&filltrain);
    tlist.AddToList(&conttrain);

    tlist.AddToList(&filltest);

    //******************************

    MProgressBar bar;
    MEvtLoop evtloop;
    evtloop.SetParList(&plist);
    evtloop.SetName("EvtLoopMatrixTrainTest");
    evtloop.SetProgressBar(&bar);

    Int_t maxev = -1;
    if (!evtloop.Eventloop(maxev))
      return kFALSE;

    tlist.PrintStatistics(0, kTRUE);

    fMatrixTrain->Print("SizeCols");
    const Int_t generatedtrain = fMatrixTrain->GetM().GetNrows();
    if (TMath::Abs(generatedtrain-howmanytrain) > TMath::Sqrt(9.*howmanytrain))
    {
      *fLog << "MCT1FindSupercuts::DefineTrainTestMatrix; no.of generated events ("
	    << generatedtrain 
            << ") is incompatible with the no.of requested events ("
            << howmanytrain << ")" << endl;
    }

    fMatrixTest->Print("SizeCols");
    const Int_t generatedtest = fMatrixTest->GetM().GetNrows();
    if (TMath::Abs(generatedtest-howmanytest) > TMath::Sqrt(9.*howmanytest))
    {
      *fLog << "MCT1FindSupercuts::DefineTrainTestMatrix; no.of generated events ("
	    << generatedtest 
            << ") is incompatible with the no.of requested events ("
            << howmanytest << ")" << endl;
    }


    *fLog << "training and test matrix were filled" << endl;
    *fLog << "=============================================" << endl;


    //--------------------------
    // write out training matrix

    if (filetrain != "")
    {
      TFile filetr(filetrain, "RECREATE");
      fMatrixTrain->Write();
      filetr.Close();

      *fLog << "MCT1FindSupercuts::DefineTrainTestMatrix; Training matrix was written onto file '"
            << filetrain << "'" << endl;
    }

    //--------------------------
    // write out test matrix

    if (filetest != "")
    {
      TFile filete(filetest, "RECREATE", "");
      fMatrixTest->Write();
      filete.Close();

      *fLog << "MCT1FindSupercuts::DefineTrainTestMatrix; Test matrix was written onto file '"
            << filetest << "'" << endl;
    }

  return kTRUE;
}

// --------------------------------------------------------------------------
//
// Read training and test matrices from files
//
//

Bool_t MCT1FindSupercuts::ReadMatrix(const TString &filetrain, const TString &filetest)
{
  //--------------------------
  // read in training matrix

  TFile filetr(filetrain);
  fMatrixTrain->Read("MatrixTrain");
  fMatrixTrain->Print("SizeCols");

  *fLog << "MCT1FindSupercuts::ReadMatrix; Training matrix was read in from file '"
        << filetrain << "'" << endl;
  filetr.Close();


  //--------------------------
  // read in test matrix

  TFile filete(filetest);
  fMatrixTest->Read("MatrixTest");
  fMatrixTest->Print("SizeCols");

  *fLog << "MCT1FindSupercuts::ReadMatrix; Test matrix was read in from file '"
        << filetest << "'" << endl;
  filete.Close();

  return kTRUE;  
}


//------------------------------------------------------------------------
//
// Steering program for optimizing the supercuts
// ---------------------------------------------
//
//      the criterion for the 'optimum' is 
//
//          - a maximum significance of the gamma signal in the alpha plot, 
//            in which the supercuts have been applied
//
// The various steps are :
//
// - setup the event loop to be executed for each call to fcnSupercuts 
// - call TMinuit to do the minimization of (-significance) :
//        the fcnSupercuts function calculates the significance 
//                                             for the current cut values
//        for this - the alpha plot is produced in the event loop, 
//                   in which the cuts have been applied
//                 - the significance of the gamma signal in the alpha plot 
//                   is calculated
//
// Needed as input : (to be set by the Set functions)
//
// - fFilenameParam      name of file to which optimum values of the 
//                       parameters are written
//
// - fHadronnessName     name of container where MCT1SupercutsCalc will
//                       put the supercuts hadronness
//
// - for the minimization, the starting values of the parameters are taken  
//     - from file parSCinit (if it is != "")
//     - or from the arrays params and/or steps 
//
//----------------------------------------------------------------------
Bool_t MCT1FindSupercuts::FindParams(TString parSCinit,
                                     TArrayD &params, TArrayD &steps)
{
    // Setup the event loop which will be executed in the 
    //                 fcnSupercuts function  of MINUIT
    //
    // parSCinit is the name of the file containing the initial values 
    // of the parameters; 
    // if parSCinit = ""   'params' and 'steps' are taken as initial values

    *fLog << "=============================================" << endl;
    *fLog << "Setup event loop for fcnSupercuts" << endl;

    if (fHadronnessName.IsNull())
    {
      *fLog << "MCT1FindSupercuts::FindParams; hadronness name is not defined... aborting"
            << endl;
      return kFALSE;
    }

    if (fMatrixTrain == NULL)
    {
      *fLog << "MCT1FindSupercuts::FindParams; training matrix is not defined... aborting"
            << endl;
      return kFALSE;
    }

    if (fMatrixTrain->GetM().GetNrows() <= 0)
    {
      *fLog << "MCT1FindSupercuts::FindParams; training matrix has no entries"
            << endl;
      return kFALSE;
    }

    MParList  parlistfcn;
    MTaskList tasklistfcn;

    // loop over rows of matrix
    MMatrixLoop loop(fMatrixTrain);

    //--------------------------------
    // create container for the supercut parameters
    // and set them to their initial values
    MCT1Supercuts super;

    // take initial values from file parSCinit
    if (parSCinit != "")
    {
      TFile inparam(parSCinit);
      super.Read("MCT1Supercuts");
      inparam.Close();
      *fLog << "MCT1FindSupercuts::FindParams; initial values of parameters are taken from file "
            << parSCinit << endl;
    }

    // take initial values from 'params' and/or 'steps'
    else if (params.GetSize() != 0  || steps.GetSize()  != 0 )
    {
      if (params.GetSize()  != 0)
      {
        *fLog << "MCT1FindSupercuts::FindParams; initial values of parameters are taken from 'params'"
              << endl;
        super.SetParameters(params);
      }
      if (steps.GetSize()  != 0)
      {
        *fLog << "MCT1FindSupercuts::FindParams; initial step sizes are taken from 'steps'"
              << endl;
        super.SetStepsizes(steps);
      }
    }
    else
    {
        *fLog << "MCT1FindSupercuts::FindParams; initial values and step sizes are taken from the MCT1Supercits constructor"
              << endl;
    }


    //--------------------------------
    // calculate supercuts hadronness
    fCalcHadTrain->SetHadronnessName(fHadronnessName);

    // apply the supercuts
    MF scfilter(fHadronnessName+".fHadronness>0.5");
    MContinue supercuts(&scfilter);

    // plot |alpha|
    const TString  mh3Name = "AlphaFcn";
    MBinning binsalpha("Binning"+mh3Name);
    binsalpha.SetEdges(54, -12.0, 96.0);

    *fLog << warn << "WARNING------------>ALPHA IS ASSUMED TO BE IN COLUMN 7!!!!!!!!" << endl;

    // |alpha| is assumed to be in column 7 of the matrix
    MH3 alpha("MatrixTrain[7]");
    alpha.SetName(mh3Name);

    MFillH fillalpha(&alpha);

    // book histograms to be filled during the optimization
    //                              ! not in the event loop !
    MHCT1Supercuts plotsuper;
    plotsuper.SetupFill(&parlistfcn);

    //******************************
    // entries in MParList (extension of old MParList)
    
    parlistfcn.AddToList(&tasklistfcn);
    parlistfcn.AddToList(&super);
    parlistfcn.AddToList(fCam);
    parlistfcn.AddToList(fMatrixTrain);

    parlistfcn.AddToList(&binsalpha);
    parlistfcn.AddToList(&alpha);

    parlistfcn.AddToList(&plotsuper);

    //******************************
    // entries in MTaskList

    tasklistfcn.AddToList(&loop);
    tasklistfcn.AddToList(fCalcHadTrain);
    tasklistfcn.AddToList(&supercuts);
    tasklistfcn.AddToList(&fillalpha);


    //******************************

    MEvtLoop evtloopfcn("EvtLoopFCN");
    evtloopfcn.SetParList(&parlistfcn);
    *fLog << "Event loop for fcnSupercuts has been setup" << endl;

    // address of evtloopfcn is used in CallMinuit()


    //-----------------------------------------------------------------------
    //
    //----------   Start of minimization part   --------------------
    //
    // Do the minimization with MINUIT
    //
    // Be careful: This is not thread safe
    //
    *fLog << "========================================================" << endl;
    *fLog << "Start minimization for supercuts" << endl;


    // -------------------------------------------
    // prepare call to MINUIT
    //

    // get initial values of parameters 
    fVinit = super.GetParameters();
    fStep  = super.GetStepsizes();

    TString name[fVinit.GetSize()];
    fStep.Set(fVinit.GetSize());
    fLimlo.Set(fVinit.GetSize());
    fLimup.Set(fVinit.GetSize());
    fFix.Set(fVinit.GetSize());

    fNpar = fVinit.GetSize();

    for (UInt_t i=0; i<fNpar; i++)
    {
        name[i]   = "p";
        name[i]  += i+1;
        //fStep[i]  = TMath::Abs(fVinit[i]/10.0);
        fLimlo[i] = -100.0;
        fLimup[i] =  100.0;
        fFix[i]   =     0;
    }

    // these parameters make no sense, fix them at 0.0
    fVinit[33] = 0.0;
    fStep[33]  = 0.0;
    fFix[33]   = 1;

    fVinit[36] = 0.0;
    fStep[36]  = 0.0;
    fFix[36]   = 1;

    fVinit[39] = 0.0;
    fStep[39]  = 0.0;
    fFix[39]   = 1;

    fVinit[41] = 0.0;
    fStep[41]  = 0.0;
    fFix[41]   = 1;

    fVinit[44] = 0.0;
    fStep[44]  = 0.0;
    fFix[44]   = 1;

    fVinit[47] = 0.0;
    fStep[47]  = 0.0;
    fFix[47]   = 1;

    // vary only first 48 parameters
    //for (UInt_t i=0; i<fNpar; i++)
    //{
    //    if (i >= 48)
    //	{
    //      fStep[i] = 0.0;
    //      fFix[i]  =   1;
    //	}
    //}
 

    // -------------------------------------------
    // call MINUIT

    TStopwatch clock;
    clock.Start();

    *fLog << "before calling CallMinuit" << endl;

    MMinuitInterface inter;               
    Bool_t rc = inter.CallMinuit(fcnSupercuts, name,
                                 fVinit, fStep, fLimlo, fLimup, fFix,
                                 &evtloopfcn, "SIMPLEX", kFALSE);
 
    *fLog << "after calling CallMinuit" << endl;

    *fLog << "Time spent for the minimization in MINUIT :   " << endl;;
    clock.Stop();
    clock.Print();

    plotsuper.DrawClone();

    if (!rc)
        return kFALSE;

    *fLog << "Minimization for supercuts finished" << endl;
    *fLog << "========================================================" << endl;


    // -----------------------------------------------------------------
    // in 'fcnSupercuts' (IFLAG=3) the optimum parameter values were put 
    //                    into MCT1Supercuts

    // write optimum parameter values onto file filenameParam
    
    TFile outparam(fFilenameParam, "RECREATE"); 
    super.Write();
    outparam.Close();

    *fLog << "Optimum parameter values for supercuts were written onto file '"
              << fFilenameParam << "' :" << endl;

    const TArrayD &check = super.GetParameters();
    for (Int_t i=0; i<check.GetSize(); i++)
        *fLog << check[i] << ",  ";
    *fLog << endl;



    *fLog << "End of  supercuts optimization part" << endl;
    *fLog << "======================================================" << endl;

    return kTRUE;
}


// -----------------------------------------------------------------------
//
// Test the supercuts on the test sample
//

Bool_t MCT1FindSupercuts::TestParams()
{
    if (fMatrixTest->GetM().GetNrows() <= 0)
    {
        *fLog << "MCT1FindSupercuts::TestParams; test matrix has no entries" 
              << endl;
        return kFALSE;
    }

    // -------------   TEST the supercuts    ------------------------------
    //
    *fLog << "Test the supercuts on the test sample" << endl;

    // -----------------------------------------------------------------
    // read optimum parameter values from file filenameParam
    // into array 'supercutsPar'

    TFile inparam(fFilenameParam);
    MCT1Supercuts scin; 
    scin.Read("MCT1Supercuts");
    inparam.Close();

    *fLog << "Optimum parameter values for supercuts were read from file '";
    *fLog << fFilenameParam << "' :" << endl;

    const TArrayD &supercutsPar = scin.GetParameters();
    for (Int_t i=0; i<supercutsPar.GetSize(); i++)
        *fLog << supercutsPar[i] << ",  ";
    *fLog << endl;
    //---------------------------


    // -----------------------------------------------------------------
    if (fHadronnessName.IsNull())
    {
        *fLog << "MCT1FindSupercuts::TestParams; hadronness name is not defined... aborting";
        *fLog << endl;
        return kFALSE;
    }

    MParList  parlist2;
    MTaskList tasklist2;

    MCT1Supercuts supercuts;
    supercuts.SetParameters(supercutsPar);

    fCalcHadTest->SetHadronnessName(fHadronnessName);


    //-------------------------------------------

    MMatrixLoop loop(fMatrixTest);

    // plot alpha before applying the supercuts
    const TString  mh3NameB = "AlphaBefore";
    MBinning binsalphabef("Binning"+mh3NameB);
    binsalphabef.SetEdges(54, -12.0, 96.0);

    // |alpha| is assumed to be in column 7 of the matrix
    MH3 alphabefore("MatrixTest[7]");
    alphabefore.SetName(mh3NameB);

    TH1 &alphahistb = alphabefore.GetHist();
    alphahistb.SetName("alphaBefore-TestParams");

    MFillH fillalphabefore(&alphabefore);
    fillalphabefore.SetName("FillAlphaBefore");

    // apply the supercuts
    MF scfilter(fHadronnessName+".fHadronness>0.5");
    MContinue applysupercuts(&scfilter);

    // plot alpha after applying the supercuts
    const TString  mh3NameA = "AlphaAfter";
    MBinning binsalphaaft("Binning"+mh3NameA);
    binsalphaaft.SetEdges(54, -12.0, 96.0);

    MH3 alphaafter("MatrixTest[7]");
    alphaafter.SetName(mh3NameA);

    TH1 &alphahista = alphaafter.GetHist();
    alphahista.SetName("alphaAfter-TestParams");

    MFillH fillalphaafter(&alphaafter);
    fillalphaafter.SetName("FillAlphaAfter");

    //******************************
    // entries in MParList

    parlist2.AddToList(&tasklist2);

    parlist2.AddToList(&supercuts);

    parlist2.AddToList(fCam);
    parlist2.AddToList(fMatrixTest);

    parlist2.AddToList(&binsalphabef);
    parlist2.AddToList(&alphabefore);

    parlist2.AddToList(&binsalphaaft);
    parlist2.AddToList(&alphaafter);

    //******************************
    // entries in MTaskList

    tasklist2.AddToList(&loop);
    tasklist2.AddToList(&fillalphabefore);

    tasklist2.AddToList(fCalcHadTest);
    tasklist2.AddToList(&applysupercuts);

    tasklist2.AddToList(&fillalphaafter);

    //******************************

    MProgressBar bar2;
    MEvtLoop evtloop2;
    evtloop2.SetParList(&parlist2);
    evtloop2.SetName("EvtLoopTestParams");
    evtloop2.SetProgressBar(&bar2);
    Int_t maxevents2 = -1;
    if (!evtloop2.Eventloop(maxevents2))
        return kFALSE;

    tasklist2.PrintStatistics(0, kTRUE);


    //-------------------------------------------
    // draw the alpha plots

    MH3* alphaBefore = (MH3*)parlist2.FindObject(mh3NameB, "MH3");
    TH1  &alphaHist1 = alphaBefore->GetHist();
    alphaHist1.SetXTitle("|\\alpha|  [\\circ]");

    MH3* alphaAfter = (MH3*)parlist2.FindObject(mh3NameA, "MH3");
    TH1  &alphaHist2 = alphaAfter->GetHist();
    alphaHist2.SetXTitle("|\\alpha|  [\\circ]");
    alphaHist2.SetName("alpha-TestParams");

    TCanvas *c = new TCanvas("AlphaAfterSC", "AlphaTest", 600, 300);
    c->Divide(2,1);

    gROOT->SetSelectedPad(NULL);

    c->cd(1);
    alphaHist1.DrawCopy();

    c->cd(2);
    alphaHist2.DrawCopy();



    //-------------------------------------------
    // fit alpha distribution to get the number of excess events and
    // calculate significance of gamma signal in the alpha plot
  
    const Double_t alphasig = 20.0;
    const Double_t alphamin = 30.0;
    const Double_t alphamax = 90.0;
    const Int_t    degree   =    2;
    const Bool_t   drawpoly  = kTRUE;
    const Bool_t   fitgauss  = kTRUE;
    const Bool_t   print     = kTRUE;

    MHFindSignificance findsig;
    findsig.SetRebin(kTRUE);
    findsig.SetReduceDegree(kFALSE);

    findsig.FindSigma(&alphaHist2, alphamin, alphamax, degree, 
                      alphasig, drawpoly, fitgauss, print);

    const Double_t significance = findsig.GetSignificance();
    const Double_t alphasi = findsig.GetAlphasi();

    *fLog << "Significance of gamma signal after supercuts in test sample : "
         << significance << " (for |alpha| < " << alphasi << " degrees)" 
         << endl;
    //-------------------------------------------


    *fLog << "Test of supercuts part finished" << endl;
    *fLog << "======================================================" << endl;

    return kTRUE;
}

