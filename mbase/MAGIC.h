#ifndef MARS_MAGIC
#define MARS_MAGIC
///////////////////////////////////////////////////////////////////////////////
//
//     Magic.h
//
//     defines MAGIC base informations
//
///////////////////////////////////////////////////////////////////////////////
#ifndef ROOT_TROOT
#include <TROOT.h>
#endif

//
// Check here if Mars can be compiled with the present root version
//
#ifndef __CINT__
#if ROOT_VERSION_CODE < ROOT_VERSION(3,05,06)
#error Your root version is too old to compile Mars, use root>=3.05/06
#endif
#endif

#ifndef R__FOR_EACH
#define R__FOR_EACH ForEach
#endif


//
// Values for the eventloop control
//
const Int_t kCONTINUE = 2;
const Int_t kSKIP     = 2;
const Int_t kERROR    = 3;

const Double_t kRad2Deg = 180.0/3.1415926535897932384626433832795028841971693993751;

namespace MARS
{
    bool CheckRootVer();
    TString GetPackageName();
    TString GetPackageVersion();
    TString GetRevision();
}

// Check for example: http://www.utf8-zeichentabelle.de
namespace UTF8
{
    const char *const kCent        = "\xc2\xa2";
    const char *const kPound       = "\xc2\xa3";
    const char *const kYen         = "\xc2\xa5";
    const char *const kParagraph   = "\xc2\xa7";
    const char *const kCopyright   = "\xc2\xa9";
    const char *const kRegistered  = "\xc2\xae";
    const char *const kDeg         = "\xc2\xb0";
    const char *const kPlusMinus   = "\xc2\xb1";
    const char *const kSquare      = "\xc2\xb2";
    const char *const kThird       = "\xc2\xb3";
    const char *const kMu          = "\xc2\xb5";
    const char *const kDot         = "\xc2\xb7";
    const char *const kQuarter     = "\xc2\xbc";
    const char *const kHalf        = "\xc2\xbd";
    const char *const kSmaller     = "\xc2\xab";
    const char *const kLarger      = "\xc2\xbb";
    const char *const kThreeFourth = "\xc2\xbe";
    const char *const kTimes       = "\xc3\x97";
    const char *const kCapGamma    = "\xce\x93";
    const char *const kCapDelta    = "\xce\x94";
    const char *const kCapSigma    = "\xce\xa3";
    const char *const kCapTheta    = "\xce\x98";
    const char *const kCapLambda   = "\xce\x9b";
    const char *const kCapXi       = "\xce\x9e";
    const char *const kCapPi       = "\xce\xa0";
    const char *const kCapPhi      = "\xce\xa6";
    const char *const kCapPsi      = "\xce\xa8";
    const char *const kCapOmega    = "\xce\xa9";
    const char *const kAlpha       = "\xce\xb1";
    const char *const kBata        = "\xce\xb2";
    const char *const kGamma       = "\xce\xb3";
    const char *const kDelta       = "\xce\xb4";
    const char *const kEpsilon     = "\xce\xb5";
    const char *const kZeta        = "\xce\xb6";
    const char *const kEta         = "\xce\xb7";
    const char *const kTheta       = "\xce\xb8";
    const char *const kIota        = "\xce\xb9";
    const char *const kKappa       = "\xce\xba";
    const char *const kLambda      = "\xce\xbb";
    const char *const kNu          = "\xce\xbd";
    const char *const kXi          = "\xce\xbe";
    const char *const kOmicron     = "\xce\xbf";
    const char *const kPi          = "\xce\xc0";
    const char *const kRho         = "\xce\xc1";
    const char *const kSigma       = "\xce\xc3";
    const char *const kTau         = "\xce\xc4";
    const char *const kPhi         = "\xce\xc6";
    const char *const kChi         = "\xce\xc7";
    const char *const kPsi         = "\xce\xc8";
    const char *const kOmega       = "\xce\xc9";
    const char *const kEuro        = "\xe2\x82\xac";
}

#endif
