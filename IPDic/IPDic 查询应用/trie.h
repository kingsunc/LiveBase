// stdafx.h : include file for standard system include files,  
// or project specific include files that are used frequently, but  
// are changed infrequently  
//  

#pragma once  

#include <cstdio>    
#include <cstdlib> 
#include <iostream>  
#include <cstring>  
using namespace std;

//�궨��      
#define TRUE   1      
#define FALSE   0     
#define nullptr 0  
#define OK    1      
#define ERROR   0    
#define INFEASIBLE -1      
#define OVERFLOW -2    

#define MAX_CHARS_NUM 36

class Routes
{
public:
	string strMatchCode;
	string strCarrier;
	string strCountry;
	string strProvince;
	string strCity;
	string strArea;

	bool Copy(Routes* const pValue)
	{
		if (nullptr == pValue)
		{
			return false;
		}
		pValue->strMatchCode = this->strMatchCode;
		pValue->strCarrier = this->strCarrier;
		pValue->strCountry = this->strCountry;
		pValue->strProvince = this->strProvince;
		pValue->strCity = this->strCity;
		pValue->strArea = this->strArea;

		return true;
	}
	Routes* Clone()
	{
		Routes* pReturn = new Routes();
		if (nullptr != pReturn)
		{
			Copy(pReturn);
		}
		return pReturn;
	}
	void Release()
	{
		delete this;
	}
};

class Trie
{
public:
	Trie();
	Trie(Trie& tr);
	virtual ~Trie();
	int Search(const char* strKey, Routes* pValue);
	int Insert(const char* strKey,  Routes& objData);
	//int Remove(const char* strKey, Routes* pValue);

protected:

	struct TrieNode
	{
		Routes*		m_pData;	// ����Ϊ�գ���ʾ��root���˽�㹹��һ������;
		TrieNode*	m_pNodes[MAX_CHARS_NUM];	// ��֧;  
		TrieNode();				// ���캯��;
		~TrieNode();
	};

	TrieNode*	m_pRoot;		// �����(ָ��);

private:
	int GetCode(char ch);
};