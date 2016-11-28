#ifndef MARS_MMatrix
#define MARS_MMatrix

#ifndef MARS_MParContainer
#include "MParContainer.h"
#endif

#include <iostream>     // std::cout
#include <sstream>
#include <fstream>      // std::ifstream
#include <iomanip>
#include <algorithm> 
#include <cctype>
#include <locale>
#include "MLog.h"
#include "MLogManip.h"
      
class MMatrix : public MParContainer
{
public:
    MMatrix(const char *name, const char *title);
    ~MMatrix();

    Int_t ReadEnv(const TEnv &env, TString prefix, Bool_t print=kFALSE);
    void print()const;

    // The floating point data stored in this object has an arbitrary number of
    // rows and each row can have a unique number of floating numbers.
    // M[row i][column j]
    std::vector< std::vector< double > > fM;
private:
    // The deilimiter symbol which seperates the data chunks in the file to be 
    // parsed.
    char fDelimiter;

    // The data text file to be parsed may have comment lines which are 
    // indicated with the comment character.
    // Leading whitespaces in front of the comment symbol are fine/ are ignored. 
    char fComment;

    // The fFileName can only be set using ReadFile( filename ). This is to make
    // sure that the fFileName is always the one of the actual file parsed in.
    TString fFileName;

    // the line number worked on is stored here by the ReadFile( fileneam ) 
    // function so that other functions as the pedantic_strtod() can provide
    // an additional information in case they have to throw exceptions.
    unsigned int fLineNumber;

    void initialize();

    // a more pedantic version of std::strtod()
    double pedantic_strtod(std::string text)const;

    // deleting all information stored in the Matrix fM
    void clear();

    // the text file parser
    void ReadFile(const TString path_to_text_file );

    // text parsing helpers
    bool is_empty_or_has_only_white_spaces(std::string line)const;
    bool is_comment(std::string line)const;
    std::string remove_leading_white_spaces(std::string &s)const;
    std::string remove_tailing_white_spaces(std::string &s)const;

    ClassDef(MMatrix, 1) // generic parameter container with text file parser
};
    
#endif //MMatrix
