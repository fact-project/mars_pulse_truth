#ifndef MARS_MJTrainSeparation
#define MARS_MJTrainSeparation

#ifndef MARS_MJTrainRanForest
#include "MJTrainRanForest.h"
#endif

#ifndef MARS_MDataSet
#include "MDataSet.h"
#endif

class MH3;

class MJTrainSeparation : public MJTrainRanForest
{
public:
    enum Type_t { kTrainOn, kTrainOff, kTestOn, kTestOff };

private:
    MDataSet fDataSetTest;
    MDataSet fDataSetTrain;

    UInt_t fNum[4];

    TList fPreTasksSet[4];
    TList fPostTasksSet[4];

    Bool_t fAutoTrain;
    Bool_t fUseRegression;

    Bool_t fEnableWeights[4];

    Float_t fFluxTrain;
    Float_t fFluxTest;

    // Result
    void     DisplayResult(MH3 &h31, MH3 &h32, Float_t ontime);

    // Auto training
    Bool_t   GetEventsProduced(MDataSet &set, Double_t &num, Double_t &min, Double_t &max) const;
    Double_t GetDataRate(MDataSet &set, Double_t &num) const;
    Double_t GetNumMC(MDataSet &set) const;
    Float_t  AutoTrain(MDataSet &set, Type_t typon, Type_t typoff, Float_t flux);

public:
    MJTrainSeparation() :
        fAutoTrain(kFALSE), fUseRegression(kFALSE),
        fFluxTrain(2e-7), fFluxTest(2e-7)
    { for (int i=0; i<4; i++) { fEnableWeights[i]=kFALSE; fNum[i] = (UInt_t)-1; } }

    void SetDataSetTrain(const MDataSet &ds, UInt_t non=(UInt_t)-1, UInt_t noff=(UInt_t)-1)
    {
        ds.Copy(fDataSetTrain);

        fDataSetTrain.SetNumAnalysis(1);

        fNum[kTrainOn]  = non;
        fNum[kTrainOff] = noff;
    }
    void SetDataSetTest(const MDataSet &ds, UInt_t non=(UInt_t)-1, UInt_t noff=(UInt_t)-1)
    {
        ds.Copy(fDataSetTest);

        fDataSetTest.SetNumAnalysis(1);

        fNum[kTestOn]  = non;
        fNum[kTestOff] = noff;
    }

    // Deprecated, used for test purpose
    void AddPreTask(Type_t typ, MTask *t)                                      { Add(fPreTasksSet[typ],  t); }
    void AddPreTask(Type_t typ, const char *rule, const char *name="MWeight")  { AddPar(fPreTasksSet[typ], rule, name); }

    void AddPostTask(Type_t typ, MTask *t)                                     { Add(fPostTasksSet[typ],  t); }
    void AddPostTask(Type_t typ, const char *rule, const char *name="MWeight") { AddPar(fPostTasksSet[typ], rule, name); }

    void SetWeights(Type_t typ, const char *rule)  { if (fEnableWeights[typ]) return; fEnableWeights[typ]=kTRUE; AddPostTask(typ, rule); }
    void SetWeights(Type_t typ, MTask *t)          { if (fEnableWeights[typ]) return; fEnableWeights[typ]=kTRUE; AddPostTask(typ, t); }

    // Standard user interface
    void AddPreTaskOn(MTask *t)                                       { AddPreTask(kTrainOn, t); AddPreTask(kTestOn, t); }
    void AddPreTaskOn(const char *rule, const char *name="MWeight")   { AddPreTask(kTrainOn, rule, name); AddPreTask(kTestOn, rule, name); }
    void AddPreTaskOff(MTask *t)                                      { AddPreTask(kTrainOff, t); AddPreTask(kTestOff, t); }
    void AddPreTaskOff(const char *rule, const char *name="MWeight")  { AddPreTask(kTrainOff, rule, name); AddPreTask(kTestOff, rule, name); }

    void AddPostTaskOn(MTask *t)                                      { AddPostTask(kTrainOn, t); AddPostTask(kTestOn, t); }
    void AddPostTaskOn(const char *rule, const char *name="MWeight")  { AddPostTask(kTrainOn, rule, name); AddPostTask(kTestOn, rule, name); }
    void AddPostTaskOff(MTask *t)                                     { AddPostTask(kTrainOff, t); AddPostTask(kTestOff, t); }
    void AddPostTaskOff(const char *rule, const char *name="MWeight") { AddPostTask(kTrainOff, rule, name); AddPostTask(kTestOff, rule, name); }

    void SetWeightsOn(const char *rule)  { SetWeights(kTrainOn, rule); SetWeights(kTestOn, rule); }
    void SetWeightsOn(MTask *t)          { SetWeights(kTrainOn, t); SetWeights(kTestOn, t); }
    void SetWeightsOff(const char *rule) { SetWeights(kTrainOff, rule); SetWeights(kTestOff, rule); }
    void SetWeightsOff(MTask *t)         { SetWeights(kTrainOff, t); SetWeights(kTestOff, t); }

    void SetFluxTrain(Float_t f) { fFluxTrain = f; }
    void SetFluxTest(Float_t f)  { fFluxTest  = f; }
    void SetFlux(Float_t f)      { SetFluxTrain(f); SetFluxTest(f); }

    void EnableAutoTrain(Bool_t b=kTRUE)      { fAutoTrain     =  b; }
    void EnableRegression(Bool_t b=kTRUE)     { fUseRegression =  b; }
    void EnableClassification(Bool_t b=kTRUE) { fUseRegression = !b; }

    Bool_t Train(const char *out);

    ClassDef(MJTrainSeparation, 0)//Class to train Random Forest gamma-/background-separation
};

#endif
