#ifndef ROOT_MBasketSQL
#define ROOT_MBasketSQL

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// MBasketSQL                                                           //
//                                                                      //
// Implement TBasket for a SQL backend                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TBasket.h"

class TSQLRow;

class MBasketSQL : public TBasket
{
private:
   MBasketSQL(const MBasketSQL&);            // MBasketSQL objects are not copiable.
   MBasketSQL& operator=(const MBasketSQL&); // MBasketSQL objects are not copiable.

protected:
   TSQLRow **fRowPtr; //!

public:
   MBasketSQL();
   MBasketSQL(TBranch *b, TSQLRow **r, Int_t index, TString type);

   void PrepareBasket(Long64_t entry);

   virtual Int_t ReadBasketBuffers(Long64_t pos, Int_t len, TFile *file);
   virtual Int_t ReadBasketBytes(Long64_t pos, TFile *file);

   void Update(Int_t offset, Int_t skipped);

   ClassDef(MBasketSQL,1)  //the TBranch buffers

};

#endif
