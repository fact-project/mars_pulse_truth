#ifndef MARS_MJOptimizeBase
#define MARS_MJOptimizeBase

#ifndef MARS_MJob
#include "MJob.h"
#endif

class MTask;
class MFilter;

class MJOptimizeBase : public MJob
{
protected:
    Int_t  fDebug;
    Bool_t fEnableWeights;  // Enable using weights

    TList fRules;           // Contains the parameters which can be accessed by matrix column
    TList fTrainParameters; // Parameters in the last columns

    TList fPreCuts;         // Cuts executed for training and testing
    TList fTrainCuts;       // Cuts executed only in training
    TList fTestCuts;        // Cuts executed only in testing
    TList fPreTasks;        // Tasks executed before cut execution
    TList fPostTasks;       // Tasks executed after pre cut execution
    TList fTestTasks;       // Tasks executed at the end of the testing tasklist

    void AddCut(TList &l, const char *rule);
    void AddPar(TList &l, const char *rule, const char *name);
    void Add(TList &l, MTask *f);

public:
    MJOptimizeBase();

    // MJOptimizeBase
    void SetDebug(Bool_t b=kTRUE) { fDebug = b; }

    // Add a parameter to the list of parameters
    Int_t AddParameter(const char *rule);

    // Tasks which are executed after reading (training and testing if available)
    void AddPreTask(MTask *t)                    { Add(fPreTasks,  t); }
    void AddPreTask(const char *rule,
                    const char *name="MWeight")  { AddPar(fPreTasks, rule, name); }

    // Cuts which are executed after the pre-tasks (training and testing if available)
    void AddPreCut(const char *rule)             { AddCut(fPreCuts, rule); }
    void AddPreCut(MFilter *f)                   { Add(fPreCuts, (MTask*)(f)); }

    // Same as pre-cuts but only executed in taining or testing
    void AddTrainCut(const char *rule)           { AddCut(fTrainCuts, rule); }
    void AddTrainCut(MFilter *f)                 { Add(fTrainCuts, (MTask*)(f)); }

    void AddTestCut(const char *rule)            { AddCut(fTestCuts, rule); }
    void AddTestCut(MFilter *f)                  { Add(fTestCuts, (MTask*)(f)); }

    // Tasks which are excuted after event selection (training and testing if available)
    void AddPostTask(MTask *t)                   { Add(fPostTasks, t); }
    void AddPostTask(const char *rule,
                     const char *name="MWeight") { AddPar(fPostTasks, rule, name); }

    // Tasks executed at the end of the testing tasklist
    void AddTestTask(MTask *t)                   { Add(fTestTasks, t); }
    void AddTestTask(const char *rule,
                     const char *name="MWeight") { AddPar(fTestTasks, rule, name); }

    // Add calculation of weights to list of post tasks. Set fEnableWeights
    void SetWeights(const char *rule)  { if (fEnableWeights) return; fEnableWeights=kTRUE; AddPostTask(rule); }
    void SetWeights(MTask *t)          { if (fEnableWeights) return; fEnableWeights=kTRUE; AddPostTask(t);    }

    ClassDef(MJOptimizeBase, 0)//Base class for all optimizations and trainings
};

#endif
