#ifndef MARS_MDataFormat
#define MARS_MDataFormat

#ifndef MARS_MAGIC
#include "MAGIC.h"
#endif

#ifndef ROOT_Rtypes
#include <Rtypes.h>
#endif

#include <iosfwd>
#include <fstream>

class MCorsikaFormat
{
protected:
   std::istream  *fIn;

public:
    static const unsigned int kSyncMarker;
    static const unsigned int kMagicNumber;

   MCorsikaFormat(std::istream * in)
        : fIn(in) { }
   virtual ~MCorsikaFormat();

   virtual Bool_t NextBlock(Int_t   readState, Int_t & blockType, Int_t & blockVersion,
                            Int_t & blockIdentifier, Int_t & blockLength) const = 0;

           void   Seek(std::streampos offset)    {fIn->seekg(offset, std::ios::cur);}

   virtual Bool_t SeekEvtEnd() = 0;

   virtual Bool_t IsEventioFormat() const = 0;

   virtual Bool_t Eof() const;

           Bool_t Read(void *ptr, Int_t i) const;

   static MCorsikaFormat *CorsikaFormatFactory(const char *fileName);
};


class MCorsikaFormatRaw : public MCorsikaFormat
{
private:
   bool fHasMagicNumber;

public:
   MCorsikaFormatRaw(std::istream * in, bool h)
        : MCorsikaFormat(in), fHasMagicNumber(h) {}

   Bool_t NextBlock(Int_t   readState, Int_t & blockType, Int_t & blockVersion,
                    Int_t & blockIdentifier, Int_t & blockLength) const;

   Bool_t SeekEvtEnd();

   Bool_t IsEventioFormat() const {return kFALSE;}
};


class MCorsikaFormatEventIO : public MCorsikaFormat
{

public:
    MCorsikaFormatEventIO(std::istream *in)
        : MCorsikaFormat(in) {}


    Bool_t NextBlock(Int_t   readState, Int_t & blockType, Int_t & blockVersion,
                     Int_t & blockIdentifier, Int_t & blockLength) const;

    Bool_t SeekEvtEnd();

    Bool_t IsEventioFormat() const { return kTRUE; }

};

#endif

