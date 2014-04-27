// ********************************************************************************************

//	SpeedPoint String

//	This file is part of the SpeedPoint Game Engine

//	(c) 2011-2014 Pascal R. aka iSmokiieZz
//	All rights reserved.

// ********************************************************************************************

#pragma once
#include "SAPI.h"
#include "SAssert.h"
#include <cstring>

namespace SpeedPoint
{
	//!!!!!!!!!!!!!!!!!!!!!!
	//!!
	//!! WARNING: SString must NOT use Assertions as the Assertions use SString itself!
	//!!	      Instead, you have to manually extend the code!
	//!!
	//!!!!!!!!!!!!!!!!!!!!!!

	// SpeedPoint String
	class S_API SString
	{
	private:
		char* pBuffer;	// the char buffer
		unsigned int nLength; // length including terminated 0

	public:
		// default constructor
		SString()
			: pBuffer(0)
		{
		}

		// assignment
		SString(const char* str)
		{						
			if (!str)
			{				
				SpeedPoint::SResult::ThrowException(
					SpeedPoint::SResult::eEX_ASSERTION,
					__FUNCTION__, __LINE__, __FILE__, "Given char buffer is empty!");

				return;
			}

			unsigned int nLen = GetCharArrLen(str);
			pBuffer = new char[nLen + 1];	// +1 due to terminated 0			
			nLength = nLen + 1;

			// copy memory
			memcpy(pBuffer, str, nLen);
			pBuffer[nLen] = 0; // make sure last character is the terminating 0
		}

		// destructor
		~SString()
		{
			if (pBuffer)
			{
				if (nLength == 1) delete pBuffer;
				else delete[] pBuffer;
				pBuffer = 0;
			}
		}

		// Get the length of this string
		unsigned int GetLength()
		{
			return nLength;
		}

		// Get the length of a char* buffer without terminated 0
		static unsigned int GetCharArrLen(const char* str)
		{
			if (!str)
			{
				SpeedPoint::SResult::ThrowException(
					SpeedPoint::SResult::eEX_ASSERTION,
					__FUNCTION__, __LINE__, __FILE__, "Given char buffer is empty!");

				return 0;
			}

			unsigned int nCurrentLen = 0;
			unsigned int nMaxLen = 2097152; // = 2 MB
			do
			{
				if (str[nCurrentLen] == 0)
					return nCurrentLen;

				++nCurrentLen;
			} while (nCurrentLen <= nMaxLen); // catch endless loop

			if (nCurrentLen > nMaxLen)
				SResult::ThrowException(SResult::eEX_ASSERTION, __FUNCTION__, __LINE__, __FILE__, "Char buf length exceeded 2 MB");

			return nCurrentLen;
		}

		// Copy one char* to another by given size (improvement of sprintf_S)
		// size is with terminated 0
		// returns cound of bytes copied (including terminated 0)
		static unsigned int CopyCharS(char* Dst, unsigned int nSize, const char* Src, ...)
		{			
			if (!Dst || !nSize || !Src)
			{
				SpeedPoint::SResult::ThrowException(
					SpeedPoint::SResult::eEX_ASSERTION,
					__FUNCTION__, __LINE__, __FILE__, "Given parameters are invalid!");

				return 0;
			}

			for (unsigned int i = 0; i < nSize; ++i)
			{
				if (Src[i] != 0)
				{
					Dst[i] = Src[i];
				}
				else
				{
					Dst[i] = 0;
					return i + 1;
				}
			}

			return nSize;
		}

		// Catch CopyCharS with no given Source argument
		// returns 0 as nothing has been copied (not even a terminated 0)
		static unsigned int CopyCharS(char* Dst, unsigned int nSize, ...)
		{		
			if (!Dst || !nSize)
			{
				SpeedPoint::SResult::ThrowException(
					SpeedPoint::SResult::eEX_ASSERTION,
					__FUNCTION__, __LINE__, __FILE__, "Given parameters are invalid!");

				return 0;
			}

			for (unsigned int i = 0; i < nSize; ++i)
				Dst[i] = 0;

			return 0; // we copied nothing
		}

		// Conversion to char-Array
		// This does NOT copy the buffer!		
		operator char*() const
		{
			return pBuffer;
		}

		// Assignment to char-Array
		// This will copy the buffer!
		char* operator = (const SString& s) const
		{
			if (pBuffer == 0) return 0;
			char* tempRes = new char[nLength];
			memcpy(tempRes, pBuffer, nLength);
			return tempRes;
		}		
	};	 

	// compare two SString's
	inline bool operator == (const SString& sa, const SString& sb)
	{
		if ((char*)sa == (char*)sb) return true;

		char *cA = (char*)sa, *cB = (char*)sb;
		if (!cA || !cB) return false;

		// loop through all characters
		unsigned int i = 0;
		unsigned int nMaxLen = 2097152;	// check a maximum of 2MB of data
		do
		{
			if (cA[i] != cB[i]) return false;
			if (cA[i] == 0) return true;

			++i;
		} while (i <= nMaxLen);

		if (i > nMaxLen)
		{
			SResult::ThrowException(SResult::eEX_ASSERTION,
				__FUNCTION__, __LINE__, __FILE__, "Exceeded 2MB data to compare!");

			return false;
		}

		return true;
	}
}