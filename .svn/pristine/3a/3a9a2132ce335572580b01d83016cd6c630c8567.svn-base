/* ======================================================================== *\
!
! *
! * This file is part of MARS, the MAGIC Analysis and Reconstruction
! * Software. It is distributed to you in the hope that it can be a useful
! * and timesaving tool in analysing Data of imaging Cerenkov telescopes.
! * It is distributed WITHOUT ANY WARRANTY.
! *
! * Permission to use, copy, modify and distribute this software and its
! * documentation for any purpose is hereby granted without fee,
! * provided that the above copyright notice appear in all copies and
! * that both that copyright notice and this permission notice appear
! * in supporting documentation. It is provided "as is" without express
! * or implied warranty.
! *
!
!
!   Author(s): Thomas Bretz, 07/2005 <mailto:tbretz@astro.uni-wuerzburg.de>
!
!   Copyright: MAGIC Software Development, 2000-2007
!
!
\* ======================================================================== */

/////////////////////////////////////////////////////////////////////////////
//
// MPointingDevCalc
//
// Calculates the pointing deviation from the starguider information.
//
// There are some quality parameters to steer which are the valid
// starguider data points:
//
//  * MPointingDevCalc.NumMinStars: 8
//    Minimum number of identified stars required to accep the data
//
//  * MPointingDevCalc.NsbLevel:    3.0
//    Minimum deviation (in rms) from the the mean allowed for the
//    measured NSB (noise in the ccd camera)
//
//  * MPointingDevCalc.NsbMin:      30
//    - minimum NSB to be used in mean/rms calculation
//
//  * MPointingDevCalc.NsbMax:      60
//    - maximum NSB to be used in mean/rms calculation
//
//  * MPointingDevCalc.MaxAbsDev:   15
//    - Maximum absolute deviation which is consideres as valid (arcmin)
//
// Starguider data which doens't fullfill this requirements is ignored.
// If the measures NSB==0 (file too old, starguider didn't write down
// these values) the checks based on NSB and NumMinStar are skipped.
//
// The calculation of NSB mean and rms is reset for each file (ReInit)
//
// If your starguider data doesn't fullfill this requirement the latest
// value which could be correctly calculated is used instead. If the time
// for which no valid value can be calculated exceeds one minute
// the return value is reset to 0/0. The maximum time allowed without
// a valid value can be setup using:
//
//  * MPointingDevCalc.MaxAge:   1
//    Maximum time before the starguider is reset to 0/0 in minutes
//
// Note, that the starguider itself is not well calibrated. Therefore
// it is necessary to do a starguider calibration in our software.
//
// There are two options:
//
//  * Simple starguider calibration using offsets in the camera plane
//
//    The starguider is calibrated by taking its values (dZd/dAz)
//    adding them to the source position, calculating the source position
//    in the camera plane and adding the offsets. To switch off the
//    full starguider calibration do:
//
//      * MPointingDevCalc.PointingModels:
//
//    To set the offsets (in units of degree) use
//
//      * MPointingDevCalc.Dx: -0.001
//      * MPointingDevCalc.Dy: -0.004
//
//   * A starguider calibration using a pointing model calculated
//     from calibration data, so called TPoints
//
//     Because the pointing model can change from time to time
//     you can give the run-number from which on a new pointing
//     model is valid. The run itself is included, e.g.:
//
//      * MPointingDevCalc.PointingModels: 85240 89180
//      * MPointingDevCalc.FilePrefix:     resources/starguider
//
//     mean that for all runs<85240 the simple offset correction is used.
//     For runs >=85240 and <89180 the file resources/starguider0085240.txt
//     and for runs >=89180 the file resources/starguider0089180.txt is
//     used. To setup a default file for all runs before 85240 setup
//     a low number (eg. 0 or 1)
//
//     In the case a pointing model is used additional offsets in
//     the x/y-camera plane (in units of deg) can be set using the DX
//     and DY parameters of the pointing model. The fDx and fDy data
//     members of this class are ignored. To overwrite the starguider
//     calibrated offset in either Az or Zd with a constant, you
//     can use the PX/PY directive in the pointing model. (To enable
//     the overwrite set the third column, the error, to a value
//     greater than zero)
//
//
// At the PostProcessing step a table with statistics is print if the
// debug level is greater or equal 3 (in most applications it is switched
// on by -v3)
//
//
// Pointing Models:
// ----------------
//
//  What we know so far about (maybe) important changes in cosy or to the
//  hardware:
//
//   25.01.2010: Changed scaling factors for TPoint camera (M1)
//
//   11.01.2010: New TPoint camera (M1)
//
//   18.03.2006: The camera holding has been repaired and the camera got
//               shifted a little bit.
//
//   16.04.2006: Around this date a roque lamp focussing has been done
//
//   25.04.2006: A missalignment introduced with the roque adjust has been
//               corrected
//
//   The starguider pointing model for the time before 18.3.2006 and after
//   April 2006 (in fact there are no TPoints until 07/2006 to check it)
//   and for the period 07/2006 to (at least) 06/2007 are very similar.
//
//   The pointing model for the time between 18.3.2006 and 04/2006 is
//   clearly different, mainly giving different Azimuth values between
//   Zenith and roughly ~25deg, and a slight offset on both axes.
//
//   10.5.2006:  pos1 -= pos0 commented  (What was the mentioned fix?)
//   29.6.2006:  repaired
//
//   23.3.2006:  new starguider algorithm
//
//   17.3.2005:  Fixed units of "nompos" in MDriveCom
//
//
// New pointing models have been installed (if the pointing model
// is different, than the previous one it might mean, that also
// the starguider calibration is different.) The date only means
// day-time (noon) at which the model has been changed.
//
//   29. Apr. 2004    ~25800
//    5. Aug. 2004    ~32000
//   19. Aug. 2004    ~33530
//    7. Jun. 2005    ~57650
//    8. Jun. 2005
//    9. Jun. 2005    ~57860
//   12. Sep. 2005    ~68338
//   24. Nov. 2005    ~75562
//   17. Oct. 2006   ~103130
//   17. Jun. 2007   ~248193
//   18. Oct. 2007    291104      // Correction for the offsets introduced by AMC
//   14. Jan. 2008    328198      // Complete new pointing model
//   11. Jun. 2008   1000534    (ca. 23:00) // Before new TPoints
//   19. Jun. 2008              (ca. 15:00) // From   new TPoints
//    7. Mar. 2009              (ca. 14:00) // From   new TPoints (0808-0902)
//   14. May  2009                // M1/M2 after upgrade (from TPoints taken in the days before)
//   17. Aug. 2009              New pointing models for both telescopes
//   01. Feb. 2010              New pointing models for both telescopes
//   26. Feb. 2010              New pointing models for both telescopes
//   31. Mar. 2010              New pointing models for both telescopes
//   29. Sep. 2010              New pointing model MAGIC I
//   01. Dec. 2010              New pointing models for both telescopes
//   06. Jan. 2011              New pointing models for both telescopes
//
// From 2.2.2006 beginnig of the night (21:05h, run >=81855) to 24.2.2006
// beginning of the the night (20:34h, run<83722) the LEDs did not work.
// In the time after this incident the shift crew often forgot to switch on
// the LEDs at the beginning of the night!
//
// [2006-03-07 00:10:58] In the daytime, we raised the position of the
// 9 o'clock LED by one screw hole to make it visible when the TPoint
// Lid is closed. (< run 84980)
//
// Mirror refocussing around 23.Apr.2006, from the Runbook.
//
// 25./26.4.2006: Run 89180
//
// (Note: The year here is not a typo!)
// [2007-04-25 23:50:39]
// Markus is performing AMC focussing.
//
// Mirror refocussing around 4.Aug.2007, from the Runbook:
//
// [2007-08-04 04:46:47]
// We finished with the focussing with Polaris. The images need to be
// analysed and new LUTs generated.
//
// [2007-08-04 23:47:30]
// Actually we see that the mispointing is always large; probably since
// the LUT tables have not yet been adjusted to the new focussing.
//
// [2007-08-03 23:07:58]
// Data taking stopped. Mirror focussing.
//
// [2007-08-05 00:09:16]
// We take some pictures on stars nearby Cyg X3 with the sbig camera;
// actually the spot doesn't look very nice... The pictures have been
// saved with name Deneb- and Sadr- Polaris seems a bit better. Should we
// have new LUT tables after the focussing?
//
// [2007-08-10 20:18:48]
// Tonight we take first images of Polaris with a new LUT file generated
// based on the recent focussing. The image will be analysed tomorrow and
// than new LUTs will be generated. For tonight the focussing is still not
// changed.
//
// [2007-08-14 20:57:59]
// The weather is fine. There is a group of hobby astronomers at the
// helicopter parking. Before data taking, we tried to check the new LUTs.
// However, because of technical problems with the new LUTs we had to
// postpone the measurements. We lost some 10 min of data taking because
// of this.
//
// [2007-08-14 22:29:37]  Run 267253
// Before continuing the observation we perform a focussing test with the
// new LUTs from recent Polaris focussing. Note: Data on Her X-1 was taken
// with old focussing. We performed PSF measurements on Kornephorus (Zd
// 31.77, Az 264,67) first with old LUTs and then with new LUTs. We took
// T-points with both focussing. The first T-Point corresponds to the
// previous focussing and the second with the improved. Please check both
// T-points for eventual misspointing. We found big improvement of the PSF
// with the new LUTs and will therefore continue from now on with the new
// focussing. Having a first look at the SBIG pictures we see a slightly
// misspointing of ~0.1 deg with the new LUTs.
//
// [2007-08-14 22:46:10]
// Comparing the trigger rate with yesterday night we do not see an
// improvement with the new focussing.
//
// [2007-10-27 email]
// [...]
// When removing the cover of the motor all 4 nuts which fix the gear of
// the motor to the structure fell out. The motor was completely loose.
// [...]
// We checked also the second azimuth motor and .. the same situation.
// [...]
// Peter Sawallisch opened and fixed all 3 motors of MAGIC-I. We checked
// all critical screws that came to our minds and fixed them as much as
// possible with loctite and counter nuts. No damage whatsoever has been
// found.
//
// [2009-08-05 Mail from Markus Garcz. about M1]
// [...]
// On the 23.07.2009 we tested the new LUTs using the star Etamin. The
// PSF (sigma) improved from 11.0 mm to 9.5 mm and the total light content
// in one PMT from 54% to 62%.
// Furthermore I saw a small movement of the spot after the new focussing:
//   dX = 3 CCD pixel = 7 mm
//   dY = 2 CCD pixel = 4.7 mm
// The new LUTs are now installed since the 23.07.2009 and are used as
// default during the observation.
// [...]
//
//
// [2009-10-16 Email Adrian]
//
// I checked the AMC2 log files to check which LUTs had been used in which nights
//   until    09/09/12  the old version from March had always been used
//   09/09/13-09/09/16  LUT_090913 (test) was used (by mistake)
//   09/09/17-09/09/20  switched back to the version from March
//   since    09/09/21  using actual version LUT_090918
//                      (PSF at ~10deg still rather poor, but I see strange
//                      effects I do not yet understand and therefore cannot
//                      correct)
//  Especially: nothing has changed in the AMC on 09/09/09,
//  except that around that date the SBIG camera was readjusted (but this
//  has no direct affect on the AMC)
//
//
// [2010-02-03 M. Garcz.]
//
// New LUTs for M2
//
// [2010-06-14 M. Garcz.]
//
// New LUTs for M1
//
// [2010-07-02 M. Garcz.]
//
// New LUTs for M1
//
// [2010-10-24]
//
// Since about this date we see a higher spread in the TPoints. The reason we
// found is that SA sends the coordinates of the orginal position and not the
// target position to the AMC thus the mirrors are not well focussed.
// This should mainly effect the quality of the TPoints but not the pointing
// model. However, M2 shows a slight offset.
//
// [2010-11-11 M. Garcz.]
//
// Last night, TPoints for M2 taken without target
//
// [2010-11-18 M. Garcz.]
//
// New LUTs for M1
//
//
// Others
// ------
//
// The pointing of both(!) telescopes changed >2009/06/11 2:36h
// In both cases the reason is unknown.
//
// The pointing has changed for M2 after    2009/09/09 noon
// It recovered to the previous pointing at 2009/10/08 noon
// Resons unknown (there were LUT changes but at different dates)
//
//
// ToDo:
// -----
//
//   * Is 0/0 the best assumption if the starguider partly fails?
//
//
// Input Container:
//   MRawRunHeader
//   MReportStarguider
//
// Output Container:
//   MPointingDev
//
/////////////////////////////////////////////////////////////////////////////
#include "MPointingDevCalc.h"

#include <stdlib.h> // atoi (Ubuntu 8.10)

#include "MLog.h"
#include "MLogManip.h"

#include "MParList.h"

#include "MAstro.h"
#include "MPointing.h"
#include "MPointingDev.h"
#include "MRawRunHeader.h"
#include "MReportStarguider.h"

ClassImp(MPointingDevCalc);

using namespace std;

const TString MPointingDevCalc::fgFilePrefix="resources/starguider";

// --------------------------------------------------------------------------
//
// Destructor. Call Clear() and delete fPointingModels if any.
//
MPointingDevCalc::~MPointingDevCalc()
{
    Clear();
}

// --------------------------------------------------------------------------
//
// Delete fPointing and set pointing to NULL
//
void MPointingDevCalc::Clear(Option_t *o)
{
    if (fPointing)
        delete fPointing;

    fPointing = NULL;
}

// --------------------------------------------------------------------------
//
// Sort the entries in fPoinitngModels
//
void MPointingDevCalc::SortPointingModels()
{
    const int n = fPointingModels.GetSize();

    TArrayI idx(n);

    TMath::Sort(n, fPointingModels.GetArray(), idx.GetArray(), kFALSE);

    const TArrayI arr(fPointingModels);

    for (int i=0; i<n; i++)
        fPointingModels[i] = arr[idx[i]];
}

// --------------------------------------------------------------------------
//
// Set new pointing models
//
void MPointingDevCalc::SetPointingModels(const TString &models)
{
    fPointingModels.Set(0);

    if (models.IsNull())
        return;

    TObjArray *arr = models.Tokenize(" ");

    const int n = arr->GetEntries();
    fPointingModels.Set(n);

    for (int i=0; i<n; i++)
        fPointingModels[i] = atoi((*arr)[i]->GetName());

    delete arr;

    SortPointingModels();
}

// --------------------------------------------------------------------------
//
// Return a string with the pointing models, seperated by a space.
//
TString MPointingDevCalc::GetPointingModels() const
{
    TString rc;
    for (int i=0; i<fPointingModels.GetSize(); i++)
        rc += Form ("%d ", fPointingModels[i]);

    return rc;
}

// --------------------------------------------------------------------------
//
// Add a number to the pointing models
//
void MPointingDevCalc::AddPointingModel(UInt_t runnum)
{
    const int n = fPointingModels.GetSize();
    for (int i=0; i<n; i++)
        if ((UInt_t)fPointingModels[i]==runnum)
        {
            *fLog << warn << "WARNING - Pointing model " << runnum << " already in list... ignored." << endl;
            return;
        }

    fPointingModels.Set(n+1);
    fPointingModels[n] = runnum;

    SortPointingModels();
}

// --------------------------------------------------------------------------
//
// Find the highest number in the array which is lower or equal num.
//
UInt_t MPointingDevCalc::FindPointingModel(UInt_t num)
{
    const int n = fPointingModels.GetSize();
    if (n==0)
        return (UInt_t)-1;

    // Loop over all pointing models
    for (int i=0; i<n; i++)
    {
        // The number stored in the array did not yet overtake the runnumber
        if ((UInt_t)fPointingModels[i]<num)
            continue;

        // The first pointing model is later than this run: use a default
        if (i==0)
            return (UInt_t)-1;

        // The last entry in the array is the right one.
        return fPointingModels[i-1];
    }

    // Runnumber is after last entry of pointing models. Use the last one.
    return fPointingModels[n-1];
}

// --------------------------------------------------------------------------
//
// Clear the pointing model. If run-number >= 87751 read the new
// pointing model with fFilePrefix
//
Bool_t MPointingDevCalc::ReadPointingModel(const MRawRunHeader &run)
{
    const UInt_t num = FindPointingModel(run.GetRunNumber());

    // No poinitng models are defined. Use simple dx/dy-calibration
    if (num==(UInt_t)-1)
    {
        Clear();
        return kTRUE;
    }

    // compile the name for the starguider files
    // The file with the number 00000000 is the default file
    TString fname = Form("%s%08d.txt", fFilePrefix.Data(), num);

    if (!fPointing)
        fPointing = new MPointing;

    if (fname==fPointing->GetName())
    {
        *fLog << inf << fname << " already loaded." << endl;
        return kTRUE;
    }

    return fPointing->Load(fname);
}

// --------------------------------------------------------------------------
//
// Check the file/run type from the run-header and if it is a data file
// load starguider calibration.
//
Bool_t MPointingDevCalc::ReInit(MParList *plist)
{
    MRawRunHeader *run = (MRawRunHeader*)plist->FindObject("MRawRunHeader");
    if (!run)
    {
        *fLog << err << "MRawRunHeader not found... aborting." << endl;
        return kFALSE;
    }

    fNsbSum   =  0;
    fNsbSq    =  0;
    fNsbCount =  0;

    fRunType = run->GetRunType();

    switch (fRunType)
    {
    case MRawRunHeader::kRTData:
        if (!fReport)
            *fLog << warn << "MReportStarguider not found... skipped." << endl;
        return ReadPointingModel(*run);

    case MRawRunHeader::kRTMonteCarlo:
        return kTRUE;

    case MRawRunHeader::kRTPedestal:
        *fLog << err << "Cannot work in a pedestal Run!... aborting." << endl;
        return kFALSE;

    case MRawRunHeader::kRTCalibration:
        *fLog << err << "Cannot work in a calibration Run!... aborting." << endl;
        return kFALSE;

    default:
        *fLog << err << "Run Type " << fRunType << " unknown!... aborting." << endl;
        return kFALSE;
    }

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//  Search for 'MPointingPos'. Create if not found.
//
Int_t MPointingDevCalc::PreProcess(MParList *plist)
{
    fDeviation = (MPointingDev*)plist->FindCreateObj("MPointingDev");
    fReport    = (MReportStarguider*)plist->FindObject("MReportStarguider");

    // We use kRTNone here as a placeholder for data runs.
    fRunType  = MRawRunHeader::kRTNone;
    fLastMjd  = -1;

    fSkip.Reset();

    // In cases in which ReInit isn't called right in time (e.g. if
    // the first starguider event comes before the first data event)
    fNsbSum   =  0;
    fNsbSq    =  0;
    fNsbCount =  0;

    return fDeviation ? kTRUE : kFALSE;
}

// --------------------------------------------------------------------------
//
// Increase fSkip[i] by one. If the data in fDeviation is outdated (older
// than fMaxAge) and the current report should be skipped reset DevZdAz and
// DevXY and fSkip[6] is increased by one.
//
void MPointingDevCalc::Skip(Int_t i)
{
    fSkip[i]++;

    const Double_t diff = (fReport->GetMjd()-fLastMjd)*1440; // [min] 1440=24*60
    if (diff<fMaxAge && fLastMjd>0)
        return;

    fDeviation->SetDevZdAz(0, 0);
    fDeviation->SetDevXY(0, 0);
    fSkip[6]++;
}

// --------------------------------------------------------------------------
//
// Do a full starguider calibration using a pointing model for the starguider.
//
void MPointingDevCalc::DoCalibration(Double_t devzd, Double_t devaz) const
{
    if (!fPointing)
    {
        // Do a simple starguider calibration using a simple offset in x and y
        fDeviation->SetDevZdAz(devzd, devaz);

        // Linear starguider calibration taken from April/May data
        // For calibration add MDriveReport::GetErrorZd/Az !
        fDeviation->SetDevXY(fDx, fDy); // 1arcmin ~ 5mm

        return;
    }

    // Get the nominal position the star is at the sky
    // Unit: deg
    ZdAz nom(fReport->GetNominalZd(), fReport->GetNominalAz());
    nom *= TMath::DegToRad();

    // Get the mispointing measured by the telescope. It is
    // calculate as follows:
    //
    // The mispointing measured by the starguider:
    //    ZdAz mis(devzd, devaz);
    //    mis *= TMath::DegToRad();

    // The pointing model is the conversion from the real pointing
    // position of the telescope into the pointing position measured
    // by the starguider.
    //
    // To keep as close to the fitted model we use the forward correction.

    // Position at which the starguider camera is pointing in real:
    //       pointing position = nominal position - dev
    //
    // The position measured as the starguider's pointing position
    ZdAz pos(nom);        // cpos = sao - dev
    pos -= ZdAz(devzd, devaz)*TMath::DegToRad();

    // Now we convert the starguider's pointing position into the
    // telescope pointing position (the pointing model converts
    // the telescope pointing position into the starguider pointing
    // position)
    ZdAz point = fPointing->CorrectBack(pos);  //FWD!!!

    // MSrcPosCalc uses the following condition to calculate the
    // source position in the camera:
    //    real pointing pos = nominal pointing pos - dev
    //      --> dev = nominal - real
    // Therefor we calculate dev as follows:
    ZdAz dev(nom);
    dev -= point;
    dev *= TMath::RadToDeg();

    /*
     // We chose the other way. It is less accurate because is is the
     // other was than the poinitng model was fittet, but it is more
     // accurate because the nominal (i.e. real) pointing position
     // is less accurately known than the position returned by the
     // starguider.
     //
     // Calculate the deviation which would be measured by the starguider
     // if applied to a perfectly pointing telescope.
     ZdAz dev = fPointing->Correct(nom);
     dev -= nom;

     // Now add these offsets and the starguider measured offsets to
     // the real pointing deviation of the telescope (note, that
     // signs here are just conventions)
     dev += ZdAz(devzd, devaz)*TMath::DegToRad(); // --> nom-mis
     dev *= TMath::RadToDeg();
     */

    // Check if the starguider pointing model requests overwriting
    // of the values with constants (e.g. 0)
    devaz = fPointing->IsPxValid() ? fPointing->GetPx() : dev.Az();
    devzd = fPointing->IsPyValid() ? fPointing->GetPy() : dev.Zd();

    fDeviation->SetDevZdAz(devzd, devaz);
    fDeviation->SetDevXY(fPointing->GetDxy());
}

Int_t MPointingDevCalc::ProcessStarguiderReport()
{
    Double_t devzd = fReport->GetDevZd(); // [arcmin]
    Double_t devaz = fReport->GetDevAz(); // [arcmin]
    if (devzd==0 && devaz==0)
    {
        Skip(1);
        return kTRUE;
    }

    if (!fReport->IsMonitoring())
    {
        Skip(2);
        return kTRUE;
    }

    devzd /= 60; // Convert from arcmin to deg
    devaz /= 60; // Convert from arcmin to deg

    const Double_t nsb = fReport->GetSkyBrightness();
    if (nsb>0)
    {
        if (nsb>fNsbMin && nsb<fNsbMax)
        {
            fNsbSum += nsb;
            fNsbSq  += nsb*nsb;
            fNsbCount++;
        }

        if (fNsbCount>0)
        {
            const Double_t sum = fNsbSum/fNsbCount;
            const Double_t sq  = fNsbSq /fNsbCount;

            const Double_t rms = fNsbLevel*TMath::Sqrt(sq - sum*sum);

            if (nsb<sum-rms || nsb>sum+rms)
            {
                Skip(3);
                return kTRUE;
            }
        }

        if (fReport->GetNumIdentifiedStars()<fNumMinStars)
        {
            Skip(4);
            return kTRUE;
        }
    }

    // >= 87751 (31.3.06 12:00)

    // Calculate absolute deviation
    const Double_t dev = MAstro::GetDevAbs(fReport->GetNominalZd(),
                                           fReport->GetNominalZd()-fReport->GetDevZd()/60,
                                           devaz);

    // Sanity check... larger deviation are strange and ignored
    if (dev*60>fMaxAbsDev)
    {
        Skip(5);
        return kTRUE;
    }

    DoCalibration(devzd, devaz);

    fSkip[0]++;
    fLastMjd = fReport->GetMjd();

    return kTRUE;
}

// --------------------------------------------------------------------------
//
//  See class description.
//
Int_t MPointingDevCalc::Process()
{
    switch (fRunType)
    {
    case MRawRunHeader::kRTNone:
    case MRawRunHeader::kRTData:
        return fReport ? ProcessStarguiderReport() : kTRUE;

    case MRawRunHeader::kRTMonteCarlo:
        fSkip[0]++;
        fDeviation->SetDevZdAz(0, 0);
        fDeviation->SetDevXY(0, 0);
        return kTRUE;
    }
    return kTRUE;
}

// --------------------------------------------------------------------------
//
// Print execution statistics
//
Int_t MPointingDevCalc::PostProcess()
{
    if (GetNumExecutions()==0)
        return kTRUE;

    *fLog << inf << endl;
    *fLog << GetDescriptor() << " execution statistics:" << endl;
    PrintSkipped(fSkip[1], "Starguider deviation not set, is exactly 0/0");
    PrintSkipped(fSkip[2], "Starguider was not monitoring (eg. LEDs off)");
    PrintSkipped(fSkip[3], Form("NSB out of %.1f sigma range", fNsbLevel));
    PrintSkipped(fSkip[4], Form("Number of identified stars < %d", fNumMinStars));
    PrintSkipped(fSkip[5], Form("Absolute deviation > %.1farcmin", fMaxAbsDev));
    PrintSkipped(fSkip[6], Form("Events set to 0 because older than %.1fmin", fMaxAge));
    *fLog << " " << (int)fSkip[0] << " (" << Form("%5.1f", 100.*fSkip[0]/GetNumExecutions()) << "%) Evts survived calculation!" << endl;
    *fLog << endl;

    return kTRUE;
}

// --------------------------------------------------------------------------
//
// MPointingDevCalc.NumMinStars: 8
// MPointingDevCalc.NsbLevel:    3.0
// MPointingDevCalc.NsbMin:      30
// MPointingDevCalc.NsbMax:      60
// MPointingDevCalc.MaxAbsDev:   15
// MPointingDevCalc.MaxAge:      1.0
// MPointingDevCalc.Dx:         -0.001
// MPointingDevCalc.Dy:         -0.004
//
// For a detailed description see the class reference.
//
Int_t MPointingDevCalc::ReadEnv(const TEnv &env, TString prefix, Bool_t print)
{
    Bool_t rc = kFALSE;
    if (IsEnvDefined(env, prefix, "NumMinStars", print))
    {
        SetNumMinStars(GetEnvValue(env, prefix, "NumMinStars", (Int_t)fNumMinStars));
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "NsbLevel", print))
    {
        SetNsbLevel(GetEnvValue(env, prefix, "NsbLevel", fNsbLevel));
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "NsbMin", print))
    {
        SetNsbMin(GetEnvValue(env, prefix, "NsbMin", fNsbMin));
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "NsbMax", print))
    {
        SetNsbMax(GetEnvValue(env, prefix, "NsbMax", fNsbMax));
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "MaxAbsDev", print))
    {
        SetMaxAbsDev(GetEnvValue(env, prefix, "MaxAbsDev", fMaxAbsDev));
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "Dx", print))
    {
        fDx = GetEnvValue(env, prefix, "Dx", fDx);
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "Dy", print))
    {
        fDy = GetEnvValue(env, prefix, "Dy", fDy);
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "MaxAge", print))
    {
        fMaxAge = GetEnvValue(env, prefix, "MaxAge", fMaxAge);
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "FilePrefix", print))
    {
        fFilePrefix = GetEnvValue(env, prefix, "FilePrefix", fFilePrefix);
        rc = kTRUE;
    }
    if (IsEnvDefined(env, prefix, "PointingModels", print))
    {
        SetPointingModels(GetEnvValue(env, prefix, "PointingModels", GetPointingModels()));
        rc = kTRUE;
    }

    return rc;
}
