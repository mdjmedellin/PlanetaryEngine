#pragma once
#ifndef ERROR_REPORTER_H
#define ERROR_REPORTER_H

//includes
#include <string>
//

class ErrorReporter
{
private:
	ErrorReporter();
	ErrorReporter( const ErrorReporter& );
	ErrorReporter& operator=( const ErrorReporter& );

public:
	static void throwError( std::string errorTitle, std::string errorBody, std::string visualStudioOutput = "" );
};

#endif