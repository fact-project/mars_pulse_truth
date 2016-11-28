#ifndef MARS_MStarCamTrans
#define MARS_MStarCamTrans

#ifndef ROOT_TROOT
#include <TROOT.h>
#endif

#ifndef MARS_MAGIC
#include "MAGIC.h"
#endif

class MGeomCam;
class MObservatory;

class MStarCamTrans
{
private:
  Double_t  fDistCam;   // distance camera reflector [mm]
  Double_t  fCLat;      // cos(Lat)
  Double_t  fSLat;      // sin(Lat)

  Double_t fGridBinning;   // degrees
  Double_t fGridFineBin;   // degrees

public:
  MStarCamTrans(const MGeomCam &cam, const MObservatory &obs);

  Bool_t Loc0CamToLoc(Double_t theta0deg, Double_t phi0deg, 
                      Double_t X,         Double_t Y,
                      Double_t &thetadeg, Double_t &phideg); 

  Bool_t Loc0LocToCam(Double_t theta0deg, Double_t phi0deg,
                      Double_t thetadeg,  Double_t phideg,
                      Double_t &X,        Double_t &Y);

  Bool_t LocCamToLoc0(Double_t thetadeg,   Double_t phideg, 
                      Double_t X,          Double_t Y,
                      Double_t &theta0deg, Double_t &phi0deg); 

  Bool_t Cel0CamToCel(Double_t dec0deg, Double_t h0hour, 
                      Double_t X,       Double_t Y,
                      Double_t &decdeg, Double_t &hhour); 

  Bool_t Cel0CelToCam(Double_t dec0deg, Double_t h0hour,
                      Double_t decdeg,  Double_t hhour,
                      Double_t &X,      Double_t &Y);

  Bool_t CelCamToCel0(Double_t decdeg,   Double_t hhour, 
                      Double_t X,        Double_t Y,
                      Double_t &dec0deg, Double_t &h0hour); 

  Bool_t LocCamCamToLoc(Double_t theta1deg,  Double_t phi1deg, 
                        Double_t X1,         Double_t Y1,
                        Double_t X2,         Double_t Y2,
                        Double_t &theta2deg, Double_t &phi2deg); 

  Bool_t LocCamLocToCam(Double_t theta1deg, Double_t phi1deg,
                        Double_t X1,        Double_t Y1,
                        Double_t theta2deg, Double_t phi2deg,
                        Double_t &X2,       Double_t &Y2);

  Bool_t CelCamCamToCel(Double_t dec1deg,  Double_t h1hour, 
                        Double_t X1,       Double_t Y1,
                        Double_t X2,       Double_t Y2,
                        Double_t &dec2deg, Double_t &h2hour); 

  Bool_t CelCamCelToCam(Double_t dec1deg, Double_t h1hour, 
                        Double_t X1,      Double_t Y1,
                        Double_t dec2deg, Double_t h2hour,
                        Double_t &X2,     Double_t &Y2);

  Bool_t CelToLoc(Double_t decdeg,    Double_t hhour, 
                  Double_t &thetadeg, Double_t &phideg); 

  Bool_t LocToCel(Double_t thetadeg, Double_t phideg, 
                  Double_t &decdeg,  Double_t &hhour);

  Bool_t PlotGridAtDec0H0(TString name, 
                          Double_t dec0deg,   Double_t h0hour);
  Bool_t PlotGridAtTheta0Phi0(TString name,
                              Double_t theta0deg, Double_t phi0deg);

  Bool_t SetGridParameters(Double_t gridbinning,  Double_t gridfinebin);

  Bool_t PlotGrid(TString name,
                  Double_t theta0deg, Double_t phi0deg,
	          Double_t dec0deg,   Double_t h0hour);

  ClassDef(MStarCamTrans, 0) // Class for transformations between Celestial, Local and Camera coordinates 
};

#endif











