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
typedef unordered_map<string, string> ValContext;
typedef vector<ValContext> VecContext;

class CIR2LogConverter
{
public:
	int Convert(const vector<string>& fileList, const string& savedFile) const;

private:
	static void processFile(const string& fileName, KeySet& keySet, VecContext& vecContext);
	static void save2File(const string& fileName, KeySet& keySet, VecContext& vecContext);

	static void split2items(const string& strLine, vector<string>& vecItems, char SEP);
	static void split2KeyVal(const string& strItem, string& key, string& val, char SEP);
	static void formatTime(string& strTime);
	static void guessKey(string& strKey, size_t itemIndex);
};

#endif // _IR2_LOG_CONVERTER_H_