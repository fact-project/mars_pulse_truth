void runlivetime(TString filename)
{

  gROOT->Reset();
  gStyle->SetCanvasColor(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetFrameBorderMode(0);

  //
  // Make a loop only for the ON data:
  //
  
  MParList plist;
  MTaskList tlist;
  plist.AddToList(&tlist);

  //Containers
  
  MLiveTime livetime;

  plist.AddToList(&livetime);
  
  //
  //tasks
  //
  
  MReadTree read("Parameters");
  read.DisableAutoScheme();

  TString tmpfile = "./runlivetime.ls.tmp";
  TString cmd = "ls " + filename + " > " + tmpfile;
  gSystem->Exec(cmd);
  TString tmpline;
  ifstream in(tmpfile);
  while(1)
  {
      in >> tmpline;
      if(in.eof())
	  break;
      read.AddFile(tmpline);
  }
  TString cmd = "rm " + tmpfile;
  gSystem->Exec(cmd);

  Double_t timebin = 500.; //[sec]
  MLiveTimeCalc livetimecalc;
  livetimecalc.SetRealTimeBinSize(timebin);

  tlist.AddToList(&read);
  tlist.AddToList(&livetimecalc);

  //
  // Create and setup the eventloop
  //
  MEvtLoop loop;
  loop.SetParList(&plist);
     

  if (!loop.Eventloop())
      return;

//   if (!loop.PreProcess())
//     return;

//   while(loop.Process())
//     {}
  
//   loop.PostProcess();

  tlist.PrintStatistics();

  UInt_t numbertimebins = livetime.GetNumberTimeBins();
  TArrayD y(numbertimebins);
  TArrayD erry(numbertimebins);

  for (UInt_t bin=0; bin<numbertimebins; bin++)
    {
      y[bin] = bin+1.;
      erry[bin] = 0.;
    }

  TCanvas *c0 = new TCanvas;
  c0->cd(1);
  TGraphErrors *graph = new TGraphErrors(numbertimebins,livetime.GetMeanRealTimeArray(),y.GetArray(),livetime.GetWidthRealTimeArray(),erry.GetArray());
  graph->SetTitle("");
  graph->SetMinimum(0.);
  graph->SetMarkerStyle(21);
  graph->SetMarkerSize(0.03);
  graph->Draw("AP");
  graph->GetXaxis()->SetLabelSize(0.03);
  graph->GetXaxis()->SetTitle("Time [MJD]");
  //  lightcurvegraph->GetXaxis()->SetTimeDisplay(1);

  
}
