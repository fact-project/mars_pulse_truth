#ifndef MARS_MTask
#define MARS_MTask

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MTask                                                                   //
//                                                                         //
// Abstract base class for a task                                          //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef MARS_MInputStreamID
#include "MInputStreamID.h"
#endif

class TList;
class TStopwatch;

class MFilter;
class MParList;

class MTask : public MInputStreamID
{
public:
    enum {
        kAccStandard  = 0,
        kAccDontTime  = BIT(1),
        kAccDontReset = BIT(2)
    };

private:
    TList *fListOfBranches;     //! List of Branch names for auto enabeling scheme

    MFilter *fFilter;           // Filter for conditional task execution
    Byte_t   fSerialNumber;     // Serial number having more than one detector of the same type

    Bool_t fIsPreprocessed;     //! Indicates the success of the PreProcessing (set by MTaskList)

    TStopwatch *fStopwatch;     //! Count the execution time and number of executions
    UInt_t      fNumExecutions; //! Number of executions (also counted by fStopwatch, but faster)
    UInt_t      fNumExec0;      //! Total number of executions at PreProcess
    Byte_t      fAccelerator;   //!

    virtual Int_t PreProcess(MParList *pList);
    virtual Int_t Process();
    virtual Int_t PostProcess();

protected:
    void AddToBranchList(const char *b);
    void AddToBranchList(const TString &str);
    void AddToBranchList(const char *master, const char *sub, const UInt_t first, const UInt_t last)
    {
        if (first==0 && last==0)
        {
            AddToBranchList(sub);
            return;
        }

        for (unsigned int i=first; i<last+1; i++)
            AddToBranchList(Form("%s;%d.%s", master, i, sub));
    }
    void AddToBranchList(TString &master, TString &sub, const UInt_t first, const UInt_t last)
    {
        AddToBranchList((const char*)master, (const char*)sub, first, last);
    }
    void AddToBranchList(const char *master, const char *sub, const UInt_t num)
    {
        AddToBranchList(master, sub, 0, num);
    }
    void AddToBranchList(TString &master, TString &sub, const UInt_t num)
    {
        AddToBranchList(master, sub, 0, num);
    }

public:
    MTask(const char *name=NULL, const char *title=NULL);
    MTask(MTask &t);
    virtual ~MTask();

    const TList *GetListOfBranches() const { return fListOfBranches; }
    Bool_t Overwrites(const char *name, TClass *cls=NULL) const { return MParContainer::Overwrites(MTask::Class(), *this, name, cls); }

    // Filter functions
    virtual void SetFilter(MFilter *filter);
    const MFilter *GetFilter() const        { return fFilter; }
    MFilter *GetFilter()                    { return fFilter; } // for MContinue only

    // Display functions
    void SetDisplay(MStatusDisplay *d);

    // Acceleration Control
    virtual void SetAccelerator(Byte_t acc=kAccStandard) { fAccelerator=acc; }
    Byte_t GetAccelerator() const { return fAccelerator; }
    Bool_t HasAccelerator(Byte_t acc) const { return fAccelerator&acc; }

    // Function for parallel executions
    static TString AddSerialNumber(const char *str, UInt_t num) { TString s(str); if (num==0) return s; s += ";"; s += num; return s; }
    static TString AddSerialNumber(const TString &str, UInt_t num) { return AddSerialNumber((const char*)str, num); }
    TString AddSerialNumber(const char *str) const { return AddSerialNumber(str, fSerialNumber); }
    TString AddSerialNumber(const TString &str) const { return AddSerialNumber(str, fSerialNumber); }

    virtual void SetSerialNumber(Byte_t num) { fSerialNumber = num;  }
    Byte_t GetSerialNumber() const           { return fSerialNumber; }

    const TString GetDescriptor() const;

    // Task execution statistics
    UInt_t   GetNumExecutions() const;
    UInt_t   GetNumExecutionsTotal() const;
    Double_t GetCpuTime() const;
    Double_t GetRealTime() const;
    virtual void PrintStatistics(const Int_t lvl=0, Bool_t title=kFALSE, Double_t time=0) const;
    virtual void PrintSkipped(UInt_t n, const char *str);

    // Task overwrite functions
    virtual Bool_t ReInit(MParList *pList);

    virtual Int_t CallPreProcess(MParList *plist);
    virtual Int_t CallProcess();
    virtual Int_t CallPostProcess();

    void SavePrimitive(std::ostream &out, Option_t *o="");
    void SavePrimitive(std::ofstream &out, Option_t *o="");

    // TObject
    void RecursiveRemove(TObject *obj);

    ClassDef(MTask, 2) //Abstract base class for a task
};

#endif
