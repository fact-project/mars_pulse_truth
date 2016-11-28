//////////////////////////////////////////////////////////////////////////
//                                                                      //
// MBasketSQL                                                           //
//                                                                      //
// Implement TBasket for a SQL backend                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef MARS_MBasketSQL
#define MARS_MBasketSQL

#include "MBasketSQL.h"

#include "TBasket.h"
#include "TTree.h"
#include "TBranch.h"
#include "MBufferSQL.h"

ClassImp(MBasketSQL)

namespace std {} using namespace std;

//_________________________________________________________________________
MBasketSQL::MBasketSQL() : TBasket(), fRowPtr(0)
{
   // Default constructor.
}

//_________________________________________________________________________
MBasketSQL::MBasketSQL(TBranch *branch, TSQLRow **sqlrow, Int_t index, TString type)
{
    // Create a TSQLBuffer for this basket.
    SetName(branch->GetName());
    SetTitle(branch->GetName());

    fClassName   = "MBasketSQL";

    fBufferSize  = branch->GetBasketSize();
    fNevBufSize  = branch->GetEntryOffsetLen();
    fNevBuf      = 0;
    fEntryOffset = 0;  //Must be set to 0 before calling Sizeof
    fDisplacement= 0;  //Must be set to 0 before calling Sizeof
    fBuffer      = 0;  //Must be set to 0 before calling Sizeof
    fBufferRef   = 0;

    fBufferRef = new MBufferSQL(sqlrow, index, type);

    fHeaderOnly  = kTRUE;
    fLast        = 0;
    //Streamer(*fBufferRef);
    fBuffer      = 0;
    fBranch      = branch;
    fHeaderOnly  = kFALSE;
    branch->GetTree()->IncrementTotalBuffers(fBufferSize);
}

//_________________________________________________________________________
void MBasketSQL::PrepareBasket(Long64_t entry)
{
    // Prepare the basket for the next entry.
    fBranch->GetTree()->LoadTree(entry);
}

//_________________________________________________________________________
Int_t MBasketSQL::ReadBasketBytes(Long64_t , TFile *)
{
    Error("ReadBasketBytes","This member function should not be called!");
    return 0;
}

//_________________________________________________________________________
Int_t MBasketSQL::ReadBasketBuffers(Long64_t , Int_t, TFile *)
{
    Error("ReadBasketBuffers","This member function should not be called!");
    return 0;
}

//_________________________________________________________________________
void MBasketSQL::Update(Int_t, Int_t) 
{
    // See TBasket::Update.
    fNevBuf++;
}

#endif
