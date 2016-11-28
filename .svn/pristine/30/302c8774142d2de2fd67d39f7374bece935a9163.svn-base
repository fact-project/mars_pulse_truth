#ifndef MARS_MJTrainCuts
#define MARS_MJTrainCuts

#ifndef MARS_MJTrainRanForest
#include "MJTrainRanForest.h"
#endif

#ifndef ROOT_TObjArray
#include <TObjArray.h>
#endif

#ifndef MARS_MDataSet
#include "MDataSet.h"
#endif

class MH3;
class MBinning;

class MJTrainCuts : public MJTrainRanForest
{
public:
    enum Type_t { kTrainOn, kTrainOff, kTestOn, kTestOff };

private:
    MDataSet fDataSetOn;
    MDataSet fDataSetOff;

    UInt_t fNum[4];
/*
    TList fPreTasksSet[4];
    TList fPostTasksSet[4];

    Bool_t fEnableWeights[4];
*/
    Bool_t fUseRegression;

    TList     fHists;
    TObjArray fBinnings;

    // Result
    void DisplayResult(MH3 &h31, MH3 &h32, Float_t ontime);

public:
    MJTrainCuts() :  fUseRegression(kFALSE)
    {
        for (int i=0; i<4; i++)
        {
            //fEnableWeights[i]=kFALSE;
            fNum[i] = (UInt_t)-1;
        }

        fHists.SetOwner();
        fBinnings.SetOwner();
    }

    void SetDataSetOn(const MDataSet &ds, UInt_t ntrain=(UInt_t)-1, UInt_t ntest=(UInt_t)-1)
    {
        ds.Copy(fDataSetOn);

        fDataSetOn.SetNumAnalysis(1);

        fNum[kTestOn]  = ntrain;
        fNum[kTrainOn] = ntest;
    }
    void SetDataSetOff(const MDataSet &ds, UInt_t ntrain=(UInt_t)-1, UInt_t ntest=(UInt_t)-1)
    {
        ds.Copy(fDataSetOff);

        fDataSetOff.SetNumAnalysis(2);

        fNum[kTestOff]  = ntrain;
        fNum[kTrainOff] = ntest;
    }

    // Add Histogram
    void AddHist(UInt_t nx);
    void AddHist(UInt_t nx, UInt_t ny);
    void AddHist(UInt_t nx, UInt_t ny, UInt_t nz);

    void AddBinning(UInt_t n, const MBinning &bins);
    //void AddBinning(const MBinning &bins);

    // Standard user interface
/*
    void SetWeights(Type_t typ, const char *rule)  { if (fEnableWeights[typ]) return; fEnableWeights[typ]=kTRUE; AddPostTask(typ, rule); }
    void SetWeights(Type_t typ, MTask *t)          { if (fEnableWeights[typ]) return; fEnableWeights[typ]=kTRUE; AddPostTask(typ, t); }

    void AddPreTaskOn(MTask *t)                                       { AddPreTask(kTrainOn, t); AddPreTask(kTestOn, t); }
    void AddPreTaskOn(const char *rule, const char *name="MWeight")   { AddPreTask(kTrainOn, rule, name); AddPreTask(kTestOn, rule, name); }
    void AddPreTaskOff(MTask *t)                                      { AddPreTask(kTrainOff, t); AddPreTask(kTestOff, t); }
    void AddPreTaskOff(const char *rule, const char *name="MWeight")  { AddPreTask(kTrainOff, rule, name); AddPreTask(kTestOff, rule, name); }

    void AddPostTaskOn(MTask *t)                                      { AddPostTask(kTrainOn, t); AddPostTask(kTestOn, t); }
    void AddPostTaskOn(const char *rule, const char *name="MWeight")  { AddPostTask(kTrainOn, rule, name); AddPostTask(kTestOn, rule, name); }
    void AddPostTaskOff(MTask *t)                                     { AddPostTask(kTrainOff, t); AddPostTask(kTestOff, t); }
    void AddPostTaskOff(const char *rule, const char *name="MWeight") { AddPostTask(kTrainOff, rule, name); AddPostTask(kTestOff, rule, name); }

    void AddPreTask(Type_t typ, MTask *t)                                      { Add(fPreTasksSet[typ],  t); }
    void AddPreTask(Type_t typ, const char *rule, const char *name="MWeight")  { AddPar(fPreTasksSet[typ], rule, name); }
    void AddPostTask(Type_t typ, MTask *t)                                     { Add(fPostTasksSet[typ],  t); }
    void AddPostTask(Type_t typ, const char *rule, const char *name="MWeight") { AddPar(fPostTasksSet[typ], rule, name); }

    void SetWeightsOn(const char *rule)  { SetWeights(kTrainOn, rule);  SetWeights(kTestOn, rule); }
    void SetWeightsOn(MTask *t)          { SetWeights(kTrainOn, t);     SetWeights(kTestOn, t); }
    void SetWeightsOff(const char *rule) { SetWeights(kTrainOff, rule); SetWeights(kTestOff, rule); }
    void SetWeightsOff(MTask *t)         { SetWeights(kTrainOff, t);    SetWeights(kTestOff, t); }
*/
    void EnableRegression(Bool_t b=kTRUE)     { fUseRegression =  b; }
    void EnableClassification(Bool_t b=kTRUE) { fUseRegression = !b; }

    // Main function to start processing
    Bool_t Process(const char *out);

    ClassDef(MJTrainCuts, 0)//Class to help finding cuts using the random forest
};

#endif
