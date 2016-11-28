#ifndef MARS_MDataSet
#define MARS_MDataSet

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#ifndef ROOT_TArrayI
#include <TArrayI.h>
#endif

class TChain;

class MRead;
class MDirIter;
class MSequence;
class MPointingPos;

class MDataSet : public MParContainer
{
private:
    static const TString fgCatalog; //! Default Catalog path

    TString fFileName;        // File name to original file (MParContainer::fName is not streamed)
    TString fDataSet;         // Name of the dataset given by the user

    UInt_t  fNumAnalysis;     // Analysis number (artificial)

    TString fPathSequences;   // Default path to the sequence files
    TString fPathDataFiles;   // Default path to the data files

    TArrayI fNumSequencesOn;  // numbers of on-sequences
    TArrayI fNumSequencesOff; // numbers of off-sequences

    TList   fSequencesOn;     // list of names and paths of on-sequences
    TList   fSequencesOff;    // list of names and paths of off-sequences

    TString fNameSource;      // Name of source from catalog
    TString fCatalog;         // edb catalog (magic_favourites.edb)

    TString fComment;         // Comment from DS file

    Bool_t  fWobbleMode;      // Wobble Mode for this dataset?
    Bool_t  fMonteCarlo;      // For default paths use MC or data path

    // Helper functions for constrcutors
    void Split(TString &runs, TArrayI &data) const;
    void ResolveSequences(const TEnv &env, const TString &prefix, const TArrayI &num, TList &list/*, const TString &sequences, const TString &data*/) const;
    Bool_t GetWobbleMode(const TEnv &env, const TString &prefix) const;
    static void PrintFile(std::ostream &out, const MSequence &obj);
    void PrintSeq(std::ostream &out, const MSequence &seq) const;

    // Directory and file handling
    //void ReplaceDir(TList &list, const TString &old, const TString &news) const;
    //void ReplaceFile(TList &list, const TString &old, const TString &news) const;

    void SetupDefaultPath(TString &path, const TString &def) const
    {
        if (path.IsNull())
            path = def;
        if (!path.EndsWith("/"))
            path += "/";
    }

    Bool_t HasSequence(UInt_t num, const TArrayI &seq) const
    {
        for (int i=0; i<seq.GetSize(); i++)
            if ((UInt_t)seq[i]==num)
                return kTRUE;
        return kFALSE;
    }

    // Initialize from constrcutors
    void Init(const char *fname, const UInt_t num, TString sequences, TString &data);

public:
    MDataSet() : fNumAnalysis((UInt_t)-1)
    {
        fName  = "MDataSet";
        fTitle = "DataSet file";
    }
    MDataSet(const char *fname, TString sequences="", TString data="");
    MDataSet(const char *fname, Int_t num, TString sequences="", TString data="");

    const char    *GetBaseName() const;
    const char    *GetFilePath() const;
    const TString &GetFileName() const { return fFileName; }
    const TString &GetDataSet()  const { return fDataSet;  }

    void Copy(TObject &obj) const
    {
        MDataSet &ds = (MDataSet&)obj;
        ds.fNumAnalysis = fNumAnalysis;
        ds.fNumSequencesOn = fNumSequencesOn;
        ds.fNumSequencesOff = fNumSequencesOff;
        ds.fNameSource = fNameSource;
        ds.fCatalog = fCatalog;
        ds.fComment = fComment;
        ds.fWobbleMode = fWobbleMode;
        ds.fMonteCarlo = fMonteCarlo;

        TObject *o=0;

        ds.fSequencesOn.Delete();
        ds.fSequencesOff.Delete();

        TIter NextOn(&fSequencesOn);
        while ((o=NextOn()))
            ds.fSequencesOn.Add(o->Clone());

        TIter NextOff(&fSequencesOff);
        while ((o=NextOff()))
            ds.fSequencesOff.Add(o->Clone());
    }

    // Getter
    Bool_t IsValid() const { return fNumAnalysis!=(UInt_t)-1 && GetNumSequencesOn()>0; }

    UInt_t GetNumSequencesOn() const  { return fNumSequencesOn.GetSize(); }
    UInt_t GetNumSequencesOff() const { return fNumSequencesOff.GetSize(); }

    Bool_t HasOffSequences() const { return GetNumSequencesOff()>0; }

    Bool_t HasOffSequence(UInt_t num) const { return HasSequence(num, fNumSequencesOff); }
    Bool_t HasOnSequence(UInt_t num) const  { return HasSequence(num, fNumSequencesOn); }
    Bool_t HasSequence(UInt_t num) const    { return HasOnSequence(num) || HasOffSequence(num); }

    UInt_t GetNumAnalysis() const { return fNumAnalysis; }
    void   SetNumAnalysis(UInt_t num) { fNumAnalysis=num; }

    Bool_t HasSource() const { return !fNameSource.IsNull(); }
    Bool_t GetSourcePos(MPointingPos &pos) const;

    Bool_t IsWobbleMode() const { return fWobbleMode; }
    Bool_t IsMonteCarlo() const { return fMonteCarlo; }

    const char *GetDefPathDataFiles() const { return fMonteCarlo ? "/magic/montecarlo/star"      : "/magic/data/star"; }
    const char *GetDefPathSequences() const { return fMonteCarlo ? "/magic/montecarlo/sequences" : "/magic/sequences"; }

    // Setter
    void SetMonteCarlo(Bool_t ismc=kTRUE) { fMonteCarlo=ismc; }
    void SetWobbleMode(Bool_t wobm=kTRUE)  { fWobbleMode=wobm; }

    static Bool_t AddSequencesFromList(const TList &list, MDirIter &files);
    static Int_t  AddFilesToChain(MDirIter &files, TChain &chain);

    const TList &GetSequencesOn() const  { return fSequencesOn; }
    const TList &GetSequencesOff() const { return fSequencesOff; }

    const TArrayI &GetSequencesNumOn() const  { return fNumSequencesOn; }
    const TArrayI &GetSequencesNumOff() const { return fNumSequencesOff; }

    Bool_t AddFiles(MRead &read) const;
    Bool_t AddFilesOn(MRead &read) const;
    Bool_t AddFilesOff(MRead &read) const;

    Bool_t AddFiles(TChain &read) const;
    Bool_t AddFilesOn(TChain &read) const;
    Bool_t AddFilesOff(TChain &read) const;

    Bool_t AddFiles(MDirIter &iter) const;
    Bool_t AddFilesOn(MDirIter &iter) const;
    Bool_t AddFilesOff(MDirIter &iter) const;

    /*
    void ReplaceDir(const TString &old, const TString &news)
    {
        ReplaceDir(fSequencesOn,  old, news);
        ReplaceDir(fSequencesOff, old, news);
    }

    void ReplaceFile(const TString &old, const TString &news)
    {
        ReplaceFile(fSequencesOn,  old, news);
        ReplaceFile(fSequencesOff, old, news);
    }
    */

    // I/O
    void WriteFile(const char *filename, const Option_t *o) const;
    void WriteFile(const char *filename) const { WriteFile(filename,""); } //*MENU *ARGS={filename=>fBaseName}

    // TObject
    void Print(std::ostream &out, Option_t *o) const;
    void Print(Option_t *o) const;
    void Print() const { Print(""); } //*MENU*

    ClassDef(MDataSet, 3)
};

#endif
