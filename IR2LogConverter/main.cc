#include "IR2LogConverter.h"

const size_t c_nOptions = 4;
const string c_option[c_nOptions] = {
	"IR2Keep.log IR2Archive\\IR2KeepArchive.log",
	"IR2Pick.log IR2Archive\\IR2PickArchive.log",
	"IR2Bugs.log",
	"IR2Keep.log IR2Archive\\IR2KeepArchive.log IR2Pick.log IR2Archive\\IR2PickArchive.log IR2Bugs.log",
};
const string c_fileSave[c_nOptions] = {
	"IR2Keep.csv",
	"IR2Pick.csv",
	"IR2Bugs.csv",
	"IR2All.csv",
};

#define TIP cout <<  "************************************************************\n"\
                     "Put this exe to DBFolder\\TrinityLogs\\[your battle tag] to use\n"\
					 "Command:\n"\
					 "0:Convert IR2Keep.log+IR2KeepArchive.log -> IR2Keep.csv\n"\
					 "1:Convert IR2Pick.log+IR2PickArchive.log -> IR2Pick.csv\n"\
			         "2:Convert IR2Bugs.log -> IR2Bugs.csv\n"\
					 "3:Convert All Above -> IR2LogAll.csv\n"\
					 "[filename]:Convert [filename] -> [filename].csv\n"\
					 "q:quit\n"\
					 "************************************************************\n"\
					 "input:"
#define END cout << "%%%%%%%%%%%%%%%%%%%%%%%%%END%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"\
                    "%%%%%%%%%%%%%%%%%%%%%%%%%END%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n\n"

int main(int argc, char** argv)
{
	string strCMD;
	TIP;
	while (getline(cin, strCMD))
	{
		if (strCMD == "q" || strCMD == "Q")
		{
			cout << "bye" << endl;
			break;
		}
		string strFileSave;
		if (strCMD.size() == 1 && ::isdigit(strCMD[0]))
		{
			int option = stoul(strCMD);
			if (option >= c_nOptions)
			{
				cout << "unknown command..." << endl;
				END;
				TIP;
				continue;
			}
			strCMD = c_option[option];
			strFileSave = c_fileSave[option];
		}
		cout << "total files:" << strCMD << endl;
		vector<string> fileList;
		istringstream isstr(strCMD);
		string strFile;
		while (isstr >> strFile)
		{
			fileList.push_back(strFile);
		}
		if (!fileList.empty())
		{
			if (strFileSave.empty())
			{
				strFileSave = fileList[0];
				strFileSave.append(".csv");
			}
			CIR2LogConverter convertor;
			if (0 == convertor.Convert(fileList, strFileSave))
			{
				cout << "conver done, new file name " << strFileSave << endl;
				convertor.CheckDuplicate();
			}
			else
			{
				cerr << "conver failed... " << endl;
			}
			END;
		}
		//TIP;
		break;
	}
	system("pause");
	return 0;
}