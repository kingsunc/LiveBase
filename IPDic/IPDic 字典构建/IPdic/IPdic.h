#pragma once
#include "Area.h"
#include "Carrier.h"
#include "Convert.h"
#include <fstream>
#include <iomanip>
#include <cstdio>
//#include <arpa/inet.h>

#define IP_COUNT (5)

typedef map<string, Country> MapCountry;
typedef map<string, Carrier> MapCarrier;
typedef map<string, string>  MapAreaItem;
typedef map<string, string>  MapConvert;
typedef map<unsigned long, CNewIP>  MapNewIP;
typedef map<string, CNewArea>  MapNewArea;

// ip库;
enum 
{
	E_IP_STARTIP = 0,
	E_IP_ENDIP,
	E_IP_COUNTRY,
	E_IP_PROVINCE,
	E_IP_CITY,
	E_IP_AREA,
	E_IP_CARRIER,
	E_IP_COUNT
};

// 运营商转换字典;
enum
{
	E_CONVERT_COUNTRY = 0,
	E_CONVERT_ORGNAME,
	E_CONVERT_CONNAME,
	E_CONVERT_COUNT
};

// 运营商字典;
enum
{
	E_CARRIER_ID = 0,
	E_CARRIER_NAME,
	E_CARRIER_CODE,
	E_CARRIER_COUNTRY,
	E_CARRIER_COUNT
};

// 区域字典;
enum
{
	E_AREA_CODE = 0,
	E_AREA_COUNTRY_ID,
	E_AREA_COUNTRY_NAME,
	E_AREA_COUNTRY_CODE,
	E_AREA_PROVINCE_ID,
	E_AREA_PROVINCE_NAME,
	E_AREA_PROVINCE_CODE,
	E_AREA_CITY_ID,
	E_AREA_CITY_NAME,
	E_AREA_CITY_CODE,
	E_AREA_AREA_ID,
	E_AREA_AREA_NAME,
	E_AREA_AREA_CODE,
	E_AREA_COUNT
};

// 新IP字典;
enum
{
	E_NEWIP_STARTIP = 0,
	E_NEWIP_ENDIP,
	E_NEWIP_CODE,
	E_NEWIP_COUNT
};

class CIPdic
{
public:
	CIPdic();
	~CIPdic();

public: 
	int GetCarrierID(string strName);

	int GetCountryID(string strCountry);
	int GetProvinceID(string strCountry, string strProvince);
	int GetCityID(string strCountry, string strProvince, string strCity);
	int GetAreaID(string strCountry, string strProvince, string strCity, string strArea);

	// 导入原始IP库;
	bool ImportIPDic(string strPath);
	// 导入已创建运营商字典;
	bool ImportCarrierDic(string strPath);
	// 导入已创建区域字典;
	bool ImportAreaDic(string strPath);

	// 导入已创建区域字典;
	bool ImportNewCarrierDic(string strPath);
	// 导入已创建区域字典;
	bool ImportNewAreaDic(string strPath);
	// 导入最新字典;
	bool ImportNewIPDic(string strPath);

	bool OldIP2NewIP(string strOldPath, string strNewPath, char arryCode[MAX_CONVERT_DIGIT][CONVERT_FIGURES]);
	bool ExportCarrierDic(string strPath, char arryCode[MAX_CONVERT_DIGIT][CONVERT_FIGURES]);
	bool ExportAreaDic(string strPath, char arryCode[MAX_CONVERT_DIGIT][CONVERT_FIGURES]);

	string GetIPCode(string strNewIP);

	bool GetNewAreaCode(CNewArea& newArea, string& strCarrier, string strNewIP);

protected:
	string GetCarrierShortName(string strOrgName);

	bool InitCarrierConvert();

	bool ToIPList(int src[IP_COUNT], string strIP);

	// 获取简化的IP地址 例如：001.0001.001.001 -> 1.1.1.1
	string GetSimplifyIP(string strIP);

private:
	MapCountry		m_mapCountry;
	MapCarrier		m_mapCarrier;
	MapAreaItem		m_mapAreaItem;
	MapConvert		m_mapCarrierConvert;

	MapNewIP		m_mapNewIP;
	MapNewArea		m_mapNewArea;
	MapCarrier		m_mapNewCarrier;
};