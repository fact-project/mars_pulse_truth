/* ======================================================================== *\
!
!   Author(s): Sebastian Mueller and Dominik Neise for the FACT Colaboration
!              2014 Nov 05
!
!   Copyright: "THE BEER-WARE LICENSE" (Revision 42):
!
!   The FACT Colaboration wrote this file. As long as you retain this notice you
!   can do whatever you want with this stuff. If we meet some day, and you think
!   this stuff is worth it, you can buy us a beer in return.
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
//  MMatrix
//
//  A generic container to store a matrix of floating point numbers.
//  More precise it is a vector of vectors of floating numbers.
//  The rows can have an individual number of columns.
//  The data structure fM itself is public.
// 
//  A delimiter seperated value text file parser is used to obtain the data
//  from a text file.
//
//////////////////////////////////////////////////////////////////////////////
#include "MMatrix.h"

ClassImp(MMatrix);
//------------------------------------------------------------------------------
MMatrix::MMatrix(const char *name, const char *title){
    if( name == NULL ){
        std::exception error;
        *fLog << err; 
    	*fLog << "In Source: "<< __FILE__ <<" in line: "<< __LINE__ <<" ";
       	*fLog << "in function: "<< __func__ <<"\n";
		*fLog << "The name string is pointing to NULL!\n";
        throw error;	
    }else{
        fName  = name;          
    }

    fTitle = title ? title : 
    "Parameter container storing a vector of vectors to hold floating numbers";
	
    initialize();
}
//------------------------------------------------------------------------------
void MMatrix::initialize() {
    fDelimiter = ',';
    fComment = '#';
    fFileName = "";
    fLineNumber = 0;
}
//------------------------------------------------------------------------------
MMatrix::~MMatrix(){ clear(); }
//------------------------------------------------------------------------------
void MMatrix::ReadFile(const TString path_to_text_file){
	// Here a text file is parsed into the internal data structure fM
	// All errors which might occur during parsing result in exceptions.
	// Right before the exception a info text is printed using the error logger.

	// the delimiter symbol must not be a white space
	if( std::isspace(fDelimiter) ){
		std::exception error;
		*fLog << err; 
    	*fLog << "In Source: "<< __FILE__ <<" in line: "<< __LINE__ <<" ";
       	*fLog << "in function: "<< __func__ <<"\n";
		*fLog << "The delimiter symbol must not be a white space character!\n";
        throw error;			
	}

	// the comment symbol must not be a white space
	if( std::isspace(fComment) ){
		std::exception error;
		*fLog << err; 
    	*fLog << "In Source: "<< __FILE__ <<" in line: "<< __LINE__ <<" ";
       	*fLog << "in function: "<< __func__ <<"\n";
		*fLog << "The comment symbol must not be a white space character!\n";
        throw error;			
	}

	// make sure the data Matrix fM is wiped out before new data is read.
	clear();

	std::ifstream dsv_input_file( path_to_text_file, std::ifstream::in);

	if( !dsv_input_file.is_open() ){

		// a file which can not be opened must not pass silent!
		*fLog << err; 
    	*fLog << "In file: "<< __FILE__ <<" in line: "<< __LINE__ <<" ";
       	*fLog << "in function: "<< __func__ <<"\n";
		*fLog << "The file: "<< path_to_text_file <<" could no be opened!\n";
		std::exception error;
        throw error;	
	}else{
		// the file does exist and is now parsed

		// only here the filename is set to ensure it is related to the actual 
		// file being parsed.
		fFileName = path_to_text_file;

		// we set the line counter to 0 because we read a new file.
		fLineNumber = 0;

		//as long as there are lines in the file:
		std::string line;
        while ( getline(dsv_input_file, line ) ){

        	fLineNumber++;

        	// check if the line can be ignored
        	if( !is_comment(line) && !is_empty_or_has_only_white_spaces(line)){

	        	std::stringstream line_stream(line);
	        	std::string token;
				
				// create an empty row to be filled with the data chunks of 
				// a text file line
				std::vector< double > row;

	        	//as long as there are tokens in a line:
				while( getline(line_stream, token, fDelimiter)){

					// remove tailing and leading whitespaces
					token = remove_leading_white_spaces(token);
					token = remove_tailing_white_spaces(token);

					// parse the text chunk to a floating number and push it
					// back the data row
					row.push_back(pedantic_strtod(token));
				}

				// Now that a whole text file line was parsed into a data row,
				// the row is pushed back into the data matrix.
				fM.push_back( row );
		 	}
        }
	}

	dsv_input_file.close();
}
//------------------------------------------------------------------------------
double MMatrix::pedantic_strtod(std::string text)const{
	// a pedantic string to float converter

	// since std::strtod will return 0.0 in case the text string is empty we 
	// check for this in advance.
	if(text.empty() ){
		std::exception error;
		*fLog << err; 
    	*fLog << "In Source: "<< __FILE__ <<" in line: "<< __LINE__ <<" ";
       	*fLog << "in function: "<< __func__ <<"\n";
		*fLog << "In file: "<< fFileName <<" in line: "<<fLineNumber<<"\n";		
		*fLog << "The given text to be converted to a flot is empty! \n";
		throw error;
	}

	// Ok the text string is not empty and might store a valid floating number.

	// This pointer to character is used to verify if std::strtod has parsed
	// the string to its end.
	char * e;
	double FloatingNumber = std::strtod(text.c_str(), &e);

	// when std::strtod parsed until the last character in the input string,
	// e is pointing to an empty string. 
	// Otherwise e is pointing to the remaining string.
	// Example:
	// OK : '65.456456'          -> e = ''
	// BAD: '65.456456   585.46' -> e = '   585.46'

	if (*e != 0) {
		std::exception error;
		*fLog << err; 
    	*fLog << "In Source: "<< __FILE__ <<" in line: "<< __LINE__ <<" ";
       	*fLog << "in function: "<< __func__ <<"\n";
		*fLog << "In file: "<< fFileName <<" in line: "<<fLineNumber<<"\n";	
		*fLog << "The given text: '" << text <<"' can not be converted ";
		*fLog << "to a floating point number!\n";
		throw error;
	}
	return FloatingNumber;
}
//------------------------------------------------------------------------------
void MMatrix::clear(){
	// Not sure if this is neccessary but it shall enforce the correct erasing 
	// of the data Matrix fM e.g. before a new text file is parsed in.
	for(unsigned int row=0; row<fM.size(); row++)
		fM[row].clear();

	fM.clear();
}
//------------------------------------------------------------------------------
void MMatrix::print()const{
	// A info text is printed to std::cout showing the data, 
	// the number of lines parsed in and the name of the text file parsed in.

	std::stringstream info;
	info << std::fixed << std::setprecision( 2 );
	info << " ___MMatrix."<< __func__ <<"()____________________\n";
	info << "| \n";
	for(unsigned int row=0; row<fM.size(); row++){
		info << "| "<< row <<" ";
		for(unsigned int col=0; col<fM[row].size(); col++){
			info << "[" << fM[row][col] << "] ";
		}
		info << "\n";
	}
	info << "| . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .\n";
	info << "| file:  " << fFileName << "\n";
	info << "| lines: " << fLineNumber << "\n";	
	info << "|______________________________________________________________\n";

	std::cout << info.str();
}
//------------------------------------------------------------------------------
bool MMatrix::is_empty_or_has_only_white_spaces(std::string line)const{
	// Shall return TRUE when the text line is empty or only holding white 
	// spaces and FALSE otherwise.

	if(line.size() == 0){
		return true;
	}else{

		bool there_are_only_whitespaces = true;

		int i = 0;
		while ( line[i] ){

		    if( !isspace(line[i]) ){
				there_are_only_whitespaces = false;
		    }
		    i++;
		}

		return there_are_only_whitespaces;
	}
}
//------------------------------------------------------------------------------
bool MMatrix::is_comment(std::string line)const{
	// TRUE -> the whole line is a comment
	// FALSE -> the line is data or something else
	// a comment line is a line of arbitrary number of white spaces followed 
	// by the fComment symbol

	if( line.empty() ){
		// an empty line is not a comment line
		return false;
	}else{
		// remove leading whitespaces
		line = remove_leading_white_spaces(line);
		if( line.empty() ){
			// after the white spaces have been removed the line turns out to 
			// be empty; so it is not a comment
			return false;
		}else if( line[0] == fComment ){
			// this is a comment line
			return true;
		}else{
			// there is some thing not comment like in this line
			return false;
		}
	}
}
//------------------------------------------------------------------------------
// Both string trimmers are taken from StackOverflow
//http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
std::string MMatrix::remove_leading_white_spaces(std::string &s)const{
	// trim from start
    s.erase(
    	s.begin(), 
    	std::find_if(
    		s.begin(), 
    		s.end(), 
    		std::not1(std::ptr_fun<int, int>(std::isspace))
    	)
    );
    return s;
}
//------------------------------------------------------------------------------
std::string MMatrix::remove_tailing_white_spaces(std::string &s)const{
	// trim from end
    s.erase(
    	std::find_if(
    		s.rbegin(),
    		s.rend(),
    		std::not1(std::ptr_fun<int, int>(std::isspace))
    	).base(),
    	s.end()
    );
    return s;
}
//------------------------------------------------------------------------------
Int_t MMatrix::ReadEnv(const TEnv &env, TString prefix, Bool_t print){

    if (IsEnvDefined(env, prefix, "FileName", print))
    {
        TString temp_file_name = 
        GetEnvValue(env, prefix, "FileName", fFileName);

		try{// to parse the text file
			ReadFile(temp_file_name);
		}
		catch(std::exception error){
			// Oh oh. There was some trouble when parsing the file. This must 
			// not be accepted.
			return kERROR;
		}

		// The text file was correctly parsed
		return kTRUE;
    }

    /*
    *fLog << err << "In Source: "<< __FILE__ <<" in line: "<< __LINE__;
    *fLog << " in function: "<< __func__ <<"\n";
    *fLog << "There is no:'"<< prefix<<".FileName' in the config file. ";
    *fLog << "The filename is crucial because there are no default values ";
    *fLog << "to be asigned in case the input file with name filename is not ";
    *fLog << "found." << std::endl;
    */
    return kFALSE;
}
