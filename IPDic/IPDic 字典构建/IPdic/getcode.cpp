#include "IPdic.h"
#include <sys/time.h>
#include <cstring>

int main()
{
	Convert objConvert;
	char arryCode[MAX_CONVERT_DIGIT][CONVERT_FIGURES] = { 0 };
	objConvert.InitCustomArry(arryCode);

        struct timeval tBegin, tEnd;
        gettimeofday(&tBegin, NULL);
	CIPdic dicIP;
	string strPath = "config//ip_out.txt";
	if (!dicIP.ImportNewIPDic(strPath.c_str()))
	{
		return 0;
	}
        gettimeofday(&tEnd, NULL);
        printf("ImportNewIPDic cost times: %d.%d\n", tEnd.tv_sec - tBegin.tv_sec, tEnd.tv_usec - tBegin.tv_usec);

        gettimeofday(&tBegin, NULL);
        int iCount = 1000 * 10000;
        string strCode;
        string strIP = "87.214.8.112";
	//unsigned int iNewIP = dicIP.Str2Int((strIP));
        for(int i = 0; i < iCount; i++)
        {
           strCode = dicIP.GetIPCode(strIP);
        }
        gettimeofday(&tEnd, NULL);
        printf("GetIPCode调用%d次：(%s -> %s) cost times: %d.%d\n", iCount, strIP.c_str(), strCode.c_str(), tEnd.tv_sec - tBegin.tv_sec, tEnd.tv_usec - tBegin.tv_usec);

        /*while(1)
	{
		string strIP;
                cin >> strIP;
                string strCode = dicIP.GetIPCode(strIP);
                cout << strCode << "\n";
        }*/

	cout << "完成！\n";
	getchar();
}
