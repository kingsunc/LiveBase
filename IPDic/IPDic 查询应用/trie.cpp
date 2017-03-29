// Test.cpp : Defines the entry point for the console application.  
//  
#include "trie.h"
#include <cstring>

Trie::TrieNode::TrieNode()
{
	m_pData = nullptr;
	for (int i = 0; i < MAX_CHARS_NUM; ++i)
	{
		m_pNodes[i] = nullptr;
	}
}

Trie::TrieNode::~TrieNode()
{
	if (nullptr != m_pData)
	{
		m_pData->Release();
		m_pData = nullptr;
	}

	for (int i = 0; i < MAX_CHARS_NUM; i++)
	{
		if (nullptr != m_pNodes[i])
		{
			delete m_pNodes[i];
			m_pNodes[i] = nullptr;
		}
	}
}

Trie::Trie() :m_pRoot(nullptr)
{
}

Trie::~Trie()
{
	if (nullptr != m_pRoot)
	{
		delete m_pRoot;
		m_pRoot = nullptr;
	}
}

int Trie::Search(const char* strKey, Routes* pValue)
{
	if (nullptr == pValue)
	{
		return 0;
	}

	int iPos = 0;  // 层数;
	int iCode = 0;
	TrieNode *location = m_pRoot;  // 从根结点开始; 
	while (location != nullptr && *strKey != 0)
	{
		iCode = GetCode(*strKey);
		if (iCode < 0)
		{
			// 不合法的单词;
			return 0;
		} 

		// 转入相应分支指针;
		if(nullptr != location->m_pNodes[iCode])
		{
			location = location->m_pNodes[iCode];
		}
		else
		{
			break;
		}

		iPos++;
		strKey++;
	}

	// 找到,获取数据,成功返回; 
	if (location != nullptr && location->m_pData != nullptr)
	{
		location->m_pData->Copy(pValue);
		return 1;
	}

	return 0;
}

int Trie::Insert(const char* strKey, Routes& objData)
{
	int result = 1;
	int iPos = 0;
	if (m_pRoot == nullptr)
	{
		m_pRoot = new TrieNode;		// 初始插入，根结点为空;
	}

	int iCode = 0;
	TrieNode* location = m_pRoot;	// 从根结点开始; 
	while (location != nullptr && *strKey != 0)
	{
		iCode = GetCode(*strKey);
		if (iCode < 0)
		{
			// 不合法的单词;
			return 0;
		}

		// 不存在此分支; 
		if (location->m_pNodes[iCode] == nullptr)
		{
			location->m_pNodes[iCode] = new TrieNode;    // 创建空分支;
		}

		// 转入分支;
		location = location->m_pNodes[iCode];
		iPos++;
		strKey++;
	}

	if (location->m_pData != nullptr)
	{
		// 欲插入的单词已经存在;
		result = 0; 
	}
	else 
	{    // 插入数据;
		location->m_pData = objData.Clone();
	}
	return result;
}

int Trie::GetCode(char ch)
{
	int iResult = -1;
	if (ch >= '0' && ch <= '9')
	{
		iResult = ch - '0';
	}
	else if (ch >= 'A' && ch <= 'Z')
	{
		iResult = ch - 'A' + 10;
	}

	return iResult;
}