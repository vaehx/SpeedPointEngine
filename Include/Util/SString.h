// ********************************************************************************************

//	SpeedPoint String

//	This file is part of the SpeedPoint Game Engine

//	(c) 2011-2014 Pascal R. aka iSmokiieZz
//	All rights reserved.

// ********************************************************************************************

#pragma once
#include "SResult.h"
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
		{
			pBuffer = new char[2];
			nLength = 2;
		}

		// Copy constructor
		// does a real copy!
		SString(const SString& s)
		{
			CopyFromOther(s);
		}

		// intialize with size
		SString(unsigned int size)
		{
			if (size <= 1)
				size = 2;

			pBuffer = new char[size];
			nLength = size;
			memset(pBuffer, 0, nLength);
		}

		// assignment, copies data
		SString(const char* str)
		{
			CopyFromBytes(str);
		}

		// destructor
		~SString()
		{
			if (pBuffer)
			{
				if (nLength == 1) delete pBuffer;
				else delete[] pBuffer;
				pBuffer = 0;
				nLength = 0; // !
			}
		}

		// Get the length of this string
		unsigned int GetLength() const
		{
			return nLength;
		}

		// Get the length of a char* buffer without terminated 0
		static unsigned int GetCharArrLen(const char* str)
		{
			if (!str)
			{
				SResult::ThrowExceptionAssertion(__FUNCTION__, __LINE__, __FILE__, "Given char buffer is empty!");

				return 0;
			}

			unsigned int nCurrentLen = 0;
			unsigned int nMaxLen = 2097152; // = 2 MB
			do
			{
				if (str[nCurrentLen] == 0)
					return nCurrentLen + 1;

				++nCurrentLen;
			} while (nCurrentLen <= nMaxLen); // catch endless loop

			if (nCurrentLen > nMaxLen)
				SResult::ThrowExceptionAssertion(__FUNCTION__, __LINE__, __FILE__, "Char buf length exceeded 2 MB");

			return nCurrentLen;
		}

		// Copy one char* to another by given size (improvement of sprintf_S)
		// size is with terminated 0
		// returns cound of bytes copied (including terminated 0)
		static unsigned int CopyCharS(char* Dst, unsigned int nSize, const char* Src, ...)
		{			
			if (!Dst || !nSize || !Src)
			{
				SResult::ThrowExceptionAssertion(__FUNCTION__, __LINE__, __FILE__, "Given parameters are invalid!");

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
				SResult::ThrowExceptionAssertion(__FUNCTION__, __LINE__, __FILE__, "Given parameters are invalid!");

				return 0;
			}

			for (unsigned int i = 0; i < nSize; ++i)
				Dst[i] = 0;

			return 0; // we copied nothing
		}

		void CopyFromBytes(const char* str)
		{
			if (!str)
			{
				SResult::ThrowExceptionAssertion(__FUNCTION__, __LINE__, __FILE__, "Given char buffer is empty!");
				return;
			}			

			unsigned int nLen = GetCharArrLen(str);
			nLength = nLen;
			if (str[nLen - 1] != 0)
				nLength = nLen + 1;	// make sure to have a terminating 0

			pBuffer = new char[nLength];

			// copy memory
			memcpy(pBuffer, str, nLen);
			pBuffer[nLength - 1] = 0; // make sure last character is the terminating 0
		}

		void CopyFromOther(const SString& s)
		{
			char* pS = (char*)s;
			if (pS)
			{
				// we assume the length is correct, we can save a lot of time doing so!
				unsigned int nLen = s.nLength;
				if (pS[nLen - 1] != 0)
					nLen++;

				pBuffer = new char[nLen];	// +1 due to terminated 0			
				nLength = nLen;

				// copy memory
				memcpy(pBuffer, pS, s.nLength);
				pBuffer[nLength - 1] = 0;
			}
		}

		// Conversion to char-Array
		// This does NOT copy the buffer!		
		operator char*() const
		{
			return pBuffer;
		}		

		// Concatenates strings
		SString operator + (const SString& s) const
		{
			if (nLength == 0 && s.GetLength())
				return SString();

			char* pBuf = new char[nLength + s.GetLength()];
			int c = nLength + s.GetLength();
			memcpy(pBuf, pBuffer, nLength);
			memcpy(pBuf + nLength - 1, s, s.GetLength());
			SString res(pBuf);
			delete[] pBuf;
			return res;
		}

		SString& operator = (const char* s)
		{
			if (pBuffer && nLength > 0)
			{
				if (nLength == 1) delete pBuffer;
				else delete[] pBuffer;
				pBuffer = 0;
				nLength = 0;
			}
			CopyFromBytes(s);
			return *this;
		}

		SString& operator = (const SString& s)
		{
			if (pBuffer && nLength > 0)
			{
				if (nLength == 1) delete pBuffer;
				else delete[] pBuffer;
				pBuffer = 0;
				nLength = 0;
			}
			CopyFromOther(s);
			return *this;
		}

		// Conatenates string
		SString& operator += (const SString& s)
		{
			if (s.GetLength() == 0)
				return *this;

			char* newBuf = new char[nLength + s.GetLength()];
			memcpy(newBuf, pBuffer, nLength);
			memcpy(newBuf + nLength, s, s.GetLength());
			delete[] pBuffer;
			pBuffer = newBuf;
			nLength += s.GetLength();
			return *this;
		}
	};	 

	// check if string buffer pointer againt integer	
	inline bool operator == (const SString& sa, const int& i)
	{
		return ((char*)sa == (char*)i);
	}

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
			SResult::ThrowExceptionAssertion(__FUNCTION__, __LINE__, __FILE__, "Exceeded 2MB data to compare!");

			return false;
		}

		return true;
	}
}