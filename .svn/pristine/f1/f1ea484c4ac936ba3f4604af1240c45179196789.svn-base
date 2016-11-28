/* ======================================================================== *\
!
! *
! * This file is part of MARS, the MAGIC Analysis and Reconstruction
! * Software. It is distributed to you in the hope that it can be a useful
! * and timesaving tool in analysing Data of imaging Cerenkov telescopes.
! * It is distributed WITHOUT ANY WARRANTY.
! *
! * Permission to use, copy, modify and distribute this software and its
! * documentation for any purpose is hereby granted without fee,
! * provided that the above copyright notice appear in all copies and
! * that both that copyright notice and this permission notice appear
! * in supporting documentation. It is provided "as is" without express
! * or implied warranty.
! *
!
!
!   Author(s): Antonio Stamerra  1/2003 <mailto:antono.stamerra@pi.infn.it>
!   Author(s): Marcos Lopez 1/2003 <mailto:marcos@gae.ucm.es>
!   Author(s): Nicola Galante 7/2003 <mailto:nicola.galante@pi.infn.it>
!
!   Copyright: MAGIC Software Development, 2000-2003
!
!
\* ======================================================================== */


/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// MMcTriggerLvl2                                                          //
//   Storage container for the 2nd level trigger selection parameters      //
//    as part of the 2nd level trigger simulation                          //
//                                                                         //
// input parameter:                                                        //
//    fCompactNN number of next neighboors that define a compact pixel     //
//                                                                         //
//                                                                         //
// Basic L2T Selection Parameters:                                         //
//                                                                         //
//    fLutPseudoSize number of compact pixels in one LUT                   //
//    fPseudoSize Multiplicity of the bigger cluster                       //
//    fSizeBiggerCell higher number of fired pixel in cell                 //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#include "MMcTriggerLvl2.h"

#include "MGeomCam.h"
#include "MGeomPix.h"
#include "MGeomCamMagic.h"

#include "MMcTrig.hxx"

#include "MMcEvt.hxx"

#include "MLog.h"

#include <TCanvas.h>

ClassImp(MMcTriggerLvl2);

using namespace std;

// ---------------------------
// Initialization of static const data members pixel_in_cell and pixel_in_lut

//
// Correspondence TABLE between pixel numbering in the trigger cells and
// the standard spiral counting 
// (*Note*: Pixels start to count ** from 1 ** instead of 0)
//
// This correspondence is valid only for MAGIC-like geometries!
//
//  FIXME!  These definitions should be included in a GeomTrig class
//
const  Int_t  MMcTriggerLvl2::gsPixelsInCell[gsNPixInCell][gsNCells] = {
    {26,  91,  66,  71,  76,  81,  86,  269,  224,  233,  242,  251,  260,  391,  336,  347,  358,	369,  380},
    {25,  61,  41,  45,  49,  53,  57,  215,  175,  183,  191,  199,  207,  325,  275,  285,  295,  305,  315},
    {24,  37,  22,  25,  28,  31,  34,  167,  132,  139,  146,  153,  160,  265,  220,  229,  238,  247,  256},
    {23,  19,   9,  11,  13,  15,  17,  125,   95,  101,  107,  113,  119,  211,  171,  179,  187,  195,  203},
    {27,  62,  67,  72,  77,  82,  87,  270,  225,  234,  243,  252,  261,  392,  337,  348,  359,  370,  381},
    {12,  38,  42,  46,  50,  54,  58,  216,  176,  184,  192,  200,  208,  326,  276,  286,  296,  306,  316},
    {11,  20,  23,  26,  29,  32,  35,  168,  133,  140,  147,  154,  161,  266,  221,  230,  239,  248,  257},
    {10,   8,  10,  12,  14,  16,  18,  126,   96,  102,  108,  114,  120,  212,  172,  180,  188,  196,  204},
    {22,   2,   3,   4,   5,   6,   7,   90,   65,   70,   75,   80,   85,  164,  129,  136,  143,  150,  157},
    {28,  93,  99, 105, 111, 117, 123,  271,  226,  235,  244,  253,  262,  393,  338,  349,  360,  371,  382},
    {13,  63,  68,  73,  78,  83,  88,  217,  177,  185,  193,  201,  209,  327,  277,  287,  297,  307,  317},
    { 4,  39,  43,  47,  51,  55,  59,  169,  134,  141,  148,  155,  162,  267,  222,  231,  240,  249,  258},
    { 3,  21,  24,  27,  30,  33,  36,  127,   97,  103,  109,  115,  121,  213,  173,  181,  189,  197,  205},
    { 9,   9,  11,  13,  15,  17,  19,   91,   66,   71,   76,   81,   86,  165,  130,  137,  144,  151,  158},
    {21,   3,   4,   5,   6,   7,   2,   61,   41,   45,   49,   53,   57,  123,   93,   99,  105,  111,  117},
    {29, 130, 137, 144, 151, 158, 165,  218,  227,  236,  245,  254,  263,  394,  339,  350,  361,  372,  383},
    {14,  94, 100, 106, 112, 118, 124,  170,  178,  186,  194,  202,  210,  328,  278,  288,  298,  308,  318},
    { 5,  64,  69,  74,  79,  84,  89,  128,  135,  142,  149,  156,  163,  268,  223,  232,  241,  250,  259},
    { 1,  40,  44,  48,  52,  56,  60,   92,   98,  104,  110,  116,  122,  214,  174,  182,  190,  198,  206},
    { 2,  22,  25,  28,  31,  34,  37,   62,   67,   72,   77,   82,   87,  166,  131,  138,  145,  152,  159},
    { 8,  10,  12,  14,  16,  18,   8,   38,   42,   46,   50,   54,   58,  124,   94,  100,  106,  112,  118},
    {20,  11,  13,  15,  17,  19,   9,   20,   23,   26,   29,   32,   35,   88,   63,   68,   73,   78,   83},
    {30, 131, 138, 145, 152, 159, 166,  219,  228,  237,  246,  255,  264,  329,  279,  289,  299,  309,  319},
    {15,  95, 101, 107, 113, 119, 125,  171,  179,  187,  195,  203,  211,  269,  224,  233,  242,  251,  260},
    { 6,  65,  70,  75,  80,  85,  90,  129,  136,  143,  150,  157,  164,  215,  175,  183,  191,  199,  207},
    { 7,  41,  45,  49,  53,  57,  61,   93,   99,  105,  111,  117,  123,  167,  132,  139,  146,  153,  160},
    {19,  23,  26,  29,  32,  35,  20,   63,   68,   73,   78,   83,   88,  125,   95,  101,  107,  113,  119},
    {37,  24,  27,  30,  33,  36,  21,   39,   43,   47,   51,   55,   59,   89,   64,   69,   74,   79,   84},
    {31, 132, 139, 146, 153, 160, 167,  220,  229,  238,  247,  256,  265,  270,  225,  234,  243,  252,  261},
    {16,  96, 102, 108, 114, 120, 126,  172,  180,  188,  196,  204,  212,  216,  176,  184,  192,  200,  208},
    {17,  66,  71,  76,  81,  86,  91,  130,  137,  144,  151,  158,  165,  168,  133,  140,  147,  154,  161},
    {18,  42,  46,  50,  54,  58,  38,   94,  100,  106,  112,  118,  124,  126,   96,  102,  108,  114,  120},
    {36,  43,  47,  51,  55,  59,  39,   64,   69,   74,   79,   84,   89,   90,   65,   70,   75,   80,   85},
    {32, 133, 140, 147, 154, 161, 168,  221,  230,  239,  248,  257,  266,  217,  177,  185,  193,  201,  209},
    {33,  97, 103, 109, 115, 121, 127,  173,  181,  189,  197,  205,  213,  169,  134,  141,  148,  155,  162},
    {35,  68,  73,  78,  83,  88,  63,   95,  101,  107,  113,  119,  125,   91,   66,   71,   76,   81,   86}
  };

//
// corrispondence between pixels in cell and lut (pix numbering starts from 1)
//
const Int_t MMcTriggerLvl2::gsPixelsInLut[gsNLutInCell][gsNPixInLut] = {
    { 1,  2,  3,  4,  6,  7,  8,  9, 12, 13, 14, 15},
    {34, 29, 23, 16, 30, 24, 17, 10, 25, 18, 11,  5},
    {35, 31, 26, 20, 19, 32, 27, 21, 36, 33, 28, 22},
  };


// --------------------------------------------------------------------------
//
//  Default constructor
//
MMcTriggerLvl2::MMcTriggerLvl2(const char *name, const char *title):fCompactNN(2),fTriggerPattern(0)
{
  fName = name ? name : ClassName();
  fTitle = title;

  *fLog << "created MMcTriggerLvl2" << endl;

  //
  // Initialization of the fPixels array to zero
  //
  memset (fPixels,0,gsNPixInCell*gsNCells*sizeof(Int_t));
  memset (fFiredPixel,0,gsNTrigPixels*sizeof(Int_t));

  // create a new camera
  SetNewCamera(new MGeomCamMagic);

}

// --------------------------------------------------------------------------
//
//  Destructor
//
MMcTriggerLvl2::~MMcTriggerLvl2()
{
  delete fGeomCam;
}

// --------------------------------------------------------------------------
//  
//  Print functions for debugging and testing
//
//  Options available:
//
// "cell":
//     Print out the pixel number of a given trigger cell
//
// "status":
//     Print a table with the status (pixel is fired or not after Lvl1) for 
//     all the pixels in all the trigger cells
//
// "check"
//     Print a warning message when no starting pixel is found in the 
//     CalcPseudoSize method.
//
//  no option:
//     Print the value of the selection parameters
//
//
void MMcTriggerLvl2::Print(Option_t *opt) const
{
  TString str(opt);

  if (str.Contains("status", TString::kIgnoreCase)) 
    {
      *fLog << "    Status of cells 1-9" <<endl;
      for(int i=0; i<gsNPixInCell; i++)
	{
	  for(int j=0; j<9; j++)
	    {
	      //      *fLog.width(3);
	      *fLog <<gsPixelsInCell[i][j]-1 << ":" << fPixels[i][j]  << "\t ";
	    } 
	  *fLog << endl;
	}      
    }
  else if (str.Contains("cell", TString::kIgnoreCase)) 
    {
      *fLog << "   Pixel numbering in cells 1-9" <<endl;
      for (int i=0;i<gsNPixInCell;i++)
	{
	  for(int j=0; j<9; j++)
	    {
	      *fLog << gsPixelsInCell[i][j]-1 << "\t";
	    }
	  *fLog << endl;
	}
    }
  else if (str.Contains("check", TString::kIgnoreCase)) 
    {
      // check when no starting pixels was found (<<--to be fixed)
      if (fPseudoSize < 0)
	*fLog << "  Warning: starting pixel not found. Pseudosize set to -1." << endl;
    }
  else 
    {
      *fLog << "  L2T selection parameters: " << endl;
      *fLog << "   - LutPseudoSize  = " << fLutPseudoSize <<  endl;
      *fLog << "   - CellPseudoSize  = " << fCellPseudoSize <<  endl;
      *fLog << "   - PseudoSize  = " << fPseudoSize <<  endl;
      *fLog << "   - BiggerCellSize = " << fSizeBiggerCell << endl;
      *fLog << "   - TriggerPattern = " << fTriggerPattern << endl;
    }
  
}

// --------------------------------------------------------------------------
//
// Take the information supplied by the Lvl1 (it reads from the object MMcTrig
// the status of all pixels after Lvl1) and pass it to the Lvl2 (fill the 
// array fPixels)
//
//
void MMcTriggerLvl2::SetLv1(MMcTrig *trig)
{
  if (!trig)
    return;

  fMcTrig = trig;

  for(int i=0; i<gsNPixInCell; i++)
    {
      for(int j=0; j<gsNCells; j++)
	{
	  int pixel = gsPixelsInCell[i][j]-1;
	  fPixels[i][j] = (fMcTrig->IsPixelFired(pixel,0)) ? 1 : 0;
	  fFiredPixel[pixel]=(fMcTrig->IsPixelFired(pixel,0)) ? 1 : 0;
	  //if (fFiredPixel[pixel]==1)
	  //*fLog << pixel<<",";
	}
    }
  //*fLog<<endl<<"Fine evento"<<endl;
}


// --------------------------------------------------------------------------
//
// Set the trigger status ( pixel fired(=1) or not(=0) ) manually for a given
// pixel of a given cell
//
//
void MMcTriggerLvl2::SetPixelFired(Int_t pixel, Int_t fired)
{
  for(int i=0; i<gsNPixInCell; i++)
    {
      for(int j=0; j<gsNCells; j++)
	{
	  if(gsPixelsInCell[i][j]-1==pixel) fPixels[i][j]=fired;
	}
    }

}

// --------------------------------------------------------------------------
//
//  Calculate the Level 2 Trigger (L2T) parameters. They can be used 
//  to select the events that have passed the L2T selection rule.
//
//
void MMcTriggerLvl2::Calc()
{

  // Find the Lut and cell with the higher LutPseudoSize. 
  int lutcell = CalcBiggerLutPseudoSize(); 
  fMaxCell = lutcell/10;
  int maxlut = lutcell-fMaxCell*10;
  fLutPseudoSize = GetLutCompactPixel(fMaxCell,maxlut);

  fMaxCell = CalcBiggerCellPseudoSize();  // fCellPseudoSize
  // fMaxCell is used by the PseudoSize to find the starting pixel
  CalcPseudoSize();

  fSizeBiggerCell = GetCellNumberFired(CalcBiggerFiredCell());

  //*fLog << "fLPS="<<fLutPseudoSize<<endl;
}



// --------------------------------------------------------------------------
//  
// For a given cell, just count how many pixels have been fired after Lvl1
//
Int_t MMcTriggerLvl2::GetCellNumberFired(int cell)
{
  int size=0;
 
  for(int i=0; i<gsNPixInCell; i++)
    {
      size += fPixels[i][cell];
    }

  return size;
  
}


// --------------------------------------------------------------------------
//  
//  Find the cell which the bigger number of fired pixels
// 
//
Int_t MMcTriggerLvl2::CalcBiggerFiredCell()
{
  int size=-1;
  int cell=-1;

  for(int j=0; j<gsNCells; j++)
    {
      if (GetCellNumberFired(j) > size) 
	{
	  size = GetCellNumberFired(j);
	  cell = j;
	}
    }
  
  //  *fLog << "size " <<size <<" in cell " << cell << endl;

  return cell;
  
}

// --------------------------------------------------------------------------
//  
// Calculate the higher LutPseudoSize of one event defined as the higher number
//   of compact pixel in the LUTs of the trigger cells.
//  neighpix is the number of Next-Neighbors which defines the compact pixel
//    topology (it can be 2-NN or 3-NN)
//
//   It returns the cell and the LUT with the bigger LutPseudoSize, coded
//      accordingly to:   cell*10 + LUT 
//
Int_t MMcTriggerLvl2::CalcBiggerLutPseudoSize()
{
  int size=0;
  int cell=-1;
  int lut=-1;
 
  for(int j=0; j<gsNCells; j++)
    {
      for(int i=0; i<gsNLutInCell; i++)
	{
	  if (GetLutCompactPixel(j,i) >= size) 
	    {
	      size = GetLutCompactPixel(j,i);
	      cell = j;
	      lut = i;
	    }      
	}
    }
  
  //*fLog <<"Max cell: " << cell+1 << "  Max Lut: " << lut+1 << "  PseudoSize: " << size <<endl;
    
  return cell*10+lut;
}


// --------------------------------------------------------------------------
//  
// Identify compact pixels in one LUT and calculate the LutPseudoSize 
//   (=number of compact pixels in one LUT)
//  neighpix is the number of Next-Neighbors which defines the compact pixel
//    topology.  
//   Up to now only  2NN or 3NN are considered 
//    <!if changed: change check made by Preprocess in MMcTriggerLvl2Calc>    
//
//   Returns: 
//     -1    wrong neighpix
//     -2    wrong cell (<1 or >19)
//     -3    wrong lut  (<1 or >3)
//    else:
//      the number of compact pixels in one LUT.
//     
Int_t MMcTriggerLvl2::GetLutCompactPixel(int cell, int lut)
{
  int size=0;
  int lutpix, a[gsNPixInLut]; 
  int neighpix= (*this).fCompactNN;

  // check on input variables

  if (neighpix >3 || neighpix < 2) 
    return(-1);

  if (cell<0 || cell> gsNCells-1)
    return(-2);

  if (lut<0 || lut> gsNLutInCell-1)
    return(-3);


  //
  // Warning!!! Following configurations are valid only for the standard MAGIC 
  // trigger geometry; FIXME! these should be coded somewhere else....
  //

  // LUT 1 and 2 are similar; LUT 3 differs
  
  for(int j=0; j< gsNPixInLut; j++)
    {
      lutpix = gsPixelsInLut[lut][j]-1;
      //    *fLog << "j=" <<j<<"  lutpix="<<lutpix<<"  Cell="<<cell<<endl;
      a[j] = fPixels[lutpix][cell]; 
    }
  
  //
  // Look for compact pixels 2NN
  //
  if (neighpix==2)
    {    
      //  case Lut 1 and 2
      if (lut == 0 || lut == 1)
	{
	  if (a[0] && a[1] && a[4])
	    size ++;
	  if (a[1] && ((a[0] && a[4]) || 
		       (a[4] && a[5]) || 
		       (a[5] && a[2]) )) 
	    size ++;
	  if (a[2] && ((a[1] && a[5]) || 
		       (a[5] && a[6]) || 
		       (a[6] && a[3]) )) 
	    size ++;
	  if (a[3] && ((a[2] && a[6]) || 
		       (a[6] && a[7]) )) 
	    size ++;
	  if (a[4] && ((a[0] && a[1]) || 
		       (a[1] && a[5]) || 
		       (a[5] && a[8]) )) 
	    size ++;
	  if (a[5] && ((a[1] && a[2]) || 
		       (a[2] && a[6]) || 
		       (a[6] && a[9]) || 
		       (a[9] && a[8]) || 
		       (a[8] && a[4]) || 
		       (a[4] && a[1]) )) 
	    size ++;
	  if (a[6] && ((a[2] && a[3]) || 
		       (a[3] && a[7]) || 
		       (a[7] && a[10]) || 
		       (a[10] && a[9]) || 
		       (a[9] && a[5]) || 
		       (a[5] && a[2]) )) 
	    size ++;
	  if (a[7] && ((a[3] && a[6]) || 
		       (a[6] && a[10]) || 
		       (a[10] && a[11]) )) 
	    size ++;
	  if (a[8] && ((a[4] && a[5]) || 
		       (a[5] && a[9]) )) 
	    size ++;
	  if (a[9] && ((a[8] && a[5]) || 
		       (a[5] && a[6]) || 
		       (a[6] && a[10]) )) 
	    size ++;
	  if (a[10] && ((a[9] && a[6]) || 
			(a[6] && a[7]) || 
			(a[7] && a[11]) )) 
	    size ++;
	  if (a[11] && a[7] && a[10]) 
	    size ++;
	}
      
      //  case Lut 3
      if (lut==2) 
	{
	  if (a[0] && a[1] && a[5])
	    size ++;
	  if (a[1] && ((a[0] && a[5]) || 
		       (a[5] && a[2]) )) 
	    size ++;
	  if (a[2] && ((a[1] && a[5]) || 
		       (a[5] && a[6]) || 
		       (a[3] && a[4]) || 
		       (a[6] && a[3]) )) 
	    size ++;
	  if (a[3] && ((a[2] && a[6]) || 
		       (a[6] && a[7]) || 
		       (a[2] && a[4]) )) 
	    size ++;
	  if (a[4] && ((a[2] && a[3]) )) 
	    size ++;
	  if (a[5] && ((a[1] && a[2]) || 
		       (a[2] && a[6]) || 
		       (a[6] && a[9]) || 
		       (a[9] && a[8]) )) 
	    size ++;
	  if (a[6] && ((a[2] && a[3]) || 
		       (a[3] && a[7]) || 
		       (a[7] && a[10]) || 
		       (a[10] && a[9]) || 
		       (a[9] && a[5]) || 
		       (a[5] && a[2]) )) 
	    size ++;
	  if (a[7] && ((a[3] && a[6]) || 
		       (a[6] && a[10]) || 
		       (a[10] && a[11]) )) 
	    size ++;
	  if (a[8] && a[5] && a[9]) 
	    size ++;
	  if (a[9] && ((a[8] && a[5]) || 
		       (a[5] && a[6]) || 
		       (a[6] && a[10]) )) 
	    size ++;
	  if (a[10] && ((a[9] && a[6]) || 
			(a[6] && a[7]) || 
			(a[7] && a[11]) )) 
	    size ++;
	  if (a[11] && a[7] && a[10]) 
	    size ++;    
	}
    }
  
  //
  // Look for compact pixels 3NN 
  //
  if (neighpix==3)
    {
      //  case Lut 1 and 2
      if (lut == 0 || lut == 1)
	{
	  if (a[0] && a[1] && a[4] && a[5]) // pix 0 is compact if there is a 4NN
	    size ++;
	  if (a[1] && ((a[0] && a[4] && a[5]) || 
		       (a[2] && a[4] && a[5]) )) 
	    size ++;
	  if (a[2] && ((a[1] && a[5] && a[6]) || 
		       (a[5] && a[6] && a[3]) )) 
	    size ++;
	  if (a[3] && (a[2] && a[6] && a[7] ))  
	    size ++;
	  if (a[4] && ((a[0] && a[1] && a[5]) || 
		       (a[1] && a[5] && a[8]) )) 
	    size ++;
	  if (a[5] && ((a[1] && a[2] && a[6]) || 
		       (a[2] && a[6] && a[9]) || 
		       (a[6] && a[9] && a[8]) || 
		       (a[9] && a[8] && a[4]) || 
		       (a[8] && a[4] && a[1]) || 
		       (a[4] && a[1] && a[2]) )) 
	    size ++;
	  if (a[6] && ((a[2] && a[3] && a[7]) || 
		       (a[3] && a[7] && a[10]) || 
		       (a[7] && a[10] && a[9]) || 
		       (a[10] && a[9] && a[5]) || 
		       (a[9] && a[5] && a[2]) || 
		       (a[5] && a[2] && a[3]) )) 
	    size ++;
	  if (a[7] && ((a[3] && a[6] && a[10]) || 
		       (a[6] && a[10] && a[11]) )) 
	    size ++;
	  if (a[8] && (a[4] && a[5] && a[9] )) 
	    size ++;
	  if (a[9] && ((a[8] && a[5] && a[6]) || 
		       (a[5] && a[6] && a[10]) )) 
	    size ++;
	  if (a[10] && ((a[9] && a[6] && a[7]) || 
			(a[6] && a[7] && a[11]) )) 
	    size ++;
	  if (a[11] && a[7] && a[10] && a[6]) //pix 0 is compact if there is a 4NN
	    size ++;
	}
      
      //  case Lut 3
      if (lut==2) 
	{
	  if (a[0] && a[1] && a[5] && a[8]) // pix 0 is compact if there is a 4NN
	    size ++;
	  if (a[1] && (a[0] && a[5] && a[2])) //pix 0 is compact if there is a 4NN 
	    size ++;
	  if (a[2] && ((a[1] && a[5] && a[6]) || 
		       (a[3] && a[5] && a[6]) || 
		       (a[6] && a[3] && a[4]) )) 
	    size ++;
	  if (a[3] && ((a[2] && a[4] && a[6]) || 
		       (a[2] && a[6] && a[7]) )) 
	    size ++;
	  if (a[4] && (a[2] && a[3] && a[6] )) 
	    size ++;
	  if (a[5] && ((a[1] && a[2] && a[6]) || 
		       (a[2] && a[6] && a[9]) || 
		       (a[6] && a[9] && a[8]) )) 
	    size ++;
	  if (a[6] && ((a[2] && a[3] && a[7]) || 
		       (a[3] && a[7] && a[10]) || 
		       (a[7] && a[10] && a[9]) || 
		       (a[10] && a[9] && a[5]) || 
		       (a[9] && a[5] && a[2]) || 
		       (a[5] && a[2] && a[3]) )) 
	    size ++;
	  if (a[7] && ((a[3] && a[6] && a[10]) || 
		       (a[6] && a[10] && a[11]) )) 
	    size ++;
	  if (a[8] && a[5] && a[9] && a[6])  //pix 0 is compact if there is a 4NN
	    size ++;
	  if (a[9] && ((a[8] && a[5] && a[6]) || 
		       (a[5] && a[6] && a[10]) )) 
	    size ++;
	  if (a[10] && ((a[9] && a[6] && a[7]) || 
			(a[6] && a[7] && a[11]) )) 
	    size ++;
	  if (a[11] && a[7] && a[10] && a[6]) //pix 0 is compact if there is a 4NN
	    size ++;
	}
    }
  

  if(size<0 ||size>gsNPixInLut)
    *fLog << "*" << size <<"-";
  
  return size;
}


// --------------------------------------------------------------------------
//  
// Look for clusters and calculate the PseudoSize of one event, 
//   defined as the multiplicity of the bigger cluster. 
//
//
//
void MMcTriggerLvl2::CalcPseudoSize()
{
  // Fill the fCompactPixel array with the compact pixels
  CalcCompactPixels(fGeomCam);

  // seek the LUT with higher number of compact pixels
  //
  //int fMaxCell = CalcBiggerCellPseudoSize();
  int sizetemp=0;
  int maxlut=0;

  for (int i=0;i<gsNLutInCell;i++)
    if (GetLutCompactPixel(fMaxCell,i) > sizetemp)
      {
        maxlut=i;
        sizetemp = GetLutCompactPixel(fMaxCell,i);
      }

  int startpix;
  // 
  // seek a starting pixel for the iteration inside the lut
  //
  int check=1;
  for (int pixlut=0;pixlut<gsNPixInLut;pixlut++)
    {
      int pixcell =gsPixelsInLut[maxlut][pixlut]-1;
      startpix = gsPixelsInCell[pixcell][fMaxCell]-1;
      //*fLog << "pix, compact:" << startpix << "@"<<fCompactPixel[startpix];
      if (fCompactPixel[startpix]) // a starting pixel was found
		break;
      check++;
    }

  //*fLog << "check = " << check << endl;
  // A LUT contains 12 pixels
  if (check > gsNPixInLut)
	  {   
		   *fLog <<"Warning: a starting pixel was not found! - PseudoSize = "<< fPseudoSize << endl;
	    fPseudoSize=-1;
	    return;
	  }

  //
  // Bulding cluster
  //
  Int_t cluster[gsNTrigPixels];
  int pnt=0;
  int pnt2=0; //pointer in the array fCluster_pix, needed for filling

  memset (cluster,0,gsNTrigPixels*sizeof(Int_t));
  memset (fCluster_pix,-1,gsNTrigPixels*sizeof(Int_t));

  cluster[startpix]=1;  
  fCluster_pix[0]=startpix; //the starting pix is the first in cluster
  
  // Look at neighbour pixs if they are compact (iterative search)
  //  until the array (fCluster_pix) has no more compact pixels.
  // pnt points to the pixel in the array fCluster_pix whose neighbors are
  // under study; pnt2 points to the last element of this array.
  //
  while (fCluster_pix[pnt] != -1)
    { 
      const MGeomPix &pix=(*fGeomCam)[fCluster_pix[pnt]];

      for (int i=0;i<pix.GetNumNeighbors();i++)
	    {
	       int pix_neigh = pix.GetNeighbor(i);
	       // check if pixel is fired and doesn't belong to cluster
	       if (fCompactPixel[pix_neigh] && !cluster[pix_neigh])
	         {
	            cluster[pix_neigh] = 1;
	            fCluster_pix[++pnt2] = pix_neigh;
	         }
	    }
      pnt++;
    }

  fPseudoSize = pnt;

  //if (fPseudoSize < 4)
    //    *fLog << "fPseudoSize = " << fPseudoSize << endl;

  //  *fLog << "ClusterID:" <<(*clust).GetClusterId() << " Mult:" << (*clust).GetMultiplicity()<<endl;  
  /*
  *fLog <<"PSize: "<< fPseudoSize << " in cell:" << fMaxCell << " lut:" <<maxlut << endl << "   Pixels: ";
  for (int i =0;i<fPseudoSize; i++)
    *fLog << fCluster_pix[i]+1 <<"; ";
  *fLog << endl;
  */
  return;
}

// --------------------------------------------------------------------------
//  
//  Fill the fCompactPixels array with the pixels which are compact
//
//  neighpix is the number of Next-Neighbors which defines the compact pixel
//    topology (it can be 2-NN or 3-NN)
//
//   Note: it is a *global* method; it looks in the all camera as a whole
//         
//
void MMcTriggerLvl2::CalcCompactPixels(MGeomCam *geom)
{
  memset (fCompactPixel,0,gsNTrigPixels*sizeof(Int_t));
  //  *fLog << endl << "NEW Event!";
  for(Int_t pixid=0; pixid<gsNTrigPixels; pixid++)
    {
      // Look if the pixel is fired, otherwise continue
      if (!fFiredPixel[pixid])
	     continue;
      
      const MGeomPix &pix=(*geom)[pixid];
      
      // Reshuffle pixneighbour order, to arrange them (anti)clockwise
      //   around the current pixel (Bubble sorting)
      // The NeighPixOrdered array has a doubledd size so that
      // the content in the first index is repeated in the last one
      // to have a closed loop

      Int_t NeighPixOrdered[2*pix.GetNumNeighbors()];

      for (Int_t j=0; j<pix.GetNumNeighbors(); j++)
        NeighPixOrdered[j] = pix.GetNeighbor(j);
      
      for (Int_t j=0; j<pix.GetNumNeighbors()-1; j++)
        {
          for (Int_t jk=pix.GetNumNeighbors()-1;jk>j;--jk)
            {
               UInt_t tmp = NeighPixOrdered[j+1];
               const MGeomPix &pixneigh = (*geom)[NeighPixOrdered[j]];
               for (int k=0; k<pix.GetNumNeighbors(); k++)
                 if (NeighPixOrdered[jk] == pixneigh.GetNeighbor(k))
     		       {
                      NeighPixOrdered[j+1] = NeighPixOrdered[jk];
                      NeighPixOrdered[jk] = tmp;
              	   }
	        }
        }
      
      // Duplicate second half to take into account configurations
 	  //  containing tha last and first pixel
      for (Int_t j=pix.GetNumNeighbors(); j<2*pix.GetNumNeighbors(); j++)
        NeighPixOrdered[j] = NeighPixOrdered[j-pix.GetNumNeighbors()];

      // Look for compact pixels. 
      // A compact pixel must have at least fCompactNN adjacent fired neighbors
      // It checks the 6 different configurations of neighbors pixels; if
	  // one has fCompacNN adjacent fired pixels than the pixel pixid is
	  // promoted to compact pixel and saved into the fCompactPixel array.
	  //
      for (int i=0;i<pix.GetNumNeighbors();i++)
	    { 
	      int j=0; // j counts the adjacent fired pixels
	      //*fLog << pixid <<"->"<< pix.GetNeighbor(i+j) << endl;
	      while ((i+j < 2*pix.GetNumNeighbors()) && (fFiredPixel[NeighPixOrdered[i+j]]==1) && (j < fCompactNN))
	        j++;  
	      if (j>=fCompactNN)	 // configuration satisfies the compact condition  
		{
	      	  fCompactPixel[pixid]=1; // pixel is compact
	      	  //	  *fLog << ","<<pixid;
		  break;
		}
	    }      
    }

}

// --------------------------------------------------------------------------
//  
//  The Energy has to be given by this class to the Energy-PSSize correlation 
//   histogram (MHMcTriggerLvl2)  
//
void MMcTriggerLvl2::GetEnergy(MMcEvt *fMcEvt)
{
   const MMcEvt &h = *(MMcEvt *)fMcEvt; 
   fEnergy = h.GetEnergy();
   return;
}


// --------------------------------------------------------------------------
//  
//   Looks for a x-NN compact pattern in the whole camera
//   We have x-NN compact pattern when a triggered pix has
//   x-1 triggered neighbor pixels.  
//   The variable fTriggerPattern = x is computed
//   (x= 2,3,4,5,6,7)  
//
//   x=2   * *
//
//   x=3   * *
//          *        *
//            x=4   * *
//                   *       
//                       * *
//                 x=5  * * *
//          * 
//   x=6   * * *
//          * *
//               * *
//          x=7 * * *
//               * *
//
void MMcTriggerLvl2::CalcTriggerPattern(MGeomCam *geom)
{
  fTriggerPattern=0; //initialize

  for(Int_t pixid=0;pixid<gsNTrigPixels;pixid++)
    {
      // Look if the pixel is fired, otherwise continue
      if (!fFiredPixel[pixid])
		continue;
      
      const MGeomPix &pix=(*geom)[pixid];
      
      // Look for x-NN compact pattern
      // If a x-NN pattern exists then a pixel must have 
      // at least x-1 adjacent neighbors (look at patterns)
      // For each triggered pixel the number of adjacent triggered pixels 
      // is counted.
      //
      int j=1;
      for (int i=0;i<pix.GetNumNeighbors();i++)
		if (fFiredPixel[pix.GetNeighbor(i)]==1) j++;
	     
      if (j > fTriggerPattern) 
		fTriggerPattern=j;	 

      if (fTriggerPattern==7) 
	  	break;	 // the 7-NN (max) pattern was found: exit  

    } // next pixel
}


// --------------------------------------------------------------------------
//  Look for the cell with higher number of Compact pixels
//  Return the cell number (starting from 0)
//
Int_t MMcTriggerLvl2::CalcBiggerCellPseudoSize()
{
  Int_t maxcell=-1;

  fCellPseudoSize=0;

  for (Int_t i=0;i<gsNCells;i++)
    {
      int size = GetCellCompactPixel(i,fGeomCam);
      if (size > fCellPseudoSize)
        {
          fCellPseudoSize = size;
          maxcell = i;
        }
    }
  
  //*fLog << "fCellPseudoSize = " << fCellPseudoSize << " in cell N. " << fMaxCell+1 << endl;

  return maxcell;
}

// --------------------------------------------------------------------------
//  Compute the number of compact pixels in one cell
//
Int_t MMcTriggerLvl2::GetCellCompactPixel(int cell, MGeomCam *geom)
{
  int size=0;

  // check on input variables

  if (cell<0 || cell>gsNCells-1)
    return(-2);

  //*fLog << " CNN:" << fCompactNN;
  //*fLog << "Cell: " << cell+1 << " Compat Pixels:"; 

  for(Int_t id=0; id<gsNPixInCell; id++)
    {
      UInt_t pixid = gsPixelsInCell[id][cell]-1;
 
      // Look if the pixel is fired, otherwise continue
      if (!fFiredPixel[pixid])
        continue;

      //*fLog << "Fired pix:"<<pixid+1 << " ";

      const MGeomPix &pix=(*geom)[pixid];
      
      // Reshuffle pixneighbour order, to arrange them (anti)clockwise 
      //   around the current pixel (Bubble sorting)
      // The NeighPixOrdered has one index more so that 
      // the content in the first index is repeated in the last one
      // to have a closed loop
      Int_t NeighPixOrdered[2*pix.GetNumNeighbors()];
      for (Int_t j=0; j<pix.GetNumNeighbors(); j++)
        NeighPixOrdered[j] = pix.GetNeighbor(j);
      
      for (Int_t j=0; j<pix.GetNumNeighbors()-1; j++)
        {
          for (Int_t jk=pix.GetNumNeighbors()-1;jk>j;--jk)
            {
              UInt_t tmp = NeighPixOrdered[j+1];                  
              const MGeomPix &pixneigh = (*geom)[NeighPixOrdered[j]];
              for (int k=0; k<pix.GetNumNeighbors(); k++)
                if (NeighPixOrdered[jk] == pixneigh.GetNeighbor(k))
                  {
                    NeighPixOrdered[j+1] = NeighPixOrdered[jk];
                    NeighPixOrdered[jk] = tmp;
                  }
            }
        }

      // Duplicate second half of the array to take into account configurations
      // containing tha last and first pixel
      for (Int_t j=pix.GetNumNeighbors(); j<2*pix.GetNumNeighbors(); j++)
        NeighPixOrdered[j] = NeighPixOrdered[j-pix.GetNumNeighbors()];

      // Look for compact pixels. 

      // A compact pixel must have at least fCompactNN adjacent neighbors
      // It checks the 6 different configurations of neighbors pixels.
      // The neighbour pixels must belong to the cell

      //      *fLog << "cell:"<< cell << "   ordered pixels:";
      /*
      for (int i=0;i<2*pix.GetNumNeighbors();i++)
	{
	  if (fFiredPixel[NeighPixOrdered[i]])
	    *fLog << NeighPixOrdered[i]+1 << "*;";
	  else
	    *fLog << NeighPixOrdered[i]+1 << ";";
	}
      */
      //*fLog <<endl;
      //*fLog << pixid <<"->"<< pix.GetNumNeighbors() << " CNN="<< fCompactNN <<endl;


      for (int i=0;i<pix.GetNumNeighbors();i++)
        { 
          int j=0;
          while ((i+j < 2*pix.GetNumNeighbors()) && (fFiredPixel[NeighPixOrdered[i+j]]==1) && (j < fCompactNN) && IsPixelInCell(NeighPixOrdered[i+j],cell) )
            j++;  

          if (j>=fCompactNN) //configuration satisfies the compact condition  
	    {          
	      size++; // pixel is compact
	      //*fLog << "->" << pixid+1;
	      break; // (new pixel)
	    }
	}
    }
  
  //*fLog <<" - size:" << size << endl<<endl;
  
  return size;
  
}

//---------------------------------------------------------------------
//  Check if a given pixel belongs to a given cell
//
Bool_t MMcTriggerLvl2::IsPixelInCell(Int_t pixel, Int_t cell)
{
  for (int i=0; i<gsNPixInCell; i++)
    if ((gsPixelsInCell[i][cell]-1) == pixel)
      return kTRUE;
  
  return kFALSE;
}

//---------------------------------------------------------------------
//  Check if a given pixel is in the trigger region
//
Bool_t MMcTriggerLvl2::IsPixelInTrigger(Int_t pixel) const
{
  for (int cell=0; cell<gsNCells; cell++)
    for (int i=0; i<gsNPixInCell; i++)
      if ((gsPixelsInCell[i][cell]-1) == pixel)
	return kTRUE;
  
  return kFALSE;
}

// --------------------------------------------------------------------------
//
// Returns, depending on the type flag:
//
//  0: 1,0 if the pixel is triggered (1) or not (0)
// 
//
Bool_t MMcTriggerLvl2::GetPixelContent(Double_t &val, Int_t idx, const MGeomCam &cam, Int_t type) const
{
  // Pixel in no-trigger region are set to 0
  val = this->IsPixelInTrigger(idx) ?  fFiredPixel[idx]+fCompactPixel[idx] : 0;

  return kTRUE;
}

void MMcTriggerLvl2::DrawPixelContent(Int_t num) const
{
    *fLog << "MMcTriggerLvl2::DrawPixelContent - not available." << endl;
}
