#include "IPdic.h"

int main()
{
	//Convert objConvert;
	//char arryCode[MAX_CONVERT_DIGIT][CONVERT_FIGURES] = { 0 };
	//objConvert.InitCustomArry(arryCode);

	CIPdic dicIP;
	//string strPath = "..//config//CarrierDic.txt";
	//if (!dicIP.ImportCarrierDic(strPath.c_str()))
	//{
	//	return 0;
	//}

	//strPath = "..//config//AreaDic.txt";
	//if (!dicIP.ImportAreaDic(strPath.c_str()))
	//{
	//	return 0;
	//}

	//string strOldPath = "..//config//ip_xcn.txt";
	string strNewPath = "..//config//ip_out.txt";
	//if (!dicIP.OldIP2NewIP(strOldPath, strNewPath.c_str(), arryCode))
	//{
	//	return 0;
	//}

	if (!dicIP.ImportNewIPDic(strNewPath.c_str()))
	{
		return 0;
	}

	cout << "Íê³É£¡\n";
	getchar();
}