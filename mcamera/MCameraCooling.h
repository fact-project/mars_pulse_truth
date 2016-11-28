#ifndef MARS_MCameraCooling
#define MARS_MCameraCooling

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

class MCameraCooling : public MParContainer
{
    friend class MReportCamera;
private:
    Byte_t  fStatus;              // CaCo Monitored cooling status: 0-9, Cam.COOLING_state

    Bool_t  fStatusPressureHi;    // 0=ok,  1=Obstruct
    Bool_t  fStatusPressureLo;    // 0=ok,  1=leakage
    Bool_t  fStatusPump;          // 0=off, 1=on
    Bool_t  fStatusRefrigrerator; // 0=off, 1=on
    Bool_t  fStatusValve;         // 0=recirculate, 1=new
    Bool_t  fStatusResistor;      // 0=off, 1=on
    Bool_t  fStatusFans;          // 0=off, 1=on

    Float_t fTempCenter;          // [deg C] Temperature at the camera center
    Float_t fTempWall;            // [deg C] Temperature at the camera wall
    Float_t fTempOptLink;         // [deg C] Temperature at the optical link
    Float_t fTempWater;           // [deg C] Temperature of the water in the water tank

    Byte_t  fHumWall;             // [%] Relative humidity at camera wall
    Byte_t  fHumCenter;           // [%] Relative humidity camera center

public:
    MCameraCooling()
    {
        fName  = "MCameraCooling";
        fTitle = "Container storing information about the Camera cooling system";
    }

    Byte_t  GetStatus() const              { return fStatus;              }

    Bool_t  GetStatusPressureHi() const    { return fStatusPressureHi;    }
    Bool_t  GetStatusPressureLo() const    { return fStatusPressureLo;    }
    Bool_t  GetStatusPump() const          { return fStatusPump;          }
    Bool_t  GetStatusRefrigrerator() const { return fStatusRefrigrerator; }
    Bool_t  GetStatusValve() const         { return fStatusValve;         }
    Bool_t  GetStatusResistor() const      { return fStatusResistor;      }
    Bool_t  GetStatusFans() const          { return fStatusFans;          }

    Float_t GetTempCenter() const          { return fTempCenter;          }
    Float_t GetTempWall() const            { return fTempWall;            }
    Float_t GetTempOptLink() const         { return fTempOptLink;         }
    Float_t GetTempWater() const           { return fTempWater;           }

    Byte_t  GetHumWall() const             { return fHumWall;             }
    Byte_t  GetHumCenter() const           { return fHumCenter;           }

    ClassDef(MCameraCooling, 1) // Container storing information about the Camera cooling
};

#endif
