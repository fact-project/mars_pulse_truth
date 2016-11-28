#ifndef MARS_MDrsCalibration
#define MARS_MDrsCalibration

// -------------------------------------------------------------------

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef ROOT_TSystem
#include <TSystem.h>
#endif

#ifndef MARS_MLogManip
#include "MLogManip.h"
#endif

#ifndef MARS_DrsCalib
#include "DrsCalib.h"
#endif


class MDrsCalibration : public MParContainer, public DrsCalibration
{
public:
    MDrsCalibration(const char *name=0, const char *title=0)
    {
        fName  = name  ? name  : "MDrsCalibration";
        fTitle = title ? title : "";
    }

    bool ReadFits(TString fname)
    {
        gSystem->ExpandPathName(fname);

        std::string msg;
        try
        {
            msg = ReadFitsImp(fname.Data());
        }
        catch (const std::exception &e)
        {
            msg = e.what();
        }

        if (msg.empty())
        {
            *fLog << inf << "Read DRS calibration file " << fname << std::endl;
            return true;
        }

        *fLog << err << "Error reading from " << fname << ": " << msg << std::endl;
        return false;
    }

    void Plot();

    ClassDef(MDrsCalibration, 1)
};

#endif
