//
// A. Moralejo, March 2003
// This macro merges several MC camera files into one. The output files
// are useful to run Mars over larger samples of triggered events than 
// those contained in a single file.
//

void mergecamera(Char_t *in="Proton*root", Char_t *out="all.root")
{
  cout << endl
       << "WARNING: this macro merges camera output files, but the" << endl
       << "Run headers of the resulting file are those of the first" <<endl
       << "merged file, and so do not represent the whole of the" << endl
       << "merged events. Be careful not to mix files with different" << endl
       << "parameters (primary, theta...)." << endl << endl;

  //
  // Open output file:
  //
  TFile* f = new TFile(out,"recreate");

  //
  // First the Events tree:
  //
  MMcEvt* mmcevt = new MMcEvt();;
  MMcTrig* mmctrig = new MMcTrig();
  MRawEvtHeader* mrawhead = new MRawEvtHeader();
  MRawEvtData* mrawdata = new MRawEvtData();

  TChain c("Events");
  c.SetBranchAddress("MMcEvt",&mmcevt);
  c.SetBranchAddress("MMcTrig",&mmctrig);
  c.SetBranchAddress("MRawEvtHeader",&mrawhead);
  c.SetBranchAddress("MRawEvtData",&mrawdata);
  c.Add(in);
  c.Merge(f,1000);

  //
  // Now the RunHeaders tree:
  //
  MMcConfigRunHeader* mcconfig = new MMcConfigRunHeader();
  MMcCorsikaRunHeader* mccorsi = new MMcCorsikaRunHeader();
  MMcFadcHeader* mcfadc = new MMcFadcHeader();
  MMcRunHeader* mcrunhead = new MMcRunHeader();
  MMcTrigHeader* mctrighead = new MMcTrigHeader();
  MRawRunHeader* mrawrunhead = new MRawRunHeader();
  TChain d("RunHeaders");
  d.SetBranchAddress("MMcConfigRunHeader",&mcconfig);
  d.SetBranchAddress("MMcCorsikaRunHeader",&mccorsi);
  d.SetBranchAddress("MMcFadcHeader",&mcfadc);
  d.SetBranchAddress("MMcRunHeader",&mcrunhead);
  d.SetBranchAddress("MMcTrigHeader",&mctrighead);
  d.SetBranchAddress("MRawRunHeader",&mrawrunhead);
  d.Add(in);
  d.Merge(f,1000);

  f->Close();

  return;
}
