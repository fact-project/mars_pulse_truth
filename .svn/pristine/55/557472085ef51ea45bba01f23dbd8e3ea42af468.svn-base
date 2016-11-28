#include "MAGIC.h"
#include "MLogManip.h"

#include "MFitsArray.h"

#include <TDataMember.h>
#include <TClonesArray.h>
#include <TClass.h>
#include <TList.h>

using namespace std;

// ----------------------------------------------------------------------------
//                                                                             
// Opens the FITS table of this TClonesArray. This should be done after all    
// columns are initialized. baseFileName is the name of the group, this        
// children table belongs to. dataName is the name of the TClonesArray         
// variable in the MParContainer - class.                                      
//                                                                             
Bool_t MArrayHelperBase::OpenFitsTable(const char * baseFileName, 
                                         const char * dataName, 
                                         const char * option, MLog * log )
{
   // get the baseFileName without extension                     
   char fileNameNoExt[strlen(baseFileName) + 1];
   strcpy(fileNameNoExt, baseFileName);
   // remove the extension                                       
   char * pos = strrchr(fileNameNoExt, '.');
   if (pos) *pos = 0;

   // add the dataName to the file name define the table name:   
   // result will be something like:                             
   // baseFileName_dataName.fits[dataName]                       
   TString dol = fileNameNoExt;
   TString fileName;
   dol += "_";
   dol += dataName;
   dol += ".fits";
   fileName = dol;
//   dol += "[";
//   dol += dataName;
//   dol += "]";

//   try {

      if (strcmp(option, "RECREATE") == 0)
         // remove the file
         unlink(fileName.Data());

      // create the file with the FITS table and all its columns    
      //FIXME I must implement a constructor and set fFitsTable to NULL
      //Otherwise I cannot safely destroy the object (in the destructor, also to be implemented)
      fFitsTable = new ofits();
      fFitsTable->open(dol.Data());

//      }
//   catch (exception & e)
//      {
//      *log << err << e.what() << endl;
//      return kFALSE;
 //     }
   return kTRUE;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
//                                                                             
// Determines the size of the buffer to store all data of the clonesArray -    
// Class, which should be written to a FITS table. The buffer is allocated.    
//                                                                             
// status is set to kFALSE if some data cannot be stored in a FITS table,      
// for example more than 1 dimensional arrays.                                 
MClonesArrayHelper::MClonesArrayHelper(TClonesArray * clonesArray, MLog * log, 
                                       Bool_t & status)
  :  fClonesArray(clonesArray) 
{
   status = kTRUE;

   // get size of buffer
   fDataBufferSize = 0;
   fStartOfData    = 0x7fffffff;

   TList * dataMembers = fClonesArray->GetClass()->GetListOfDataMembers();
   TIter next(dataMembers);
   TDataMember * dataMember;

   while ((dataMember = (TDataMember*)next()) != NULL)
      {
      if (!dataMember->IsPersistent())
         continue;

      if (dataMember->Property() & ( G__BIT_ISENUM | G__BIT_ISSTATIC))
         continue;

      if (strcmp(dataMember->GetTrueTypeName(), "TClass*") == 0)
         continue;

      // is it an array of more than 1 dimension?
      if (dataMember->GetArrayDim() > 1)
         {
         *log << err << "Two and more dimensional arrays of member variables"
                         " are not supported." << endl;
         *log <<        "See variable " << dataMember->GetName() << 
                         " in container " << fClonesArray->GetClass()->GetName() <<  endl;
         status = kFALSE;
         return;
         }


      // get the position, where the current variable ends.
      Int_t endPos;
      if (dataMember->GetArrayDim() == 1)
         endPos = dataMember->GetOffset() + (dataMember->GetUnitSize() * 
                                             dataMember->GetMaxIndex(0)  );
      else
         endPos = dataMember->GetOffset() + dataMember->GetUnitSize();

      if (endPos > fDataBufferSize)
         fDataBufferSize = endPos;

      if (dataMember->GetOffset() < fStartOfData)
         fStartOfData = dataMember->GetOffset();
      }      

   fDataBuffer = new Byte_t[fDataBufferSize];

}

// ----------------------------------------------------------------------------
//                                                                             
// the fClonesArray, the size of the data buffer and the fits Table are        
// copied. A new buffer is allocated. Note: the fArraySize is not copied.      
// Anyhow only its pointer is needed, which will change during this copy       
//                                                                             
MClonesArrayHelper::MClonesArrayHelper(const MClonesArrayHelper & clHelper)
   : fClonesArray(clHelper.fClonesArray)
{
   fFitsTable       = clHelper.fFitsTable;
   fDataBufferSize  = clHelper.fDataBufferSize;
   fStartOfData     = clHelper.fStartOfData;
   fDataBuffer      = new Byte_t[fDataBufferSize];
}



// ----------------------------------------------------------------------------
//                                                                             
// Writes all currently stored classes in the fClonesArray, i. e. writes 0 to  
// may rows to the FITS table.                                                 
// fArraySize is updated with the number of new row, written to the FITS       
// table. Therefore the parent FITS table must be updated after calling this   
// method.                                                                     
//                                                                             
void MClonesArrayHelper::Write()
{
   // get number of classes in fClonesArray.                                 
   fArraySize = fClonesArray->GetLast() + 1;

   for (UInt_t idx = 0; idx < fArraySize; idx++)
      {
      // copy the data of one class to fDataBuffer                           
      memcpy(fDataBuffer, (*fClonesArray)[idx], fDataBufferSize);

      // now the data can be written from the fDataBuffer to the FITS table  
      fFitsTable->WriteRow(fDataBuffer, fDataBufferSize);
      }
}

// ----------------------------------------------------------------------------
//                                                                             
// resize the clone array (fClonesArray) and reads fArraySize data sets from   
// the FITS file into the fClonesArray.                                        
// The number of data sets must be determined before calling this function,    
// i.e. the parent FITS table must be read before calling this method.         
Int_t MClonesArrayHelper::Read()
{
 /*  fClonesArray->ExpandCreate(fArraySize);

   for (int idx = 0; idx < fArraySize; idx++)
      {
      // read the data in the buffer
      if (!fFitsTable.Read())
         {
         gLog << "Expecting more entries in " << fFitsTable.GetFileName() << endl;
         return kERROR;
         }
      // copy the data of one class from fDataBuffer into the class
      memcpy( (Byte_t*)((*fClonesArray)[idx]) + fStartOfData, 
              fDataBuffer + fStartOfData, fDataBufferSize - fStartOfData);
      }
*/
   return kTRUE;
}
