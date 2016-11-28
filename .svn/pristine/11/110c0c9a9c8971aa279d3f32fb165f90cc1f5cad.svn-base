#ifndef MARS_MWreitFitsFile
#define MARS_MWreitFitsFile

#include <list>
#include <vector>
#include <map>

#ifndef ROOT_TClass
#include <TClass.h>
#endif

#ifndef MARS_MLogManip
#include "MLogManip.h"
#endif

#ifndef MARS_MWriteFile
#include "MWriteFile.h"
#endif

#ifndef MARS_MTopFitsGroup
#include "MTopFitsGroup.h"
#endif

#ifndef MARS_MFitsArray
#include "MFitsArray.h"
#endif

#ifndef MARS_MRawRunHeader
#include "MRawRunHeader.h"
#endif

class MRawRunHeader;

///////////////////////////////////////////////////////////////////////////////
// Information of one MParContainer, which data should be written into all or  
// just a few columns of one FITS table                                        
class MFitsSubTable
{
   // the container, which data should be written to the FITS sub-table.   
   MParContainer  *fContainer; 

   // used only if the Container is defined by its name.                   
   // if kTRUE  must exist in the MParList of the program                  
   // if kFALSE the container is ignored. if it is not in the              
   //           MParList of the program                                    
   Bool_t         fMust;

public:
   MFitsSubTable( Bool_t must)
      : fContainer(NULL), fMust(must)
      {}

   MFitsSubTable(MParContainer * container,  Bool_t must)
      : fContainer(container),  fMust(must)
      {}

  
   MFitsSubTable(const MFitsSubTable & subTable)
      : fContainer(subTable.fContainer), fMust(subTable.fMust)
      {}

   Bool_t            MustHave()           {return fMust;}
   MParContainer *   GetContainer()       {return fContainer;}
   void              SetContainer(MParContainer * cont)
                                          {fContainer = cont;}

};



///////////////////////////////////////////////////////////////////////////////
// A class to write data of MParContainer into FITS files.                     
class MWriteFitsFile : public MWriteFile
{
private:
   // all (global) currently open top FITS groups.                      
   // Note: several instances of MWriteFitsFile can write to the same   
   //       top level FITS group (a top level FITS group corresponds to 
   //       one ROOT file)                                              
   static std::map<MUniqueFileId, MTopFitsGroup>    fTopFitsGroups;

   // pointer to the top level FITS group of this instance              
   std::map<MUniqueFileId, MTopFitsGroup>::iterator iTopFitsGroup;

   // the key (TString) of following maps is the FITS table name, which 
   // corresponds to one TTree                                          

   // all main FITS table. Children tables to store data of             
   // TClonesArray are not in this map.
   //ETIENNE ofits objects cannot be copied. So store pointers instead
   std::map<TString, ofits*>                       fFitsTables;

   // all information needed to write data of TClonesArray to their      
   // own FITS table. Note: one parent FTIS table may have several       
   // children tables, each for one TClonesArray.                        
   std::map<TString, std::list<MArrayHelperBase *> >    fClHelper;

   // all information about the MParContainer, which data are stored     
   // in the same FITS table. The key of map<TString, MFitsSubTable>     
   // is the class name of the MParContainer                             
   std::map<TString, std::map<TString, MFitsSubTable> > fSubTables;

   std::map<TString, std::vector<void*> >    fDataPointers;
   std::map<TString, std::vector<char> >     fTypeChars;
   std::map<TString, std::vector<uint32_t> > fColSizes;
   std::map<TString, std::vector<uint32_t> > fColWidth;
   std::map<TString, bool> fTableObjectCreated;
   std::map<TString, bool> fTableHeaderWritten;

   void DeleteArrayHelper();

   // rule to construct an output file-name from the input-filename      
   TString fRule;  

   TString fOpenOption; // RECREATE or NEW
   TString fGroupName;  // name of top level group
   TString fTitle;      // title of top level group

   void OpenTopLevelGroup(const char * fname);
   void CloseTopLevelGroup();

   Bool_t InitColumns(const TString & tableName,
                      const TString & parentVarName,
                      ofits* fitsTable,
                      void * baseAdr,
                      TClass * classDef);
   void InitSingleColumn(const TString& tableName,
                         uint32_t count,
                         const std::string& typeName,
                         void* dataPointer,
                         const std::string& columnName,
                         const std::string& unit,
                         const std::string& comment);
   void writeOneRow(const TString& tableName);
   void InitAttr(const char* attrName,
                 const char* dataType,
                 void* var,
                 const char* unit=NULL,
                 const char* comment=NULL,
                 ofits* outFile=NULL);

   // MWrite
   Bool_t      IsFileOpen() const  
                              {return iTopFitsGroup != fTopFitsGroups.end();}
   Bool_t      CheckAndWrite();
   Bool_t      GetContainer(MParList *pList);
   const char *GetFileName() const;

   Int_t       PreProcess(MParList *pList);
   Int_t       PostProcess();

   void        SetupHeaderKeys(MRawRunHeader &header);

public:
   //Header keys related stuff
   template<typename _T>
   std::string GetFitsString(const _T& value)
   {
       std::ostringstream returnVal;
       std::string typeName = typeid(_T).name();
       if (typeName == "i" || typeName == "j" || //int
           typeName == "s" || typeName == "t" || //short
           typeName == "l" || typeName == "m" || //long
           typeName == "x" || typeName == "y")   //long long
           returnVal << value;

       if (typeName == "b")
       {
           if (value) returnVal << "T"; else returnVal << "F";
       }
       if (typeName == "Ss" || typeName == "PKc" ||
           (typeName.size() >= 4 && typeName[0] == 'A' && typeName[typeName.size()-1] == 'c' && typeName[typeName.size()-2] == '_'))
       {
           returnVal << value;
           std::string temp = returnVal.str();
           returnVal.str("");
           for (std::string::iterator c=temp.begin(); c<temp.end(); c++)
               if (*c=='\'')
                   temp.insert(c++, '\'');
           returnVal << "'" << temp << "'";
       }
       if (returnVal.str() == "")
           *fLog << warn << "WARNING: could not construct fits string from \"" << value << "\" typename: " << typeName << std::endl;
       return returnVal.str();
   }

   template<typename _T>
   bool SetHeaderKey(const std::string& key,
                     const _T& value,
                     const std::string& comment="")
   {
       //check if header was already written
       for (std::map<TString, bool>::iterator it=fTableHeaderWritten.begin(); it!= fTableHeaderWritten.end(); it++)
           if (it->second == true)
               return false;
       //headers were not written yet. Add one key
       std::vector<ofits::Key>::iterator it = fHeaderKeys.begin();
       //check if the value had been set beforel
       for (;it!=fHeaderKeys.end();it++)
       {
           if (it->key == key)
           {
               it->value = GetFitsString(value);
               it->comment = comment;
               break;
           }
       }
       //did we find the key ?
       if (it == fHeaderKeys.end())
       {//no ? add it !
           ofits::Key temp;
           temp.key = key;
           temp.value = GetFitsString(value);
           temp.comment = comment;
           fHeaderKeys.push_back(temp);
       }
       return true;
   }
private:
   std::vector<ofits::Key> fHeaderKeys;

   std::string Trim(const std::string &str);

   // MTask
   Bool_t ReInit(MParList *pList);

   std::vector<std::string> fVetoedColumns;
   std::map<std::string, uint32_t> fBytesPerSamples;


public:

   //Veto a type-mapping functions and variables
   Bool_t VetoColumn(const std::string& colName);
   Bool_t SetBytesPerSample(const std::string& colName, uint32_t numBytes);

   enum FILE_MODE {
      kMultiFiles,
      kSingleFile
      } ;

    MWriteFitsFile(const char *fname,
                   FILE_MODE fileMode = kMultiFiles,
                   const Option_t *opt="RECREATE",
                   const char *name=NULL,
                   const char *title=NULL);
    //Similar contructor to what MWriteRootFiles takes.
    //Some of the args are not used: comp as they does not make sense in FITS
    MWriteFitsFile(const Int_t comp,
                   const char* rule,
                   const Option_t* option="RECREATE",
                   const char* fTitle="Untitled",
                   const char* name=NULL,
                   const char* title=NULL);

   ~MWriteFitsFile();


   void AddContainer(const char *cName,   const char *tName=NULL, Bool_t must=kTRUE, UInt_t max=0);
   void AddContainer(MParContainer *cont, const char *tName=NULL, Bool_t must=kTRUE, UInt_t max=0);


   void AddTree(const char *name, Bool_t force=kTRUE)
   {
       AddContainer(Form("MReport%s", name), name, force);
       AddContainer(Form("MTime%s", name),   name, force);
   }

   ClassDef(MWriteFitsFile, 0)   
};
//Specializations for float and doubles (because of the precision handling, I could not deal with it in the main function
template<>
std::string MWriteFitsFile::GetFitsString(const double& value);
template<>
std::string MWriteFitsFile::GetFitsString(const float& value);
#endif
