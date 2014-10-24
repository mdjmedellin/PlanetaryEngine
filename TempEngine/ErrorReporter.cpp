#include "ErrorReporter.h"
#include <Windows.h>

void ErrorReporter::throwError( std::string errorTitle, std::string errorBody, std::string visualStudioOutput )
{
	MessageBoxA( NULL, errorBody.c_str(), errorTitle.c_str(), MB_ICONERROR );

	if( !visualStudioOutput.empty() )
	{
		OutputDebugStringA( visualStudioOutput.c_str() );
	}

	exit(0);
}
