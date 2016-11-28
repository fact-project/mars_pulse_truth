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
!   Author(s): Raquel de los Reyes, 02/2004 <mailto:reyes@gae.ucm.es>
!
!   Copyright: MAGIC Software Development, 2000-2004
!
!
\* ======================================================================== *//////////////////////////////////////////////////////////////////////////////
//
// This macro made the check of the central control files (.rep files).
// It displays 9 tabs:
//   - Drive system
//   - Camera status
//   - HV of power supplies and HV settings
//   - DC of power supplies 
//   - LV power supplies
//   - Cooling system
//   - Trigger system
//   - Trigger macrocells
//   - Weather station
//
////////////////////////////////////////////////////////////////////////////

void CCDataCheck(const TString filename="CC_2004_04_28.root", const TString directory="/home/raquel/")
{

  //
  // If you want to run the macro in batch mode, set batchmode to kTRUE
  //
  Bool_t batchmode = kFALSE;

  if(!batchmode)
    {
      MStatusDisplay *d = new MStatusDisplay;
      d->SetTitle(filename);
      d->SetLogStream(&gLog, kTRUE);            // Disables output to stdout
    }

  //
  // Create a empty Parameter List and an empty Task List
  // The tasklist is identified in the eventloop by its name
  //
  MParList  plist;
  
  MTaskList tlist;
  plist.AddToList(&tlist);

  //
  // Now setup the tasks and tasklist:
  // ---------------------------------
  //

  // Set all the MHVsTime classes:
  // Drive system
  MHVsTime hZd("MReportDrive.fNominalZd");
  hZd.SetName("Zd");
  MHVsTime hAz("MReportDrive.fNominalAz");
  hAz.SetName("Az");
  MHVsTime hDState("MReportDrive.fState");
  hDState.SetName("State");
  // HV power supplies
  MHVsTime hHVA("MCameraHV.fVoltageA");
  hHVA.SetName("HVA");
  MHVsTime hHVB("MCameraHV.fVoltageB");
  hHVB.SetName("HVB");
  MHVsTime hCA("MCameraHV.fCurrentA");
  hCA.SetName("CA");
  MHVsTime hCB("MCameraHV.fCurrentB");
  hCB.SetName("CB");
  // LV power supplies
  MHVsTime hLVTemp("MCameraLV.fTemp");
  hLVTemp.SetName("LVTemp");
  MHVsTime hLVHum("MCameraLV.fHumidity");
  hLVHum.SetName("LVHum");
  MHVsTime hLVStatus("MCameraHV.fStatus");
  hLVStatus.SetName("LVStatus");
  MHVsTime hLVRPS("MCameraLV.fRequestPowerSupply");
  hLVRPS.SetName("LVRPS");
  // Cooling system
  MHVsTime hTCenter("MCameraCooling.fTempCenter");
  hTCenter.SetName("TCenter");
  MHVsTime hTWall("MCameraCooling.fTempWall");
  hTWall.SetName("TWall");
  MHVsTime hTWater("MCameraCooling.fTempWater");
  hTWater.SetName("TWater");
  MHVsTime hTOptLink("MCameraCooling.fTempOptLink");
  hTOptLink.SetName("TOptLink");
  MHVsTime hHWall("MCameraCooling.fHumWall");
  hHWall.SetName("HWall");
  MHVsTime hHCenter("MCameraCooling.fHumCenter");
  hHCenter.SetName("HCenter");
  // Weather station
  MHVsTime hCCHum("MReportCC.fHumidity");
  hCCHum.SetName("CCHum");
  MHVsTime hCCTemp("MReportCC.fTemperature");
  hCCTemp.SetName("CCTemp");
  MHVsTime hCCWS("MReportCC.fWindSpeed");
  hCCWS.SetName("CCWS");
  MHVsTime hCCSR("MReportCC.fSolarRadiation");
  hCCSR.SetName("CCSR");
  // Trigger system
  MHVsTime hTrigBL2("MReportTrigger.fL2BeforePrescaler");
  hTrigBL2.SetName("TrigBL2");
  MHVsTime hTrigAL2("MReportTrigger.fL2AfterPrescaler");
  hTrigAL2.SetName("TrigAL2");
  MHVsTime hTrigStatus("MReportTrigger.fState");
  hTrigStatus.SetName("TrigStatus");
  // Camera status
  MHVsTime hCamStatus("MReportCamera.fState");
  hCamStatus.SetName("CamStatus");
  MHVsTime hCamSentStatus("MReportCamera.fStatus");
  hCamSentStatus.SetName("CamSentStatus");
  MHVsTime hCamStatusDC("MReportCamera.fStatusDC");
  hCamStatusDC.SetName("CamDCStatus");
  MHVsTime hHVStatus("MCameraHV.fStatus");
  hHVStatus.SetName("HVStatus");
  MHVsTime hLidsStatus("MCameraLids.fStatus");
  hLidsStatus.SetName("LidsStatus");
  MHVsTime hCoolStatus("MCameraCooling.fStatus");
  hCoolStatus.SetName("CoolingStatus");
  MHVsTime hCamMean("MCameraHV.fMean");
  hCamMean.SetName("CameraMean");

  // Reads the trees of the root file and the analysed branches
  MReadReports read;
  read.AddTree("Drive");
  read.AddTree("Camera");
  read.AddTree("CC");
  read.AddTree("Trigger");
  read.AddFile(directory+filename);     // after the reading of the trees!!!
  read.AddToBranchList("MReportDrive.*");
  read.AddToBranchList("MCameraHV.*");
  read.AddToBranchList("MCameraLV.*");
  read.AddToBranchList("MCameraCooling.*");
  read.AddToBranchList("MReportCC.*");
  read.AddToBranchList("MReportTrigger.*");
  read.AddToBranchList("MReportCamera.*");
  read.AddToBranchList("MCameraLids.*");

  MGeomApply geomapl;
  tlist.AddToList(&geomapl);

  // Set of MHCamEvents classes
  MHCamEvent HVdisplay("CamHV","Camera mean HV settings");
  HVdisplay.SetBit(MHCamera::kVariance);
  plist.AddToList(&HVdisplay);

  // Set of MHPixVsTime classes (Trigger macroscells)
  MHPixVsTime htrigmc1(0,"TrigMacrocell1");
  htrigmc1.SetNameTime("MTimeTrigger");
  plist.AddToList(&htrigmc1);
  MHPixVsTime htrigmc2(1,"TrigMacrocell2");
  htrigmc2.SetNameTime("MTimeTrigger");
  plist.AddToList(&htrigmc2);
  MHPixVsTime htrigmc3(2,"TrigMacrocell3");
  htrigmc3.SetNameTime("MTimeTrigger");
  plist.AddToList(&htrigmc3);
  MHPixVsTime htrigmc4(3,"TrigMacrocell4");
  htrigmc4.SetNameTime("MTimeTrigger");
  plist.AddToList(&htrigmc4);
  MHPixVsTime htrigmc5(4,"TrigMacrocell5");
  htrigmc5.SetNameTime("MTimeTrigger");
  plist.AddToList(&htrigmc5);
  MHPixVsTime htrigmc6(5,"TrigMacrocell6");
  htrigmc6.SetNameTime("MTimeTrigger");
  plist.AddToList(&htrigmc6);
  MHPixVsTime htrigmc7(6,"TrigMacrocell7");
  htrigmc7.SetNameTime("MTimeTrigger");
  plist.AddToList(&htrigmc7);
  MHPixVsTime htrigmc8(7,"TrigMacrocell8");
  htrigmc8.SetNameTime("MTimeTrigger");
  plist.AddToList(&htrigmc8);
  MHPixVsTime htrigmc9(8,"TrigMacrocell9");
  htrigmc9.SetNameTime("MTimeTrigger");
  plist.AddToList(&htrigmc9);
  MHPixVsTime htrigmc10(9,"TrigMacrocell10");
  htrigmc10.SetNameTime("MTimeTrigger");
  plist.AddToList(&htrigmc10);
  MHPixVsTime htrigmc11(10,"TrigMacrocell11");
  htrigmc11.SetNameTime("MTimeTrigger");
  plist.AddToList(&htrigmc11);
  MHPixVsTime htrigmc12(11,"TrigMacrocell12");
  htrigmc12.SetNameTime("MTimeTrigger");
  plist.AddToList(&htrigmc12);
  MHPixVsTime htrigmc13(12,"TrigMacrocell13");
  htrigmc13.SetNameTime("MTimeTrigger");
  plist.AddToList(&htrigmc13);
  MHPixVsTime htrigmc14(13,"TrigMacrocell14");
  htrigmc14.SetNameTime("MTimeTrigger");
  plist.AddToList(&htrigmc14);
  MHPixVsTime htrigmc15(14,"TrigMacrocell15");
  htrigmc15.SetNameTime("MTimeTrigger");
  plist.AddToList(&htrigmc15);
  MHPixVsTime htrigmc16(15,"TrigMacrocell16");
  htrigmc16.SetNameTime("MTimeTrigger");
  plist.AddToList(&htrigmc16);
  MHPixVsTime htrigmc17(16,"TrigMacrocell17");
  htrigmc17.SetNameTime("MTimeTrigger");
  plist.AddToList(&htrigmc17);
  MHPixVsTime htrigmc18(17,"TrigMacrocell18");
  htrigmc18.SetNameTime("MTimeTrigger");
  plist.AddToList(&htrigmc18);
  MHPixVsTime htrigmc19(18,"TrigMacrocell19");
  htrigmc19.SetNameTime("MTimeTrigger");
  plist.AddToList(&htrigmc19);

  // Set of MH3 classes
  MH3 hError("MReportDrive.GetAbsError*60");
  hError.SetName("DeltaH");
  MBinning bins("BinningDeltaH");
  bins.SetEdges(18, 0, 3.6);
  plist.AddToList(&bins);
  MH3 hError2("MReportDrive.fNominalZd","MReportDrive.GetAbsError*60");
  hError2.SetName("DeltaHvsZd");
  MBinning bins2("BinningDeltaHvsZdX");
  MBinning bins3("BinningDeltaHvsZdY");
  bins2.SetEdges(90, 0, 90);
  bins3.SetEdges(18, 0, 3.6);
  plist.AddToList(&bins2);
  plist.AddToList(&bins3);
  MH3 hTempOptLink("MCameraCooling.fTempOptLink");
  hTempOptLink.SetName("TOptLink");
  MBinning bins4("BinningTOptLinkX");
  bins4.SetEdges(50, 0, 50);
  plist.AddToList(&bins4);
  MH3 hTrigZd("MReportDrive.fNominalZd","MReportTrigger.fL2AfterPrescaler");
  hTrigZd.SetName("TrigZd");
  MBinning bins5("BinningTrigZdX");
  bins5.SetEdges(90, 0, 90);
  plist.AddToList(&bins5);
  MBinning bins6("BinningTrigZdY");
  bins6.SetEdges(10000, 0, 15000);
  plist.AddToList(&bins6);


  // Fill all the MH classes defined before
  MFillH fillZd(&hZd,             "MTimeDrive");
  MFillH fillAz(&hAz,             "MTimeDrive");
  MFillH fillError(&hError);
  MFillH fillDState(&hDState,     "MTimeDrive");
  MFillH fillError2(&hError2);
  MFillH fillHVA(&hHVA,           "MTimeCamera");
  MFillH fillHVB(&hHVB,           "MTimeCamera");
  MFillH fillCA(&hCA,             "MTimeCamera");
  MFillH fillCB(&hCB,             "MTimeCamera");
  MFillH fillLVTemp(&hLVTemp,     "MTimeCamera");
  MFillH fillLVHum(&hLVHum,       "MTimeCamera");
  MFillH fillLVStatus(&hLVStatus, "MTimeCamera");
  MFillH fillLVRPS(&hLVRPS,       "MTimeCamera");
  MFillH fillTCenter(&hTCenter,   "MTimeCamera");
  MFillH fillTWall(&hTWall,       "MTimeCamera");
  MFillH fillTWater(&hTWater,     "MTimeCamera");
  MFillH fillTOptLink(&hTOptLink, "MTimeCamera");
  MFillH fillTempOptLink(&hTempOptLink);
  MFillH fillHWall(&hHWall,       "MTimeCamera");
  MFillH fillHCenter(&hHCenter,   "MTimeCamera");
  MFillH fillCCHum(&hCCHum,       "MTimeCC");
  MFillH fillCCTemp(&hCCTemp,     "MTimeCC");
  MFillH fillCCWS(&hCCWS,         "MTimeCC");
  MFillH fillCCSR(&hCCSR,         "MTimeCC");
  MFillH fillTrigBL2(&hTrigBL2,   "MTimeTrigger");
  MFillH fillTrigAL2(&hTrigAL2,   "MTimeTrigger");
  MFillH fillTrigStatus(&hTrigStatus,   "MTimeTrigger");
  MFillH fillTrigZd(&hTrigZd);
  MFillH fillCamStatus(&hCamStatus,"MTimeCamera");
  MFillH fillCamSentStatus(&hCamSentStatus,"MTimeCamera");
  MFillH fillCamStatusDC(&hCamStatusDC, "MTimeCamera");
  MFillH fillHVStatus(&hHVStatus, "MTimeCamera");
  MFillH fillLidsStatus(&hLidsStatus, "MTimeCamera");
  MFillH fillCoolStatus(&hCoolStatus, "MTimeCamera");
  MFillH filltrigmc1("TrigMacrocell1","MReportTrigger");
  MFillH filltrigmc2("TrigMacrocell2","MReportTrigger");
  MFillH filltrigmc3("TrigMacrocell3","MReportTrigger");
  MFillH filltrigmc4("TrigMacrocell4","MReportTrigger");
  MFillH filltrigmc5("TrigMacrocell5","MReportTrigger");
  MFillH filltrigmc6("TrigMacrocell6","MReportTrigger");
  MFillH filltrigmc7("TrigMacrocell7","MReportTrigger");
  MFillH filltrigmc8("TrigMacrocell8","MReportTrigger");
  MFillH filltrigmc9("TrigMacrocell9","MReportTrigger");
  MFillH filltrigmc10("TrigMacrocell10","MReportTrigger");
  MFillH filltrigmc11("TrigMacrocell11","MReportTrigger");
  MFillH filltrigmc12("TrigMacrocell12","MReportTrigger");
  MFillH filltrigmc13("TrigMacrocell13","MReportTrigger");
  MFillH filltrigmc14("TrigMacrocell14","MReportTrigger");
  MFillH filltrigmc15("TrigMacrocell15","MReportTrigger");
  MFillH filltrigmc16("TrigMacrocell16","MReportTrigger");
  MFillH filltrigmc17("TrigMacrocell17","MReportTrigger");
  MFillH filltrigmc18("TrigMacrocell18","MReportTrigger");
  MFillH filltrigmc19("TrigMacrocell19","MReportTrigger");
  MFillH fillcammean(&hCamMean,"MTimeCamera");
  MFillH fillHVD(&HVdisplay,"MCameraHV");

  // Do not display the result given by the default draw function
  fillZd.SetBit(MFillH::kDoNotDisplay);
  fillAz.SetBit(MFillH::kDoNotDisplay);
  fillError.SetBit(MFillH::kDoNotDisplay);
  fillDState.SetBit(MFillH::kDoNotDisplay);
  fillError2.SetBit(MFillH::kDoNotDisplay);
  fillHVA.SetBit(MFillH::kDoNotDisplay);
  fillHVB.SetBit(MFillH::kDoNotDisplay);
  fillCA.SetBit(MFillH::kDoNotDisplay);
  fillCB.SetBit(MFillH::kDoNotDisplay);
  fillLVTemp.SetBit(MFillH::kDoNotDisplay);
  fillLVHum.SetBit(MFillH::kDoNotDisplay);
  fillLVStatus.SetBit(MFillH::kDoNotDisplay);
  fillLVRPS.SetBit(MFillH::kDoNotDisplay);
  fillTCenter.SetBit(MFillH::kDoNotDisplay);
  fillTWall.SetBit(MFillH::kDoNotDisplay);
  fillTWater.SetBit(MFillH::kDoNotDisplay);
  fillTOptLink.SetBit(MFillH::kDoNotDisplay);
  fillTempOptLink.SetBit(MFillH::kDoNotDisplay);
  fillHWall.SetBit(MFillH::kDoNotDisplay);
  fillHCenter.SetBit(MFillH::kDoNotDisplay);
  fillCCHum.SetBit(MFillH::kDoNotDisplay);
  fillCCTemp.SetBit(MFillH::kDoNotDisplay);
  fillCCWS.SetBit(MFillH::kDoNotDisplay);
  fillCCSR.SetBit(MFillH::kDoNotDisplay);
  fillTrigBL2.SetBit(MFillH::kDoNotDisplay);
  fillTrigAL2.SetBit(MFillH::kDoNotDisplay);
  fillTrigStatus.SetBit(MFillH::kDoNotDisplay);
  fillTrigZd.SetBit(MFillH::kDoNotDisplay);
  fillCamStatus.SetBit(MFillH::kDoNotDisplay);
  fillCamSentStatus.SetBit(MFillH::kDoNotDisplay);
  fillCamStatusDC.SetBit(MFillH::kDoNotDisplay);
  fillHVStatus.SetBit(MFillH::kDoNotDisplay);
  fillLidsStatus.SetBit(MFillH::kDoNotDisplay);
  fillCoolStatus.SetBit(MFillH::kDoNotDisplay);
  filltrigmc1.SetBit(MFillH::kDoNotDisplay);
  filltrigmc2.SetBit(MFillH::kDoNotDisplay);
  filltrigmc3.SetBit(MFillH::kDoNotDisplay);
  filltrigmc4.SetBit(MFillH::kDoNotDisplay);
  filltrigmc5.SetBit(MFillH::kDoNotDisplay);
  filltrigmc6.SetBit(MFillH::kDoNotDisplay);
  filltrigmc7.SetBit(MFillH::kDoNotDisplay);
  filltrigmc8.SetBit(MFillH::kDoNotDisplay);
  filltrigmc9.SetBit(MFillH::kDoNotDisplay);
  filltrigmc10.SetBit(MFillH::kDoNotDisplay);
  filltrigmc11.SetBit(MFillH::kDoNotDisplay);
  filltrigmc12.SetBit(MFillH::kDoNotDisplay);
  filltrigmc13.SetBit(MFillH::kDoNotDisplay);
  filltrigmc14.SetBit(MFillH::kDoNotDisplay);
  filltrigmc15.SetBit(MFillH::kDoNotDisplay);
  filltrigmc16.SetBit(MFillH::kDoNotDisplay);
  filltrigmc17.SetBit(MFillH::kDoNotDisplay);
  filltrigmc18.SetBit(MFillH::kDoNotDisplay);
  filltrigmc19.SetBit(MFillH::kDoNotDisplay);
  fillcammean.SetBit(MFillH::kDoNotDisplay);
  fillHVD.SetBit(MFillH::kDoNotDisplay);

  // Skip the fill function is there is no tree entries
  fillZd.SetBit(MFillH::kCanSkip);
  fillAz.SetBit(MFillH::kCanSkip);
  fillError.SetBit(MFillH::kCanSkip);
  fillDState.SetBit(MFillH::kCanSkip);
  fillError2.SetBit(MFillH::kCanSkip);
  fillHVA.SetBit(MFillH::kCanSkip);
  fillHVB.SetBit(MFillH::kCanSkip);
  fillCA.SetBit(MFillH::kCanSkip);
  fillCB.SetBit(MFillH::kCanSkip);
  fillLVTemp.SetBit(MFillH::kCanSkip);
  fillLVHum.SetBit(MFillH::kCanSkip);
  fillLVStatus.SetBit(MFillH::kCanSkip);
  fillLVRPS.SetBit(MFillH::kCanSkip);
  fillTCenter.SetBit(MFillH::kCanSkip);
  fillTWall.SetBit(MFillH::kCanSkip);
  fillTWater.SetBit(MFillH::kCanSkip);
  fillTOptLink.SetBit(MFillH::kCanSkip);
  fillTempOptLink.SetBit(MFillH::kCanSkip);
  fillHWall.SetBit(MFillH::kCanSkip);
  fillHCenter.SetBit(MFillH::kCanSkip);
  fillCCHum.SetBit(MFillH::kCanSkip);
  fillCCTemp.SetBit(MFillH::kCanSkip);
  fillCCWS.SetBit(MFillH::kCanSkip);
  fillCCSR.SetBit(MFillH::kCanSkip);
  fillTrigBL2.SetBit(MFillH::kCanSkip);
  fillTrigAL2.SetBit(MFillH::kCanSkip);
  fillTrigStatus.SetBit(MFillH::kCanSkip);
  fillTrigZd.SetBit(MFillH::kCanSkip);
  fillCamStatus.SetBit(MFillH::kCanSkip);
  fillCamSentStatus.SetBit(MFillH::kCanSkip);
  fillCamStatusDC.SetBit(MFillH::kCanSkip);
  fillHVStatus.SetBit(MFillH::kCanSkip);
  fillLidsStatus.SetBit(MFillH::kCanSkip);
  fillCoolStatus.SetBit(MFillH::kCanSkip);
  filltrigmc1.SetBit(MFillH::kCanSkip);
  filltrigmc2.SetBit(MFillH::kCanSkip);
  filltrigmc3.SetBit(MFillH::kCanSkip);
  filltrigmc4.SetBit(MFillH::kCanSkip);
  filltrigmc5.SetBit(MFillH::kCanSkip);
  filltrigmc6.SetBit(MFillH::kCanSkip);
  filltrigmc7.SetBit(MFillH::kCanSkip);
  filltrigmc8.SetBit(MFillH::kCanSkip);
  filltrigmc9.SetBit(MFillH::kCanSkip);
  filltrigmc10.SetBit(MFillH::kCanSkip);
  filltrigmc11.SetBit(MFillH::kCanSkip);
  filltrigmc12.SetBit(MFillH::kCanSkip);
  filltrigmc13.SetBit(MFillH::kCanSkip);
  filltrigmc14.SetBit(MFillH::kCanSkip);
  filltrigmc15.SetBit(MFillH::kCanSkip);
  filltrigmc16.SetBit(MFillH::kCanSkip);
  filltrigmc17.SetBit(MFillH::kCanSkip);
  filltrigmc18.SetBit(MFillH::kCanSkip);
  filltrigmc19.SetBit(MFillH::kCanSkip);
  fillcammean.SetBit(MFillH::kCanSkip);
  fillHVD.SetBit(MFillH::kCanSkip);

  // Add all the task to the task list
  tlist.AddToList(&read);
  tlist.AddToList(&fillZd,       "Drive");
  tlist.AddToList(&fillAz,       "Drive");
  tlist.AddToList(&fillError,    "Drive");
  tlist.AddToList(&fillDState,   "Drive");
  tlist.AddToList(&fillError2,   "Drive");
  tlist.AddToList(&fillHVA,      "Camera");
  tlist.AddToList(&fillHVB,      "Camera");
  tlist.AddToList(&fillCA,       "Camera");
  tlist.AddToList(&fillCB,       "Camera");
  tlist.AddToList(&fillLVTemp,   "Camera");
  tlist.AddToList(&fillLVHum,    "Camera");
  tlist.AddToList(&fillLVStatus, "Camera");
  tlist.AddToList(&fillLVRPS,    "Camera");
  tlist.AddToList(&fillTCenter,  "Camera");
  tlist.AddToList(&fillTWall,    "Camera");
  tlist.AddToList(&fillTWater,   "Camera");
  tlist.AddToList(&fillTOptLink, "Camera");
  tlist.AddToList(&fillTempOptLink, "Camera");
  tlist.AddToList(&fillHWall,    "Camera");
  tlist.AddToList(&fillHCenter,  "Camera");
  tlist.AddToList(&fillCCHum,    "CC");
  tlist.AddToList(&fillCCTemp,   "CC");
  tlist.AddToList(&fillCCWS,     "CC");
  tlist.AddToList(&fillCCSR,     "CC");
  tlist.AddToList(&fillTrigBL2,  "Trigger");
  tlist.AddToList(&fillTrigAL2,  "Trigger");
  tlist.AddToList(&fillTrigStatus,  "Trigger");
  tlist.AddToList(&fillTrigZd);
  tlist.AddToList(&fillCamStatus,  "Camera");
  tlist.AddToList(&fillCamSentStatus,  "Camera");
  tlist.AddToList(&fillCamStatusDC,"Camera");
  tlist.AddToList(&fillHVStatus,"Camera");
  tlist.AddToList(&fillLidsStatus,"Camera");
  tlist.AddToList(&fillCoolStatus,"Camera");
  tlist.AddToList(&filltrigmc1, "Trigger");
  tlist.AddToList(&filltrigmc2, "Trigger");
  tlist.AddToList(&filltrigmc3, "Trigger");
  tlist.AddToList(&filltrigmc4, "Trigger");
  tlist.AddToList(&filltrigmc5, "Trigger");
  tlist.AddToList(&filltrigmc6, "Trigger");
  tlist.AddToList(&filltrigmc7, "Trigger");
  tlist.AddToList(&filltrigmc8, "Trigger");
  tlist.AddToList(&filltrigmc9, "Trigger");
  tlist.AddToList(&filltrigmc10, "Trigger");
  tlist.AddToList(&filltrigmc11, "Trigger");
  tlist.AddToList(&filltrigmc12, "Trigger");
  tlist.AddToList(&filltrigmc13, "Trigger");
  tlist.AddToList(&filltrigmc14, "Trigger");
  tlist.AddToList(&filltrigmc15, "Trigger");
  tlist.AddToList(&filltrigmc16, "Trigger");
  tlist.AddToList(&filltrigmc17, "Trigger");
  tlist.AddToList(&filltrigmc18, "Trigger");
  tlist.AddToList(&filltrigmc19, "Trigger");
  tlist.AddToList(&fillcammean, "Camera");
  tlist.AddToList(&fillHVD);

  //
  // Create and setup the eventloop
  //
  MEvtLoop evtloop;
  evtloop.SetParList(&plist);
  if(!batchmode)
    evtloop.SetDisplay(d);
    
  //
  // Execute your analysis
  //
  if (!evtloop.Eventloop())
    return;
 
  tlist.PrintStatistics();

  gStyle->SetPadGridX(kTRUE);
  gStyle->SetPadGridY(kTRUE);
  gStyle->SetCanvasColor(kWhite);
  MHVsTime *clone1;
  TGraph *g;
  TH1 *hist;
  TH1 *hist2;
  TAxis *axey;
  TAxis *axex;
  MH3 *mh3;
  MHPixVsTime *pixclone1;
  MHCamera *Sum;
  //
  // Drive report (pointing.C from T. Bretz)
  //
  TCanvas *c1;
  if ((d = evtloop.GetDisplay()))
      c1 = &d->AddTab("DRIVE SYSTEM");
  else
      c1 = new TCanvas();

  // ************* Plot the telescope tracking positions on the sky ***********
  TGraph *g1 = hZd.GetGraph();
  TGraph *g2 = hAz.GetGraph();
  TPad *p = new TPad("", "",0,0.05,0.6,0.95);
  p->Draw();
  p->cd();
  gPad->SetTheta(-90);
  gPad->SetPhi(90);
  gPad->SetBorderMode(0);
  gStyle->SetOptStat(0);
  TH2F h("pol", "Telescope Tracking Positions on the Sky", 16, 0, 1, 9, 0, 1);
  h.DrawClone("surf1pol");
  gPad->Modified();
  gPad->Update();
  TView *view = gPad->GetView();
  if (!view)
    {
      cout << "No View!" << endl;
      return;
    }
  Double_t *zd=g1->GetY();
  Double_t *az=g2->GetY();
  Double_t old[2] = {0,0};
  for (int i=0; i<g1->GetN(); i++)
    {
      az[i] += 180;
      az[i] *= TMath::Pi()/180;
      Double_t x[3] = { zd[i]*cos(az[i])/90, zd[i]*sin(az[i])/90, 0};
      Double_t y[3];
      view->WCtoNDC(x, y);
      if (old[0]!=0 && old[1]!=1)
        {
	  TLine *l = new TLine(y[0], y[1], old[0], old[1]);
	  l->SetLineColor(kBlue);
	  l->Draw();
        }
      TMarker *m = new TMarker(y[0], y[1], kFullDotMedium);
      m->SetMarkerColor(i==g1->GetN()-1 ? kGreen : kRed);
      m->Draw();
      old[0] = y[0];
      old[1] = y[1];
    }
  // ---------------------- Control deviation of the motors -------------------
  c1->cd();
  p = new TPad("", "", 0.6, 0, 1, 0.29);
  p->Draw();
  p->cd();
  gStyle->SetOptStat(1110);
  gStyle->SetStatFormat(".2g");
  mh3 = (MH3*)hError.DrawClone("nonew");
  mh3->GetHist().SetXTitle("\\Delta [arcmin]");
  mh3->GetHist().SetYTitle("");
  mh3->GetHist().SetTitle("Control deviation of the motors");
  mh3->GetHist().SetStats(1);
  mh3->GetHist().SetLabelSize(0.06,"X");
  mh3->GetHist().SetLabelSize(0.06,"Y");
  mh3->GetHist().SetTitleSize(0.06,"X");
  TLine ln;
  ln.SetLineColor(kGreen);
  ln.DrawLine(0.5*360*60/16384., 0, 0.5*360*60/16384., hError.GetHist().GetMaximum());
  ln.SetLineColor(kYellow);
  ln.DrawLine(1.0*360*60/16384., 0, 1.0*360*60/16384., hError.GetHist().GetMaximum());
  ln.SetLineColor(kRed);
  ln.DrawLine(2.0*360*60/16384., 0, 2.0*360*60/16384., hError.GetHist().GetMaximum());
  // ---------------------- Plot the drive status vs time ---------------------
  c1->cd();
  p = new TPad("", "", 0.6, 0.29, 1, 0.42);
  p->Draw();
  p->cd();
  MHVsTime *hvt = (MHVsTime*)hDState.DrawClone("nonew");
  hvt->GetGraph()->SetMarkerStyle(kFullDotSmall);
  if (hvt->GetGraph()->GetN())
    {
      hist = hvt->GetGraph()->GetHistogram();
      TAxis *axey = hist->GetYaxis();
      TAxis *axex = hist->GetXaxis();
      hist->GetXaxis()->SetTimeFormat("%H:%M %F1995-01-01 00:00:00");
      hist->SetXTitle("Time");
      hist->SetYTitle("");
      hist->SetTitle("");
      hist->SetLabelSize(0.1,"X");
      hist->SetTitleSize(0.06,"X");
      hist->SetStats(0);
      hist->SetMinimum(-0.5);
      hist->SetMaximum(4.5);
      axey->Set(5, -0.5, 4.5);
      axey->SetBinLabel(axey->FindFixBin(0), "Error");
      axey->SetBinLabel(axey->FindFixBin(1), "Stopped");
      axey->SetBinLabel(axey->FindFixBin(3), "Moving");
      axey->SetBinLabel(axey->FindFixBin(4), "Tracking");
      axey->SetLabelSize(0.15);
      axex->SetLabelSize(0.08);
      axex->SetTitleSize(0.09);
      axex->SetTitleOffset(0.45);
    }  
  // --------------- Control deviation of the motors vs zenith angle ----------
  c1->cd();
  p = new TPad("", "", 0.6, 0.71, 1, 1);
  p->Draw();
  p->cd();
  gStyle->SetOptStat(1110);
  gStyle->SetStatFormat(".2g");
  mh3 = (MH3*)hError2.DrawClone("nonew");
  mh3->GetHist().SetXTitle("Zd [\\circ]");
  mh3->GetHist().SetYTitle("\\Delta [arcmin]");
  mh3->GetHist().SetTitle("Control deviation of the motors");
  mh3->GetHist().SetLabelSize(0.06,"X");
  mh3->GetHist().SetLabelSize(0.06,"Y");
  mh3->GetHist().SetTitleSize(0.06,"X");
  mh3->GetHist().SetTitleSize(0.06,"Y");
  mh3->GetHist().SetStats(1);
  mh3->GetHist().Draw("box");
  // -------------------------- Zenith angle vs time --------------------------
  c1->cd();
  p = new TPad("", "", 0.6, 0.42, 1, 0.71);
  p->Draw();
  p->cd();
  gPad->SetBorderMode(0);
  hvt = (MHVsTime*)hZd.DrawClone("nonew");
  hvt->GetGraph()->SetMarkerStyle(kFullDotSmall);
  if (hvt->GetGraph()->GetN())
    {
      hvt->GetGraph()->GetHistogram()->GetXaxis()->SetTimeFormat("%H:%M %F1995-01-01 00:00:00");
      hvt->GetGraph()->GetHistogram()->SetXTitle("Time");
      hvt->GetGraph()->GetHistogram()->SetYTitle("Zd [\\circ]");
      hvt->GetGraph()->GetHistogram()->SetTitle("Zd vs. Time");
      hvt->GetGraph()->GetHistogram()->SetLabelSize(0.06,"X");
      hvt->GetGraph()->GetHistogram()->SetTitleSize(0.06,"X");
      hvt->GetGraph()->GetHistogram()->SetLabelSize(0.06,"Y");
      hvt->GetGraph()->GetHistogram()->SetTitleSize(0.06,"Y");
      hvt->GetGraph()->GetHistogram()->SetStats(0);
    }
  
  
  //
  // Camera report 
  // 
  // **************************** CAMERA STATUS ********************
  if ((d = evtloop.GetDisplay()))
    TCanvas &c2 = d.AddTab("CAMERA STATUS");
  else
    TCanvas *c2 = new TCanvas();
  c2->Divide(1,2);
  c2->cd(1);
  g = hCamStatus.GetGraph();
  g->SetMarkerStyle(kFullDotSmall);
  g->SetTitle("Camera status");
  clone1 = (MHVsTime*)hCamStatus.DrawClone("nonew");
  if(clone1->GetGraph()->GetN())
    {
      hist = clone1->GetGraph()->GetHistogram();
      axey = hist->GetYaxis();
      hist->SetMinimum(-0.5);
      hist->SetMaximum(9.5);
      axey->Set(10,-0.5,9.5);
      axey->SetBinLabel(axey->FindFixBin(0),"EROR");
      axey->SetBinLabel(axey->FindFixBin(1),"ALARM");
      axey->SetBinLabel(axey->FindFixBin(2),"BLOCKED!");
      axey->SetBinLabel(axey->FindFixBin(3),"WARM!");
      axey->SetBinLabel(axey->FindFixBin(4),"HOT");
      axey->SetBinLabel(axey->FindFixBin(5),"HVRAMPING");
      axey->SetBinLabel(axey->FindFixBin(6),"OK");
      axey->SetBinLabel(axey->FindFixBin(7),"INIT");
      axey->SetBinLabel(axey->FindFixBin(8),"SHUTD");
      axey->SetBinLabel(axey->FindFixBin(9),"N/A");
      hist->SetLabelSize(0.04,"X");
      hist->SetLabelSize(0.05,"Y");
      hist->GetXaxis()->SetTimeFormat("%H:%M %F1995-01-01 00:00:00");
      hist->SetXTitle("Time");
      hist->SetYTitle("");
    }
  c2->cd(2);
  g = hLidsStatus.GetGraph();
  g->SetMarkerStyle(kFullDotSmall);
  g->SetTitle("Camera Lids status");
  clone1 = (MHVsTime*)hLidsStatus.DrawClone("nonew");
  if(clone1->GetGraph()->GetN())
    {
      hist = clone1->GetGraph()->GetHistogram();
      axey = hist->GetYaxis();
      hist->SetMinimum(-0.5);
      hist->SetMaximum(9.5);
      axey->Set(10,-0.5,9.5);
      axey->SetBinLabel(axey->FindFixBin(0),"Error");
      axey->SetBinLabel(axey->FindFixBin(1),"SafetyLimit");
      axey->SetBinLabel(axey->FindFixBin(4),"Closed");
      axey->SetBinLabel(axey->FindFixBin(5),"Opened");
      axey->SetBinLabel(axey->FindFixBin(6),"Moving");
      axey->SetBinLabel(axey->FindFixBin(7),"Stopped");
      axey->SetBinLabel(axey->FindFixBin(9),"n/a");
      hist->SetLabelSize(0.04,"X");
      hist->SetLabelSize(0.06,"Y");
      hist->GetXaxis()->SetTimeFormat("%H:%M %F1995-01-01 00:00:00");
      hist->SetXTitle("Time");
      hist->SetYTitle("");
    }
  // ************************* HV of power supplies ***************************
  if ((d = evtloop.GetDisplay()))
    TCanvas &c3 = d.AddTab("CAMERA HV");
  else
    TCanvas *c3 = new TCanvas();
  c3->Divide(2,2);
  c3->cd(1);
  // --------------- Power supplies  and HV settings --------------------------
  TLegend *legHV = new TLegend(0.75,0.85,0.99,0.99);
  g = hHVA.GetGraph();
  g->SetMarkerColor(2);
  g->SetLineColor(2);
  g->SetTitle("Voltages of power supplies and camera settings");
  legHV->AddEntry(g,"PS A (hvps1)","l");
  clone1 = (MHVsTime*)hHVA.DrawClone("nonew");
  if(clone1->GetGraph()->GetN())
    {
      hist = clone1->GetGraph()->GetHistogram();
      hist->GetXaxis()->SetTimeFormat("%H:%M %F1995-01-01 00:00:00");
      hist->SetXTitle("Time");
      hist->SetYTitle("High voltage [V]");
      hist->SetLabelSize(0.04,"X");
      hist->SetMinimum(0);
    }
  g = hHVB.GetGraph();
  g->SetMarkerColor(3);
  g->SetLineColor(3);
  legHV->AddEntry(g,"PS B (hvps2)","l");
  clone1 = (MHVsTime*)hHVB.DrawClone("nonewsame");
  g = hCamMean.GetGraph();
  g->SetMarkerColor(4);
  g->SetLineColor(4);
  legHV->AddEntry(g,"Camera mean HV","l");
  clone1 = (MHVsTime*)hCamMean.DrawClone("nonewsame");
  legHV->DrawClone();
 // ---------------------------- HV status -----------------------------------
  c3->cd(2);
  g = hHVStatus.GetGraph();
  g->SetMarkerStyle(kFullDotSmall);
  g->SetTitle("Camera HV status");
  clone1 = (MHVsTime*)hHVStatus.DrawClone("nonew");
  if(clone1->GetGraph()->GetN())
    {
      hist = clone1->GetGraph()->GetHistogram();
      axey = hist->GetYaxis();
      hist->SetMinimum(-0.5);
      hist->SetMaximum(9.5);
      axey->Set(10,-0.5,9.5);
      axey->SetBinLabel(axey->FindFixBin(0),"Error");
      axey->SetBinLabel(axey->FindFixBin(1),"Mismatch");
      axey->SetBinLabel(axey->FindFixBin(2),"Trip");
      axey->SetBinLabel(axey->FindFixBin(3),"Ramping");
      axey->SetBinLabel(axey->FindFixBin(4),"Off");
      axey->SetBinLabel(axey->FindFixBin(5),"Nominal");
      axey->SetBinLabel(axey->FindFixBin(6),"LimCurrentWarning");
      axey->SetBinLabel(axey->FindFixBin(9),"n/a");
      hist->SetLabelSize(0.04,"X");
      hist->SetLabelSize(0.05,"Y");
      hist->GetXaxis()->SetTimeFormat("%H:%M %F1995-01-01 00:00:00");
      hist->SetXTitle("Time");
      hist->SetYTitle("");
    }
  // ---------------------------- HV settings ---------------------------------
  c3->cd(3);
  Sum = (MHCamera*)HVdisplay.GetHistByName("sum");
  TVirtualPad *c = gPad;
  c->cd(1);
  gPad->SetBorderMode(0);
  Sum->SetYTitle("Mean");
  Sum->DrawClone("pixelindex"); 
  c3->cd(4);
  gPad->SetBorderMode(0);
  Sum->SetYTitle("Mean HV settings");
  Sum->DrawClone("EPhist");

  // ******************** Currents of power supplies *******************
  if ((d = evtloop.GetDisplay()))
    TCanvas &c4 = d.AddTab("CAMERA DC");
  else
    TCanvas *c4 = new TCanvas();
 // ----------------------------- Power supplies ------------------------------
  c4->Divide(1,2);
  c4->cd(1);
  TLegend *legC = new TLegend(0.85,0.75,0.99,0.99);
  g = hCA.GetGraph();
  g->SetMarkerColor(2);
  g->SetLineColor(2);
  g->SetTitle("Currents of power supplies");
  legC->AddEntry(g,"PS A (curr1)","l");
  clone1 = (MHVsTime*)hCA.DrawClone("nonew");
  if(clone1->GetGraph()->GetN())
    {
      hist = clone1->GetGraph()->GetHistogram();
      hist->GetXaxis()->SetTimeFormat("%H:%M %F1995-01-01 00:00:00");
      hist->SetXTitle("Time");
      hist->SetYTitle("Current [mA]");
      hist->SetLabelSize(0.04,"X");
      hist->SetMinimum(0);
    }
  g = hCB.GetGraph();
  g->SetMarkerColor(3);
  g->SetLineColor(3);
  legC->AddEntry(g,"PS B (curr2)","l");
  clone1 = (MHVsTime*)hCB.DrawClone("nonewsame");
  legC->DrawClone();
  c4->cd(2);
  g = hCamStatusDC.GetGraph();
  g->SetMarkerStyle(kFullDotSmall);
  g->SetTitle("Camera DC status");
  clone1 = (MHVsTime*)hCamStatusDC.DrawClone("nonew");
  if(clone1->GetGraph()->GetN())
    {
      hist = clone1->GetGraph()->GetHistogram();
      axey = hist->GetYaxis();
      hist->SetMinimum(-0.5);
      hist->SetMaximum(9.5);
      axey->Set(10,-0.5,9.5);
      axey->SetBinLabel(axey->FindFixBin(0),"Error");
      axey->SetBinLabel(axey->FindFixBin(1),"Alarm");
      axey->SetBinLabel(axey->FindFixBin(3),"Hot");
      axey->SetBinLabel(axey->FindFixBin(5),"Ok");
      axey->SetBinLabel(axey->FindFixBin(6),"Warm");
      axey->SetBinLabel(axey->FindFixBin(9),"n/a");
      hist->SetLabelSize(0.09,"Y");
      hist->SetLabelSize(0.04,"X");
      hist->GetXaxis()->SetTimeFormat("%H:%M %F1995-01-01 00:00:00");
      hist->SetXTitle("Time");
      hist->SetYTitle("");
    }


  // ***************************** LV power supplies **************************
  if ((d = evtloop.GetDisplay()))
    TCanvas &c5 = d.AddTab("CAMERA LV");
  else
    TCanvas *c5 = new TCanvas();
  c5->Divide(2,2);
  c5->cd(1);
  g = hLVTemp.GetGraph();
  g->SetMarkerColor(2);
  g->SetLineColor(2);
  g->SetTitle("LV temperature");
  clone1 = (MHVsTime*)hLVTemp.DrawClone("nonew");
  if(clone1->GetGraph()->GetN())
    {
      hist = clone1->GetGraph()->GetHistogram();
      hist->GetXaxis()->SetTimeFormat("%H:%M %F1995-01-01 00:00:00");
      hist->SetXTitle("Time");
      hist->SetYTitle("Temperature [\\circ]");
      hist->SetLabelSize(0.04,"X");
    }
  c5->cd(2);
  g = hLVHum.GetGraph();
  g->SetMarkerColor(4);
  g->SetLineColor(4);
  g->SetTitle("LV humidity");
  clone1 = (MHVsTime*)hLVHum.DrawClone("nonew");
  if(clone1->GetGraph()->GetN())
    {
      hist = clone1->GetGraph()->GetHistogram();
      hist->GetXaxis()->SetTimeFormat("%H:%M %F1995-01-01 00:00:00");
      hist->SetXTitle("Time");
      hist->SetYTitle("Humidity [%]");
      hist->SetLabelSize(0.04,"X");
    }
  c5->cd(3);
  g = hLVStatus.GetGraph();
  g->SetMarkerStyle(kFullDotSmall);
  g->SetTitle("LV status");
  clone1 = (MHVsTime*)hLVStatus.DrawClone("nonew");
  if(clone1->GetGraph()->GetN())
    {
      hist = clone1->GetGraph()->GetHistogram();
      axey = hist->GetYaxis();
      hist->SetMinimum(-0.5);
      hist->SetMaximum(9.5);
      axey->Set(10,-0.5,9.5);
      axey->SetBinLabel(axey->FindFixBin(0),"Error");
      axey->SetBinLabel(axey->FindFixBin(1),"Alarm");
      axey->SetBinLabel(axey->FindFixBin(2),"Trip");
      axey->SetBinLabel(axey->FindFixBin(4),"Off");
      axey->SetBinLabel(axey->FindFixBin(5),"On");
      axey->SetBinLabel(axey->FindFixBin(9),"n/a");
      hist->SetLabelSize(0.04,"X");
      hist->SetLabelSize(0.05,"Y");
      hist->GetXaxis()->SetTimeFormat("%H:%M %F1995-01-01 00:00:00");
      hist->SetXTitle("Time");
      hist->SetYTitle("");
    }
  c5->cd(4);
  g = hLVRPS.GetGraph();
  g->SetMarkerStyle(kFullDotSmall);
  g->SetTitle("LV request power supply");
  clone1 = (MHVsTime*)hLVRPS.DrawClone("nonew");
  if(clone1->GetGraph()->GetN())
    {
      hist = clone1->GetGraph()->GetHistogram();
      hist->GetXaxis()->SetTimeFormat("%H:%M %F1995-01-01 00:00:00");
      hist->SetXTitle("Time");
      hist->SetYTitle("");
      hist->SetLabelSize(0.04,"X");
      axey = hist->GetYaxis();
      hist->SetMinimum(-0.5);
      hist->SetMaximum(1.5);
      axey->Set(2, -0.5, 1.5);
      axey->SetBinLabel(axey->FindFixBin(0), "OFF");
      axey->SetBinLabel(axey->FindFixBin(1), "ON");
    }
  // ****************************** Cooling ***********************************
  if ((d = evtloop.GetDisplay()))
    TCanvas &c6 = d.AddTab("COOLING SYSTEM");
  else
    TCanvas *c6 = new TCanvas();
  gStyle->SetPadGridX(kTRUE);
  gStyle->SetPadGridY(kTRUE);
  c6->Divide(2,2);
  c6->cd(1);
  // -------------------------- Camera temperatures ---------------------------
  TLegend *legtemp = new TLegend(0.75,0.80,0.99,0.99);
  g = hTCenter.GetGraph();
  g->SetMarkerColor(8);
  g->SetLineColor(8);
  g->SetTitle("Camera temperature");
  legtemp->AddEntry(g,"camera center","l");
  clone1 = (MHVsTime*)hTCenter.DrawClone("nonew");
  if(clone1->GetGraph()->GetN())
    {
      hist = clone1->GetGraph()->GetHistogram();
      hist->GetXaxis()->SetTimeFormat("%H:%M %F1995-01-01 00:00:00");
      hist->SetXTitle("Time");
      hist->SetYTitle("Temperature [\\circ C]");
      hist->SetLabelSize(0.04,"X");
      hist->SetMinimum(0);
    }
  g = hTWall.GetGraph();
  g->SetMarkerColor(12);
  g->SetLineColor(12);
  legtemp->AddEntry(g,"camera wall","l");
  clone1 = (MHVsTime*)hTWall.DrawClone("nonewsame");
  g = hTWater.GetGraph();
  g->SetMarkerColor(4);
  g->SetLineColor(4);
  legtemp->AddEntry(g,"water deposit","l");
  clone1 = (MHVsTime*)hTWater.DrawClone("nonewsame");
  g = hTOptLink.GetGraph();
  g->SetMarkerColor(2);
  g->SetLineColor(2);
  legtemp->AddEntry(g,"optical links","l");
  clone1 = (MHVsTime*)hTOptLink.DrawClone("nonewsame");
  legtemp->DrawClone();
  c6->cd(2);
  g = hCoolStatus.GetGraph();
  g->SetMarkerStyle(kFullDotSmall);
  g->SetTitle("Camera cooling status");
  clone1 = (MHVsTime*)hCoolStatus.DrawClone("nonew");
  if(clone1->GetGraph()->GetN())
    {
      hist = clone1->GetGraph()->GetHistogram();
      axey = hist->GetYaxis();
      hist->SetMinimum(-0.5);
      hist->SetMaximum(9.5);
      axey->Set(10,-0.5,9.5);
      axey->SetBinLabel(axey->FindFixBin(0),"Error");
      axey->SetBinLabel(axey->FindFixBin(1),"Alarm");
      axey->SetBinLabel(axey->FindFixBin(4),"Off");
      axey->SetBinLabel(axey->FindFixBin(5),"Ok");
      axey->SetBinLabel(axey->FindFixBin(6),"Temp.Warning");
      axey->SetBinLabel(axey->FindFixBin(7),"Cond.Warning");
      axey->SetBinLabel(axey->FindFixBin(9),"n/a");
      hist->SetLabelSize(0.04,"X");
      hist->SetLabelSize(0.05,"Y");
      hist->GetXaxis()->SetTimeFormat("%H:%M %F1995-01-01 00:00:00");
      hist->SetXTitle("Time");
      hist->SetYTitle("");
    }
  // ------------------------- Camera relative humidity -----------------------
  c6->cd(3);
  TLegend *leghum = new TLegend(0.75,0.85,0.99,0.99);
  g = hHCenter.GetGraph();
  g->SetMarkerColor(8);
  g->SetLineColor(8);
  g->SetTitle("Camera relative humidity");
  leghum->AddEntry(g,"camera center","l");
  clone1 = (MHVsTime*)hHCenter.DrawClone("nonew");
  if(clone1->GetGraph()->GetN())
    {
      hist = clone1->GetGraph()->GetHistogram();
      hist->GetXaxis()->SetTimeFormat("%H:%M %F1995-01-01 00:00:00");
      hist->SetXTitle("Time");
      hist->SetYTitle("Relative humidity [%]");
      hist->SetLabelSize(0.04,"X");
      hist->SetMaximum(50);
      hist->SetMinimum(0);
    }
  g = hHWall.GetGraph();
  g->SetMarkerColor(12);
  g->SetLineColor(12);
  leghum->AddEntry(g,"camera wall","l");
  clone1 = (MHVsTime*)hHWall.DrawClone("nonewsame");
  leghum->DrawClone();
  // ---------------------- Temperature distribution --------------------------
  c6->cd(4);
  hist = hTempOptLink.GetHistByName("TOptLink");
  hist->SetXTitle("Temperature [\\circ C]");
  hist->SetYTitle("");
  hist->SetLineColor(2);
  hist->SetTitle("Distribution of opt. links temperature");
  hist->SetLabelSize(0.04,"X");
  hist->SetStats(0);
  mh3 = (MH3*)hTempOptLink.DrawClone("nonew");
  //
  // **************************** Trigger report ******************************
  //
  if ((d = evtloop.GetDisplay()))
    TCanvas &c7 = d.AddTab("TRIGGER SYSTEM");
  else
    TCanvas *c7 = new TCanvas();
  c7->Divide(1,3);
  c7->cd(1);
  gStyle->SetPadGridX(kTRUE);
  gStyle->SetPadGridY(kTRUE);
  gPad->SetLogy();
  TLegend *legtrig = new TLegend(0.85,0.85,0.99,0.99);
  g = hTrigBL2.GetGraph();
  g->SetMarkerStyle(kFullDotSmall);
  g->SetMarkerColor(2);
  g->SetLineColor(2);
  g->SetTitle("L2 trigger rate");
  legtrig->AddEntry(g,"Before prescaler","l");
  clone1 = (MHVsTime*)hTrigBL2.DrawClone("nonew");
  if(clone1->GetGraph()->GetN())
    {
      hist = clone1->GetGraph()->GetHistogram();
      hist->SetMinimum(1);
      hist->GetXaxis()->SetTimeFormat("%H:%M %F1995-01-01 00:00:00");
      hist->SetLabelSize(0.06,"X");
      hist->SetLabelSize(0.06,"Y");
      hist->SetTitleSize(0.06,"X");
      hist->SetTitleSize(0.06,"Y");
      hist->SetXTitle("Time");
      hist->SetYTitle("L2 trigger rate [Hz]"); 
    }
  g = hTrigAL2.GetGraph();
  g->SetMarkerColor(4);
  g->SetLineColor(4);
  legtrig->AddEntry(g,"After prescaler","l");
  clone1 = (MHVsTime*)hTrigAL2.DrawClone("nonewsame");
  legtrig->DrawClone();
   c7->cd(2);
   gPad->SetLogy();
   hist = hTrigZd.GetHistByName("TrigZd");
   hist->SetLabelSize(0.06,"X");
   hist->SetLabelSize(0.06,"Y");
   hist->SetTitleSize(0.06,"X");
   hist->SetTitleSize(0.06,"Y");
   hist->SetXTitle("Zenith angle[\\circ]");
   hist->SetYTitle("Trigger rate [Hz]");
   hist->SetMarkerColor(4);
   hist->SetTitle("L2 trigger rate after prescaler");
   hist->SetStats(0);
   mh3 = (MH3*)hTrigZd.DrawClone("nonew");
   hist->SetMinimum(1);
   c7->cd(3);
   g = hTrigStatus.GetGraph();
   g->SetMarkerStyle(kFullDotSmall);
   g->SetTitle("Trigger status");
   clone1 = (MHVsTime*)hTrigStatus.DrawClone("nonew");
   if(clone1->GetGraph()->GetN())
     {
       hist = clone1->GetGraph()->GetHistogram();
       axey = hist->GetYaxis();
       hist->SetMinimum(-0.5);
       hist->SetMaximum(5.5);
       axey->Set(6,-0.5,5.5);
       axey->SetBinLabel(axey->FindFixBin(0),"Error");
       axey->SetBinLabel(axey->FindFixBin(1),"Idle");
       axey->SetBinLabel(axey->FindFixBin(2),"Loading");
       axey->SetBinLabel(axey->FindFixBin(3),"Ready");
       axey->SetBinLabel(axey->FindFixBin(4),"Active");
       axey->SetBinLabel(axey->FindFixBin(5),"Stopped");
       hist->SetLabelSize(0.07,"Y");
       hist->SetLabelSize(0.06,"X");
       hist->SetTitleSize(0.06,"X");
       hist->GetXaxis()->SetTimeFormat("%H:%M %F1995-01-01 00:00:00");
       hist->SetXTitle("Time");
       hist->SetYTitle("");
     }
  // ***************************** Trigger of macrocells **********************
  if ((d = evtloop.GetDisplay()))
    TCanvas &c8 = d.AddTab("TRIG. MACROCELLs");
  else
    TCanvas *c8 = new TCanvas();
  c8->Divide(2,2);
  c8->cd(1);
  gPad->SetLogy();
  TLegend *legtrig = new TLegend(0.80,0.80,0.99,0.99);
  g = htrigmc1.GetGraph();
  g->SetMarkerStyle(kFullDotSmall);
  g->SetMarkerColor(2);
  g->SetLineColor(2);
  g->SetTitle("Trigger rate of macrocell 1 : (maximum)");
  pixclone1 = (MHPixVsTime*)htrigmc1.DrawClone("nonew");
  if(clone1->GetGraph()->GetN())
    {
      hist = htrigmc1->GetGraph()->GetHistogram();
      if(hist->GetMaximum()<g->GetHistogram()->GetMaximum())
	hist->SetMaximum(g->GetHistogram()->GetMaximum());
      hist->SetMinimum(1);
      hist->GetXaxis()->SetTimeFormat("%H:%M %F1995-01-01 00:00:00");
      hist->SetXTitle("Time");
      hist->SetYTitle("Trigger [Hz]");
      hist->SetLabelSize(0.04,"X");
      hist->SetLabelSize(0.05,"Y");
    }
  legtrig->AddEntry(hist,Form("Mc 1: %3.0e",hist->GetMaximum()),"p");
  legtrig->DrawClone();
  c8->cd(2);
  gPad->SetLogy();
  TLegend *legtrig = new TLegend(0.80,0.70,0.99,0.99);
  g = htrigmc2.GetGraph();
  g->SetMarkerStyle(kFullDotSmall);
  g->SetMarkerColor(2);
  g->SetTitle("Trigger rate of macrocells 2-7 : (maximum)");
  pixclone1 = (MHPixVsTime*)htrigmc2.DrawClone("nonew");
  if(clone1->GetGraph()->GetN())
    {
      hist = pixclone1->GetGraph()->GetHistogram();
      if(hist->GetMaximum()<g->GetHistogram()->GetMaximum())
	hist->SetMaximum(g->GetHistogram()->GetMaximum());
      hist->SetMinimum(1);
      hist->GetXaxis()->SetTimeFormat("%H:%M %F1995-01-01 00:00:00");
      hist->SetXTitle("Time");
      hist->SetYTitle("Trigger [Hz]");
      hist->SetLabelSize(0.04,"X");
      hist->SetLabelSize(0.05,"Y");
    }
  legtrig->AddEntry(hist,Form("Mc 2: %3.0e",hist->GetMaximum()),"p");
  g=htrigmc3.GetGraph();
  g->SetMarkerStyle(kFullDotSmall);
  g->SetMarkerColor(3);
  if(htrigmc3.GetGraph()->GetN())
  {
      if(hist->GetMaximum()<g->GetHistogram()->GetMaximum())
	  hist->SetMaximum(g->GetHistogram()->GetMaximum());
      legtrig->AddEntry(g,Form("Mc 3: %3.0e",g->GetHistogram()->GetMaximum()),"p");
  }
  pixclone1 = (MHPixVsTime*)htrigmc3.DrawClone("nonewsame");
  g=htrigmc4.GetGraph();
  g->SetMarkerStyle(kFullDotSmall);
  g->SetMarkerColor(4);
  if(htrigmc4.GetGraph()->GetN())
  {
      if(hist->GetMaximum()<g->GetHistogram()->GetMaximum())
	  hist->SetMaximum(g->GetHistogram()->GetMaximum());
      legtrig->AddEntry(g,Form("Mc 4: %3.0e",g->GetHistogram()->GetMaximum()),"p");
  }
  pixclone1 = (MHPixVsTime*)htrigmc4.DrawClone("nonewsame");
  g=htrigmc5.GetGraph();
  g->SetMarkerStyle(kFullDotSmall);
  g->SetMarkerColor(5);
  if(htrigmc5.GetGraph()->GetN())
  {
      legtrig->AddEntry(g,Form("Mc 5: %3.0e",g->GetHistogram()->GetMaximum()),"p");
      if(hist->GetMaximum()<g->GetHistogram()->GetMaximum())
	  hist->SetMaximum(g->GetHistogram()->GetMaximum());
  }
  pixclone1 = (MHPixVsTime*)htrigmc5.DrawClone("nonewsame");
  g=htrigmc6.GetGraph();
  g->SetMarkerStyle(kFullDotSmall);
  g->SetMarkerColor(6);
  if(htrigmc6.GetGraph()->GetN())
  {
      legtrig->AddEntry(g,Form("Mc 6: %3.0e",g->GetHistogram()->GetMaximum()),"p");
      if(hist->GetMaximum()<g->GetHistogram()->GetMaximum())
	  hist->SetMaximum(g->GetHistogram()->GetMaximum());
  }
  pixclone1 = (MHPixVsTime*)htrigmc6.DrawClone("nonewsame");
  g=htrigmc7.GetGraph();
  g->SetMarkerStyle(kFullDotSmall);
  g->SetMarkerColor(7);
  if(htrigmc7.GetGraph()->GetN())
  {
      legtrig->AddEntry(g,Form("Mc 7: %3.0e",g->GetHistogram()->GetMaximum()),"p");
      if(hist->GetMaximum()<g->GetHistogram()->GetMaximum())
	  hist->SetMaximum(g->GetHistogram()->GetMaximum());
  }
  pixclone1 = (MHPixVsTime*)htrigmc7.DrawClone("nonewsame");
  legtrig->DrawClone();
  c8->cd(3);
  gPad->SetLogy();
  TLegend *legtrig = new TLegend(0.80,0.70,0.99,0.99);
  g = htrigmc8.GetGraph();
  g->SetMarkerStyle(kFullDotSmall);
  g->SetMarkerColor(2);
  g->SetTitle("Trigger rate of macrocells 8-13 : (maximum)");
  if(htrigmc8.GetGraph()->GetN())
  {
      legtrig->AddEntry(g,Form("Mc 8: %3.0e",g->GetHistogram()->GetMaximum()),"p");
      if(hist->GetMaximum()<g->GetHistogram()->GetMaximum())
	  hist->SetMaximum(g->GetHistogram()->GetMaximum());
  }
  pixclone1 = (MHPixVsTime*)htrigmc8.DrawClone("nonew");
  if(clone1->GetGraph()->GetN())
    {
	hist = pixclone1->GetGraph()->GetHistogram();
      if(hist->GetMaximum()<g->GetHistogram()->GetMaximum())
	hist->SetMaximum(g->GetHistogram()->GetMaximum());
      hist->SetMinimum(1);
      hist->GetXaxis()->SetTimeFormat("%H:%M %F1995-01-01 00:00:00");
      hist->SetXTitle("Time");
      hist->SetYTitle("Trigger [Hz]");
      hist->SetLabelSize(0.04,"X");
      hist->SetLabelSize(0.05,"Y");
    }
  g=htrigmc9.GetGraph();
  g->SetMarkerStyle(kFullDotSmall);
  g->SetMarkerColor(3);
  if(htrigmc9.GetGraph()->GetN())
  {
      legtrig->AddEntry(g,Form("Mc 9: %3.0e",g->GetHistogram()->GetMaximum()),"p");
      if(hist->GetMaximum()<g->GetHistogram()->GetMaximum())
	  hist->SetMaximum(g->GetHistogram()->GetMaximum());
  }
  pixclone1 = (MHPixVsTime*)htrigmc9.DrawClone("nonewsame");
  g=htrigmc10.GetGraph();
  g->SetMarkerStyle(kFullDotSmall);
  g->SetMarkerColor(4);
  if(htrigmc10.GetGraph()->GetN())
  {
      legtrig->AddEntry(g,Form("Mc 10: %3.0e",g->GetHistogram()->GetMaximum()),"p");
      if(hist->GetMaximum()<g->GetHistogram()->GetMaximum())
	  hist->SetMaximum(g->GetHistogram()->GetMaximum());
  }
  pixclone1 = (MHPixVsTime*)htrigmc10.DrawClone("nonewsame");
  g=htrigmc11.GetGraph();
  g->SetMarkerStyle(kFullDotSmall);
  g->SetMarkerColor(5);
  if(htrigmc11.GetGraph()->GetN())
  {
      legtrig->AddEntry(g,Form("Mc 11: %3.0e",g->GetHistogram()->GetMaximum()),"p");
      if(hist->GetMaximum()<g->GetHistogram()->GetMaximum())
	  hist->SetMaximum(g->GetHistogram()->GetMaximum());
  }
  pixclone1 = (MHPixVsTime*)htrigmc11.DrawClone("nonewsame");
  g=htrigmc12.GetGraph();
  g->SetMarkerStyle(kFullDotSmall);
  g->SetMarkerColor(6);
  if(htrigmc12.GetGraph()->GetN())
  {
      legtrig->AddEntry(g,Form("Mc 12: %3.0e",g->GetHistogram()->GetMaximum()),"p");
      if(hist->GetMaximum()<g->GetHistogram()->GetMaximum())
	  hist->SetMaximum(g->GetHistogram()->GetMaximum());
  }
  pixclone1 = (MHPixVsTime*)htrigmc12.DrawClone("nonewsame");
  g=htrigmc13.GetGraph();
  g->SetMarkerStyle(kFullDotSmall);
  g->SetMarkerColor(7);
  if(htrigmc13.GetGraph()->GetN())
  {
      legtrig->AddEntry(g,Form("Mc 13: %3.0e",g->GetHistogram()->GetMaximum()),"p");
      if(hist->GetMaximum()<g->GetHistogram()->GetMaximum())
	  hist->SetMaximum(g->GetHistogram()->GetMaximum());
  }
  pixclone1 = (MHPixVsTime*)htrigmc13.DrawClone("nonewsame");
  legtrig->DrawClone();
  c8->cd(4);
  gPad->SetLogy();
  TLegend *legtrig = new TLegend(0.80,0.70,0.99,0.99);
  TGraph *g = htrigmc14.GetGraph();
  g->SetMarkerStyle(kFullDotSmall);
  g->SetMarkerColor(2);
  if(htrigmc14.GetGraph()->GetN())
      legtrig->AddEntry(g,Form("Mc 14: %3.0e",g->GetHistogram()->GetMaximum()),"p");
  g->SetTitle("Trigger rate of macrocells 14-19 : (maximum)");
  pixclone1 = (MHPixVsTime*)htrigmc14.DrawClone("nonew");
  if(clone1->GetGraph()->GetN())
    {
      hist = pixclone1->GetGraph()->GetHistogram();
      if(hist->GetMaximum()<g->GetHistogram()->GetMaximum())
	hist->SetMaximum(g->GetHistogram()->GetMaximum());
      hist->SetMinimum(1);
      hist->GetXaxis()->SetTimeFormat("%H:%M %F1995-01-01 00:00:00");
      hist->SetXTitle("Time");
      hist->SetYTitle("Trigger [Hz]");
      hist->SetLabelSize(0.04,"X");
      hist->SetLabelSize(0.05,"Y");
    }
  g=htrigmc15.GetGraph();
  g->SetMarkerStyle(kFullDotSmall);
  g->SetMarkerColor(3);
  if(htrigmc15.GetGraph()->GetN())
  {
      legtrig->AddEntry(g,Form("Mc 15: %3.0e",g->GetHistogram()->GetMaximum()),"p");
      if(hist->GetMaximum()<g->GetHistogram()->GetMaximum())
	  hist->SetMaximum(g->GetHistogram()->GetMaximum());
  }
  pixclone1 = (MHPixVsTime*)htrigmc15.DrawClone("nonewsame");
  g=htrigmc16.GetGraph();
  g->SetMarkerStyle(kFullDotSmall);
  g->SetMarkerColor(4);
  if(htrigmc16.GetGraph()->GetN())
  {
      legtrig->AddEntry(g,Form("Mc 16: %3.0e",g->GetHistogram()->GetMaximum()),"p");
      if(hist->GetMaximum()<g->GetHistogram()->GetMaximum())
	  hist->SetMaximum(g->GetHistogram()->GetMaximum());
  }
  pixclone1 = (MHPixVsTime*)htrigmc16.DrawClone("nonewsame");
  g=htrigmc17.GetGraph();
  g->SetMarkerStyle(kFullDotSmall);
  g->SetMarkerColor(5);
  if(htrigmc17.GetGraph()->GetN())
  {
      legtrig->AddEntry(g,Form("Mc 17: %3.0e",g->GetHistogram()->GetMaximum()),"p");
      if(hist->GetMaximum()<g->GetHistogram()->GetMaximum())
	  hist->SetMaximum(g->GetHistogram()->GetMaximum());
  }
  pixclone1 = (MHPixVsTime*)htrigmc17.DrawClone("nonewsame");
  g=htrigmc18.GetGraph();
  g->SetMarkerStyle(kFullDotSmall);
  g->SetMarkerColor(6);
  if(htrigmc18.GetGraph()->GetN())
  {
      legtrig->AddEntry(g,Form("Mc 18: %3.0e",g->GetHistogram()->GetMaximum()),"p");
      if(hist->GetMaximum()<g->GetHistogram()->GetMaximum())
	  hist->SetMaximum(g->GetHistogram()->GetMaximum());
  }
  pixclone1 = (MHPixVsTime*)htrigmc18.DrawClone("nonewsame");
  g=htrigmc19.GetGraph();
  g->SetMarkerStyle(kFullDotSmall);
  g->SetMarkerColor(7);
  if(htrigmc19.GetGraph()->GetN())
  {
      legtrig->AddEntry(g,Form("Mc 19: %3.0e",g->GetHistogram()->GetMaximum()),"p");
      if(hist->GetMaximum()<g->GetHistogram()->GetMaximum())
	  hist->SetMaximum(g->GetHistogram()->GetMaximum());
  }
  pixclone1 = (MHPixVsTime*)htrigmc19.DrawClone("nonewsame");
  legtrig->DrawClone();

  //
  // *************************** Weather station ******************************
  //
  if ((d = evtloop.GetDisplay()))
    TCanvas &c9 = d.AddTab("WEATHER STATION");
  else
    TCanvas *c9 = new TCanvas();
  c9->Divide(2,2);
  // ----------------------- Relative humidity --------------------------------
  c9->cd(1);
  g = hCCHum.GetGraph();
  g->SetMarkerStyle(kFullDotSmall);
  g->SetMarkerColor(4);
  g->SetLineColor(4);
  g->SetTitle("Relative humidity outside");
  clone1 = (MHVsTime*)hCCHum.DrawClone("nonew");
  if(clone1->GetGraph()->GetN())
    {
      hist = clone1->GetGraph()->GetHistogram();
      hist->GetXaxis()->SetTimeFormat("%H:%M %F1995-01-01 00:00:00");
      hist->SetXTitle("Time");
      hist->SetYTitle("Humidity [%]"); 
    }
  // -------------------------- Temperature -----------------------------------
  c9->cd(2);
  g = hCCTemp.GetGraph();
  g->SetMarkerStyle(kFullDotSmall);
  g->SetMarkerColor(2);
  g->SetLineColor(2);
  g->SetTitle("Temperature outside");
  clone1 = (MHVsTime*)hCCTemp.DrawClone("nonew");
  if(clone1->GetGraph()->GetN())
    {
      hist = clone1->GetGraph()->GetHistogram();
      hist->GetXaxis()->SetTimeFormat("%H:%M %F1995-01-01 00:00:00");
      hist->SetXTitle("Time");
      hist->SetYTitle("Temperature [\\circ C]"); 
    }
  // --------------------------- Wind speed -----------------------------------
  c9->cd(3);
  g = hCCWS.GetGraph();
  g->SetMarkerStyle(kFullDotSmall);
  g->SetMarkerColor(3);
  g->SetLineColor(3);
  g->SetTitle("Wind speed outside");
  clone1 = (MHVsTime*)hCCWS.DrawClone("nonew");
  if(clone1->GetGraph()->GetN())
    {
      hist = clone1->GetGraph()->GetHistogram();
      hist->GetXaxis()->SetTimeFormat("%H:%M %F1995-01-01 00:00:00");
      hist->SetXTitle("Time");
      hist->SetYTitle("Wind speed [km/h]"); 
    }
  // -------------------------- Solar radiation -------------------------------
  c9->cd(4);
  g = hCCSR.GetGraph();
  g->SetMarkerStyle(kFullDotSmall);
  g->SetMarkerColor(9);
  g->SetLineColor(9);
  g->SetTitle("Solar radiation outside");
  clone1 = (MHVsTime*)hCCSR.DrawClone("nonew");
  if(clone1->GetGraph()->GetN())
    {
      hist = clone1->GetGraph()->GetHistogram();
      hist->GetXaxis()->SetTimeFormat("%H:%M %F1995-01-01 00:00:00");
      hist->SetXTitle("Time");
      hist->SetYTitle("Solar radiation [W/m^2]"); 
    }

  // Save data in a postscriptfile (filename.ps)
  TString file;
  if (filename.Last('.')>0)
    file = filename(0, filename.Last('.')); 

  // Make sure the display hasn't been deleted by the user while the
  // eventloop was running.
  if ((d = evtloop.GetDisplay()))
      d->SaveAsPS(directory+file+".ps");
  else
    {
      c1->Print(directory+file+".ps(");
      c2->Print(directory+file+".ps");
      c3->Print(directory+file+".ps");
      c4->Print(directory+file+".ps");
      c5->Print(directory+file+".ps");
      c6->Print(directory+file+".ps");
      c7->Print(directory+file+".ps");
      c8->Print(directory+file+".ps");
      c9->Print(directory+file+".ps)");
    }
}














