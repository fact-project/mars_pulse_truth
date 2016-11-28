#ifndef MARS_MFitsArray
#define MARS_MFitsArray

//#include "astroroot.h"
//#include "fitsio.h"
#ifndef MARS_fits
#include "fits.h"
#endif

#ifndef MARS_ofits
#include "ofits.h"
#endif

class TClonesArray;
class MLog;

///////////////////////////////////////////////////////////////////////////////
// A base class for arrays, which has to be written into their own FITS table  
class MArrayHelperBase
{
protected:

   // the FITS table to which the data are written                      
   ofits*     fFitsTable;
   
   // number of elements in fClonesArray / MArrayX / TArrayX  during 
   // one write operation. This number is written into a column of   
   // the parent FITS table.                                         
   UInt_t           fArraySize;

public:
    virtual ~MArrayHelperBase() { };

   ofits* GetFitsTable()     {return fFitsTable;}
   UInt_t *      GetArraySizePtr()  {return &fArraySize;}

   Bool_t  OpenFitsTable(const char * baseFileName, const char * dataName, 
                         const char * option, MLog * log);

   virtual void  InitCol() {}

   virtual Int_t Read() = 0;
   virtual void  Write() = 0;

};

///////////////////////////////////////////////////////////////////////////////
// A class to write data of one TClonesArray into one FITS table.              
class MClonesArrayHelper : public MArrayHelperBase
{
   // data of this TClonesArray are written to fFitsTable               
   TClonesArray *  fClonesArray;  



   // fFitsTable writes during one fFitsTable.Write() call data which   
   // are currently stored in this buffer. That means the data pointers 
   // of fFitsTable for each column point to this data buffer.          
   Byte_t *        fDataBuffer;

   // the size of fDataBufferer.                                        
   Int_t           fDataBufferSize;

   // the first used byte in fDataBuffer                                
   Int_t           fStartOfData;

public:
   MClonesArrayHelper(TClonesArray * clonesArray, MLog * log, Bool_t & status);
   MClonesArrayHelper(const MClonesArrayHelper & clHelper);

   ~MClonesArrayHelper()   {delete [] fDataBuffer;}

   void  *       GetDataBuffer()    {return fDataBuffer;}

   Int_t   Read();
   void    Write();

};

///////////////////////////////////////////////////////////////////////////////
// Classes to write and read data of MArray* classes                           
template<class BaseT, class ClassT>
   class MArrayHelper : public MArrayHelperBase
{
   ClassT   *  fArrayClass;

   BaseT       fDataBuffer;
   char        fDataType[10];

public:
   MArrayHelper(ClassT * arrayClass, const char * dataType)
      {fArrayClass = arrayClass;
       strcpy(fDataType, dataType);}

   void    InitCol()
   {
       //FIXME ETIENNE HERE
//       fFitsTable.InitCol("Data", fDataType, &fDataBuffer);
//       fFitsTable->AddColumn(
//       fFitsTables[tableName]->AddColumn(count, typeChar, truncatedName, unit, truncatedComment);

   }

   Int_t   Read() { return 0;}
   void    Write()
   {
       fArraySize = fArrayClass->GetSize();
       for (UInt_t idx = 0; idx < fArraySize; idx++)
       {
          fDataBuffer = (*fArrayClass)[idx];
//FIXME ETIENNE HERE
          //          fFitsTable.Write();
       }
   }


};

#endif

