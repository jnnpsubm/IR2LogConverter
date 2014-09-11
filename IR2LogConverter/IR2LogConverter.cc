#include "IR2LogConverter.h"

int CIR2LogConverter::Convert(const vector<string>& fileList, const string& savedFile) const
{
	if (fileList.empty())
	{
		cerr << "no file specified..." << endl;
		return -1;
	}

	KeySet setKey(key_sort);
	VecContext vecContext;
	for (size_t i = 0; i < fileList.size(); i++)
	{
		processFile(fileList[i], setKey, vecContext);
	}

	// save to xlsx
	save2File(savedFile, setKey, vecContext);

	return 0;
}

void CIR2LogConverter::processFile(const string& fileName, KeySet& keySet, VecContext& vecContext)
{
	ifstream inFile(fileName);
	if (!inFile)
	{
		cerr << "error open file:" << fileName << endl;
		return;
	}

	string strLine;
	while (inFile >> strLine)
	{
		ValContext mapContext;
		vector<string> vecItems;
		split2items(strLine, vecItems, ';');
		for (size_t i = 0; i < vecItems.size(); i++)
		{
			string strItem = vecItems[i];
			//cout << strItem << endl;

			string strKey, strVal;
			split2KeyVal(strItem, strKey, strVal, ':');

			if (0 == i) // assume first item is time 
			{
				formatTime(strVal);
			}
			if (strKey.empty())
			{
				guessKey(strKey, i);
			}
			if (strVal.empty())
			{
				strVal = "null";
			}

			auto itSet = keySet.find(strKey);
			if (itSet == keySet.end())
			{
				//cout << strKey << endl;
				keySet.insert(keySet.begin(), strKey);
			}

			auto itMap = mapContext.find(strKey);
			if (itMap != mapContext.end())
			{
				cerr << "multi key found..." << endl;
			}
			mapContext[strKey] = strVal;
		}
		vecContext.push_back(mapContext);
	}
}

void CIR2LogConverter::save2File(const string& fileName, KeySet& keySet, VecContext& vecContext)
{
	ofstream oFile(fileName, ofstream::out);
	if (!oFile)
	{
		cerr << "error open file " << fileName << endl;
	}

	string strHeader;
	for (auto it = keySet.cbegin(); it != keySet.cend(); ++it)
	{
		const string& key = *it;
		strHeader.append(key);
		strHeader.push_back('\t');
	}
	oFile << strHeader << "\n";

	for (const auto& map : vecContext)
	{
		string strLine;
		for (auto it = keySet.cbegin(); it != keySet.cend(); ++it)
		{
			const string& key = *it;

			auto itMap = map.find(key);
			if (itMap == map.cend())
			{
				strLine.append("null");
			}
			else
			{
				strLine.append((*itMap).second);
			}
			strLine.push_back('\t');
		}
		oFile << strLine << "\n";
	}
}

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
void CIR2LogConverter::formatTime(string& strTime)
{
	int year, month, day, hour, minute, second, ffff;
	sscanf_s(strTime.c_str(), "%4d%2d%2d%2d%2d%2d%4d", &year, &month, &day, &hour, &minute, &second, &ffff);

	// 2005-11-05 14:23:23
	ostringstream osstream;
	osstream << setfill('0')
		<< setw(4) << year << '-'
		<< setw(2) << month << '-'
		<< setw(2) << day << ' '
		<< setw(2) << hour << ':'
		<< setw(2) << minute << ':'
		<< setw(2) << second << ' ';
	//<< setw(4) << ffff;
	strTime = osstream.str();
	//cout << val << endl;
}

void CIR2LogConverter::guessKey(string& strKey, size_t itemIndex)
{
	static const string c_defaultKey[] = { "Time", "Log", "Action", "Rule" };

	strKey = itemIndex < 4 ? c_defaultKey[itemIndex] : c_defaultKey[3];
}