#pragma once
#include <iostream>
#include <map>
#include <string>
#include <cstdint>
#include <cstring>
using namespace std;

// 区域;
class Area
{
public:
	Area() : m_iID(0)
	{
	}
	~Area()
	{
	}

	string		m_strName;
	int			m_iID;
};

// 地级市;
class City
{
public:
	City() : m_iID(0), m_iNextAllocID(1)
	{
	}
	~City()
	{
	}

	map<string, Area> m_mapArea;
	string		m_strName;
	int			m_iID;
	int			m_iNextAllocID;
};

// 省份;
class Province
{
public:
	Province() : m_iID(0), m_iNextAllocID(1)
	{
	}
	~Province()
	{
	}

	map<string, City> m_mapCity;
	string		m_strName;
	int			m_iID;
	int			m_iNextAllocID;
};

// 国家;
class Country
{
public:
	Country() : m_iID(0), m_iNextAllocID(1)
	{
	}
	~Country()
	{
	}

	map<string, Province> m_mapProvince;
	string		m_strName;
	int			m_iID;
	int			m_iNextAllocID;
};

class CNewIP
{
public:
	CNewIP()
	{
	}
	~CNewIP()
	{
	}

	unsigned 	m_iStartIP;
	unsigned 	m_iEndIP;
	string		m_strCode;
};

class CNewArea
{
public:
	CNewArea()
	{
	}
	~CNewArea()
	{
	}

	string		m_strCode;
	string		m_strCountry;
	string		m_strProvince;
	string		m_strCity;
	string		m_strArea;
};