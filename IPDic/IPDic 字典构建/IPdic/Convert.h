#pragma once
#include <iostream>
using namespace std;

#define CONVERT_FIGURES		(3)			// 存两位 最后一位存\0;
#define CONVERT_HEX			(36)		// 0-9 A-Z 36进制;
#define MAX_CONVERT_DIGIT	(1295)		// (CONVERT_HEX的平方减一);

class Convert
{
public:
	Convert()
	{
		for (int iIndex = 0; iIndex < CONVERT_HEX; iIndex++)
		{
			if (iIndex < 10)
			{
				m_hexCustom[iIndex] = '0' + iIndex;
			}
			else
			{
				m_hexCustom[iIndex] = 'A' + (iIndex - 10);
			}
		}
	}
	~Convert()
	{
	}

	// 初始化自定义进制数组;
	bool InitCustomArry(char arry[MAX_CONVERT_DIGIT][CONVERT_FIGURES])
	{
			for (int iIndex = 0; iIndex < MAX_CONVERT_DIGIT; iIndex++)
			{
				int iDivision = iIndex / CONVERT_HEX;
				int iRemainder = iIndex % CONVERT_HEX;

				arry[iIndex][0] = m_hexCustom[iDivision];
				arry[iIndex][1] = m_hexCustom[iRemainder];
				arry[iIndex][2] = '\0';
			}

			return true;
		}

private:
	char m_hexCustom[CONVERT_HEX];
};

