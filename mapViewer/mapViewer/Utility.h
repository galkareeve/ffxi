#pragma once
#include <string>
#include <list>
#include <vector>

using namespace std;

class CUtility {
public:
	static bool explode(string separater, string word, list<string> &ret )
	{
		string tmp;
		int remain;
		size_t pos;

		remain = word.length();
		ret.clear();
		do {
			pos = word.find(separater,0);
			if( pos !=string::npos ) {
				tmp = word.substr(0, pos);
				ret.push_back( tmp );

				remain = remain - (pos+1);
				word = word.substr( pos+1, remain );
				
				remain = word.length();
			}
		}while(pos != string::npos);

		//need this, if there is no separator for the last word
		if( remain > 0 )
			ret.push_back(word);

		return true;
	}

	static bool explode(string separater, string word, vector<string> &ret )
	{
		string tmp;
		int remain;
		size_t pos;

		remain = word.length();
		ret.clear();
		do {
			pos = word.find(separater,0);
			if( pos !=string::npos ) {
				tmp = word.substr(0, pos);
				ret.push_back( tmp );

				remain = remain - (pos+1);
				word = word.substr( pos+1, remain );
				
				remain = word.length();
			}
		}while(pos != string::npos);

		//need this, if there is no separator for the last word
		if( remain > 0 )
			ret.push_back(word);

		return true;
	}

	static string stripFirst(string separater, string &word) 
	{
		size_t pos = word.find(separater,0);
		string tmp = word.substr(0,pos);
		word = word.substr(pos+1);
		return tmp;
	}

	static bool implode( string separater, list<string> &arr, string &ret )
	{
		int size = arr.size();
		if( size==0 )
			return false;

		auto it = arr.begin();
		ret = arr.front();
		++it;
		for(; it!=arr.end(); ++it) {
			ret = ret + separater + *it;
		}
		return true;
	}
};

