// This is a file of the SpeedPoint Standard Func Test Project

#pragma once

#include "Assert.h" // we need the assertion stuff

namespace SpeedPoint
{
	//////////////////////////////////////////////////////////////////////////

	// Extended, high quality super-dupa String class. Unbeatable!
	class String
	{
	private:
		char* pBuffer;	// the char buffer
		unsigned int nLength; // length including terminated 0

	public:
		// default constructor
		String()
			: pBuffer(0)
		{
		}

		// assignment
		String(const char* str)
		{
			TEST_ASSERTR(!str);
			unsigned int nLen = GetCharArrLen(str);			
			pBuffer = new char[nLen + 1];	// +1 due to terminated 0			
			nLength = nLen + 1;

			// copy memory
			memcpy(pBuffer, str, nLen);
			pBuffer[nLen] = 0; // make sure last character is the terminating 0
		}

		// destructor
		~String()
		{
			if (pBuffer)
			{
				if (nLength == 1) delete pBuffer;
				else delete[] pBuffer;
				pBuffer = 0;
			}
		}

		// Get the length of a char* buffer without terminated 0
		static unsigned int GetCharArrLen(const char* str)
		{
			TEST_ASSERTR(!str, 0);

			unsigned int nCurrentLen = 0;
			unsigned int nMaxLen = 2097152; // = 2 MB
			do
			{
				if (str[nCurrentLen] == 0)
					return nCurrentLen;
				
				++nCurrentLen;
			} while (nCurrentLen < nMaxLen); // catch endless loop

			return nCurrentLen;
		}

		// Copy one char* to another by given size (improvement of sprintf_S)
		// size is with terminated 0
		// returns cound of bytes copied (including terminated 0)
		static unsigned int CopyCharS(char* Dst, unsigned int nSize, const char* Src)
		{
			TEST_ASSERTR(!Dst || !Src || !nSize, 0);			

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
		static unsigned int CopyCharS(char* Dst, unsigned int nSize)
		{
			TEST_ASSERTR(!Dst || !nSize, 0);

			for (unsigned int i = 0; i < nSize; ++i)
				Dst[i] = 0;

			return 0; // we copied nothing
		}

		// conversion to char-Array
		operator char*() const
		{
			return pBuffer;
		}

		// assignment to char-Array
		char* operator = (const String& s) const
		{
			if (pBuffer == 0) return 0;
			char* tempRes = new char[nLength];
			memcpy(tempRes, pBuffer, nLength);
			return tempRes;
		}		
	};	

	// >> operator for use in stdio libraries
	// if String pBuffer is empty, a temporary one of size 256 is created
	// Warning: If this size is exceeded during input, heap might get damaged!
	std::istream& operator >> (std::istream& input, const String& s)
	{
		char* pBuf = (char*)s;
		bool bEmpty = !pBuf;		
		if (bEmpty)
			pBuf = new char[256]; // 1 char and \0

		input >> pBuf;
		if (bEmpty)
			delete[] pBuf;

		return input;
	}
}