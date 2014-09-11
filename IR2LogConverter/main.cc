#include "IR2LogConverter.h"

#define PICK_FILES "IR2Pick.log IR2Keep.log IR2Bugs.log IR2Archive\\IR2KeepArchive.log"

#define SAVE_FILE "IR2Log.iex"

#define TIP cout << "input file names to convert or q to quit(input 0 as convert all default logs)\nyour input:"

int main(int argc, char** argv)
{
	string strFileList;
	TIP;
	while (getline(cin,strFileList))
	{
		if (strFileList == "q" || strFileList == "Q")
		{
			cout << "bye" << endl;
			break;
		}
		if (strFileList == "0")
		{
			strFileList = PICK_FILES;
		}
		cout << "total files:" << strFileList << endl;
		vector<string> fileList;
		istringstream isstr(strFileList);
		string strFile;
		while (isstr >> strFile)
		{
			fileList.push_back(strFile);
		}
		if (!fileList.empty())
		{
			CIR2LogConverter convertor;

			string strNewFile(SAVE_FILE);
			if (0 == convertor.Convert(fileList, strNewFile))
			{
				cout << "conver done, xlsx file name " << strNewFile << endl;
			}
			else
			{
				cerr << "conver failed... " << strNewFile << endl;
			}
		}
		TIP;
	}
	//system("pause");
	return 0;
}