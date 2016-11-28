const int no_samples=6;


void calculate_of_weights(TH1F *shape, TString noise_file = "/mnt/home/pcmagic17/hbartko/mars/results/04sep12/noise_autocorr_AB_36038.root", Int_t t_offset = -10)
{

  
  TH1F * derivative = new TH1F("derivative","derivative",161,-1.05,15.05);


  for (int i = 1; i<162;i++){
    derivative->SetBinContent(i,((shape->GetBinContent(i+1)-shape->GetBinContent(i-1))/0.2));
    derivative->SetBinError(i,(sqrt(shape->GetBinError(i+1)*shape->GetBinError(i+1)+shape->GetBinError(i-1)*shape->GetBinError(i-1))/0.2));
  }
  
  // normalize the shape, such that the integral for 6 slices is one!

  float sum = 0;
  for (int i=40; i<101; i++){sum+=shape->GetBinContent(i);}
  sum /= 10;

  shape->Scale(1./sum);
  derivative->Scale(1./sum);


  TCanvas * c1 = new TCanvas("c1","c1",600,400);
  //c1= canvas();
  shape->Draw();

  TCanvas *c2 = new TCanvas("c2","c2",600,400);
  //c2=canvas();
  derivative->Draw();


  // book the histograms for the weights

  TH1F * hw_amp = new TH1F("hw_amp","hw_amp",no_samples*10,-0.5,no_samples-0.5);
  TH1F * hw_time = new TH1F("hw_time","hw_time",no_samples*10,-0.5,no_samples-0.5);
  TH1F * hshape = new TH1F("hshape","hshape",no_samples*10,-0.5,no_samples-0.5);
  TH1F * hderivative = new TH1F("hderivative","hderivative",no_samples*10,-0.5,no_samples-0.5);


  // read in the noise auto-correlation function:

  TMatrix B(no_samples,no_samples);

  f = new TFile(noise_file);
  TH2F * noise_corr = (TH2F*)c_corr->FindObject("hcorr");
  for (int i=0; i<no_samples; i++){
    for (int j=0; j<no_samples; j++){
      B[i][j]=noise_corr->GetBinContent(i+1,j+1);
    }
  }  
  f->Close();

  B.Invert();

  // now the loop over t_{rel} in [-0.4;0.6[ :

  for (int i=-4; i<6; i++){
    
  
    TMatrix g(no_samples,1);
    TMatrix gT(1,no_samples);
    TMatrix d(no_samples,1);
    TMatrix dT(1,no_samples);
    
    
    for (int count=0; count < no_samples; count++){
      
      g[count][0]=shape->GetBinContent(55+t_offset-int((10*no_samples-50)/2.)+10*count+i); 
      gT[0][count]=shape->GetBinContent(55+t_offset-int((10*no_samples-50)/2.)+10*count+i);
      d[count][0]=derivative->GetBinContent(55+t_offset-int((10*no_samples-50)/2.)+10*count+i);
      dT[0][count]=derivative->GetBinContent(55+t_offset-int((10*no_samples-50)/2.)+10*count+i);
      
      hshape->SetBinContent(i+5+10*count,shape->GetBinContent(55+t_offset-int((10*no_samples-50)/2.)+10*count+i));
      hderivative->SetBinContent(i+5+10*count,derivative->GetBinContent(55+t_offset-int((10*no_samples-50)/2.)+10*count+i));
    }
    
    
    TMatrix m_denom = (gT*(B*g))*(dT*(B*d)) - (dT*(B*g))*(dT*(B*g));
    float denom = m_denom[0][0];  // m_denom is a real number
    
    TMatrix m_first = dT*(B*d);     // m_first is a real number
    float first = m_first[0][0]/denom;
    
    TMatrix m_last = gT*(B*d);      // m_last is a real number 
    float last = m_last[0][0]/denom;
    
    TMatrix m1 = gT*B;
  
    m1 *=first;
    
    TMatrix m2 = dT*B; 
    
    m2 *=last;
    
    TMatrix w_amp= m1 - m2;
    
    
    TMatrix m_first1 = gT*(B*g);
    float first1 = m_first1[0][0]/denom;
    
    TMatrix m_last1 = gT*(B*d);
    float last1 = m_last1[0][0]/denom;
    
    TMatrix m11 = dT*B; 
   
    m11 *=first1;
    
    TMatrix m21 = gT*B;
    
    m21 *=last1;
    
    
    TMatrix w_time= m11 - m21; 
    
    
    float amp = 0;
    float amp_time = 0;

    for (int count=0; count < no_samples; count++){
      hw_amp->SetBinContent(i+5+10*count,w_amp[0][count]);
      hw_time->SetBinContent(i+5+10*count,w_time[0][count]);
    }
   

    
    TMatrix m_delta_E = dT*(B*d);      // m_last is a real number 
    float delta_E = m_delta_E[0][0]/denom;


    TMatrix m_delta_Et = gT*(B*g);      // m_last is a real number 
    float delta_Et = m_delta_Et[0][0]/denom;

    cout << " i " << i << " delta E " << sqrt(delta_E) << " delta Et " << sqrt(delta_Et) << endl;


  } // end loop over t_rel


  TCanvas * c3 = new TCanvas("c3","c3",600,400);
  hw_amp->Draw();

  TCanvas * c4 = new TCanvas("c4","c4",600,400);
  hw_time->Draw();

  TCanvas * c5 = new TCanvas("c5","c5",600,400);
  hshape->Draw();

  TCanvas * c6 = new TCanvas("c6","c6",600,400);
  hderivative->Draw();

  /*
  f_out = new TFile("/home/pcmagic17/hbartko/mars/results/04sep15/calibration_weights_corr_iterated.root","RECREATE");  //"../of_weights_bin4.root"
  hw_amp->Write();
  hw_time->Write();
  hshape->Write();
  hderivative->Write();
  shape->Write();
  derivative->Write();
  f_out->Close();
  */
}
