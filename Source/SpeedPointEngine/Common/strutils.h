//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <algorithm>
#include <cctype>

//////////////////////////////////////////////////////////////////////////////////////////////////
// Note: This function assumes that the string properly terminates with a 0!
//	Otherwise a memory access violation occurs.
inline unsigned short sp_strlen(const char* str)
{
	if (!str)
		return 0;

	unsigned short cnt = 0;
	while (str[cnt] != '\0')
		cnt++;

	return cnt;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Note: This function assumes that src properly terminates with a 0!
//	DST IS CREATED WITH new BY THIS FUNCTION IF NOT A VALID POINTER!
//	Otherwise a memory access violation occurs.
//	MAKE SURE TO DELETE IT PROPERLY!
inline void sp_strcpy(char** dst, const char* src)
{
	if (!dst)
		return;

	unsigned short srcLn = sp_strlen(src);
	if (!*dst)
		*dst = new char[srcLn + 1];

	memcpy(*dst, src, srcLn);
	(*dst)[srcLn] = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Case-Insensitive string comparison
inline bool striequals(const std::string& a, const std::string& b)
{
	size_t sz = a.size();

	if (b.size() != sz)
		return false;

	for (size_t i = 0; i < sz; ++i)
		if (tolower(a[i]) != tolower(b[i]))
			return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
inline int sp_extended_isspace(char c)
{
	return std::isspace((unsigned char)c);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Removes whitespaces at the start of the string
inline std::string strltrim(const std::string& s)
{
	std::string r = s;
	r.erase(r.begin(), std::find_if(r.begin(), r.end(), std::not1(std::ptr_fun(sp_extended_isspace))));
	return r;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Removes whitespaces at the end of the string
inline std::string strrtrim(const std::string& s)
{
	std::string r = s;
	r.erase(std::find_if(r.rbegin(), r.rend(), std::not1(std::ptr_fun(sp_extended_isspace))).base(), r.end());
	return r;
}

// Removes all consecutive characters at the start of the string
inline std::string strrtrim(const std::string& s, char c)
{
	size_t first = s.length();
	for (; first > 0; --first)
	{
		if (s[first - 1] != c)
			break;
	}

	return s.substr(0, first);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Removes whitespaces from the string at both sides
inline std::string strtrim(const std::string& s)
{
	return strltrim(strrtrim(s));
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Splits the given string into pieces delimited by the given delimiter character (exclusive).
// If no delimiter was found in the string, the only part will be the string itself.
// If keepEmpty is false, empty parts are not added to the list.
inline std::vector<std::string> strexplode(const std::string& s, const char delimiter, bool keepEmpty = true)
{
	static std::vector<std::string> parts;

	parts.clear();

	if (s.empty())
		return parts;

	std::size_t pos1 = 0;
	std::size_t pos2;
	do
	{
		pos2 = s.find_first_of(delimiter, pos1);
		if (pos2 - pos1 > 0 || keepEmpty)
			parts.push_back(s.substr(pos1, pos2 - pos1));
		
		pos1 = pos2 + 1;
	} while (pos2 != s.npos);

	return parts;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Joins the single strings by putting the delimiter in between the parts
inline std::string strimplode(const std::vector<std::string>& v, const char delimiter)
{
	std::string result;
	std::size_t length = 0;
	std::size_t i;

	for (i = 0; i < v.size(); ++i)
	{
		length += (i > 0) + v[i].length();
	}

	result.reserve(length);
	for (i = 0; i < v.size(); ++i)
	{
		if (i > 0)
			result.resize(result.size() + 1, delimiter);

		result.append(v[i]);
	}

	return result;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Converts all characters in the string to lower case
inline std::string strtolower(const std::string& s)
{
	std::string r = s;
	std::transform(r.begin(), r.end(), r.begin(), ::tolower);
	return r;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Converts all characters in the string to upper case
inline std::string strtoupper(const std::string& s)
{
	std::string r = s;
	std::transform(r.begin(), r.end(), r.begin(), ::toupper);
	return r;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Replaces all occurences of oldChar in s with newChar and returns the resulting string
inline std::string strreplace(const std::string& s, const char oldChar, const char newChar)
{
	std::string r;
	r.resize(s.length());
	for (std::size_t pos = 0; pos < s.length(); ++pos)
		r[pos] = (s[pos] == oldChar ? newChar : s[pos]);

	return r;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Checks if s1 starts with s2
inline bool strstartswith(const std::string& s1, const std::string& s2)
{
	if (s1.length() < s2.length())
		return false;

	if (s2.empty())
		return true;

	const std::size_t ln = s2.length();
	for (std::size_t pos = 0; pos < ln; ++pos)
	{
		if (s1[pos] != s2[pos])
			return false;
	}

	return true;
}

// Checks if s starts with the character c
inline bool strstartswith(const std::string& s, const char c)
{
	if (!s.empty())
		return s.front() == c;
	else
		return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Checks if s1 ends with s2
inline bool strendswith(const std::string& s1, const std::string& s2)
{
	if (s1.length() < s2.length())
		return false;

	if (s2.empty())
		return true;

	const std::size_t offs = s1.length() - s2.length();
	const std::size_t ln = s2.length();
	for (std::size_t pos = 0; pos < ln; ++pos)
	{
		if (s1[pos + offs] != s2[pos])
			return false;
	}

	return true;
}

// Checks if s ends with the character c
inline bool strendswith(const std::string& s, const char c)
{
	if (!s.empty())
		return s.back() == c;
	else
		return false;
}
