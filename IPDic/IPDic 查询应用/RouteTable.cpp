#include <fstream>
#include <arpa/inet.h>
#include <sys/time.h>
#include "RouteTable.h"

RouteTable::RouteTable()
{
	if(!LoadRouteData())
	{
		printf("LoadRouteData 加载路线数据失败！\n");
	}
}

RouteTable::~RouteTable()
{
}

bool RouteTable::LoadRouteData()
{
	std::string strPath = "config//ip_route.txt";
	std::ifstream inFile(strPath.c_str());
	if (!inFile.is_open())
	{
		std::cout << "打开导入文件" << strPath.c_str() << "失败！\n";
		inFile.close();
		return false;
	}

	while (!inFile.eof())
	{
		std::string strTemp[E_ROUTE_COUNT];

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

		string strKey = strTemp[E_ROUTE_CODE];
		Routes routes;
		routes.strMatchCode = strTemp[E_ROUTE_CODE];
		routes.strCountry = strTemp[E_ROUTE_NO];
		
		m_objTrie.Insert(strKey.c_str(), routes);
	}

	inFile.close();
	return true;
}

Routes RouteTable::getRoute(const std::string& strCode)
{
	Routes routes;
	if(!m_objTrie.Search(strCode.c_str(), &routes))
	{
		printf("getRoute 失败：Code-%s\n", strCode.c_str());
	}
	return routes;
}

#define _TEST	0

int main()
{
#if _TEST
	struct timeval tBegin, tEnd;
	gettimeofday(&tBegin, NULL);
	RouteTable routeTab;
	gettimeofday(&tEnd, NULL);
	printf("routeTab加载 cost times: %d.%d\n", tEnd.tv_sec - tBegin.tv_sec, tEnd.tv_usec - tBegin.tv_usec);

	Routes routes;
	std::string strCode = "R02040303";
	int iCount = 1000 * 10000;

	gettimeofday(&tBegin, NULL);
	for(int i = 0; i < iCount; i++)
	{
		routes = routeTab.getRoute(strCode);
	}
	gettimeofday(&tEnd, NULL);
	printf("getRoute 调用%d次 cost times: %d.%d\n", iCount, tEnd.tv_sec - tBegin.tv_sec, tEnd.tv_usec - tBegin.tv_usec);
	printf("Code: %s -> MatchCode: %s -> Routes: %s \n", strCode.c_str(), routes.strMatchCode.c_str(), routes.strCountry.c_str());
#else
	RouteTable routeTab;
	std::string strCode = "R02040303";
	Routes routes = routeTab.getRoute(strCode);
	printf("Code: %s -> MatchCode: %s -> Routes: %s \n", strCode.c_str(), routes.strMatchCode.c_str(), routes.strCountry.c_str());
#endif

	return 0;
}