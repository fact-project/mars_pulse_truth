#include "MIslands.h"

#include <TList.h>

#include "MLog.h"
#include "MLogManip.h"

#include "MImgIsland.h"

#include "MCerPhotPix.h"
#include "MCerPhotEvt.h"


using namespace std;

// --------------------------------------------------------------------------
//
// Default constructor.
//
MIslands::MIslands(const char *name, const char *title)
{
    fName  = name  ? name  : "MIslands";
    fTitle = title ? title : "Storage container for the island information of one event";

    fIslands = new TList;
    fIslands->SetOwner();    
}

// --------------------------------------------------------------------------
//
// Destructor.
//
MIslands::~MIslands()
{
   fIslands->Delete();
}


// --------------------------------------------------------------------------
//
// Get i-th
//

MImgIsland &MIslands::operator[] (Int_t i)
{
  MImgIsland& isl = *static_cast<MImgIsland*>(fIslands->At(i));
  return isl;
}

// --------------------------------------------------------------------------
//
// Get i-th
//

const MImgIsland &MIslands::operator[] (Int_t i) const
{
    return *static_cast<MImgIsland*>(fIslands->At(i));
}


// --------------------------------------------------------------------------
//
// Print the island parameters to *fLog
//
 
void MIslands::Print(Option_t *opt) const
{
  *fLog << all;
  *fLog << "Island Parameters (" << GetName() << ")"  << endl;

  TIter Next(fIslands);
  MImgIsland* isl;
  UInt_t islnum = 0;
  while ((isl=(MImgIsland*)Next())) 
    {
      *fLog << inf << "*** Island #" << islnum << " parameters ***" << endl;
      isl->Print();
      islnum++;
    }  
  
}


