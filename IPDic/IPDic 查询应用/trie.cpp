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

	int iPos = 0;  // ����;
	int iCode = 0;
	TrieNode *location = m_pRoot;  // �Ӹ���㿪ʼ; 
	while (location != nullptr && *strKey != 0)
	{
		iCode = GetCode(*strKey);
		if (iCode < 0)
		{
			// ���Ϸ��ĵ���;
			return 0;
		} 

		// ת����Ӧ��ָ֧��;
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

	// �ҵ�,��ȡ����,�ɹ�����; 
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
		m_pRoot = new TrieNode;		// ��ʼ���룬�����Ϊ��;
	}

	int iCode = 0;
	TrieNode* location = m_pRoot;	// �Ӹ���㿪ʼ; 
	while (location != nullptr && *strKey != 0)
	{
		iCode = GetCode(*strKey);
		if (iCode < 0)
		{
			// ���Ϸ��ĵ���;
			return 0;
		}

		// �����ڴ˷�֧; 
		if (location->m_pNodes[iCode] == nullptr)
		{
			location->m_pNodes[iCode] = new TrieNode;    // �����շ�֧;
		}

		// ת���֧;
		location = location->m_pNodes[iCode];
		iPos++;
		strKey++;
	}

	if (location->m_pData != nullptr)
	{
		// ������ĵ����Ѿ�����;
		result = 0; 
	}
	else 
	{    // ��������;
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