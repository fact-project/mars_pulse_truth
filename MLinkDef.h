#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

// ==========================================================

// I/O
#pragma link C++ namespace Huffman+;
#pragma link C++ class Checksum+;
#pragma link C++ namespace FITS+;
#pragma link C++ struct FITS::Compression+;
#pragma link C++ enum FITS::CompressionProcess_t+;
#pragma link C++ enum FITS::RowOrdering_t+;
#pragma link C++ class izstream+;
#pragma link C++ class fits+;
#pragma link C++ class ofits+;
#pragma link C++ class zfits+;
#pragma link C++ class zofits+;
#pragma link C++ class factfits+;
#pragma link C++ class factofits+;

// mcore
#pragma link C++ class DrsCalibrateTime+;
#pragma link C++ class PixelMap+;
#pragma link C++ class PixelMapEntry+;
#pragma link C++ class BiasMap+;
#pragma link C++ class BiasMapEntry+;
#pragma link C++ class Interpolator2D+;
#pragma link C++ namespace FACT+;
#pragma link C++ function FACT::PredictI;

// ======================== libnova =========================

#pragma link C++ global LN_SOLAR_STANDART_HORIZON;

#pragma link C++ struct ln_lnlat_posn;
#pragma link C++ struct ln_hrz_posn;
#pragma link C++ struct ln_equ_posn;

// angular_separation
#pragma link C++ function ln_get_angular_separation;

// transform
#pragma link C++ function ln_get_hrz_from_equ;

// rise_set
#pragma link C++ struct ln_rst_time;

// solar
#pragma link C++ function ln_get_solar_rst;
#pragma link C++ function ln_get_solar_rst_horizon;
#pragma link C++ function ln_get_solar_equ_coords;

// lunar
#pragma link C++ function ln_get_lunar_equ_coords;
#pragma link C++ function ln_get_lunar_equ_coords_prec;
#pragma link C++ function ln_get_lunar_disk;
#pragma link C++ function ln_get_lunar_sdiam;
#pragma link C++ function ln_get_lunar_phase;
#pragma link C++ function ln_get_lunar_rst;
#pragma link C++ function ln_get_lunar_earth_dist;

// Nova wrapper
#pragma link C++ namespace Nova;
#pragma link C++ struct Nova::ZdAzPosn;
#pragma link C++ struct Nova::HrzPosn;
#pragma link C++ struct Nova::RaDecPosn;
#pragma link C++ struct Nova::EquPosn;

#endif
