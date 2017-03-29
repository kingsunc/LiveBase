#include "IPdic.h"
#include <cstdlib>
#include <ctime>  
#include <regex>
//#include <arpa/inet.h>

string g_strLastEndIP = "000.000.000.000";

CIPdic::CIPdic()
{
	if (!InitCarrierConvert())
	{
		cout << "InitCarrierConvert" << "失败！\n";
	}
}

CIPdic::~CIPdic()
{
}

int CIPdic::GetCarrierID(string strName)
{
	MapCarrier::iterator it = m_mapCarrier.find(strName);
	if (it != m_mapCarrier.end())
	{
		return  m_mapCarrier[strName].m_iID;
	}
	else
	{
		printf("GetCarrierID未找到： %s", strName.c_str());
	}
	return -1;
}

int CIPdic::GetCountryID(string strCountry)
{
	MapCountry::iterator it = m_mapCountry.find(strCountry);
	if (it != m_mapCountry.end())
	{
		return  m_mapCountry[strCountry].m_iID;
	}
	else
	{
		printf("GetCountryID未找到： %s", strCountry.c_str());
	}
	return -1;
}

int CIPdic::GetProvinceID(string strCountry, string strProvince)
{
	map<string, Province>& mapProvince = m_mapCountry[strCountry].m_mapProvince;
	map<string, Province>::iterator it = mapProvince.find(strProvince);
	if (it != mapProvince.end())
	{
		return  mapProvince[strProvince].m_iID;
	}
	else
	{
		printf("GetProvinceID未找到：%s-%s", strCountry.c_str(), strProvince.c_str());
	}
	return -1;
}

int CIPdic::GetCityID(string strCountry, string strProvince, string strCity)
{
	map<string, City>& mapCity = m_mapCountry[strCountry].m_mapProvince[strProvince].m_mapCity;
	map<string, City>::iterator it = mapCity.find(strCity);
	if (it != mapCity.end())
	{
		return  mapCity[strCity].m_iID;
	}
	else
	{
		printf("GetCityID未找到：%s-%s-%s", strCountry.c_str(), strProvince.c_str(), strCity.c_str());
	}

	return -1;
}

int CIPdic::GetAreaID(string strCountry, string strProvince, string strCity, string strArea)
{
	map<string, Area>& mapArea = m_mapCountry[strCountry].m_mapProvince[strProvince].m_mapCity[strCity].m_mapArea;
	map<string, Area>::iterator it = mapArea.find(strArea);
	if (it != mapArea.end())
	{
		return  mapArea[strArea].m_iID;
	}
	else
	{
		printf("GetCityID未找到：%s-%s-%s-%s", strCountry.c_str(), strProvince.c_str(), strCity.c_str(), strArea.c_str());
	}

	return -1;
}

bool CIPdic::ImportIPDic(string strPath)
{
	ifstream inFile(strPath.c_str());
	if (!inFile.is_open())
	{
		cout << "打开导入文件" << strPath.c_str() << "失败！\n";
		inFile.close();
		return false;
	}

	while (!inFile.eof())
	{
		std::string strTemp[E_IP_COUNT];

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

		if ( strTemp[E_IP_COUNTRY].empty() || (0 == strTemp[E_IP_COUNTRY].compare("*")) )
		{
			strTemp[E_IP_PROVINCE] = "*";
		}
		if ( strTemp[E_IP_PROVINCE].empty() || (0 == strTemp[E_IP_PROVINCE].compare("*")) )
		{
			strTemp[E_IP_CITY] = "*";
		}
		if ( strTemp[E_IP_CITY].empty() || (0 == strTemp[E_IP_CITY].compare("*")) )
		{
			strTemp[E_IP_AREA] = "*";
		}

		string strCarrierName = GetCarrierShortName(strTemp[E_IP_CARRIER]);
		MapCarrier::iterator itCarrier = m_mapCarrier.find(strCarrierName);
		if (itCarrier == m_mapCarrier.end())
		{
			string strCountry;
			if (0 == strTemp[E_IP_PROVINCE].compare("香港"))
			{
				strCountry = "中国香港";
			}
			else if (0 == strTemp[E_IP_PROVINCE].compare("澳门"))
			{
				strCountry = "中国澳门";
			}
			else if (0 == strTemp[E_IP_PROVINCE].compare("台湾"))
			{
				strCountry = "中国台湾";
			}
			else
			{
				strCountry = strTemp[E_IP_COUNTRY];
			}

			Carrier tempCarrier;
			tempCarrier.m_strName = strCarrierName;
			tempCarrier.m_strCountry = strCountry;
			tempCarrier.m_iID = m_mapCarrier.size() + 1;
			m_mapCarrier[tempCarrier.m_strName] = tempCarrier;
		}

		MapCountry::iterator itCoutry = m_mapCountry.find(strTemp[E_IP_COUNTRY]);
		if (itCoutry == m_mapCountry.end())
		{
			Area tempArea;
			tempArea.m_strName = strTemp[E_IP_AREA];

			City tempCity;
			if (0 != tempArea.m_strName.compare("*"))
			{
				tempArea.m_iID = tempCity.m_iNextAllocID++;
			}
			tempCity.m_mapArea[tempArea.m_strName] = tempArea;
			tempCity.m_strName = strTemp[E_IP_CITY];

			Province tempProvince;
			if (0 != tempCity.m_strName.compare("*"))
			{
				tempCity.m_iID = tempProvince.m_iNextAllocID++;
			}
			tempProvince.m_mapCity[tempCity.m_strName] = tempCity;
			
			tempProvince.m_strName = strTemp[E_IP_PROVINCE];
			Country tempCountry;
			if (0 != tempProvince.m_strName.compare("*"))
			{
				tempProvince.m_iID = tempCountry.m_iNextAllocID++;
			}
			tempCountry.m_mapProvince[tempProvince.m_strName] = tempProvince;

			if ( (!tempProvince.m_strName.empty()) && (0 != tempProvince.m_strName.compare("*")) )
			{
				// 插入一条省为*的数据;
				Area tempArea;
				tempArea.m_strName = "*";

				City tempCity;
				if (0 != tempArea.m_strName.compare("*"))
				{
					tempArea.m_iID = tempCity.m_iNextAllocID++;
				}
				tempCity.m_mapArea[tempArea.m_strName] = tempArea;
				tempCity.m_strName = "*";

				Province tempProvince;
				if (0 != tempCity.m_strName.compare("*"))
				{
					tempCity.m_iID = tempProvince.m_iNextAllocID++;
				}
				tempProvince.m_mapCity[tempCity.m_strName] = tempCity;

				tempProvince.m_strName = "*";
				if (0 != tempProvince.m_strName.compare("*"))
				{
					tempProvince.m_iID = tempCountry.m_iNextAllocID++;
				}

				tempCountry.m_mapProvince[tempProvince.m_strName] = tempProvince;
			}

			tempCountry.m_strName = strTemp[E_IP_COUNTRY];
			if (0 != tempCountry.m_strName.compare("*"))
			{
				tempCountry.m_iID = m_mapCountry.size() + 1;
			}
			m_mapCountry[tempCountry.m_strName] = tempCountry;
		}
		else
		{
			int iProvinceTemp = 0;
			map<string, Province>& mapProvince = itCoutry->second.m_mapProvince;
			map<string, Province>::iterator itProvince = mapProvince.find(strTemp[E_IP_PROVINCE]);
			if (itProvince == mapProvince.end())
			{
				Area tempArea;
				tempArea.m_strName = strTemp[E_IP_AREA];

				City tempCity;
				if (0 != tempArea.m_strName.compare("*"))
				{
					tempArea.m_iID = tempCity.m_iNextAllocID++;
				}
				tempCity.m_mapArea[tempArea.m_strName] = tempArea;
				tempCity.m_strName = strTemp[E_IP_CITY];

				Province tempProvince;
				if (0 != tempCity.m_strName.compare("*"))
				{
					tempCity.m_iID = tempProvince.m_iNextAllocID++;
				}
				tempProvince.m_mapCity[tempCity.m_strName] = tempCity;

				if ((!tempCity.m_strName.empty()) && (0 != tempCity.m_strName.compare("*")))
				{
					// 插入一条市为*的数据;
					Area tempArea;
					tempArea.m_strName = "*";

					City tempCity;
					if (0 != tempArea.m_strName.compare("*"))
					{
						tempArea.m_iID = tempCity.m_iNextAllocID++;
					}
					tempCity.m_mapArea[tempArea.m_strName] = tempArea;
					tempCity.m_strName = "*";

					if (0 != tempCity.m_strName.compare("*"))
					{
						tempCity.m_iID = tempProvince.m_iNextAllocID++;
					}
					tempProvince.m_mapCity[tempCity.m_strName] = tempCity;
				}

				tempProvince.m_strName = strTemp[E_IP_PROVINCE];
				if (0 != tempProvince.m_strName.compare("*"))
				{
					tempProvince.m_iID = itCoutry->second.m_iNextAllocID++;
				}
				mapProvince[tempProvince.m_strName] = tempProvince;
			}
			else
			{
				int iCityTemp = 0;
				map<string, City>& mapCity = itProvince->second.m_mapCity;
				map<string, City>::iterator itCity = mapCity.find(strTemp[E_IP_CITY]);
				if (itCity == mapCity.end())
				{
					Area tempArea;
					tempArea.m_strName = strTemp[E_IP_AREA];

					City tempCity;
					if (0 != tempArea.m_strName.compare("*"))
					{
						tempArea.m_iID = tempCity.m_iNextAllocID++;
					}
					tempCity.m_mapArea[tempArea.m_strName] = tempArea;

					if ( (!tempArea.m_strName.empty()) && (0 != tempArea.m_strName.compare("*")) )
					{
						// 插入一条区域为*的数据;
						Area tempArea;
						tempArea.m_strName = "*";

						if (0 != tempArea.m_strName.compare("*"))
						{
							tempArea.m_iID = tempCity.m_iNextAllocID++;
						}
						tempCity.m_mapArea[tempArea.m_strName] = tempArea;
					}

					tempCity.m_strName = strTemp[E_IP_CITY];
					if (0 != tempCity.m_strName.compare("*"))
					{
						tempCity.m_iID = itProvince->second.m_iNextAllocID++;
					}
					mapCity[tempCity.m_strName] = tempCity;
				}
				else
				{
					int iAreaTemp = 0;
					map<string, Area>& mapArea = itCity->second.m_mapArea;
					map<string, Area>::iterator itArea = mapArea.find(strTemp[E_IP_AREA]);
					if (itArea == mapArea.end())
					{
						Area tempArea;
						tempArea.m_strName = strTemp[E_IP_AREA];

						if (0 != tempArea.m_strName.compare("*"))
						{
							tempArea.m_iID = itCity->second.m_iNextAllocID++;
						}
						mapArea[tempArea.m_strName] = tempArea;
					}
				}
			}
		}
	}

	inFile.close();
	return true;
}

bool CIPdic::ImportCarrierDic(string strPath)
{
	ifstream inFile(strPath.c_str());
	if (!inFile.is_open())
	{
		cout << "打开导入文件" << strPath.c_str() << "失败！\n";
		inFile.close();
		return false;
	}

	while (!inFile.eof())
	{
		std::string strTemp[E_CARRIER_COUNT];

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

		string strName = strTemp[E_CARRIER_NAME];
		MapCarrier::iterator itCarrier = m_mapCarrier.find(strName);
		if (itCarrier == m_mapCarrier.end())
		{
			Carrier tempCarrier;
			tempCarrier.m_strName = strName;
			tempCarrier.m_iID = atoi(strTemp[E_CARRIER_ID].c_str());
			m_mapCarrier[strName] = tempCarrier;
		}
	}

	inFile.close();
	return true;
}

bool CIPdic::ImportAreaDic(string strPath)
{
	ifstream inFile(strPath.c_str());
	if (!inFile.is_open())
	{
		cout << "打开导入文件" << strPath.c_str() << "失败！\n";
		inFile.close();
		return false;
	}

	while (!inFile.eof())
	{
		std::string strTemp[E_AREA_COUNT];

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

		std::string strKey;
		strKey.append(strTemp[E_AREA_COUNTRY_NAME]);
		strKey.append("_");
		strKey.append(strTemp[E_AREA_PROVINCE_NAME]);
		strKey.append("_");
		strKey.append(strTemp[E_AREA_CITY_NAME]);
		strKey.append("_");
		strKey.append(strTemp[E_AREA_AREA_NAME]);

		m_mapAreaItem[strKey] = strTemp[E_AREA_CODE].c_str();
	}

	inFile.close();
	return true;
}

bool CIPdic::ExportCarrierDic(string strPath, char arryCode[MAX_CONVERT_DIGIT][CONVERT_FIGURES])
{
	ofstream outFile(strPath.c_str());
	if (!outFile.is_open())
	{
		cout << "打开导出文件" << strPath.c_str() << "失败！\n";
		outFile.close();
		return false;
	}

	for (MapCarrier::iterator itCarrier = m_mapCarrier.begin(); itCarrier != m_mapCarrier.end(); itCarrier++)
	{
		int iID = itCarrier->second.m_iID;
		string strCode = "";
		if (iID < MAX_CONVERT_DIGIT)
		{
			strCode = arryCode[iID];
		}
		else
		{
			cout << "Error: iID >= MAX_CONVERT_DIGIT !\n";
		}
		outFile << iID << "\t" << itCarrier->second.m_strCountry.c_str() << "\t" << itCarrier->second.m_strName.c_str() << "\t" << strCode.c_str() << "\t" << endl;
	}

	/*ifstream inFile("..//config//org_aa.txt");
	ofstream outFile("..//config//CarrierDic.txt");
	if (!inFile.is_open())
	{
		cout << "打开导入文件" << strPath.c_str() << "失败！\n";
		return false;
	}

	int iCount = -1;
	while (!inFile.eof())
	{
		std::string strTemp[E_CARRIER_COUNT];

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

		++iCount;
		outFile << iCount << "\t" << strTemp[0].c_str() << "\t" << strTemp[1].c_str() << "\t" << arryCode[iCount] << "\t\n";
	}*/

	outFile.close();
	return true;
}

bool CIPdic::ExportAreaDic(string strPath, char arryCode[MAX_CONVERT_DIGIT][CONVERT_FIGURES])
{
	ofstream outFile(strPath.c_str());
	if (!outFile.is_open())
	{
		cout << "打开导出文件" << strPath.c_str() << "失败！\n";
		outFile.close();
		return false;
	}

	int iCount = 0;
	for (MapCountry::iterator itCoutry = m_mapCountry.begin(); itCoutry != m_mapCountry.end(); itCoutry++)
	{
		map<string, Province>& mapProvince = itCoutry->second.m_mapProvince;
		for (map<string, Province>::iterator itPro = mapProvince.begin(); itPro != mapProvince.end(); itPro++)
		{
			map<string, City>& mapCity = itPro->second.m_mapCity;
			for (map<string, City>::iterator itCity = mapCity.begin(); itCity != mapCity.end(); itCity++)
			{
				map<string, Area>& mapArea = itCity->second.m_mapArea;
				for (map<string, Area>::iterator itArea = mapArea.begin(); itArea != mapArea.end(); itArea++)
				{
					std::string strCoutryCode = itCoutry->second.m_iID ? arryCode[itCoutry->second.m_iID] : "";
					std::string strProCode;
					if (!strCoutryCode.empty())
					{
						strProCode = itPro->second.m_iID ? arryCode[itPro->second.m_iID] : "";
					}
					std::string strCityCode;
					if ((!strCoutryCode.empty()) && (!strProCode.empty()))
					{
						strCityCode = itCity->second.m_iID ? arryCode[itCity->second.m_iID] : "";
					}
					std::string strAreaCode;
					if ((!strCoutryCode.empty()) && (!strProCode.empty()) && (!strCityCode.empty()))
					{
						strAreaCode = itArea->second.m_iID ? arryCode[itArea->second.m_iID] : "";
					}
					outFile << ++iCount << "\t" 
						<< strCoutryCode.c_str() << strProCode.c_str() << strCityCode.c_str() << strAreaCode.c_str() << "\t"
						<< itCoutry->second.m_iID << "\t" << itCoutry->second.m_strName.c_str() << "\t" << arryCode[itCoutry->second.m_iID] << "\t"
						<< itPro->second.m_iID << "\t" << itPro->second.m_strName.c_str() << "\t" << arryCode[itPro->second.m_iID] << "\t"
						<< itCity->second.m_iID << "\t" << itCity->second.m_strName.c_str() << "\t" << arryCode[itCity->second.m_iID] << "\t"
						<< itArea->second.m_iID << "\t" << itArea->second.m_strName.c_str() << "\t" << arryCode[itArea->second.m_iID] << "\t\n";
				}
			}
		}
	}

	outFile.close();
	return true;
}

std::string CIPdic::GetCarrierShortName(string strOrgName)
{
	MapConvert::iterator itName = m_mapCarrierConvert.find(strOrgName);
	if (itName != m_mapCarrierConvert.end())
	{
		return m_mapCarrierConvert[strOrgName];
	}

	return strOrgName;
}

bool CIPdic::InitCarrierConvert()
{
	string strPath = "..//config//CarrierConvert.txt";
	ifstream inFile(strPath.c_str());
	if (!inFile.is_open())
	{
		cout << "打开导入文件" << strPath.c_str() << "失败！\n";
		inFile.close();
		return false;
	}

	while (!inFile.eof())
	{
		std::string strTemp[E_CONVERT_COUNT];
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

		MapConvert::iterator itItem = m_mapCarrierConvert.find(strTemp[E_CONVERT_ORGNAME]);
		if (itItem == m_mapCarrierConvert.end())
		{
			m_mapCarrierConvert[strTemp[E_CONVERT_ORGNAME]] = strTemp[E_CONVERT_CONNAME];
		}
	}

	inFile.close();
	return true;
}

bool CIPdic::ImportNewAreaDic(string strPath)
{
	ifstream inFile(strPath.c_str());
	if (!inFile.is_open())
	{
		cout << "打开导入文件" << strPath.c_str() << "失败！\n";
		inFile.close();
		return false;
	}

	while (!inFile.eof())
	{
		std::string strTemp[E_AREA_COUNT];

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

		CNewArea newArea;
		newArea.m_strCode = strTemp[E_AREA_CODE];
		newArea.m_strCountry = strTemp[E_AREA_COUNTRY_NAME];
		newArea.m_strProvince = strTemp[E_AREA_PROVINCE_NAME];
		newArea.m_strCity = strTemp[E_AREA_CITY_NAME];
		newArea.m_strArea = strTemp[E_AREA_AREA_NAME];

		m_mapNewArea[newArea.m_strCode] = newArea;
	}

	inFile.close();
	return true;
}

bool CIPdic::ImportNewCarrierDic(string strPath)
{
	ifstream inFile(strPath.c_str());
	if (!inFile.is_open())
	{
		cout << "打开导入文件" << strPath.c_str() << "失败！\n";
		inFile.close();
		return false;
	}

	while (!inFile.eof())
	{
		std::string strTemp[E_CARRIER_COUNT];

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

		string strCode = strTemp[E_CARRIER_CODE];
		MapCarrier::iterator itCarrier = m_mapNewCarrier.find(strCode);
		if (itCarrier == m_mapNewCarrier.end())
		{
			Carrier tempCarrier;
			tempCarrier.m_strCode = strCode;
			tempCarrier.m_strName = strTemp[E_CARRIER_NAME];
			tempCarrier.m_iID = atoi(strTemp[E_CARRIER_ID].c_str());
			m_mapNewCarrier[strCode] = tempCarrier;
		}
	}

	inFile.close();
	return true;
}

bool CIPdic::OldIP2NewIP(string strOldPath, string strNewPath, char arryCode[MAX_CONVERT_DIGIT][CONVERT_FIGURES])
{
	ifstream inFile(strOldPath.c_str());
	if (!inFile.is_open())
	{
		cout << "打开导入文件" << strOldPath.c_str() << "失败！\n";
		inFile.close();
		return false;
	}
	ofstream outFile(strNewPath.c_str());
	if (!outFile.is_open())
	{
		cout << "打开导出文件" << strNewPath.c_str() << "失败！\n";
		outFile.close();
		return false;
	}

	string strExtra = "..//config//extra.txt";
	ofstream outExtra(strExtra.c_str());
	if (!outExtra.is_open())
	{
		cout << "打开导出文件" << strNewPath.c_str() << "失败！\n";
		return false;
	}

	while (!inFile.eof())
	{
		std::string strTemp[E_IP_COUNT];

		char strBuff[256] = {0};
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

		string strStartIP = strTemp[E_IP_STARTIP];
		string strEndIP = strTemp[E_IP_ENDIP];
		if ((strStartIP.compare(strEndIP)) > 0 || (g_strLastEndIP.compare(strStartIP)) >= 0)
		{
			// 若 strStartIP > strEndIP 或者 g_strLastEndIP >= strStartIP， 则出现IP范围异常或重叠;
			outExtra << "IP范围重叠:\t" << strBuff << "\n";
			printf("范围重叠: StartIP:%s , EndIP:%s \n", strStartIP.c_str(), strEndIP.c_str());
			continue;
		}
		g_strLastEndIP = strEndIP;

		if (strTemp[E_IP_COUNTRY].empty() || (0 == strTemp[E_IP_COUNTRY].compare("*")))
		{
			strTemp[E_IP_PROVINCE] = "*";
		}
		if (strTemp[E_IP_PROVINCE].empty() || (0 == strTemp[E_IP_PROVINCE].compare("*")))
		{
			strTemp[E_IP_CITY] = "*";
		}
		if (strTemp[E_IP_CITY].empty() || (0 == strTemp[E_IP_CITY].compare("*")))
		{
			strTemp[E_IP_AREA] = "*";
		}

		std::string strCode;
		{
			string strName = GetCarrierShortName(strTemp[E_IP_CARRIER]);
			if (strName.empty())
			{
				outExtra << "Carrier:\t" << strBuff << "\n";
				cout << strName.c_str() << ": CarrierItem未找到！ \n";
				continue;
			}
			int iCarrierID = GetCarrierID(strName);
			if (iCarrierID >= 0)
			{
				strCode.append(arryCode[iCarrierID]);
			}
			else
			{
				outExtra << "Carrier:\t" << strBuff << "\n";
				cout << strName.c_str() << ": CarrierID未找到！ \n";
				strCode.append("00");
				continue;
			}
		}

		std::string strKey;
		strKey.append(strTemp[E_IP_COUNTRY]);
		strKey.append("_");
		strKey.append(strTemp[E_IP_PROVINCE]);
		strKey.append("_");
		strKey.append(strTemp[E_IP_CITY]);
		strKey.append("_");
		strKey.append(strTemp[E_IP_AREA]);
		{
			MapAreaItem::iterator itItem = m_mapAreaItem.find(strKey);
			if (itItem != m_mapAreaItem.end())
			{
				strCode.append(m_mapAreaItem[strKey]);
			}
			else
			{
				outExtra << "Area:\t" << strBuff << "\n";
				cout << strKey.c_str() << ": AreaItem未找到！ \n";
				continue;
			}
		}

		string strSimplyStartIP = GetSimplifyIP(strStartIP);
		string strSimplyEndIP = GetSimplifyIP(strEndIP);
		outFile << strSimplyStartIP.c_str() << "\t"
			<< strSimplyEndIP.c_str() << "\t"
                        << "R" << strCode.c_str() << "\t\n";
	}

	inFile.close();
	outFile.close();
	return true;
}

bool CIPdic::ToIPList(int src[IP_COUNT], string strIP)
{
	int i = 0;
	memset(src, 0, IP_COUNT);
	// 处理IP地址：4个字节
	while (!strIP.empty())
	{
		string strTemp;
		int iPos = strIP.find(".");
		if (iPos > -1)
		{
			strTemp = strIP.substr(0, iPos);
			src[i] = atoi(strTemp.c_str());
			strIP = strIP.substr(iPos + 1, strIP.length() - (iPos + 1));
		}
		else
		{
			src[i] = atoi(strIP.c_str());
			strIP = "";
		}

		if ((src[i] < 0) || (src[i] > 255))
		{
			printf("不合理的IP地址转换： %s", strIP.c_str());
			return false;
		}
		i++;
	}

	return true;
}

bool CIPdic::ImportNewIPDic(string strPath)
{
	//ifstream inFile(strPath.c_str());
	//if (!inFile.is_open())
	//{
	//	cout << "打开导入文件" << strPath.c_str() << "失败！\n";
	//	inFile.close();
	//	return false;
	//}

	//while (!inFile.eof())
	//{
	//	std::string strTemp[E_NEWIP_COUNT];

	//	char strBuff[256];
	//	inFile.getline(strBuff, 256);

	//	int iStart = 0;
	//	int iIndex = 0;
	//	for (int i = 0; i < (int)strlen(strBuff); i++)
	//	{
	//		if ('\t' == strBuff[i])
	//		{
	//			strTemp[iIndex++] = string(strBuff, iStart, i - iStart);
	//			iStart = i + 1;
	//		}
	//	}

	//	unsigned long iStartIP = htonl(inet_addr(strTemp[E_NEWIP_STARTIP].c_str()));
	//	unsigned long iEndIP = htonl(inet_addr(strTemp[E_NEWIP_ENDIP].c_str()));
	//	MapNewIP::iterator itIP = m_mapNewIP.find(iEndIP);
	//	if (itIP == m_mapNewIP.end())
	//	{
	//		CNewIP newIP;
	//		newIP.m_iStartIP = iStartIP;
	//		newIP.m_iEndIP = iEndIP;
	//		newIP.m_strCode = strTemp[E_NEWIP_CODE];

	//		m_mapNewIP[iEndIP] = newIP;
	//	}
	//}

	//inFile.close();
	//return true;

	ifstream inFile(strPath.c_str());
	ofstream outFile("..//config//ip_route.txt");
	if (!inFile.is_open() || !outFile.is_open())
	{
		cout << "打开导入文件" << strPath.c_str() << "失败！\n";
		inFile.close();
		return false;
	}

	typedef map<std::string, int> MapStr;
	MapStr mapstr;

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

		string strKey = strTemp[E_NEWIP_CODE].substr(0, 5);
		mapstr[strKey] = 1;

		/*unsigned long iStartIP = htonl(inet_addr(strTemp[E_NEWIP_STARTIP].c_str()));
		unsigned long iEndIP = htonl(inet_addr(strTemp[E_NEWIP_ENDIP].c_str()));
		MapNewIP::iterator itIP = m_mapNewIP.find(iEndIP);
		if (itIP == m_mapNewIP.end())
		{
			CNewIP newIP;
			newIP.m_iStartIP = iStartIP;
			newIP.m_iEndIP = iEndIP;
			newIP.m_strCode = strTemp[E_NEWIP_CODE];

			m_mapNewIP[iEndIP] = newIP;
		}*/
	}

	for (MapStr::iterator it = mapstr.begin(); it != mapstr.end(); it++)
	{
		outFile << it->first << "\n";
	}

	inFile.close();
	outFile.close();
	return true;
}

string CIPdic::GetIPCode(string strNewIP)
{
	//unsigned long iNewIP = htonl(inet_addr(strNewIP.c_str()));		
	//MapNewIP::iterator itNewIP = m_mapNewIP.lower_bound(iNewIP);
	//if (itNewIP != m_mapNewIP.end() && itNewIP->second.m_iStartIP <= iNewIP)
	//   {
	//           return itNewIP->second.m_strCode;
	//   }

	//  printf("m_mapNewIP 查找不到: %s\n", strNewIP.c_str());
   return "R";
}

bool CIPdic::GetNewAreaCode(CNewArea& newArea, string& strCarrier, string strNewIP)
{
	/*MapNewIP::iterator itNewIP = m_mapNewIP.lower_bound(strNewIP);
	if (itNewIP != m_mapNewIP.end() && itNewIP->second.m_iStartIP <= strNewIP)
	{
		string strCode = itNewIP->second.m_strCode;
		string strCarrierCode = strCode.substr(0, 2);
		MapCarrier::iterator itCarrier = m_mapNewCarrier.find(strCarrierCode);
		if (itCarrier != m_mapNewCarrier.end())
		{
			strCarrier = m_mapNewCarrier[strCarrierCode].m_strName;
		}
		else
		{
			cout << "m_mapNewCarrier 查找不到\n";
		}

		string strAreaCode = strCode.substr(2, strCode.length() - 2);
		MapNewArea::iterator itArea = m_mapNewArea.find(strAreaCode);
		if (itArea != m_mapNewArea.end())
		{
			newArea = m_mapNewArea[strAreaCode];
			return true;
		}
		else
		{
			cout << "m_mapNewArea 查找不到\n";
		}
	}
	else
	{
		cout << "m_mapNewIP 查找不到\n";
	}*/

	return false;
}

std::string CIPdic::GetSimplifyIP(string strIP)
{
	char strSimplifyIP[17] = "0.0.0.0";
	//int arrIP[IP_COUNT];
	//if (ToIPList(arrIP, strIP.c_str()))
	//{
	//	sprintf(strSimplifyIP, "%d.%d.%d.%d", arrIP[0], arrIP[1], arrIP[2], arrIP[3]);
	//}
	return strSimplifyIP;
}