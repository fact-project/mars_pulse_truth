//************************************************************************
//
// Authors : Eva Domingo,     12/2004 <mailto:domingo@ifae.es>
//           Wolfgang Wittek, 12/2004 <mailto:wittek@mpppmu.mpg.de>
//
//
// Macro for estimating the DISP parameter
// ---------------------------------------
//
// DISP is the distance between the center of gravity of the shower image
//      and the estimated source position, assumed to lie on the major 
//      axis of the shower
//
// In order to get an estimate of DISP 
// - one assumes a parametrization DISP = f(p[0],p[1],p[2],... ; s1,s2,s3,...)
//           where s1,s2,s3,... are measurable quantities like
//                              image parameters, (theta, phi), etc.
//           and p[0],p[1],p[2], ... are free parameters
//
// - and determines the free parameters p[0],p[1],p[2]... from MC gamma data 
//   by minimizing a parameter which measures the deviation of the estimated
//   source position (using DISP) and the true source position
//
// The following classes are used :
//
// MDispParameters  container holding the parameters p[0],p[1],p[2],...
//
// MDispCalc        task calculating DISP with the parameters stored in 
//                  MDispParameters
//
//   ::Calc         member function where the DISP parameterization is 
//                  defined. It calculates DISP for a given event;
//                  it is called by MDispCalc::Process()      
//
// MFindDisp        class with several functions :
//
//  ::DefineTrainMatrix     \  member functions which generate the training
//  ::DefineTestMatrix       | and test samples (in the form of matrices)
//  ::DefineTrainTestMatrix /  from the MC gamma data
//
//  ::FindParams    member function steering the minimization
//                  (definition of the fcn function for Minuit, setting up the 
//                  event loop to be executed in each minimization step,
//                  call to Minuit);
//                  for the minimization the training matrices are used
//
//  ::TestParams    member function testing the quality of the DISP estimate;
//                  for the test the test matrices are used
//
// MHDisp           container for histograms which are useful for judging
//                  the quality of the DISP estimate
//                  Also task calculating the Minimization parameter
//                  contribution of an event (in Fill()) 
//                  and computing the final Minimization parameter of the 
//                  whole event sample (in Finalize())
//
// MFDisp           filter to select sample for the optimization     
//
// 
// The classes are stored in the CVS directory Mars/mtemp/mifae/library
//                                   and       Mars/mtemp/mifae/macros
//
//
//************************************************************************

void OptimizeDisp()
{
    //************************************************************************

    Bool_t CMatrix     = kFALSE;  // Create training and test matrices 
    Bool_t WOptimize   = kFALSE;  // Do optimization using the training sample
                                  // and write Disp parameter values 
                                  // onto the file parDispfile
                                  // Optimize Disp with :
                        //TString typeOpt = "Data";
                        TString typeOpt = "MC";
    Bool_t RTest       = kFALSE;  // Test the quality of the Disp estimate
                                  // using the test matrix
    Bool_t WDisp       = kTRUE;  // Make Disp plots for the data of type :
                        //TString typeInput = "ON";
                        //TString typeInput = "OFF";
                        TString typeInput = "MC";

    //************************************************************************

    gLog.SetNoColors();

    if (gRandom)
      delete gRandom;
    gRandom = new TRandom3(0);
    
    //-----------------------------------------------
    //names of files to be read for generating the training and test matrices
    const char *filetrain  = "...";
    const char *filetest   = "...";
    
    //-----------------------------------------------
    // path for input for Mars
    TString inPathON  = 
      "/home/pcmagic14/wittek/CalibData/CrabLow42/2004_10_16/";
    TString inPathOFF = 
      "/home/pcmagic14/wittek/CalibData/CrabLow42/2004_10_17/";
    TString inPathMC  = 
    //"/discpepe/root_0.73mirror/wuerzburg/";
    "~domingo/MAGIC/Disp05/";
    
    // path for output from Mars
    TString outPath = "~domingo/MAGIC/Disp05/DispOptimization/";
    
    //-----------------------------------------------
    // names of files for which Disp plots should be made
    const char *offfile  = "*OffCrabLow42*";
    const char *onfile   = "*CrabLowEn42*";
    const char *mcfile   = "All_gammas0.73wuerzburg_zbin0to12_cleaned_3020_PeakSearch6_RunHeaders";
    //-----------------------------------------------
    
    
    //---------------------------------------------------------------------
    gLog << "=====================================================" << endl;
    gLog << "Macro OptimizeDisp : Start " << endl;
    
    gLog << "" << endl;
    gLog << "Macro OptimizeDisp : CMatrix, WOptimize, RTest, WDisp = "
         << (CMatrix    ? "kTRUE" : "kFALSE")  << ",  "
         << (WOptimize  ? "kTRUE" : "kFALSE")  << ",  "
         << (RTest      ? "kTRUE" : "kFALSE")  << ",  "
         << (WDisp      ? "kTRUE" : "kFALSE")  << endl;
    

    //--------------------------------------------
    // files to contain the matrices (generated from filenameTrain and
    //                                               filenameTest)
    // 
    // for the training
    TString fileMatrixTrain = outPath;
    fileMatrixTrain += "MatrixTrainDisp";
    fileMatrixTrain += ".root";
    gLog << "" << endl;
    gLog << "Files containing the Training and Test matrices :" << endl;
    gLog << "      fileMatrixTrain = " << fileMatrixTrain << endl; 
    
    // for testing
    TString fileMatrixTest = outPath;
    fileMatrixTest += "MatrixTestDisp";
    fileMatrixTest += ".root";
    gLog << "      fileMatrixTest = " << fileMatrixTest << endl; 
    gLog << "" << endl;
    

    //---------------
    // file to contain the optimum Disp parameter values  
    TString parDispFile = outPath;
    //    parDispFile += "parDispMC.root";
    parDispFile += "parDispMC_withcuts.root";

    gLog << "" << endl;
    gLog << "File containing the optimum Disp parameter values : parDispFile = " 
         << parDispFile << endl;


    // Set the filter cuts to select a sample of events for the Disp optimization
    //    (islandsmin,islandsmax,usedpixelsmin,usedpixelsmax,corepixelsmin,
    //     corepixelsmax,sizemin,sizemax,leakage1min,leakage1max,leakage2min,
    //     leakage2max,lengthmin,widthmin);
    MFDisp *fdisp = NULL;
    fdisp = new MFDisp;
    fdisp->SetCuts(0,2,7,600,0,600,0.,3000.,0.,0.,0.,0.,0.,0.);
    fdisp->SetName("FilterSelector2");


    // Create the MFindDisp object and set the file to store optimium Disp parameters
    MFindDisp finddisp(fdisp);
    finddisp.SetFilenameParam(parDispFile);


    
    //======================================================================
    // Create matrices and write them onto files 
    //======================================================================
    
    if (CMatrix)
    {
      gLog << "-----------------------------------------------------" << endl; 
      gLog << "Generate the training and test samples" << endl;
      
      //--------------------------------------------
      // files to be read for optimizing the Disp parameters
      // 
      // for the training
      TString filenameTrain = inPathMC;
      filenameTrain += mcfile;
      filenameTrain += ".root";
      Int_t howManyTrain = 30000;
      gLog << "filenameTrain = " << filenameTrain << ",   howManyTrain = "
	   << howManyTrain  << endl; 
      
      // for testing
      TString filenameTest = inPathMC;
      filenameTest += mcfile;
      filenameTest += ".root";
      Int_t howManyTest = 30000;
      gLog << "filenameTest = " << filenameTest << ",   howManyTest = "
	   << howManyTest  << endl; 
      
      
      //--------------------------------------------      
      // For the events in the matrices define requested distribution
      // in some quantities; this may be a 1-dim, 2-dim or 3-dim distribution
      
      /*
      // Define the cos(theta) distribution
      TString mname("costheta");
      MBinning bincost("Binning"+mname);
      bincost.SetEdges(10, 0., 1.0);
      
      //MH3 mh3("cos((MPointingPos.fZd)/kRad2Deg)");
      MH3 mh3("cos(MMcEvt.fTelescopeTheta)");
      mh3.SetName(mname);
      MH::SetBinning(&mh3.GetHist(), &bincost);
      
      for (Int_t i=1; i<=mh3.GetNbins(); i++)
	mh3.GetHist().SetBinContent(i, 1.0);
      */

      // Define the log10(Etrue) distribution
      TString mh3name("log10Etrue");
      MBinning binslogE("Binning"+mh3name);
      binslogE.SetEdges(50, 1., 3.);
      
      MH3 mh3("log10(MMcEvt.fEnergy)");
      mh3.SetName(mh3name);
      MH::SetBinning(&mh3.GetHist(), &binslogE);
      
      // Flat energy distribution 
      //      for (Int_t i=1; i<=mh3.GetNbins(); i++)
      //      	mh3.GetHist().SetBinContent(i, 1.0);
      
      // Power law energy distribution
      //      for (Int_t i=1; i<=mh3.GetNbins(); i++)
      //      {
      //        Double_t weight = pow((Double_t)i, -1.7);
      //        mh3.GetHist().SetBinContent(i, weight);
      //      }      

      /* 
      // define the 'cos(Theta) vs. log10(Etrue)' distribution
      TString mh3name("cosThetaVslog10Etrue");
      MBinning binslogE("Binning"+mh3name+"X");
      binslogE.SetEdges(18, 1.5, 2.2);
      MBinning binscost("Binning"+mh3name+"Y");
      binscost.SetEdges(10, 0., 1.0);
      
      //    MH3 mh3("log10(MMcEvt.fEnergy)", "cos((MPointingPos.fZd)/kRad2Deg)");
      MH3 mh3("log10(MMcEvt.fEnergy)", "cos(MMcEvt.fTelescopeTheta)");
      mh3.SetName(mh3name);
      MH::SetBinning((TH2*)&mh3.GetHist(), &binslogE, &binscost);
      
      for (Int_t i=1; i<=((TH2*)mh3.GetHist()).GetNbinsX(); i++)
      {
	//	Double_t weight = pow((Double_t)i, -1.7);
	for (Int_t j=1; j<=((TH2*)mh3.GetHist()).GetNbinsY(); j++)
	{
	  //	  mh3.GetHist().SetBinContent(i, j, weight);
	  mh3.GetHist().SetBinContent(i, j, 1.);
	}
      }
      */
      
      //--------------------------
      // Training and test samples are generated from the same input files
      if (filenameTrain == filenameTest)
      {
        if ( !finddisp.DefineTrainTestMatrix(
                              filenameTrain,   mh3, 
                              howManyTrain,    howManyTest,  
                              fileMatrixTrain, fileMatrixTest, 0)     )
	{
	  *fLog << "OptimizeDisp.C : DefineTrainTestMatrix failed" << endl;
          return;
        }	
      }
      
      //--------------------------
      // Training and test samples are generated from different input files
      else
      {
        if ( !finddisp.DefineTrainMatrix(filenameTrain, mh3,
					 howManyTrain,  fileMatrixTrain, 0) )
        {
          *fLog << "OptimizeDisp.C : DefineTrainMatrix failed" << endl;
          return;
        }

	if ( !finddisp.DefineTestMatrix( filenameTest, mh3, 
					 howManyTest,  fileMatrixTest, 0)  )
	{
          *fLog << "OptimizeDisp.C : DefineTestMatrix failed" << endl;
          return;
        }
      }

      gLog << "Generation of training and test samples finished" << endl;
      gLog << "-----------------------------------------------------" << endl; 
    }
    
    

    //======================================================================
    // Optimize Disp parameters using the training sample
    // 
    // the initial values of the parameters are taken 
    //     - from the file parDispInit (if != "")
    //     - or from the arrays params and steps (if their sizes are != 0)
    //     - or from the MDispParameters constructor
    //======================================================================
    
    if (WOptimize)
    {
      gLog << "-----------------------------------------------------" << endl; 
      gLog << "Optimize the Disp parameters over a " 
	   << typeOpt << " sample, using the training matrices" << endl;
      
      // Read training matrices from file
      finddisp.ReadMatrix(fileMatrixTrain, fileMatrixTest);

      //--------------------------------------------
      // file which contains the initial values for the Disp parameters; 
      // if parDispInit ="" 
      //    - the initial values are taken from the arrays params and steps
      //      if their sizes are different from 0
      //    - otherwise they are taken from the MDispParameters constructor
      
      TString parDispInit = outPath;
      //parDispInit += "parDispInit.root";
      parDispInit = "";
      
      //--------------------------------------------
      
      TArrayD params(0);
      TArrayD steps(0);
      
      if (parDispInit == "")
      {
	Double_t vparams[5] = {
	// p[0],     p[1],     p[2],     p[3],     p[4],     p[5]
	   1.0,      0.6,     -0.8,     -0.8,     -1.2/*,      0.5*/};
	// 0.5,      0.,       0.03,     0.,       0./*,       0.5*/};
	// 0.8,      0.,       0.,       0.,       0./*,       0.5*/};
	
	Double_t vsteps[5] = {
	// dp[0],    dp[1],    dp[2],    dp[3],    dp[4],    dp[5]
	   0.01,     0.005,    0.005,    0.005,    0.01/*,    0.001*/};
	// 0.01,     0.01,     0.01,     0.01,     0.01/*,     0.001*/};
	// 0.01,     0.01,     0.01,     0.01,     0.01/*,     0.001*/};

	params.Set(5, vparams);
	steps.Set (5, vsteps );
      }

      
      Bool_t rf;
      rf = finddisp.FindParams(parDispInit, params, steps);
      
      if (!rf) 
      {
	gLog << "OptimizeDisp.C : optimization of the Disp parameters failed" << endl;
	return;
      }
      
      gLog << "Optimization of Disp parameters finished" << endl;
      gLog << "-----------------------------------------------------" << endl; 
    }



    //======================================================================
    // Test the Disp parameters on the test sample
    //======================================================================
    
    if (RTest)
    {
      // Read test matrices from file
      finddisp.ReadMatrix(fileMatrixTrain, fileMatrixTest);
      
      gLog << "-----------------------------------------------------" << endl; 
      gLog << "Test the Disp parameters using the test matrices" << endl;
      Bool_t rt = finddisp.TestParams();
      if (!rt) 
      {
	gLog << "Test of the Disp parameters on the test matrices failed" 
	     << endl;
      }
      gLog << "Test of the Disp parameters finished" << endl;
      gLog << "-----------------------------------------------------" << endl; 
    }
    
    


    //======================================================================
    // Make Disp plots
    //======================================================================

    if (WDisp)
    {
      gLog << "" << endl;
      gLog << "-----------------------------------------------------" << endl; 
      gLog << "Make plots for Disp for data of the type " << typeInput << endl;
      
      //--------------------------------------------
      // type of data to be read (ON, OFF or MC)
      if (typeInput == "ON")
	TString file(onfile);
      else if (typeInput == "OFF")
	TString file(offfile);
      else if (typeInput == "MC")
	TString file(mcfile);
      
      // name of input root file
      TString filenameData = inPathMC;
      filenameData += mcfile;
      filenameData += ".root";
      gLog << "Input file '" <<  filenameData << endl;
      
      //----------------------------------------------------
      // read in optimum Disp parameter values 
      
      TFile inparam(parDispFile);
      MDispParameters dispin;
      dispin.Read("MDispParameters");
      inparam.Close();
      
      gLog << "Disp parameter values were read in from file '"
	   << parDispFile << "'" << endl;
      
      TArrayD dispPar;
      dispPar =  dispin.GetParameters();
      
      TArrayD dispStep;
      dispStep =  dispin.GetStepsizes();
      
      gLog << "optimum parameter values for calculating Disp : " << endl;
      for (Int_t i=0; i<dispPar.GetSize(); i++)
      {
	gLog << dispPar[i] << ",  ";
      }
      gLog << endl;
      
      
      //----------------------------------------------------
      MTaskList tliston;
      MParList  pliston;
      
      MReadMarsFile read("Events", filenameData);
      read.DisableAutoScheme();
      
      // set cuts to select an event sample to apply Disp
      MContinue contdisp(fdisp);
      
      // calculate Disp
      MDispCalc dispcalc;
      
      // make Disp plots
      // SelectedPos = 1  means choose the right source position
      //               2                   wrong
      //               3               the position according to M3Long
      //               4               the position according to Asym
      MHDisp hdisp1;
      hdisp1.SetName("MHDispCorr");
      hdisp1.SetSelectedPos(1);
      MFillH filldisp1("MHDispCorr[MHDisp]", "");
      
      MHDisp hdisp2;
      hdisp2.SetName("MHDispWrong");
      hdisp2.SetSelectedPos(2);
      MFillH filldisp2("MHDispWrong[MHDisp]", "");
      
      MHDisp hdisp3;
      hdisp3.SetName("MHDispM3Long");
      hdisp3.SetSelectedPos(3);
      MFillH filldisp3("MHDispM3Long[MHDisp]", "");
      
      MHDisp hdisp4;
      hdisp4.SetName("MHDispAsym");
      hdisp4.SetSelectedPos(4);
      MFillH filldisp4("MHDispAsym[MHDisp]", "");
      
      
      //*****************************
      // entries in MParList

      pliston.AddToList(&tliston);
      pliston.AddToList(&dispin);
      pliston.AddToList(&hdisp1);
      pliston.AddToList(&hdisp2);
      pliston.AddToList(&hdisp3);
      pliston.AddToList(&hdisp4);
      
      //*****************************
      // entries in MTaskList
    
      tliston.AddToList(&read);
      if (fdisp != NULL)
	tliston.AddToList(&contdisp);
      tliston.AddToList(&dispcalc);
      tliston.AddToList(&filldisp1);
      tliston.AddToList(&filldisp2);
      tliston.AddToList(&filldisp3);
      tliston.AddToList(&filldisp4);
      
      //*****************************
      
      //-------------------------------------------
      // Execute event loop
      //
      MProgressBar bar;
      MEvtLoop evtloop;
      evtloop.SetParList(&pliston);
      evtloop.SetProgressBar(&bar);
      
      Int_t maxevents = -1;
      if ( !evtloop.Eventloop(maxevents) )
        return;
      
      tliston.PrintStatistics(0, kTRUE);
      
      //-------------------------------------------
      // Display the histograms
      //
      hdisp1.Draw();
      hdisp2.Draw();
      hdisp3.Draw();
      hdisp4.Draw();
      
      //-------------------------------------------
      
      gLog << "Disp plots were made for file '" << filenameData << endl; 
      gLog << "-----------------------------------------------------" << endl; 
    }
    
    delete fdisp;
    
    gLog << "Macro OptimizeDisp.C : End " << endl;
    gLog << "=======================================================" << endl;
    
}









