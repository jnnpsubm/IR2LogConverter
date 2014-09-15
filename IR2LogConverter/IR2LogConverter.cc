#include "IR2LogConverter.h"

bool key_sort(const string& str1, const string& str2)
{
	static unordered_map<string, int> keyWeight;
	static int nWeight = 0;
	if (keyWeight.empty())
	{
		keyWeight["Time"] = nWeight++;
		keyWeight["Action"] = nWeight++;
		keyWeight["[QUALITY]"] = nWeight++;
		keyWeight["[TYPE]"] = nWeight++;
		keyWeight["[NAME]"] = nWeight++;
		keyWeight["[AS%]"] = nWeight++;
		keyWeight["[CRIT%]"] = nWeight++;
		keyWeight["[CRITDMG%]"] = nWeight++;
		keyWeight["[ALLRES]"] = nWeight++;
		keyWeight["[DPS]"] = nWeight++;
		keyWeight["[ITEMID]"] = nWeight++;
		keyWeight["Rule"] = nWeight++;
	}
	if (keyWeight.find(str1) == keyWeight.end())
	{
		keyWeight[str1] = nWeight++;
	}
	if (keyWeight.find(str2) == keyWeight.end())
	{
		keyWeight[str2] = nWeight++;
	}

	return keyWeight[str1] < keyWeight[str2];
}

int CIR2LogConverter::Convert(const vector<string>& fileList, const string& fileSave)
{
	if (fileList.empty())
	{
		cerr << "no file specified..." << endl;
		return -1;
	}

	mFileList = fileList;
	mFileSave = fileSave;
	
	Clear();
	for (size_t i = 0; i < mFileList.size(); i++)
	{
		processFile(mFileList[i]);
	}

	if (mKey.empty() || mContext.empty())
	{
		cerr << "no context found..." << endl;
		return -1;
	}

	// save to xlsx
	save2File();

	return 0;
}

void CIR2LogConverter::CheckDuplicate()
{
	std::sort(mRawData.begin(), mRawData.end());
	auto it = std::unique(mRawData.begin(), mRawData.end());
	if (it == mRawData.end())
	{
		cout << "no duplicate data" << endl;
	}
	else
	{
		cout << "found duplicated data..." << endl;
	}
}

void CIR2LogConverter::processFile(const string& fileName)
{
	cout << "processing file:" << fileName << endl;
	ifstream inFile(fileName);
	if (!inFile)
	{
		cerr << "cannot open file:" << fileName << endl;
		return;
	}

	string strLine;
	while (inFile >> strLine)
	{
		mRawData.push_back(strLine);
		processLine(strLine);
	}
}

void CIR2LogConverter::processLine(const string& strLine)
{
	ItemMap mapItem;
	vector<string> vecItems;
	time_t time;
	split2items(strLine, vecItems, ';');
	for (size_t i = 0; i < vecItems.size(); i++)
	{
		string strItem = vecItems[i];
		//cout << strItem << endl;

		string strKey, strVal;
		split2KeyVal(strItem, strKey, strVal, ':');

		if (0 == i) // assume first item is time 
		{
			formatTime(strVal, time);
		}
		if (strKey.empty())
		{
			guessKey(strKey, i);
		}
		if (strVal.empty())
		{
			strVal = "null";
		}

		auto itSet = mKey.find(strKey);
		if (itSet == mKey.end())
		{
			//cout << strKey << endl;
			mKey.insert(itSet, strKey);
		}

		auto itMap = mapItem.find(strKey);
		if (itMap != mapItem.end())
		{
			cerr << "multi key found..." << endl;
		}
		mapItem[strKey] = strVal;
	}
	mContext[time] = mapItem;
}

void CIR2LogConverter::save2File() const
{
	static const char c_sep = ',';
	ofstream oFile(mFileSave, ofstream::out);
	if (!oFile)
	{
		cerr << "error open file " << mFileSave << endl;
		return;
	}

	string strHeader;
	for (auto it = mKey.cbegin(); it != mKey.cend(); ++it)
	{
		const string& key = *it;
		strHeader.append(key);
		strHeader.push_back(c_sep);
	}
	oFile << strHeader << "\n";

	for (const auto& pair : mContext)
	{
		string strLine;
		for (auto it = mKey.cbegin(); it != mKey.cend(); ++it)
		{
			const string& key = *it;

			const auto& map = pair.second;
			auto itMap = map.find(key);
			if (itMap == map.cend())
			{
				strLine.append("null");
			}
			else
			{
				strLine.append((*itMap).second);
			}
			strLine.push_back(c_sep);
		}
		oFile << strLine << "\n";
	}
}

void CIR2LogConverter::split2items(const string& strLine, vector<string>& vecItems, char SEP)
{
	string::size_type pos1 = 0;
	auto pos2 = strLine.find_first_of(SEP, pos1);
	while (pos2 != string::npos)
	{
		string strItem = strLine.substr(pos1, pos2 - pos1);
		if (strItem.size())
		{
			vecItems.push_back(strItem);
		}
		pos1 = pos2 + 1;
		pos2 = strLine.find_first_of(';', pos1);
	}

	vecItems.emplace_back(strLine.substr(pos1));
}

void CIR2LogConverter::split2KeyVal(const string& strItem, string& key, string& val, char SEP)
{
	auto pos = strItem.find_first_of(SEP);
	if (pos != string::npos)
	{
		key = strItem.substr(0, pos);
		val = strItem.substr(pos + 1, strItem.size() - pos);
	}
	else
	{
		val = strItem;
	}
}

// assume formated as yyyyMMddHHmmssffff.hero
void CIR2LogConverter::formatTime(string& strTime, time_t& tmLog)
{
	struct tm t;
	memset(&t, 0, sizeof(tm));
	sscanf_s(strTime.c_str(), "%4d%2d%2d%2d%2d%2d", &t.tm_year, &t.tm_mon, &t.tm_mday, &t.tm_hour, &t.tm_min, &t.tm_sec);
	t.tm_year -= 1900;
	tmLog = mktime(&t);
	localtime_s(&t, &tmLog);

	// 2005-11-05 14:23:23
	ostringstream osstream;
	osstream << setfill('0')
		<< setw(4) << t.tm_year+1900 << '-'
		<< setw(2) << t.tm_mon << '-'
		<< setw(2) << t.tm_mday << ' '
		<< setw(2) << t.tm_hour << ':'
		<< setw(2) << t.tm_min << ':'
		<< setw(2) << t.tm_sec << ' ';
	strTime = osstream.str();
}

void CIR2LogConverter::guessKey(string& strKey, size_t itemIndex)
{
	static const string c_defaultKey[] = { "Time", "Log", "Action", "Rule" };

	strKey = itemIndex < 4 ? c_defaultKey[itemIndex] : c_defaultKey[3];
}