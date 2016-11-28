#include "DrsCalib.h"


using namespace std;

// --------------------------------------------------------------------------

int fake_drs_calibration()
{
    DrsCalibration drscalib;
    uint16_t fRoi = 300;
    const string filename = "/fhgfs/groups/app/fact/mc_test/testingdrsfile/12bitADC/offsetMinus1850/test300samples.drs.fits";
    uint16_t fNumTm = 0;
    float baselineMean = -1850.0 * 2000.0/4096.0;
    float baselineRms = 0;
    float gainMean = 1907.35;
    float gainRms = 0;
    float triggerOffsetMean = 0;
    float triggerOffsetRms = 0;
    float triggerOffsetTmMean = 0;
    float triggerOffsetTmRms = 0;

    const size_t n = 1440*1024*4 + 1440*fRoi*2 + fNumTm*fRoi*2 + 3;
    vector<float> vec;
    vec.resize(n);

    reinterpret_cast<uint32_t*>(vec.data())[0] = fRoi;
    reinterpret_cast<uint32_t*>(vec.data())[1] = 1;
    reinterpret_cast<uint32_t*>(vec.data())[2] = 2;
    reinterpret_cast<uint32_t*>(vec.data())[3] = 3;

    for (int i = 0 ; i < 1024*1440 ; i++)
    {
        vec.data()[i+4]             = baselineMean;
        vec.data()[i+4+1*1024*1440] = baselineRms;
        vec.data()[i+4+2*1024*1440] = gainMean;
        vec.data()[i+4+3*1024*1440] = gainRms;
    }
    for (int i = 0 ; i < fRoi*1440 ; i++)
    {
        vec.data()[i+4+4*1024*1440]             = triggerOffsetMean;
        vec.data()[i+4+4*1024*1440+1*fRoi*1440] = triggerOffsetRms;
    }
    for (int i = 0 ; i < fRoi*fNumTm ; i++)
    {
        vec.data()[i+4+2*1024*1440+2*fRoi*1440]             = triggerOffsetTmMean;
        vec.data()[i+4+3*1024*1440+2*fRoi*1440+fRoi*fNumTm] = triggerOffsetTmRms;
    }

    drscalib.fRoi   = fRoi;
    drscalib.fNumTm = fNumTm;
    drscalib.fStep  = 2;

    drscalib.fNumOffset = 1000;
    drscalib.fNumGain   = 1000*1953125;
    drscalib.fNumTrgOff = 1000;

    drscalib.WriteFitsImp(filename,vec);

    return 0;
}

