void Flux6c() 
{

  TString datafile = "files/AlphaEnergyTheta.LastKeichiRF.27MC.root";
  TString areafile = "files/area.root";

  TFile* file = new TFile(datafile);

  MHAlphaEnergyTheta hAlpha;
  hAlpha.Read("MHAlphaEnergyTheta");
  hAlpha.DrawClone();

  MHEffectiveOnTime hEffTime;
  hEffTime.Read("MHEffectiveOnTime");
  hEffTime.DrawClone();

  TFile* file3 = new TFile(areafile);
  MHMcCollectionArea area;
  area.Read("MHMcCollectionArea");
  area.DrawClone();

  //
  // Read Migration Matrix
  //
  TFile* file4 = new TFile("files/EnergyParams.Keichi.Resized.root");
  MHMcEnergyMigration migm;
  migm.Read("MHMcEnergyMigration");
  
  // ----------------------------------------------------------------------- 
  //
  // Calculate # Excess events vs. Energy and Theta
  //
  MHExcessEnergyTheta *hex = new MHExcessEnergyTheta;
  hex->Calc(&hAlpha);
  hex->Draw();
  
  MHFlux* hFluxNUnf = new MHFlux;
  hFluxNUnf->Calc(hex, &area, &hEffTime);
  TH1D* fluxNUnf = hFluxNUnf->GetAverageFlux();

  // ----------------------------------------------------------------------- 
  //
  // Unfold # Excess events vs. Energy and Theta
  //
  
  TH2D* tobeunfolded = hex->GetHist();
  TH2D* unfolded = new TH2D(*tobeunfolded);    
  
  MUnfoldSpectrum munfs;
  munfs.SetDistToUnfold(tobeunfolded);
  munfs.SetMigrationMatrix(migm->GetHist());
  munfs.SetPriorConstant();
  //munfs.SetPriorInput(hpr);
  munfs.SetUnfoldingMethod(2);
  munfs.Calc();
  unfolded=munfs.GetUnfDist();

}

