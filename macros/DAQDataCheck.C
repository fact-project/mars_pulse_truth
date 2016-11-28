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
!   Author(s): Raquel de los Reyes, 05/2004 <mailto:reyes@gae.ucm.es>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== */
/////////////////////////////////////////////////////////////////////////////
//
// This macro makes the check of the DAQ files (.raw files).
//
// The only argument to the macro is the directory with the night raw files
// (ended with "/"). The macro sorts the entries in alphabetical order;
// it then joins all the consecutive pedestal, calibration and data runs 
// and analyses all of them together. Data runs are taken consecutively
// until it finds a run of other type (pedestal or calibration) or a data run 
// from another source. When a given source has no previous pedestal and/or
// calibration runs, the macro takes the most recent ones. 
//
// The analysis is done through the jobs classes MJPedestal and MJCalibration.
//
///////////////////////////////////////////////////////////////////////////
void DataAnalysis(const Int_t prun1,const Int_t prun2,
		  const Int_t crun1,const Int_t crun2,
		  const Int_t drun1,const Int_t drun2,
		  const TString inpath,
		  const TString outdir)
{

  // Check if no pedestal runs has been detected
  if(prun1==0)
  {
      cout << "No pedestal run(s)!!!"<< endl;
      break;
  }

  gStyle->SetOptStat(1);
  gStyle->SetOptFit();

  MRunIter pruns;
  MRunIter cruns;
  MRunIter druns;
  //
  // Set the filter of MRunIter to .raw files
  pruns.SetRawFile(kTRUE);
  cruns.SetRawFile(kTRUE);
  druns.SetRawFile(kTRUE);

  //
  // Check if you are analyzing more than one file
  if(prun1==prun2)
    pruns.AddRun(prun1,inpath);
  else
    pruns.AddRuns(prun1,prun2,inpath);
  if(crun1==crun2)
    cruns.AddRun(crun1,inpath);
  else
    cruns.AddRuns(crun1,crun2,inpath);
  if(drun1==drun2)
    druns.AddRun(drun1,inpath);
  else
    druns.AddRuns(drun1,drun2,inpath);

  // 
  // Set up the source run-range
  //
  TRegexp type("_[A-Z]_");
  TString source = "";
  if(pruns.GetNumRuns()!=0)
    source = pruns.Next();
  source = source(source.Index(type)+3,source.Length());
  source.Remove(source.Last('_'),source.Length());

  TString title = outdir+source+"_"+prun2+"-"+crun2+"-"+drun2+".ps";

  MStatusDisplay *d = new MStatusDisplay;
  d->SetTitle(title);
  d->SetLogStream(&gLog, kTRUE);

  TObject::SetObjectStat(kTRUE);
 
  //
  // Choose the signal Extractor:
  //
  //  MExtractFixedWindowPeakSearch extractor;
  //  MExtractSlidingWindow  extractor;
   MExtractFixedWindow    extractor;
 
  //
  // Set Ranges or Windows
  //
   extractor.SetRange(3,14,3,14);
  //  extractor.SetWindows(8,8);
 
  //
  // Choose the arrival time Extractor:
  //
  //  MExtractTimeHighestIntegral timeext;
  MExtractTimeFastSpline timeext;
  //
  // Set Ranges or Windows
  //
  timeext.SetRange(2,12,4,14);

  // ************************ GENERAL CONTAINERS  *************************
  MBadPixelsCam     badcam;
  MGeomCamMagic     geomcam;
  MGeomApply        geomapl;
  MCalibrationQECam qecam;  

  // **********************************************************************
  // ***************************** PEDESTALS ******************************
  // **********************************************************************
  if(pruns.GetNumRuns()==0)
    {
      cout << "Warning, no entries found in pedestal run(s)!!!"<< endl;
      break;
    }

  MJPedestal pedloop;
  pedloop.SetExtractor(&extractor);           
  pedloop.SetInput(&pruns);
  pedloop.SetDisplay(d);
  pedloop.SetBadPixels(badcam);
  //
  // If you want to run the data-check on RAW DATA!!!, choose:
  pedloop.SetDataCheck(kTRUE);

  //
  // Execute first analysis
  //
  cout << "*************************" << endl;
  cout << "** COMPUTING PEDESTALS **" << endl;
  cout << "*************************" << endl;
  if (!pedloop.Process())
    return;

  //
  // Save display into a postcript file
  //
  d->SaveAsPS(outdir+source+"_"+prun2+"-"+crun2+"-"+drun2+".ps");

  // **********************************************************************
  // ***************************** CALIBRATION ****************************
  // **********************************************************************
  if(cruns.GetNumRuns()==0)
    {
      cout << "Warning, no entries found in calibration run(s)!!!"<< endl;
      break;
    }

  //
  // Tell if you want to calibrate times:
  //
  static const  Bool_t useTimes = kTRUE;

  MJCalibration calloop;
  //
  // If you want to run the data-check on RAW DATA!!!, choose:
  calloop.SetDataCheck();
  //
  // If you want to see the data-check plots only, choose:
  calloop.SetDataCheckDisplay();

  calloop.SetRelTimeCalibration(useTimes);
  calloop.SetExtractor(&extractor);
  calloop.SetTimeExtractor(&timeext);
  calloop.SetInput(&cruns);
  calloop.SetDisplay(d);
  calloop.SetQECam(qecam);
  calloop.SetBadPixels(pedloop.GetBadPixels());            

  //
  // Execute first analysis
  //
  cout << "***************************" << endl;
  cout << "** COMPUTING CALIBRATION **" << endl;
  cout << "***************************" << endl;
  if (!calloop.Process(pedloop.GetPedestalCam()))
    return;

  //
  // Save display into a postcript file
  //
  d->SaveAsPS(outdir+source+"_"+prun2+"-"+crun2+"-"+drun2+".ps");


}

void DAQDataCheck(const TString directory="/remote/bigdown/data/Magic-DATA/Period014/rawdata/2004_02_17/",const TString outdir="./")
{

  MDirIter iter;
  iter.AddDirectory(directory,"*.raw");

  TString str=iter.Next();

  TList list;
  while(!str.IsNull())
    {
      list.Add(new TNamed(str,""));
      str=iter.Next();
    }
  list.Sort();

  TIter Next(&list);

  TString fName="file.raw";

  TRegexp run("_[0-9][0-9][0-9][0-9][0-9]_");
  TRegexp type("_[A-Z]_");

  Int_t pedrun1=0, pedrun2=0;
  Int_t calrun1=0, calrun2=0;
  Int_t datarun1=0, datarun2=0;

  TString source="";

  TObject o*;
  o=Next();
  fName = o->GetName();  // absolut path

  while(!fName.IsNull())
    {

      source = fName(fName.Index(type)+3,fName.Length());
      source.Remove(source.Last('_'),source.Length());

      // Pedestal runs
      if(fName.Contains("_P_"))
	pedrun1=atoi(fName(fName.Index(run)+1,5).Data());
      pedrun2=pedrun1;

      while(fName.Contains("_P_")&&fName.Contains(source))
	{
	  pedrun2=atoi(fName(fName.Index(run)+1,5).Data());
	  o=Next();
	  if(!o)
	    {
	      fName="";
	      break;
	    }
	  fName = o->GetName(); 
	}

      // Calibration runs
      if(fName.Contains("_C_"))//||(!fName.Contains(source)))
	calrun1=atoi(fName(fName.Index(run)+1,5).Data());
//      else
//	  calrun1=0; 
      calrun2=calrun1;

      while(fName.Contains("_C_")&&fName.Contains(source))
	{
	  calrun2=atoi(fName(fName.Index(run)+1,5).Data());
	  o=Next();
	  if(!o)
	    {
	      fName = "";
	      break;
	    }
	  fName = o->GetName(); 
	}

      // Data runs
      if(fName.Contains("_D_"))//||(!fName.Contains(source)))
	datarun1=atoi(fName(fName.Index(run)+1,5).Data());
      else
	  datarun1=0; 
      datarun2=datarun1;

      while(fName.Contains("_D_")&&fName.Contains(source))
	{
	  datarun2=atoi(fName(fName.Index(run)+1,5).Data());
	  o=Next();
	  if(!o)
	    {
	      fName = "";
	      break;
	    }
	  fName = o->GetName(); 
	}

//        cout << pedrun1 << "\t"<<pedrun2 << endl;
//        cout << calrun1 << "\t"<<calrun2 << endl;
//        cout << datarun1 << "\t"<<datarun2 << endl;

      DataAnalysis(pedrun1,pedrun2,calrun1,calrun2,datarun1,datarun2,directory,outdir);

      cout << "----------------------------------------------" << endl;

    }

}



