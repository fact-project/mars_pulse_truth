#include <TDataMember.h>
#include <TClonesArray.h>

#include "MLogManip.h"

#include "MArrayS.h"
#include "MArrayB.h"
#include "MArrayF.h"

#include "MRead.h"
#include "MParList.h"
#include "MStatusDisplay.h"

#include "MLog.h"
#include "MLogManip.h"
#include "MWriteRootFile.h"

#include "MWriteFitsFile.h"
#include "MFitsArray.h"

#include "MTime.h"

//for find
#include <algorithm>

ClassImp(MWriteFitsFile);

using namespace std;

map<MUniqueFileId, MTopFitsGroup> MWriteFitsFile::fTopFitsGroups;



// ----------------------------------------------------------------------------
//                                                                             
// if fileMode == kSingleFile                                                  
//    Opens the top level group file, if it was not already opened by an other 
//    instance of MWriteFitsFile.                                              
// if fileMode == kMultiFiles                                                  
//    fname defines the rules to build the output filenames from the current   
//    input filename. The files are opened in the ReInit() method.             
//                                                                             
// opt = RECREATE : an existing file will be deleted                           
//     = NEW      : an already existing file will not be deleted and the new   
//                  file is not created. Check with IsFileOpen() if the new    
//                  top level group could be opened.                           
// name will be the name of the group                                          
// title will be written as keyword TITLE into the header                      
//                                                                             
MWriteFitsFile::MWriteFitsFile(const char *fname,
                               FILE_MODE fileMode,
                               const Option_t *opt,
                               const char *name,
                               const char *title)
{
   fOpenOption = opt;
   if (name && name[0] != 0)
      fGroupName = name;
   else
      fGroupName = "MARS";

   if (title)
      fTitle = title;

   iTopFitsGroup = fTopFitsGroups.end();

   if (fileMode == kMultiFiles)
      fRule = fname;
   else
       *fLog << err << "The new version of MWriteFitsFile does not support multiple tables in one file. Please use kMultiFiles instead" << endl;
      //ETIENNE DEPREC
      // iTopFitsGroup will be set by OpenTopLevelGroup, if a top level   
      // group can be assigned to this MWriteFitsFile.                    
      //OpenTopLevelGroup(fname);
}

MWriteFitsFile::MWriteFitsFile(const Int_t comp,
                               const char* rule,
                               const Option_t* option,
                               const char* ftitle,
                               const char* name,
                               const char* title)
{
    fOpenOption = option;
    if (name && name[0] != 0)
        fGroupName = name;
    else
        fGroupName = "MARS";
    if (title) fTitle = ftitle;
    iTopFitsGroup = fTopFitsGroups.end();
    //always kMultiFiles
    fRule = rule;
}
// ----------------------------------------------------------------------------
//                                                                             
// Closes the top level group, it is is not used by an other instance of       
// MWriteFitsFile.                                                             
//                                                         std::map<TString, std::ofits*>
MWriteFitsFile::~MWriteFitsFile()
{
    for (std::map<TString, ofits*>::iterator it=fFitsTables.begin(); it!= fFitsTables.end();it++)
    {
        if (it->second != NULL)
        {
            it->second->close();
            delete it->second;
        }
    }
//   CloseTopLevelGroup();
   DeleteArrayHelper();
}

// ----------------------------------------------------------------------------
//                                                                             
// If the same file is already used by an other instance of MWriteFitsFile,    
// the user counter is increasesd.                                             
// Else: If the file exist and option is "RECREATE", the file is deleted and   
//       recreated.                                                            
// Finally the file with an empty group file is created.                       
//                                                                             
void MWriteFitsFile::OpenTopLevelGroup(const char * fname)
{
   // get rid of environment variables, ~, ... in filename
   char * expandedFileName = gSystem->ExpandPathName(fname);

   // get unique identifier of the new file
   struct stat fileId;
   if (stat(expandedFileName, &fileId) == 0)
      {
      // file exist already
      iTopFitsGroup = fTopFitsGroups.find(MUniqueFileId(fileId.st_dev, fileId.st_ino));
      if (iTopFitsGroup != fTopFitsGroups.end())
         {
          *fLog << err << "You are trying to use an already existing file. Please choose a different file name (" << fname << ")" << endl;
          exit(0);
         // this group is already open. we use it as well
         *fLog << inf2 << "Found open file '" << GetFileName() << "'... re-using." << endl;
         iTopFitsGroup->second.IncreaseUsage();
         delete [] expandedFileName;
         return;
         } 

      // file exist, but we cannot use
      if (fOpenOption == "RECREATE") 
         // remove the file
         unlink(expandedFileName);

      else if (fOpenOption == "NEW")
         {
         *fLog << err << "File '" << expandedFileName << "' exist already." << endl;
         delete [] expandedFileName;
         return;
         }
      else
         {
         *fLog << err << "Unsupported option (" << fOpenOption.Data() << ") to open file " <<
                          expandedFileName << endl;
         delete [] expandedFileName;
         return;
         }
      }

   // file does not yet exist or was deleted

//   try {
//      AstroRootGroup topGroup;
//      char wrTitle[fTitle.Length() + 1];
//      if (fTitle.Length() > 0)
//         {
//         strcpy(wrTitle, fTitle.Data());
//         topGroup.InitAttr("TITLE", "A", wrTitle);
//         }

   //ETIENNE do not open top level group as using one file per table
   //I kept this code active anyway, because it is checked elsewhere in the code that iTopFitsGroup is set.
        ofits* topGroup = NULL;
//      ofits* topGroup = new ofits();
//      TString dol = expandedFileName;
//      topGroup->open(dol.Data());


      // store this new file in fTopFitsGroups and get the iterator to the  
      // new group                                                          
      stat(expandedFileName, &fileId);
      iTopFitsGroup = fTopFitsGroups.insert(pair<MUniqueFileId, MTopFitsGroup>
                           (MUniqueFileId(fileId.st_dev, fileId.st_ino),
                           MTopFitsGroup(expandedFileName, topGroup)        )).first; 

   delete [] expandedFileName;

}

// --------------------------------------------------------------------------
//                                                                           
// Reduces the use - counter of the top level group. Closes the group        
// table, if the counter is 0, i.e. no other MWriteFitsFile instance uses    
// the same top level group.                                                 
//

void MWriteFitsFile::CloseTopLevelGroup()
{
   if (iTopFitsGroup != fTopFitsGroups.end())
      {
      iTopFitsGroup->second.DecreaseUsage();
      if (iTopFitsGroup->second.GetNumUsage() <= 0)
         fTopFitsGroups.erase(iTopFitsGroup);

      iTopFitsGroup = fTopFitsGroups.end();
      }
}

// --------------------------------------------------------------------------
//                                                                           
// returns the name of the top level group file. It may be expanded if the   
// name, given by the user, contained environment variables of the ~ sign.   
// If no top level group is open, "no_open_file" is returned.                
//                                                                           
const char * MWriteFitsFile::GetFileName() const            
{
   if (IsFileOpen())
      return iTopFitsGroup->second.GetFileName().Data();
   else
      return "no_open_file";
}

// --------------------------------------------------------------------------
//                                                                           
// Add a new Container to list of containers which should be written to the  
// file. Give the name of the container which will identify the container    
// in the parameterlist. tname is the name of the FITS table to which the    
// container should be written (Remark: one table can hold more than one     
// container). The default is the same name as the container name.           
// If "mus"t is set to kTRUE (the default), then the container must exist    
// in the parameterlist. The container will be ignored, if "must" is set to  
// kFALSE and the container does not exist in the parameterlist.             
//                                                                           
void MWriteFitsFile::AddContainer(const char *cName,
                                  const char *tName,
                                  Bool_t must, UInt_t max)
{
   if (cName == NULL || strlen(cName) == 0)
      {
      *fLog << warn << "Warning - container name in method MWriteFitsFile::"
                       "AddContainer not defined... ignored" << endl;
      return;
      }

   // if tName is not defined, we use cName as the table name  
   TString tableName;
   if (tName == NULL || strlen(tName) == 0)
      tableName = cName;
   else
      tableName = tName;

   // try to insert this container to the list of sub-tables. Write a     
   // warning, if the same container name was already defined and ignore  
   // this one.                                                           
   if (fSubTables[tableName].insert( pair<TString, MFitsSubTable>
                             (cName, MFitsSubTable(must))).second == false) 
      {
      *fLog << warn << "Warning - Container '"<< cName <<"' in table '" <<
                       tableName.Data() << "' already scheduled... ignored." << endl;
      }
}

// --------------------------------------------------------------------------
//                                                                           
// Add a new Container to list of containers which should be written to the  
// file. tname is the name of the FITS table to which the                    
// container should be written (Remark: one table can hold more than one     
// container). The default is the same name as the container name.           
// "must" is ignored. It is just for compatibility with MWriteRootFiles.     
//                                                                           
void MWriteFitsFile::AddContainer(MParContainer *cont, const char *tName, 
                                  Bool_t must, UInt_t max)
{
   if (cont == NULL)
      {
      *fLog << warn << "Warning - container in method MWriteFitsFile::"
                       "AddContainer not defined... ignored" << endl;
      return;
      }

   TString tableName;
   if (tName == NULL || strlen(tName) == 0)
      tableName = cont->IsA()->GetName();
   else
      tableName = tName;

   TString cName = cont->IsA()->GetName();
   if (fSubTables[tableName].insert( pair<TString, MFitsSubTable>
                             (cName, MFitsSubTable(cont, must))).second == false) 
      {
      *fLog << warn << "Warning - Container '"<< cName.Data() <<
                       "' in table '" << tableName.Data() << 
                       "' already scheduled... ignored." << endl;
      }
}

// --------------------------------------------------------------------------
//
// Tries to open the given output file.
//
Int_t MWriteFitsFile::PreProcess(MParList *pList)
{
   if (fRule.Length() != 0)
      // the file is not yet open
      return kTRUE;
    //
    // test whether file is now open or not
    //
    if (!IsFileOpen())
    {
        *fLog << err << dbginf << "Cannot open file '" << GetFileName() << "'" << endl;
        return kFALSE;
    }

    *fLog << inf << "File '" << GetFileName() << "' open for writing." << endl;

    //
    // Get the containers (pointers) from the parameter list you want to write
    //
    if (!GetContainer(pList))
        return kFALSE;

    //
    // write the container if it is already in changed state
    //
    return CheckAndWrite();

}

Int_t MWriteFitsFile::PostProcess()
{
    bool rc = true;
    for (std::map<TString, ofits*>::iterator it=fFitsTables.begin(); it!= fFitsTables.end();it++)
    {
        rc &= it->second->close();
        delete it->second;
        it->second = NULL;
    }

    return rc;
}

void MWriteFitsFile::SetupHeaderKeys(MRawRunHeader &header)
{
    const MTime now(-1);
    SetHeaderKey("ISMC",true,"Bool if File is Montecarlo File");
    SetHeaderKey("TELESCOP", "FACT", "");
    SetHeaderKey("PACKAGE", "MARS Cheobs", "");
    SetHeaderKey("VERSION", "1.0", "");
    SetHeaderKey("CREATOR", "Ceres", "");
    SetHeaderKey("EXTREL", 1., "");
    SetHeaderKey("COMPILED", __DATE__" " __TIME__, "");
    //SetHeaderKey("REVISION", "0", "");
    SetHeaderKey("ORIGIN", "FACT", "");
    SetHeaderKey("DATE", now.GetStringFmt("%Y-%m-%dT%H:%M:%S").Data(), "");
    SetHeaderKey("NIGHT", now.GetNightAsInt(), "");
    SetHeaderKey("TIMESYS", "UTC", "");
    SetHeaderKey("TIMEUNIT", "d", "");
    SetHeaderKey("MJDREF", 40587, "");
    //SetHeaderKey("BLDVER", 1, "");
    SetHeaderKey("RUNID", header.GetRunNumber(), "");
    SetHeaderKey("NBOARD", 40, "");
    SetHeaderKey("NPIX", header.GetNumPixel(), "");
    SetHeaderKey("NROI", header.GetNumSamplesHiGain(), "");
    SetHeaderKey("NROITM", 0, "");
    SetHeaderKey("TMSHIFT", 0, "");
    SetHeaderKey("CAMERA", "MGeomCamFACT", "Montecarlo File");
    SetHeaderKey("DAQ", "DRS4", "Montecarlo File");

    // FTemme: ADCRANGE and ADC have to be calculated, using the values for
    // the fadctype.
//    SetHeaderKey("ADCRANGE", 2000, "Dynamic range in mV");
//    SetHeaderKey("ADC", 12, "Resolution in bits");

    switch(header.GetRunType())
    {
    case MRawRunHeader::kRTData|MRawRunHeader::kRTMonteCarlo:
        SetHeaderKey("RUNTYPE", "data", "");
        break;
    case MRawRunHeader::kRTPedestal|MRawRunHeader::kRTMonteCarlo:
        SetHeaderKey("RUNTYPE", "pedestal", "");
        break;
    case MRawRunHeader::kRTCalibration|MRawRunHeader::kRTMonteCarlo:
        SetHeaderKey("RUNTYPE", "calibration", "");
        break;
    }
//    SetHeaderKey("ID", 777, "Board  0: Board ID");
//    SetHeaderKey("FMVER", 532, "Board  0: Firmware Version");
//    SetHeaderKey("DNA", "0", "");
//    SetHeaderKey("BOARD", 0, "");
//    SetHeaderKey("PRESC", 40, "");
//    SetHeaderKey("PHASE", 0, "");
//    SetHeaderKey("DAC0", 26500, "");
//    SetHeaderKey("DAC1", 0, "");
//    SetHeaderKey("DAC2", 0, "");
//    SetHeaderKey("DAC3", 0, "");
//    SetHeaderKey("DAC4", 28800, "");
//    SetHeaderKey("DAC5", 28800, "");
//    SetHeaderKey("DAC6", 28800, "");
//    SetHeaderKey("DAC7", 28800, "");
    SetHeaderKey("REFCLK", header.GetFreqSampling(), "");
    SetHeaderKey("DRSCALIB", false, "");
//    SetHeaderKey("TSTARTI", 0, "");
//    SetHeaderKey("TSTARTF", 0., "");
//    SetHeaderKey("TSTOPI", 0, "");
//    SetHeaderKey("TSTOPF", 0., "");
//    SetHeaderKey("DATE-OBS", "1970-01-01T00:00:00", "");
//    SetHeaderKey("DATE-END", "1970-01-01T00:00:00", "");
//    SetHeaderKey("NTRG", 0, "");
//    SetHeaderKey("NTRGPED", 0, "");
//    SetHeaderKey("NTRGLPE", 0, "");
//    SetHeaderKey("NTRGTIM", 0, "");
//    SetHeaderKey("NTRGLPI", 0, "");
//    SetHeaderKey("NTRGEXT1", 0, "");
//    SetHeaderKey("NTRGEXT2", 0, "");
//    SetHeaderKey("NTRGMISC", 0, "");
}

template<>
std::string MWriteFitsFile::GetFitsString(const double& value)
{
    std::ostringstream returnVal;
    returnVal << std::setprecision(value>1e-100 && value<1e100 ? 15 : 14) << value;
    std::string temp = returnVal.str();
    std::replace(temp.begin(), temp.end(), 'e', 'E');
    if (temp.find_first_of('E')==std::string::npos && temp.find_first_of('.')==std::string::npos)
        temp += ".";
    return temp;
}
template<>
std::string MWriteFitsFile::GetFitsString(const float& value)
{
    return GetFitsString((double)(value));
}
// --------------------------------------------------------------------------
//                                                                           
// Opens all FITS files for all container, which were registered with the    
// AddContainer() - methods. The container is taken from the pList, if it    
// was registered just by its name.                                          
// Calling the AddContainer() after calling this method will not add new     
// containers to the list.                                                   
//                                                                           
Bool_t  MWriteFitsFile::GetContainer(MParList *pList)
{
   if (iTopFitsGroup == fTopFitsGroups.end())
      // something went wrong while the top level group was created
      return kFALSE;


   // remove the extension from the filename
   // this has been disabled for now
   char fileNameNoExt[strlen(GetFileName()) + 1];
   strcpy(fileNameNoExt, GetFileName());
//   char * pos = strrchr(fileNameNoExt, '.');
//   if (pos) *pos = 0;
//*fLog << inf <<"Filename no ext: " << fileNameNoExt << endl;
   // loop over all FITS tables which have to be created.
   map<TString, map<TString, MFitsSubTable> >::iterator i_table =
      fSubTables.begin();
   while (i_table != fSubTables.end())
      {

      ofits* fitsTable = new ofits();
      fitsTable->AllowCommentsTrimming(true);
      TString dol = fileNameNoExt;
      //get rid of the ".root" extension in the file name (if any)
      if (dol(dol.Length()-5, dol.Length()) == ".root")
      {
          dol = dol(0, dol.Length()-5);
      }
      if (dol(dol.Length()-5, dol.Length()) == ".fits")
      {
          dol = dol(0, dol.Length()-5);
      }
      dol += "_";
      dol += i_table->first;
      dol += ".fits";
      fitsTable->open(dol.Data());
      *fLog << inf << "Opening FITS file: " << dol.Data() << endl;
      fFitsTables[i_table->first] = fitsTable;
      fTableObjectCreated[i_table->first] = true;
      fTableHeaderWritten[i_table->first] = false;

      // loop over all containers, which define a sub-table of the current table
      Int_t num = 0;
      map<TString, MFitsSubTable>::iterator i_subTable = i_table->second.begin();
      while (i_subTable != i_table->second.end())
         {

         MFitsSubTable & subTable = i_subTable->second;
         if (subTable.GetContainer() == NULL)
            {
            // container address is not yet known
            const char * cname = i_subTable->first.Data();
            MParContainer *cont = (MParContainer*)pList->FindObject(cname);
            if (!cont)
               {
               // no corresponding container is available in pList
               if (subTable.MustHave())
                  {
                  *fLog << err << "Cannot find parameter container '" << cname << "'." << endl;
                  return kFALSE;
                  }

               // we can ignore this container, delete it from the map
               *fLog << inf2 << "Unnecessary parameter container '" << cname << "' not found..." << endl;
               map<TString, MFitsSubTable>::iterator i_tmp = i_subTable;
               i_subTable++;
               i_table->second.erase(i_tmp);
               continue;
               }

            // we have our container address and can use it
            subTable.SetContainer(cont);
            }

         // initialize all columns of the sub-table, defined by the current container
         TString containerName = i_subTable->second.GetContainer()->GetName();
         TClass * cl = i_subTable->second.GetContainer()->IsA();
         if (!InitColumns(i_table->first, containerName + ".",  fitsTable,
                          i_subTable->second.GetContainer(), cl)     )
            return kFALSE;

         InitAttr(Form("CLNAME%d", num),
                  "A",
                  (void*)i_subTable->first.Data(),
                  NULL,
                  "MARS container name",
                  fitsTable);

         InitAttr(Form("CLTYPE%d", num),
                  "A",
                  (void*)i_subTable->second.GetContainer()->ClassName(),
                  NULL,
                  "MARS container class",
                  fitsTable);

         num++;
         i_subTable++;
         }

      // in case not all sub-tables were removed, we can now create the FITS table
      if (i_table->second.size() > 0)
         {
         // create the DOL of the table. It will be something like:     
         // fileNameNoExt_dataName.fits[dataName]                       
         TString dol = fileNameNoExt;
         dol += "_";
         dol += i_table->first;
         dol += ".fits";

         if (fOpenOption == "RECREATE") 
            // remove the file
            unlink(dol.Data());

         dol += "[";
         dol += i_table->first;
         dol += "]";
//         *fLog << err << "Reiner would have opened fits file: " << dol.Data() << endl;
         //exit(0);
         //fitsTable->open(dol.Data());

         // attach the new table to the top level group
         //TODO make sure that this attach below is not needed...
         //iTopFitsGroup->second.Attach(fitsTable);
         }
               
      i_table++;
      }
   
   return kTRUE;
}
void MWriteFitsFile::InitAttr(const char* attrName,
                              const char* dataType,
                              void* var,
                              const char* unit,
                              const char* comment,
                              ofits* outFile)
{
    if (outFile == NULL)
        return;
    string dts(dataType);
    string ans(attrName);
    string cs;
    if (comment != NULL)
        cs = string(comment);
    else
        cs = "";
    ostringstream val;
    if ((dts == "bool") || (dts == "Bool_t") || (dts == "L"))
    {
        outFile->SetBool(ans, ((bool*)(var))[0], cs);
        return;
    }
    if ((dts == "char") || (dts == "Char_t") || (dts == "S"))
    {
        val << ((char*)(var))[0];
        outFile->SetStr(ans, val.str(), cs);
        return;
    }
    if ((dts == "unsigned char") || (dts == "UChar_t") || (dts == "B"))
    {
        val << ((unsigned char*)(var))[0];
        outFile->SetStr(ans, val.str(), cs);
        return;
    }
    if ((dts == "short") || (dts == "Short_t") || (dts == "I"))
    {
        val << ((short*)(var))[0];
        outFile->SetStr(ans, val.str(), cs);
        return;
    }
    if ((dts == "unsigned short") || (dts == "UShort_t") || (dts == "U"))
    {
        val << ((unsigned short*)(var))[0];
        outFile->SetStr(ans, val.str(), cs);
        return;
    }
    if ((dts == "int") || (dts == "Int_t") || (dts == "V"))
    {
        outFile->SetInt(ans, ((int*)(var))[0], cs);
        return;
    }
    if ((dts == "unsigned int") || (dts == "UInt_t") || (dts == "V"))
    {
        outFile->SetInt(ans, ((unsigned int*)(var))[0], cs);
        return;
    }
    if ((dts == "long long") || (dts == "Long64_t") || (dts == "K"))
    {
        outFile->SetInt(ans, ((long long*)(var))[0], cs);
        return;
    }
    if ((dts == "unsigned long long") || (dts == "ULong64_t") || (dts == "W"))
    {
        val << ((unsigned long long*)(var))[0];
        outFile->SetStr(ans, val.str(), cs);
        return;
    }
    if ((dts == "float") || (dts == "TFloat_t") || (dts == "E"))
    {
        outFile->SetFloat(ans, ((float*)(var))[0], cs);
        return;
    }
    if ((dts == "double") || (dts == "TDouble_t") || (dts == "D"))
    {
        outFile->SetFloat(ans, ((double*)(var))[0], cs);
        return;
    }
    if ((dts == "char*") || (dts == "A"))
    {
        outFile->SetStr(ans, string((char*)(var)), cs);
        return;
    }
    //trigger error
    *fLog << err << "Format string not recognized while adding header entry " << ans << " with type " << dts << endl;
}
// --------------------------------------------------------------------------
//                                                                           
// This method is called when new data should be written to the FITS table.  
// In general only one row is written. (There are exceptions for Arrays).    
// A new row to a FITS table is written if at least for one container        
// (one sub-table) the ReadyToSave - flag is set.                            
//                                                                           
Bool_t MWriteFitsFile::CheckAndWrite()
{
//   try {

      // loop over all tables
      map<TString, map<TString, MFitsSubTable> >::iterator i_table =
         fSubTables.begin();
      while (i_table != fSubTables.end())
         {
         // is this table open?
         if (fFitsTables.find(i_table->first) != fFitsTables.end())
            {

            // loop over all sub-tables
            map<TString, MFitsSubTable>::iterator i_subTable = i_table->second.begin();
            while (i_subTable != i_table->second.end())
               {

               if (i_subTable->second.GetContainer()->IsReadyToSave())
                  {
                  // first write the TClonesArray and set the size of the arrays
                  list<MArrayHelperBase*> & clHelper = fClHelper[i_table->first];
                  list<MArrayHelperBase*>::iterator i_clHelper = clHelper.begin();
                  while (i_clHelper != clHelper.end())
                     {
                     // write all entries in the TClonesArray in its FITS table
  //removed this write because I do it elsewhere. is that alright ?
 //                    (*i_clHelper)->Write();
                     i_clHelper++;
                     }

                  // write one line to this table
                  writeOneRow(i_table->first);
//                  fFitsTables[i_table->first].Write();
                  break;
                  }
               i_subTable++;
               }
            }

         i_table++;
         }
//      }
//   catch (exception &e)
 //     {
 //     *fLog << err << e.what() << endl;
 //     return kFALSE;
 //     }

   return kTRUE;
}

string MWriteFitsFile::Trim(const string &str)
{
    // Trim Both leading and trailing spaces
    const size_t first = str.find_first_not_of(' '); // Find the first character position after excluding leading blank spaces
    const size_t last  = str.find_last_not_of(' ');  // Find the first character position from reverse af

    // if all spaces or empty return an empty string
    if (string::npos==first || string::npos==last)
        return string();

    return str.substr(first, last-first+1);
}

Bool_t MWriteFitsFile::VetoColumn(const std::string& colName)
{
    for (std::vector<string>::iterator it=fVetoedColumns.begin(); it != fVetoedColumns.end(); it++)
        if (*it == colName)
        {
            *fLog << warn << "Warning: column " << colName << " is being vetoed twice" << endl;
            return kFALSE;
        }
    fVetoedColumns.push_back(colName);
    return kTRUE;
}

Bool_t MWriteFitsFile::SetBytesPerSample(const std::string& colName, uint32_t numBytes)
{
    for (map<string, uint32_t>::iterator it=fBytesPerSamples.begin(); it!=fBytesPerSamples.end(); it++)
        if (it->first == colName)
        {
            *fLog << warn << "Warning: column " << colName << " bytes per sample is being redefined twice" << endl;
            return kFALSE;
        }
    if (numBytes != 1 && numBytes != 2 && numBytes != 4 && numBytes != 8)
    {
        *fLog << warn << "Only powers of two are allowed for types re-mapping." << endl;
        return kFALSE;
    }
    fBytesPerSamples[colName] = numBytes;
    return kTRUE;
}

// --------------------------------------------------------------------------
//                                                                           
// Initialize all columns in "fitsTable" of the class "classDef". The data   
// of this class are stored in a buffer, beginning at "baseAdr".             
//                                                                           
Bool_t MWriteFitsFile::InitColumns(const TString & tableName, 
                                   const TString & parentVarName,
                                   ofits* fitsTable,
                                   void * baseAdr,
                                   TClass * classDef)
{
    // get all data members of the class
   TList * dataMembers = classDef->GetListOfDataMembers();
   TIter next(dataMembers);
   TDataMember * dataMember;

   // loop over all data members
   while ((dataMember = (TDataMember*)next()) != NULL)
      {
      if (!dataMember->IsPersistent())
         // don't store this variable
         continue;

      if (dataMember->Property() & ( G__BIT_ISENUM | G__BIT_ISSTATIC))
         // we cannot store this
         continue;

      if (strcmp(dataMember->GetTrueTypeName(), "TClass*") == 0)
         // we don't want to store this. 
         continue;

      // is it an array of more than 1 dimension?
      if (dataMember->GetArrayDim() > 1)
         {
         *fLog << err << "Two and more dimensional arrays of member variables"
                         " are not supported." << endl;
         *fLog <<        "See variable " << dataMember->GetName() << 
                         " in container " << classDef->GetName() <<  endl;
         return kFALSE;
         }


      // replace � by **2 in the comment field
      string comment(dataMember->GetTitle());
      string::size_type pos1, pos2;
      if ((pos1 = comment.find(178)) != string::npos)
         comment.replace(pos1, 1, "**2");

      // get the optional mapping to the fits column names
      string fitsOptions="";
      string unit="unit";

      pos1 = comment.find("{fits: ");
      pos2 = comment.find('}');
      std::map<string, string> fitsTokens;
      if (pos1 != string::npos && pos2 != string::npos)
      {
          fitsOptions=comment.substr(pos1+7, (pos2-pos1)-7);
          //do we have more than one option ?
          string::size_type pos3= fitsOptions.find_first_of(';');
          string::size_type pos4 = string::npos;
          string key="";
          string value="";
          string keyValue="";
          while (pos3 != string::npos)
          {//we have at least 2 options left
 //             *fLog << err << "fitsOptions: " << fitsOptions << endl;
              keyValue = fitsOptions.substr(0,pos3);
 //             *fLog << err << "keyValue: " << keyValue << endl;
              pos4 = keyValue.find('=');
              if (pos4 == string::npos)
              {
                  *fLog << err << "Error while parsing comment \"" << comment << "\" from variable " << parentVarName + dataMember->GetName() << endl;
                  return kFALSE;
              }
              key = Trim(keyValue.substr(0, pos4));
              value = Trim(keyValue.substr(pos4+1, pos3));
              fitsTokens[key] = value;
 //             *fLog << err << "key: " << key << " value: " << value << endl;
              fitsOptions = fitsOptions.substr(pos3+1, fitsOptions.size());
              pos3 = fitsOptions.find_first_of(';');
          }
//          *fLog << err << "fitsOptions: " << fitsOptions << endl;
          keyValue = fitsOptions;
          pos4 = keyValue.find('=');
          if (pos4 == string::npos)
          {
              *fLog << err << "Error while parsing comment \"" << comment << "\" from variable " << parentVarName + dataMember->GetName() << endl;
              return kFALSE;
          }
          key = Trim(keyValue.substr(0, pos4));
          value = Trim(keyValue.substr(pos4+1, pos3));
          fitsTokens[key] = value;
//          *fLog << err << "key: " << key << " value: " << value << endl;
      }

      TString colName = parentVarName + dataMember->GetName();

      if (fitsTokens.find("unit") != fitsTokens.end())
          unit = fitsTokens["unit"];
      if (fitsTokens.find("name") != fitsTokens.end())
          colName = fitsTokens["name"];

      //check for column veto
      if (std::find(fVetoedColumns.begin(), fVetoedColumns.end(), colName.Data())!=fVetoedColumns.end())
      {
          *fLog << inf << "Vetoing column " << colName.Data() << endl;
          continue;
      }

      // set the array size
      TString dataType = dataMember->GetTrueTypeName();

      uint32_t dataLength = 0;
      switch (dataMember->GetArrayDim())
      {
          case 0:
              dataLength = 1;
              break;
          case 1:
              dataLength = dataMember->GetMaxIndex(0);
              break;
          default:
              *fLog << err << "n-dimensional array should have been discarded already " << colName.Data();
              break;
      };
//      if (dataLength == 0)
//          continue;

      if (dataMember->Property() & G__BIT_ISCLASS)
         {
         // special treatment for classes
          uint32_t arraySize = 0;
          TString typeName;

          if (strncmp(dataMember->GetTrueTypeName(), "MArray", 6) == 0)
          {
              if (strcmp(dataMember->GetTrueTypeName(), "MArrayS*") == 0)
              {
                  typeName = "UShort_t";
                  arraySize = (*((MArrayS**)((char*)baseAdr + dataMember->GetOffset())))->GetSize();
                  InitSingleColumn(tableName,
                                   arraySize,
                                   typeName.Data(),
                                   (char*)(*((MArrayS**)((char*)baseAdr + dataMember->GetOffset())))->GetArray(),
                                   colName.Data(),
                                   unit,
                                   comment);
              }
              else if (strcmp(dataMember->GetTrueTypeName(), "MArrayB*") == 0)
              {
                      typeName = "UChar_t";
                      arraySize = (*((MArrayB**)((char*)baseAdr + dataMember->GetOffset())))->GetSize();
                      InitSingleColumn(tableName,
                                     arraySize,
                                     typeName.Data(),
                                     (char*)(*((MArrayB**)((char*)baseAdr + dataMember->GetOffset())))->GetArray(),
                                     colName.Data(),
                                     unit,
                                     comment);
              }
              else if (strcmp(dataMember->GetTrueTypeName(), "MArrayF*") == 0)
              {
                      typeName = "TFloat_t";
                      arraySize = (*((MArrayF**)((char*)baseAdr + dataMember->GetOffset())))->GetSize();
                      InitSingleColumn(tableName,
                                     arraySize,
                                     typeName.Data(),
                                     (char*)(*((MArrayF**)((char*)baseAdr + dataMember->GetOffset())))->GetArray(),
                                     colName.Data(),
                                     unit,
                                     comment);
              }

              else {
                  *fLog << err << dataMember->GetTrueTypeName() << " not yet implemented." << endl;
                  return kFALSE;
              }


              continue;
          }
         else if (strcmp(dataMember->GetTrueTypeName(), "TClonesArray") == 0)
            {
             *fLog << warn << "I'm skipping the TClonesArray for now" << endl;
             continue;
            // each TClonesArray requires a FITS table by itself.
            MClonesArrayHelper * clHelper;

            TClonesArray * cloneArray = (TClonesArray*)((char*)baseAdr + dataMember->GetOffset());
            Bool_t status;
            clHelper = new MClonesArrayHelper(cloneArray, fLog, status);
            if (!status)   return status;

            fClHelper[tableName].push_back(clHelper);

            // add one column in the parent table of the TClonesArray to store the  
            // number of entries in the TClonesArray.
            InitSingleColumn(tableName,
                             1,
                             "UInt_t",
                             clHelper->GetArraySizePtr(),
                             colName.Data(),
                             unit,
                             comment);

            // initialize the columns of the new FITS table, which will store the   
            // data entries of the TClonesArray                                     
            if (InitColumns(TString("noName"),
                            colName + "_",
                            fitsTable,
                            clHelper->GetDataBuffer(),
                            cloneArray->GetClass())
                    == kFALSE)
               return kFALSE;

            // the columns are initialized. We can create the FITS table
            if (clHelper->OpenFitsTable(GetFileName(), dataMember->GetName(), 
                                       fOpenOption, fLog)                        == kFALSE)
               return kFALSE;
            }

         else
            {
            // the current container has a variable of an other class. We create 
            // also columns of this other class in the same table                
            TClass * newClassDef = TClass::GetClass(dataMember->GetTrueTypeName(), kFALSE, kTRUE);
            if (newClassDef)
               {
               if (InitColumns(tableName, colName + ".", fitsTable, (char*)baseAdr + dataMember->GetOffset(),
                        newClassDef) == kFALSE)
                  return kFALSE;
               }
            else
               *fLog << warn << "Cannot write data of class " << colName + "." + dataMember->GetTrueTypeName() << endl;
            }
         continue;
         } 

         InitSingleColumn(tableName,
                          dataLength,
                          dataType.Data(),
                          (char*)baseAdr + dataMember->GetOffset(),
                          colName.Data(),
                          unit,
                          comment);

      }
   return kTRUE;
}
void MWriteFitsFile::InitSingleColumn(const TString& tableName,
                                      uint32_t count,
                                      const string& typeName,
                                      void* dataPointer,
                                      const string& columnName,
                                      const string& unit,
                                      const string& comment)
{
    if (!fTableObjectCreated[tableName])
    {
        *fLog << err << "ERROR: Cannot init column " << columnName << " before assigning object: " << tableName << endl;
        return;
    }
    int typeFound = 0;
    char typeChar = '0';
    if ((typeName == "bool") || (typeName == "Bool_t") || (typeName == "L"))
    {
        typeChar = 'L';
        typeFound++;
    }
    if ((typeName == "char") || (typeName == "Char_t") || (typeName == "S"))
    {
        typeChar = 'A';
        typeFound++;
    }
    if ((typeName == "unsigned char") || (typeName == "UChar_t") || (typeName == "B"))
    {
        typeChar = 'A';
//        *fLog << warn << "Converting unsigned char to char in fits file" << endl;
        typeFound++;
    }
    if ((typeName == "short") || (typeName == "Short_t") || (typeName == "I"))
    {
        typeChar = 'I';
        typeFound++;
    }
    if ((typeName == "unsigned short") || (typeName == "UShort_t") || (typeName == "U"))
    {
        typeChar = 'I';
//        *fLog << warn << "Converting unsigned short to short in fits file" << endl;
        typeFound++;
    }
    if ((typeName == "int") || (typeName == "Int_t") || (typeName == "V"))
    {
        typeChar = 'J';
        typeFound++;
    }
    if ((typeName == "unsigned int") || (typeName == "UInt_t") || (typeName == "V"))
    {
        typeChar = 'J';
//        *fLog << warn << "Converting unsigned int to int in fits file" << endl;
        typeFound++;
    }
    if ((typeName == "long long") || (typeName == "Long64_t") || (typeName == "K"))
    {
        typeChar = 'K';
        typeFound++;
    }
    if ((typeName == "unsigned long long") || (typeName == "ULong64_t") || (typeName == "W"))
    {
        typeChar = 'K';
//        *fLog << warn << "Converting unsigned long to long in fits file" << endl;
        typeFound++;
    }
    if ((typeName == "float") || (typeName=="TFloat_t") || (typeName == "E"))
    {
        typeChar = 'E';
        typeFound++;
    }
    if ((typeName == "double") || (typeName == "TDouble_t") || (typeName == "D"))
    {
        typeChar = 'D';
        typeFound++;
    }
//    if ((typeName == "char*") || (typeName == "A"))
//    {
//        typeFound++;
//    }
    if (typeFound != 1)
    {
        *fLog << err << "We have a problem with the data type: " << typeName << endl;
        return;
    }
    uint32_t colWidth = 0;
    switch (typeChar)
    {
        case 'L':  colWidth = 1*count; break;
        case 'A':  colWidth = 1*count; break;
        case 'B':  colWidth = 1*count; break;
        case 'I':  colWidth = 2*count; break;
        case 'J':  colWidth = 4*count; break;
        case 'K':  colWidth = 8*count; break;
        case 'E':  colWidth = 4*count; break;
        case 'D':  colWidth = 8*count; break;
        default:
            *fLog << err << "ERROR: typeChar could not be resolved to an actual type" << endl;
    };
    //check for type remapping here
    if (fBytesPerSamples.find(columnName) != fBytesPerSamples.end())
    {
        if (typeChar != 'A')
        {
            *fLog << err << "Attempt to remap type " << typeChar << " to " << fBytesPerSamples[columnName] << " is only allowed on bytes (variable name: " << columnName << "). Ignoring column" << endl;
            return;
        }
        uint32_t bytesPerSample = fBytesPerSamples.find(columnName)->second;
        if (colWidth%bytesPerSample != 0)
        {
            *fLog << err << "Type remapping cannot be done using " << bytesPerSample << " bytes per sample on an array of char of size " << colWidth << ". Ignoring column " << columnName << endl;
            return;
        }
        switch (bytesPerSample)
        {
            case 1: count = count/1; typeChar = 'A'; break;
            case 2: count = count/2; typeChar = 'I'; break;
            case 4: count = count/4; typeChar = 'J'; break;
            case 8: count = count/8; typeChar = 'K'; break;
            default:
                *fLog << err << "ERROR: num bytes per sample = " << bytesPerSample << " should have been forbidden already" << endl;
        }

    }

    fDataPointers[tableName].push_back(dataPointer);
    fTypeChars[tableName].push_back(typeChar);
    fColSizes[tableName].push_back(count);
    fColWidth[tableName].push_back(colWidth);

    //FIXME ofits does not allow for much liberty regarding the size of the column names.
    //Truncating them badly here, will probably cause other problems -> Modify ofits.h instead
    string truncatedName=columnName.substr((columnName.size()>40)?columnName.size()-40:0,columnName.size());
    string truncatedComment = comment.substr((comment.size()>10)?comment.size()-10:0,comment.size());
//    *fLog << warn << "In table " << tableName << " Adding column |" << truncatedName << "| |" << truncatedComment << "| |" << count << "| |" << typeChar;
//    *fLog << warn << "| Real: "<< columnName << " comment: " << comment << endl;
    fFitsTables[tableName]->AddColumn(count, typeChar, truncatedName, unit, truncatedComment);
}

void MWriteFitsFile::writeOneRow(const TString& tableName)
{
    if (!fTableHeaderWritten[tableName])
    {
        for (vector<ofits::Key>::const_iterator it = fHeaderKeys.begin(); it != fHeaderKeys.end(); it++)
            fFitsTables[tableName]->SetRaw(it->key, it->value, it->comment);
        fFitsTables[tableName]->WriteTableHeader(tableName.Data());
        fTableHeaderWritten[tableName] = true;
    }
    if (!fTableObjectCreated[tableName])
    {
        *fLog << err << "This is not good. Please initialize the fits table before writing to it: " << tableName << endl;
        return;
    }
    //first calculate the size of one row
    uint32_t rowWidth = 0;
    for (uint32_t i=0;i<fTypeChars[tableName].size();i++)
        rowWidth += fColWidth[tableName][i];
    unsigned char* tempBuffer = new unsigned char[rowWidth];
    //then copy the data to be written contiguously
    uint32_t bytesCounter = 0;
    for (uint32_t i=0;i<fDataPointers[tableName].size();i++)
    {
        memcpy(&tempBuffer[bytesCounter], fDataPointers[tableName][i], fColWidth[tableName][i]);
        bytesCounter+=fColWidth[tableName][i];
    }
    if (fFitsTables[tableName]->WriteRow(tempBuffer, bytesCounter) == false)
        *fLog << err << "Error while writing to FITS table " << tableName << endl;
    else
        fFitsTables[tableName]->FlushNumRows();

    delete[] tempBuffer;
}
Bool_t MWriteFitsFile::ReInit(MParList *pList)
{
   if (fRule.Length() == 0)
      // there is not rule defined. We keep the old file
      return MWriteFile::ReInit(pList);

   MRead *read = (MRead*)pList->FindTask("MRead");
   if (!read)
   {
       *fLog << err;
       *fLog << "ERROR: No Task 'MRead' found in the tasklist.  This task is" << endl;
       *fLog << "  necessary to  get the filename.  Without a read-filename" << endl;
       *fLog << "  no output-filename can be created... abort." << endl;
       *fLog << endl;
       return kFALSE;
   }


   // close the current files
   CloseTopLevelGroup();
   for (std::map<TString,ofits*>::iterator it=fFitsTables.begin(); it!=fFitsTables.end(); it++)
   {
       (it->second)->close();
       delete it->second;
   }
   fFitsTables.clear();
   fDataPointers.clear();
   fTypeChars.clear();
   fColSizes.clear();
   fColWidth.clear();
   fTableObjectCreated.clear();
   fTableHeaderWritten.clear();
   DeleteArrayHelper();
   fClHelper.clear();

   // get new filename
   const TString readFileName = read->GetFullFileName();
   const TString newname = MWriteRootFile::SubstituteName(fRule, readFileName);

   // create new files
   OpenTopLevelGroup(newname.Data());
   if (!IsFileOpen())
      return kFALSE;


   MRawRunHeader* header = (MRawRunHeader*)pList->FindObject("MRawRunHeader");
   SetupHeaderKeys(*header);

   if (GetContainer(pList) == kFALSE)
      return kFALSE;

   // do, what has to be done in ReInit.
   return MWriteFile::ReInit(pList);

}

void MWriteFitsFile::DeleteArrayHelper()
{
   map<TString, list<MArrayHelperBase *> >::iterator i_helper1 = fClHelper.begin();
   while (i_helper1 != fClHelper.end())
      {
      list<MArrayHelperBase *>::iterator i_helper2 = i_helper1->second.begin();
      while(i_helper2 != i_helper1->second.end())
         {
         delete *i_helper2;

         i_helper2++;
         }

      i_helper1++;
      }
}
