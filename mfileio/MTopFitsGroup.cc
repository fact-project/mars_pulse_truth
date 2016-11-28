
#include "MTopFitsGroup.h"


// ----------------------------------------------------------------------------
//                                                                             
// Attaches the table to this FITS group                                       
// Note: This FITS group as well as the table must be open, before calling     
//       this method!                                                          
void MTopFitsGroup::Attach(ofits* table)
{
//   fTopGroup.SetChild(table);
//   fTopGroup.Write();
}

// ----------------------------------------------------------------------------
//                                                                             
// Opens "table" with the next child in this group.                            
// If table is an already open FITS table, then that table we be closed,       
// before the new child is assigned to "table"                                 
Bool_t MTopFitsGroup::GetNextChild(ofits* table)
{
//   fTopGroup.SetChild(table);
//   return fTopGroup.Read();
    return false;
}

