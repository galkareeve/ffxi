#pragma once

class CDatFactory {
public:
	static IDatBase* createDatClass(std::string datFN) {
		//if( datFN.compare("30.dat")==0 )
		//	return new CDat30;

		//if( datFN.compare("34.dat")==0 )
		//	return new CDat34;

		//if( datFN.compare("21.dat")==0 )
		//	return new CDat21;

		//if( datFN.compare("49.dat")==0 )
		//	return new CDat49;

		//if( datFN.compare("54.dat")==0 )
		//	return new CDat54;

		//if( datFN.compare("5_34.dat")==0 )
		//	return new CDat5_34;

		//if( datFN.compare("69.dat")==0 )
		//	return new CDat69;

		//if( datFN.compare("96.dat")==0 )
		//	return new CDat96;

		//if( datFN.compare("7.dat")==0 )
		//	return new CDat7;

		//MessageBox(NULL, (LPCWSTR)"DatFactory cant create dat class", (LPCWSTR)"err", MB_OK);
		//return nullptr;

		//use FFXIFile to parse dat, therefore no longer need separate Dat file
		return new CDat29(datFN);
	};

private:
	CDatFactory(void);
	~CDatFactory(void);
};