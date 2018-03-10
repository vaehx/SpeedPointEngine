///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "FileUtils.h"
#include "strutils.h"
#include <vector>

using std::string;
using std::vector;

namespace SpeedPoint
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline char DeterminePathSeparator(const string& path)
	{
		for (size_t i = 0; i < path.length(); ++i)
		{
			if (path[i] == RESOURCE_PATH_SEPARATOR || path[i] == SYSTEM_PATH_SEPARATOR)
				return path[i];
		}

		return RESOURCE_PATH_SEPARATOR;
	}

#define IsPathSeparator(s) (s == RESOURCE_PATH_SEPARATOR || s == SYSTEM_PATH_SEPARATOR)

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	S_API bool IsAbsoluteResourcePath(const string& path)
	{
		return !path.empty() && path[0] == RESOURCE_PATH_SEPARATOR;
	}

	S_API bool IsAbsoluteSystemPath(const string& path)
	{
		// TODO: Support OS's other than windows

		if (path.length() < 2)
			return false;

		if (path[1] != ':')
			return false;

		char driveLetter = toupper(path[0]);
		if (!(path[0] >= 'A' && path[0] <= 'Z'))
			return false;

		if (path.length() > 2 && !IsPathSeparator(path[2]))
			return false;

		return true;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	inline string NormalizePath(const string& path, char separator)
	{
		char incorrectSeparator = (separator == '/' ? '\\' : '/');
		return strreplace(path, incorrectSeparator, separator);
	}

	S_API string NormalizeResourcePath(const string& resourcePath)
	{
		return NormalizePath(resourcePath, '/');
	}

	S_API string NormalizeSystemPath(const string& systemPath)
	{
		return NormalizePath(systemPath, '\\');
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*
	"/../aaa/bbb/../ccc"	-> "/aaa/ccc"
	"/aaa/.."				-> "/"
	"aaa"					-> "aaa"
	"aaa.."					-> "aaa.."
	"aaa/../bbb"			-> ""
	"C:\..\aaa"				-> "C:\aaa"
	"C:aaa"					-> ""
	*/
	S_API string ExpandPath(const string& path)
	{
		if (!(IsAbsoluteResourcePath(path) || IsAbsoluteSystemPath(path)))
			return path;

		const size_t firstSepPos = path.find_first_of("/\\");
		if (firstSepPos == path.npos)
			return path;

		const char separator = path[firstSepPos];

		string rpath = "";
		vector<string> parts = strexplode(NormalizePath(path.substr(firstSepPos + 1), separator), separator);
		if (!parts.empty())
		{
			for (auto part = parts.begin(); part != parts.end();)
			{
				if (*part == "..")
				{
					part = parts.erase(part);
					if (part != parts.begin())
						part = parts.erase(part - 1);
				}
				else
				{
					++part;
				}
			}

			rpath = strimplode(parts, separator);
		}

		return path.substr(0, firstSepPos + 1) + rpath;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*
	"C:\Files\File.txt", "C:\Files"		-> "File.txt"
	"C:\File.txt", "C:\Files"			-> "C:\File.txt"
	"/models/texture.bmp", "/models"	-> "texture.bmp"
	"/", "/models"						-> "/"
	*/
	S_API string MakePathRelativeTo(const string& path, const string& reference)
	{
		if (path.empty() || reference.empty())
			return path;

		if (!(IsAbsoluteResourcePath(path) || IsAbsoluteSystemPath(path)))
			return path;

		if (!(IsAbsoluteResourcePath(reference) || IsAbsoluteSystemPath(path)))
			return path;

		char pathSeparator = DeterminePathSeparator(path);
		char referenceSeparator = DeterminePathSeparator(reference);

		if (pathSeparator != referenceSeparator)
			return path; // incompatible paths

		string relativePath = NormalizePath(path, pathSeparator);
		string normReference = NormalizePath(reference, referenceSeparator);

		if (strstartswith(relativePath, normReference))
		{
			relativePath = relativePath.substr(normReference.length());
			if (relativePath[0] == pathSeparator)
				relativePath.erase(0, 1);
		}

		return relativePath;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	S_API std::string MakePathAbsolute(const std::string& relPath, const std::string& absReferenceDir)
	{
		if (IsAbsoluteSystemPath(relPath) || IsAbsoluteResourcePath(relPath) || absReferenceDir.empty())
			return relPath;

		const char pathSeparator = DeterminePathSeparator(relPath);
		const char referenceSeparator = DeterminePathSeparator(absReferenceDir);

		if (pathSeparator != referenceSeparator)
			return relPath; // incompatible paths

		// we don't care if absReferenceDir is actually absolute or not...

		return NormalizePath(absReferenceDir, referenceSeparator) + referenceSeparator + relPath;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	S_API string GetDirectoryPath(const string& path)
	{
		if (path.empty())
			return path;

		size_t lastSepPos = path.find_last_of("/\\");
		if (lastSepPos == path.npos)
			return path;

		return path.substr(0, lastSepPos);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	S_API string GetFileNameWithExtension(const string& path)
	{
		if (path.empty())
			return "";

		string file = path;

		// Strip until and including first occurrence of a separator
		size_t lastSepPos = file.find_last_of("/\\");
		if (lastSepPos != file.npos)
			file = file.substr(lastSepPos + 1);

		return file;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	S_API string GetFileNameWithoutExtension(const string& path)
	{
		string file = GetFileNameWithExtension(path);

		// Strip part after first occurence of a dot (i.e. the extension)
		size_t firstDotPos = file.find_first_of(".");
		if (firstDotPos != file.npos)
			file = file.substr(0, firstDotPos);

		return file;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*
	path = "C:\Workspace\models\texture.bmp"
	workspacePath = "C:\Workspace"
	reference = "/models"

	=> "texture.bmp"
	*/
	S_API string SystemPathToResourcePath(const string& path, const string& workspacePath, bool relative, const std::string& reference)
	{
		if (!IsAbsoluteSystemPath(path) || !IsAbsoluteSystemPath(workspacePath))
			return path;

		string resourcePath = ExpandPath(NormalizeSystemPath(path)); // "C:\Workspace\models\texture.bmp"
		string normWorkspacePath = ExpandPath(NormalizeSystemPath(workspacePath)); // "C:\Workspace"

		if (!strstartswith(resourcePath, normWorkspacePath)) // e.g. with path="C:\WorkspaceA\texture.bmp" and workspacePath="C:\WorkspaceB"
			return resourcePath;

		resourcePath = NormalizeResourcePath(resourcePath.substr(normWorkspacePath.length())); // "/models/texture.bmp" or ""
		if (resourcePath.empty() || resourcePath[0] != RESOURCE_PATH_SEPARATOR)
			resourcePath = RESOURCE_PATH_SEPARATOR + resourcePath; // -> "/models/texture.bmp" or "/"

		if (relative)
		{
			// "/models/texture.bmp", "/models" -> "texture.bmp"
			return MakePathRelativeTo(resourcePath, reference);
		}
		else
		{
			return resourcePath;
		}
	}
}
