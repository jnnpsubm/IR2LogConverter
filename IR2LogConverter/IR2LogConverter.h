#ifndef _IR2_LOG_CONVERTER_H_
#define _IR2_LOG_CONVERTER_H_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <functional>

using namespace std;

bool key_sort(const string& str1, const string& str2);

typedef set<string, decltype(key_sort)*> KeySet;
typedef unordered_map<string, string> ItemMap;
typedef vector<ItemMap> VecContext;

class CIR2LogConverter
{
public:
	CIR2LogConverter() :mKey(key_sort){}
	int Convert(const vector<string>& fileList, const string& fileSave);
	void CheckDuplicate();

private:
	void processFile(const string& fileName);
	void processLine(const string& strLine);
	void save2File() const;
	void Clear(){
		mContext.clear();
		mKey.clear();
		mRawData.clear();
	}

	static void split2items(const string& strLine, vector<string>& vecItems, char SEP);
	static void split2KeyVal(const string& strItem, string& key, string& val, char SEP);
	static void formatTime(string& strTime);
	static void guessKey(string& strKey, size_t itemIndex);

private:
	VecContext     mContext;
	KeySet         mKey;
	vector<string> mFileList;
	string         mFileSave;

	vector<string> mRawData;
};

#endif // _IR2_LOG_CONVERTER_H_