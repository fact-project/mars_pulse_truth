#ifndef MARS_MTopFitsGroup
#define MARS_MTopFitsGroup

#include <sys/types.h>
#include <sys/stat.h>

#ifndef ROOT_TString
#include <TString.h>
#endif

#ifndef MARS_fits
#include "fits.h"
#endif

#ifndef MARS_ofits
#include "ofits.h"
#endif

///////////////////////////////////////////////////////////////////////////////
// An unique id of files. It is sortable, i. e. can be uses as a key in maps   
class MUniqueFileId
{
   dev_t       fst_dev;   // id of the device of this file                  
   ino_t       fst_ino;   // inode number of the file on the device fst_dev 

public:
  MUniqueFileId(dev_t st_dev, ino_t st_ino)
      : fst_dev(st_dev), fst_ino(st_ino) {}
   
   bool operator < (const MUniqueFileId & fileId) const
      { if (fst_dev == fileId.fst_dev)
           return fst_ino < fileId.fst_ino;
        return fst_dev < fileId.fst_dev;   }

};

///////////////////////////////////////////////////////////////////////////////
// All information of one open Top Level FITS group                            
class MTopFitsGroup
{  
   // filename of the group, but expanded by ROOT                         
   // ( see gSystem->ExpandPathName() )                                   
   TString     fFileName;  

   // the top level group as AstroROOT-2 data structure                   
   ofits*    fTopGroup;

   // number of usage of this group by different MWriteFitsFile instances 
   Int_t       fNumOpen;     

public:
   MTopFitsGroup(const char * fileName, ofits* file) : fFileName(fileName),
                                          fTopGroup(file),
                                          fNumOpen(1)
   {}

   void   IncreaseUsage()               {++fNumOpen;}
   void   DecreaseUsage()               {--fNumOpen;}
   Int_t  GetNumUsage()                 {return fNumOpen;}

   void   Attach(ofits* table);
   Bool_t GetNextChild(ofits* table);

   ofits*  GetGroup()        {return fTopGroup;}
   const TString &GetFileName() const {return fFileName;}

};

#endif

