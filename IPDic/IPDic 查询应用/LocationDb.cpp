#include <cstdio>
#include <fstream>
#include <arpa/inet.h>
#include <sys/time.h>
#include "LocationDb.h"

LocationDb::LocationDb()
{
	if(!LoadIPData())
	{
		printf("LoadIPData 加载IP数据失败！\n");
	}
}

LocationDb::~LocationDb()
{
}

bool LocationDb::LoadIPData()
{
	std::string strPath = "config//ip_out.txt";
	std::ifstream inFile(strPath.c_str());
	if (!inFile.is_open())
	{
		std::cout << "打开导入文件" << strPath.c_str() << "失败！\n";
		inFile.close();
		return false;
	}

	while (!inFile.eof())
	{
		std::string strTemp[E_NEWIP_COUNT];

		char strBuff[256];
		inFile.getline(strBuff, 256);

		int iStart = 0;
		int iIndex = 0;
		for (int i = 0; i < (int)strlen(strBuff); i++)
		{
			if ('\t' == strBuff[i])
			{
				strTemp[iIndex++] = string(strBuff, iStart, i - iStart);
				iStart = i + 1;
			}
		}

		unsigned long lStartIP = htonl(inet_addr(strTemp[E_NEWIP_STARTIP].c_str()));
		unsigned long lEndIP = htonl(inet_addr(strTemp[E_NEWIP_ENDIP].c_str()));
                if(lStartIP > lEndIP)
                {
                    printf("插入IP数据有错误：%s ~ %s\n", strTemp[E_NEWIP_STARTIP].c_str(), strTemp[E_NEWIP_ENDIP].c_str());
                    continue;
                }
                MapNewIP::iterator itIP = m_mapNewIP.lower_bound(lEndIP);
		if (itIP == m_mapNewIP.end())
		{
			CNewIP newIP;
			newIP.m_lStartIP = lStartIP;
			newIP.m_lEndIP = lEndIP;
			newIP.m_strCode = strTemp[E_NEWIP_CODE];
			m_mapNewIP[lEndIP] = newIP;
		}
                else
                {
                    printf("插入数据有重复：%s ~ %s\n", strTemp[E_NEWIP_STARTIP].c_str(), strTemp[E_NEWIP_ENDIP].c_str());
                }
	}

	inFile.close();
	return true;
}

std::string LocationDb::getLocation(const std::string& strIP)
{
	unsigned long lIP = htonl(inet_addr(strIP.c_str()));		
	MapNewIP::iterator itIP = m_mapNewIP.lower_bound(lIP);
	if (itIP != m_mapNewIP.end() && itIP->second.m_lStartIP <= lIP)
    {
		return itIP->second.m_strCode;
    }

   printf("m_mapNewIP 查找不到: %s\n", strIP.c_str());
   return "R";
}

#define _TEST	0
int main()
{
#if _TEST
	struct timeval tBegin, tEnd;

	gettimeofday(&tBegin, NULL);
	LocationDb db;
	gettimeofday(&tEnd, NULL);
	printf("ImportNewIPDic cost times: %d.%d\n", tEnd.tv_sec - tBegin.tv_sec, tEnd.tv_usec - tBegin.tv_usec);

	std::string strIP = "124.23.3.25";
	std::string strCode;
	int iCount = 1000 * 10000;
	
	gettimeofday(&tBegin, NULL);
	for(int i = 0; i < iCount; i++)
	{
		strCode = db.getLocation(strIP);
	}
	gettimeofday(&tEnd, NULL);
	printf("getLocation 调用%d次 cost times: %d.%d\n", iCount, tEnd.tv_sec - tBegin.tv_sec, tEnd.tv_usec - tBegin.tv_usec);
	printf("IP: %s -> Code: %s\n", strIP.c_str(), strCode.c_str());
#else
	LocationDb db;
	std::string strIP = "124.23.3.25";
	std::string strCode;
	strCode = db.getLocation(strIP);
	printf("IP: %s -> Code: %s\n", strIP.c_str(), strCode.c_str());
#endif
	return 0;
}
