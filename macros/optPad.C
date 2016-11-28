#include <TH1.h>
#include <TCanvas.h>
#include <TVector.h>
#include <TMatrix.h>
#include <iomanip.h>

void optPad()
{
  Int_t N = 10;
  Double_t eps = 1.e-10;

  TMatrix g(N, N);
  g.Zero();

  // hista is the normalized 1D histogram of sigmabar for ON data
  // histb is the normalized 1D histogram of sigmabar for OFF data
  // histc is the difference ON-OFF

  TH1D *hista = new TH1D("ON",   "ON  data before padding", N, 0., 5.);
  TH1D *histb = new TH1D("OFF",  "OFF data before padding", N, 0., 5.);
  TH1D *histc = new TH1D("ON-OFF", "ON-OFF before padding", N, 0., 5.);
  hista->SetMaximum( 5.0/(Double_t)N );
  hista->SetMinimum(-1.0/(Double_t)N );
  histb->SetMaximum( 5.0/(Double_t)N );
  histb->SetMinimum(-1.0/(Double_t)N );
  histc->SetMaximum( 5.0/(Double_t)N );
  histc->SetMinimum(-1.0/(Double_t)N );
 
  // at the beginning, histap is a copy of hista
  // at the end, it will be the 1D histogram for ON data after padding

  // at the beginning, histbp is a copy of histb
  // at the end, it will be the 1D histogram for OFF data after padding

  // at the beginning, histcp is a copy of histc
  // at the end, it should be zero

  TH1D *histap = new TH1D("ONp",   "ON  data after padding", N, 0., 5.);
  TH1D *histbp = new TH1D("OFFp",  "OFF data after padding", N, 0., 5.);
  TH1D *histcp = new TH1D("ON-OFFp", "ON-OFF after padding", N, 0., 5.);  
  histap->SetMaximum( 5.0/(Double_t)N );
  histap->SetMinimum(-1.0/(Double_t)N );
  histbp->SetMaximum( 5.0/(Double_t)N );
  histbp->SetMinimum(-1.0/(Double_t)N );
  histcp->SetMaximum( 1.0/(Double_t)N);
  histcp->SetMinimum(-1.0/(Double_t)N);

  Double_t shoulda[] = {1.0, 2.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 2.0, 1.0};
  Double_t shouldb[] = {0.0, 0.0, 1.0, 2.0, 3.0, 2.0, 1.0, 0.0, 1.0, 2.0};
  for (Int_t i=1; i<=N; i++)
  {
    hista->SetBinContent(i, shoulda[i-1]);
    histb->SetBinContent(i, shouldb[i-1]);
  }

  // normalize histograms
  Double_t suma, sumb, conta, contb;
  suma = 0.0;
  sumb = 0.0;
  for (Int_t i=1; i<=N; i++)
  {
    conta = hista->GetBinContent(i);
    contb = histb->GetBinContent(i);
    suma += conta;
    sumb += contb;    
  }

  for (Int_t i=1; i<=N; i++)
  {
    conta = hista->GetBinContent(i);
    contb = histb->GetBinContent(i);

    hista->SetBinContent(i,conta/suma);
    histb->SetBinContent(i,contb/sumb);
    histc->SetBinContent(i, conta/suma - contb/sumb);

    histap->SetBinContent(i,conta/suma);
    histbp->SetBinContent(i,contb/sumb);
    histcp->SetBinContent(i, conta/suma - contb/sumb);
  }


  // g[k-1][j-1] (if <0.0) tells how many ON events in bin k should be padded
  //              from sigma_k to sigma_j

  // g[k-1][j-1] (if >0.0) tells how many OFF events in bin k should be padded
  //              from sigma_k to sigma_j


  //--------   start j loop   ------------------------------------------------
  // loop over bins in histc, starting from the end
  Double_t v, s, w, t, x, u, a, b, arg;

  for (Int_t j=N; j >= 1; j--)
  {
    v = histcp->GetBinContent(j);
    if ( fabs(v) < eps ) continue;
    if (v >= 0.0) 
      s = 1.0;
    else
      s = -1.0;

    //................   start k loop   ......................................
    // look for a bin k which may compensate the content of bin j
    for (Int_t k=j-1; k >= 1; k--)
    {
      w = histcp->GetBinContent(k);
      if ( fabs(w) < eps ) continue;
      if (w >= 0.0) 
        t = 1.0;
      else
        t = -1.0;

      if (s==t) continue;

      x = v + w;
      if (x >= 0.0) 
        u = 1.0;
      else
        u = -1.0;

      if (u == s)
      {
        arg = -w;
        g(k-1, j-1)   = arg;
        cout << "k-1, j-1, arg = " << k-1 << ",  " << j-1 << ",  " 
             << arg << endl;


        //......................................
        // this is for checking the procedure
        if (arg < 0.0)
        {
          a = histap->GetBinContent(k);
          histap->SetBinContent(k, a+arg);
          a = histap->GetBinContent(j);
          histap->SetBinContent(j, a-arg);
        }
        else
        {
          b = histbp->GetBinContent(k);
          histbp->SetBinContent(k, b-arg);
          b = histbp->GetBinContent(j);
          histbp->SetBinContent(j, b+arg);
        }
        //......................................

        histcp->SetBinContent(k, 0.0);
        histcp->SetBinContent(j,   x);

        //......................................
        // redefine v 
        v = histcp->GetBinContent(j);
        if ( fabs(v) < eps ) break;
        if (v >= 0.0) 
          s = 1.0;
        else
          s = -1.0;
        //......................................
       
        continue;
      }

      arg = v;
      g(k-1, j-1) = arg;
      cout << "k-1, j-1, arg = " << k-1 << ",  " << j-1 << ",  " 
           << arg << endl;

      //......................................
      // this is for checking the procedure
      if (arg < 0.0)
      {
        a = histap->GetBinContent(k);
        histap->SetBinContent(k, a+arg);
        a = histap->GetBinContent(j);
        histap->SetBinContent(j, a-arg);
      }
      else
      {
        b = histbp->GetBinContent(k);
        histbp->SetBinContent(k, b-arg);
        b = histbp->GetBinContent(j);
        histbp->SetBinContent(j, b+arg);
      }
      //......................................

      histcp->SetBinContent(k,   x);
      histcp->SetBinContent(j, 0.0);

      break;
    }
    //................   end k loop   ......................................


}
  //--------   end j loop   ------------------------------------------------
  TVector index(N);
  index.Zero();

  TVector  map(N);
  Int_t indexold = 0;

  cout << "=========================================================" << endl;
  for (Int_t k=0; k<N-1; k++)
  {
    index(k) = (Double_t)indexold;
    Int_t add = 0;
    for (Int_t j=k+1; j<N; j++)
    { 
      if (fabs(g(k,j)) > eps)
      {
          map(indexold) = g(k, j);

          cout << "k, j, indexold, map = " << k << ",  " << j << ",  "
             << indexold << ",  " << map(indexold) << endl;;

          indexold += 1;
          add += 1;
      }
    }
    if (add == 0) index(k) = 0;
    cout << endl;
    cout << "index(k), add = " << index(k) << ",  " << add << endl;
  }

  cout << "=========================================================" << endl;
  cout << " " << endl;

  //------------------------------------------------------------------------


  TCanvas *c1 = new TCanvas("c1","", 600, 900);
  c1->Divide(2,3);

  c1->cd(1);
  hista->Draw();

  c1->cd(2);
  histap->Draw();

  c1->cd(3);
  histb->Draw();

  c1->cd(4);
  histbp->Draw();

  c1->cd(5);
  histc->Draw();

  c1->cd(6);
  histcp->Draw();

}
//=========================================================================














