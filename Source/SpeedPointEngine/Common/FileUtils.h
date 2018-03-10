#pragma once

#include "SAPI.h"
#include <string>

#define RESOURCE_PATH_SEPARATOR '/'
#define SYSTEM_PATH_SEPARATOR '\\'

namespace SpeedPoint
{
	S_API bool IsAbsoluteResourcePath(const std::string& path);
	S_API bool IsAbsoluteSystemPath(const std::string& path);

	// Normalizes slashes inside the path and removes trailing slashes
	S_API std::string NormalizeResourcePath(const std::string& resourcePath);

	// Normalizes slashes inside the path and removes trailing slashes
	S_API std::string NormalizeSystemPath(const std::string& systemPath);

	// Description:
	//	Resolves any upper directory symbols ('..') in the given path
	//	e.g. "/../aaa/bbb/../ccc" becomes "/aaa/ccc"
	//	The resulting path is normalized to the first separator found and does NOT end with a separator.
	// Arguments:
	//	path - must be an absolute path. If not, the returned path will be unchanged.
	S_API std::string ExpandPath(const std::string& path);

	// Description:
	//	Tries to make the given path relative to a reference directory path.
	//	If the pathes don't share the same slash format, any of the paths is not absolute,
	//	or another error occured, will return the original path.
	// Arguments:
	//	absPath - must be an absolute path
	//	absReferenceDir - must be an absolute path to a directory
	S_API std::string MakePathRelativeTo(const std::string& absPath, const std::string& absReferenceDir);

	// Description:
	//	Appends the relative path to the given absolute reference directory by properly inserting
	//	path separators. If the given path was absolute, it will be returned unchanged.
	S_API std::string MakePathAbsolute(const std::string& relPath, const std::string& absReferenceDir);

	// Returns the directory part of the given path up to the last separator (exclusive).
	// The given path is assumed to be pointing to a file if it does not end with a separator.
	S_API std::string GetDirectoryPath(const std::string& path);

	// Description:
	//	Returns the part after the last directory separator
	// Examples:
	//	/bar.txt or /foo/bar.txt -> bar.txt
	//	bar.txt -> bar.txt
	S_API std::string GetFileNameWithExtension(const std::string& path);

	// Description:
	//	Returns the part between (after) the last directory separator and (before) the first dot after that separator (if any)
	// Examples:
	//	/foo/bar.txt -> bar
	//	/foo/ or / -> (empty string)
	//	/foo -> foo
	//	bar.txt -> bar
	S_API std::string GetFileNameWithoutExtension(const std::string& path);

	// path - An absolute system path
	// workspacePath - An absolute system path to the workspace root directory
	// relative - If true, the resource path will be relative to the reference path if possible.
	// reference - An absolute resource path to a directory that the path will be made relative to.
	S_API std::string SystemPathToResourcePath(const std::string& path, const std::string& workspacePath, bool relative, const std::string& reference);

	// path - An absolute system path
	// workspacePath - An absolute system path to the workspace root directory
	S_API inline std::string SystemPathToResourcePath(const std::string& path, const std::string& workspacePath)
	{
		return SystemPathToResourcePath(path, workspacePath, false, "");
	}
}
