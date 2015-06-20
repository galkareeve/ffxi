// DatLoader.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "DatLoader.h"
#include "TDWAnalysis.h"

using namespace std;
int _tmain(int argc, _TCHAR* argv[])
{
	int fno=47123;
	CDatLoader dl;
	LocateFFXIDataFolder();

	if( argc>1 )
		fno = atoi(argv[1]);

	 if (!dl.loadDat(fno)) {
        fprintf(stderr, "Failed to load resources\n");
        return 1;
    }
	return 0;
}

