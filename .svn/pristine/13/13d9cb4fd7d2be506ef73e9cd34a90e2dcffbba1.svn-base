#ifndef MARS_MFFT
#define MARS_MFFT

#ifndef MARS_MAGIC
#include "MAGIC.h"
#endif

#ifndef ROOT_TArrayF
#include "TArrayF.h"
#endif

#ifndef ROOT_TArrayD
#include "TArrayD.h"
#endif

#ifndef ROOT_TH1F
#include "TH1F.h"
#endif

#ifndef ROOT_TH1D
#include "TH1D.h"
#endif

class MArrayI;
class MArrayD;
class MArrayF;
class MFFT : public TObject
{
private:

  void Swap(Float_t &a,  Float_t &b)    { Float_t  c = a;  a = b;  b = c;  }
  void Swap(Double_t &a, Double_t &b)   { Double_t c = a;  a = b;  b = c;  }

  void TransformF(const Int_t isign, TArrayF &data);
  void TransformD(const Int_t isign, TArrayD &data);  
  void RealFTF(const Int_t isign);
  void RealFTD(const Int_t isign);

  void CheckDim(Int_t a);
  TH1 *CheckHist(const TH1 *hist, const Int_t flag);

  Float_t ApplyWindow(const Int_t j, const Float_t a, const Float_t b) const 
    {

      return 1.0-TMath::Abs((j-a)*b);      // Bartlett
      // return 1.0;                        // Square
      // return 1.0-(((j-a)*b)*((j-a)*b));   // Welch
      
    }
  
  Int_t   fDim;
  TArrayF fDataF;
  TArrayD fDataD;  
  TArrayF fWindowF;
  TArrayD fWindowD;  

public:

  MFFT();
  ~MFFT();

  TArrayF*  RealFunctionFFT( const TArrayF *data);
  TArrayF*  RealFunctionIFFT(const TArrayF *data);  
  
  TArrayD*  RealFunctionFFT( const TArrayD *data);
  TArrayD*  RealFunctionIFFT(const TArrayD *data);  
  
  Float_t*  RealFunctionFFT( const Int_t n, const Float_t *data);
  Float_t*  RealFunctionIFFT(const Int_t n, const Float_t *data);  
  
  Double_t* RealFunctionFFT( const Int_t n, const Double_t *data);
  Double_t* RealFunctionIFFT(const Int_t n, const Double_t *data);  
  
  TH1F* PowerSpectrumDensity(const TH1 *hist);
  TH1F* PowerSpectrumDensity(const TH1F *hist);
  TH1F* PowerSpectrumDensity(const TH1I *hist);  
  TH1D* PowerSpectrumDensity(const TH1D *hist);

  TArrayF* PowerSpectrumDensity(const TArrayI *array);  
  TArrayF* PowerSpectrumDensity(const TArrayF *array);
  TArrayD* PowerSpectrumDensity(const TArrayD *array);

  MArrayF* PowerSpectrumDensity(const MArrayI *array);  
  MArrayF* PowerSpectrumDensity(const MArrayF *array);
  MArrayD* PowerSpectrumDensity(const MArrayD *array);

  TArrayF*  RealFunctionSpectrum(const TArrayF *data);
  
  ClassDef(MFFT,0)  // Class to perform a Fast Fourier Transform
};
    
#endif
