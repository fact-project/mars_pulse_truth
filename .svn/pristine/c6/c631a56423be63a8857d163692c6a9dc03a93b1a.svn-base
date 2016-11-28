
/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// Author(s): S.C. Commichau,  Javier Rico, 12/2003                        //
//                                                                         //
// Macro to generate pedestal vs time (event) plot for a single pixel      //
//                                                                         // 
/////////////////////////////////////////////////////////////////////////////


const Int_t default_pixel = 1 ;

void pedvsevent(Int_t pixel = default_pixel, 
		TString pname = "/disc02/Data/rootdata/Miscellaneous/2003_11_29/20031128_03118_P_Park-camera-closed_E.root")
{

    TH1F* Pedestal    = new TH1F("Pedestal","Pedestals",100,0,20);
    TH1F* PedestalRMS = new TH1F("PedestalRMS","Pedestal RMS",100,0,10);
  
    //Create an empty parameter list and an empty task list
    //the tasklist is identified in the eventloop by ist name
    MParList       plist;
    MTaskList      tlist;

    //Creates a MPedestalPix object for each pixel, i.e. it is a
    //storage container for all Pedestal information in the camera
    MPedestalCam   cam;  
  
    plist.AddToList(&cam);

    //MHCamEvent hist;
    //hist.SetType(1);
    //plist.AddToList(&hist);

    plist.AddToList(&tlist);

    //Setup task and tasklist for the pedestals
    MReadMarsFile read("Events", pname);
    read.DisableAutoScheme();

    //Apply the geometry to geometry dependant containers.
    //MGeomApply changes the size of the arrays in the containers to a size
    //matching the number of pixels, eg: MPedestalCam, MBlindPixels
    //Default geometry is MGeomCamMagic
    MGeomApply      geomapl;
   
    //Task to calculate pedestals
    MPedCalcPedRun  ped;

    tlist.AddToList(&read);
    tlist.AddToList(&geomapl);
    tlist.AddToList(&ped); 

    //ped.SetPixel(pixel);
    //ped.Draw();

    //Create and setup the 1st Eventloop  
    MEvtLoop evtloop;
    evtloop.SetParList(&plist);

    //Execute first analysis, pedestals...
     if (!tlist.PreProcess(&plist))
         return;

     const Int_t nevents = read.GetEntries();

     Float_t x[nevents], rms[nevents], mean[nevents];


     Int_t i = 0;     
     while (tlist.Process())
     {
	 mean[i] = cam[pixel].GetPedestal();
         rms[i] = cam[pixel].GetPedestalRms();
         x[i] = i;
	 i++; 
     }

     TGraphErrors* pedgraph = new TGraphErrors(nevents,x,mean,NULL,NULL);
     TGraphErrors* rmsgraph = new TGraphErrors(nevents,x,rms,NULL,NULL);

	 //plist.FindObject("MPedestalCam")->Print();
    
     tlist.PostProcess(); 
     //if (!evtloop.Eventloop())
     // return;

     Float_t finalmean = cam[pixel].GetPedestal();
     Float_t finalrms = cam[pixel].GetPedestalRms();
   
     TLine* pedline = new TLine(0,finalmean,nevents,finalmean);
     TLine* rmsline = new TLine(0,finalrms,nevents,finalrms);
    
     //The draw area
     


     gROOT->Reset();
     gStyle->SetOptStat(0);
     // Set statistics options, 1111111
     //                         |||||||
     //                         ||||||histogram name
     //                         |||||number of entries
     //                         ||||mean value
     //                         |||RMS
     //                         ||underflows
     //                         |overflows
     //                         sum of bins
     
     // Set gPad options
     gStyle->SetFrameBorderMode(0);
     gStyle->SetPalette(1);
     // Delete Frames of subCanvas' does not work, hook it Mr. gPad!
     gStyle->SetFrameBorderSize(0);
     gStyle->SetCanvasColor(0);
     gStyle->SetFrameFillColor(0);
     gStyle->SetTitleFont(102);
     gStyle->SetTitleFillColor(0);
     gStyle->SetTitleBorderSize(0);
     gStyle->SetStatColor(0);
     gStyle->SetStatBorderSize(0);
     
     // Set Canvas options
     TCanvas *MyC1 = new TCanvas("MyC","Pedestal vs Event", 0, 100, 900, 500);
     MyC->SetBorderMode(0);    // Delete the Canvas' border line
     
     MyC->cd();
     gPad->SetBorderMode(0);
     
     //  TLine* pedline = new TLine(0,finalmean,nevents,finalmean);
     //TLine* rmsline = new TLine(0,finalrms,nevents,finalrms);
     
     tlist.PrintStatistics();
     //plist.FindObject("MPedestalCam")->Print();
     Size_t pos = pname.Last('/')+10;
     TString iRun = TString(pname(pos,5));
     
     char str[64];
     
     sprintf(str,"Run %s Pixel %d",iRun.Data(),pixel);
     
     pedgraph->SetMaximum(30);
     pedgraph->SetMinimum(0);
     pedgraph->SetMarkerStyle(24);
     pedgraph->SetMarkerSize(.5);
     pedgraph->GetXaxis()->SetTitleFont(102);
     pedgraph->GetYaxis()->SetTitleFont(102);
     pedgraph->GetXaxis()->SetLabelFont(102);
     pedgraph->GetYaxis()->SetLabelFont(102);
     pedgraph->SetTitle(str); 
//     pedgraph->SetTitleFont(102);
     pedgraph->GetYaxis()->SetTitleFont(102);
     pedgraph->GetXaxis()->SetTitle("Event");
     pedgraph->GetYaxis()->SetTitle("[FADC Counts]");
     pedgraph->GetXaxis()->SetLimits(0,nevents-1);
     
     rmsgraph->SetMarkerStyle(25);
     rmsgraph->SetMarkerSize(.5);
     rmsgraph->SetMarkerColor(8);

     pedline->SetLineColor(2);
     rmsline->SetLineColor(4);
     pedline->SetLineWidth(2);
     rmsline->SetLineWidth(2);

     pedgraph->Draw("AP");
     rmsgraph->Draw("P");
     
     pedline->Draw("same");
     rmsline->Draw("same");
     
     TLegend* leg = new TLegend(.14,.68,.39,.88);
     leg->SetHeader("");
     leg->AddEntry(pedgraph,"Event based Pedestal","P");
     leg->AddEntry(pedline,"Run based Pedestal","L");
     leg->AddEntry(rmsgraph,"Event based RMS","P");
     leg->AddEntry(rmsline,"Run based RMS","L");
     leg->SetFillColor(0);
     leg->SetLineColor(1);
     leg->SetBorderSize(1);


     leg->Draw("same");
}






























