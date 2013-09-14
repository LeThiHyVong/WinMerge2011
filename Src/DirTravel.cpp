/** 
 * @file  DirTravel.cpp
 *
 * @brief Implementation file for Directory traversal functions.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "StdAfx.h"
#include "paths.h"
#include "Environment.h"
#include "Common/stream_util.h"
#include "DiffContext.h"
#include "FileFilterHelper.h"
#include "DirItem.h"
#include "CompareStats.h"

/**
 * @brief Load arrays with all directories & files in specified dir
 */
void CDiffContext::LoadAndSortFiles(LPCTSTR sDir, DirItemArray *dirs, DirItemArray *files) const
{
	LoadFiles(sDir, dirs, files);
	Sort(dirs);
	Sort(files);
	// If recursing the flat way, reset total count of items to 0
	if (m_nRecursive == 2)
		m_pCompareStats->SetTotalItems(0);
}

/**
 * @brief Find files and subfolders from given folder.
 * This function saves all files and subfolders in given folder to arrays.
 * We use 64-bit version of stat() to get times since find doesn't return
 * valid times for very old files (around year 1970). Even stat() seems to
 * give negative time values but we can live with that. Those around 1970
 * times can happen when file is created so that it  doesn't get valid
 * creation or modification dates.
 * @param [in] sDir Base folder for files and subfolders.
 * @param [in, out] dirs Array where subfolders are stored.
 * @param [in, out] files Array where files are stored.
 */
void CDiffContext::LoadFiles(LPCTSTR sDir, DirItemArray *dirs, DirItemArray *files) const
{
	if (LPCTSTR filter = m_piFilterGlobal->getSql())
	{
		String path = paths_ConcatPath(sDir, _T("*")).c_str();
		String exe = env_ExpandVariables(_T("%LogParser%\\LogParser.exe"));
		string_format cmd(
			_T("\"%s\" \"SELECT Name, CreationTime, LastWriteTime, Attributes, Size FROM '%s' %s\" -i:FS -o:TSV -q -recurse:0 -oCodepage:65001"),
			exe.c_str(), path.c_str(), filter);
		STARTUPINFO si;
		ZeroMemory(&si, sizeof si);
		si.cb = sizeof si;
		si.dwFlags = STARTF_USESTDHANDLES;
		SECURITY_ATTRIBUTES sa = { sizeof sa, NULL, TRUE };
		HANDLE hReadPipe = NULL;
		if (!CreatePipe(&hReadPipe, &si.hStdOutput, &sa, 0))
		{
			OException::Throw(GetLastError());
		}
		si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
		PROCESS_INFORMATION pi;
		if (!CreateProcess(NULL, const_cast<LPTSTR>(cmd.c_str()),
			NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
		{
			CloseHandle(hReadPipe);
			CloseHandle(si.hStdOutput);
			exe += _T(":\n");
			exe += OException(GetLastError()).msg;
			OException::Throw(0, exe.c_str());
		}
		CloseHandle(si.hStdOutput);
		HandleReadStream stream = hReadPipe;
		StreamLineReader reader = &stream;
		int state = 0;
		stl::string head;
		stl::string line;
		while (stl::string::size_type size = reader.readLine(line))
		{
			switch (state)
			{
			case 0:
				state = line == "Name\tCreationTime\tLastWriteTime\tAttributes\tSize\r\n" ? 1 : -1;
				// fall through
			case -1:
				// TODO: LogParser error messages tend to be followed by some garbage (pipe issue?)
				head += line;
				continue;
			}
			DirItem ent;
			ent.path = sDir;
			FileTime ft;
			const char *p = line.c_str();
			if (const char *q = strchr(p, '\t'))
			{
				ent.filename = OString(HString::Oct(p, static_cast<UINT>(q - p))->Uni(CP_UTF8)).W;
				p = q + 1;
			}
			if (const char *q = strchr(p, '\t'))
			{
				if (ft.Parse(p))
					LocalFileTimeToFileTime(&ft, &ent.ctime);
				p = q + 1;
			}
			if (const char *q = strchr(p, '\t'))
			{
				if (ft.Parse(p))
					LocalFileTimeToFileTime(&ft, &ent.mtime);
				p = q + 1;
			}
			if (const char *q = strchr(p, '\t'))
			{
				while (p < q)
				{
					switch (*p++)
					{
					case 'D':
						ent.flags.attributes |= FILE_ATTRIBUTE_DIRECTORY;
						break;
					case 'R':
						ent.flags.attributes |= FILE_ATTRIBUTE_READONLY;
						break;
					case 'A':
						ent.flags.attributes |= FILE_ATTRIBUTE_ARCHIVE;
						break;
					case 'H':
						ent.flags.attributes |= FILE_ATTRIBUTE_HIDDEN;
						break;
					case 'S':
						ent.flags.attributes |= FILE_ATTRIBUTE_SYSTEM;
						break;
					}
				}
				p = q + 1;
			}
			if ((ent.flags.attributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
				ent.size.int64 = _atoi64(p);
				ent.flags.attributes |= FILE_ATTRIBUTE_NORMAL;
				files->push_back(ent);
				// If recursing the flat way, increment total count of items
				if (m_nRecursive == 2)
					m_pCompareStats->IncreaseTotalItems();
			}
			else if (_tcsstr(_T(".."), ent.filename.c_str()) == NULL)
			{
				if (m_nRecursive == 2)
				{
					// Allow user to abort scanning
					if (ShouldAbort())
						break;
					path = paths_ConcatPath(ent.path, ent.filename);
					if (m_piFilterGlobal->includeDir(path.c_str()))
						LoadFiles(path.c_str(), dirs, files);
				}
				else
				{
					dirs->push_back(ent);
				}
			}
		}
		WaitForSingleObject(pi.hProcess, INFINITE);
		DWORD dwExitCode = 0;
		GetExitCodeProcess(pi.hProcess, &dwExitCode);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		CloseHandle(hReadPipe);
		if (state < 0 && dwExitCode != ERROR_PATH_NOT_FOUND)
			OException::Throw(0, UTF82W(head.c_str()));
	}
	else
	{
		WIN32_FIND_DATA ff;
		HANDLE h = FindFirstFile(paths_ConcatPath(sDir, _T("*.*")).c_str(), &ff);
		if (h != INVALID_HANDLE_VALUE)
		{
			do
			{
				DirItem ent;
				ent.path = sDir;
				ent.ctime = ff.ftCreationTime;
				ent.mtime = ff.ftLastWriteTime;
				ent.filename = ff.cFileName;
				ent.flags.attributes = ff.dwFileAttributes;
				if ((ff.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
				{
					// Ensure attributes to be nonzero for existing files
					ent.flags.attributes |= FILE_ATTRIBUTE_NORMAL;
					ent.size.Lo = ff.nFileSizeLow;
					ent.size.Hi = ff.nFileSizeHigh;
					files->push_back(ent);
					// If recursing the flat way, increment total count of items
					if (m_nRecursive == 2)
						m_pCompareStats->IncreaseTotalItems();
				}
				else if (_tcsstr(_T(".."), ff.cFileName) == NULL)
				{
					if (m_nRecursive == 2)
					{
						// Allow user to abort scanning
						if (ShouldAbort())
							break;
						String sDir = paths_ConcatPath(ent.path, ent.filename);
						if (m_piFilterGlobal->includeDir(sDir.c_str()))
							LoadFiles(sDir.c_str(), dirs, files);
					}
					else
					{
						dirs->push_back(ent);
					}
				}
			} while (FindNextFile(h, &ff));
			FindClose(h);
		}
	}
}

/**
 * @brief case-sensitive collate function for qsorting an array
 */
static bool __cdecl cmpstring(const DirItem &elem1, const DirItem &elem2)
{
	int cmp = _tcscoll(elem1.filename.c_str(), elem2.filename.c_str());
	if (cmp == 0)
		cmp = _tcscoll(elem1.path.c_str(), elem2.path.c_str());
	return cmp < 0;
}

/**
 * @brief case-insensitive collate function for qsorting an array
 */
static bool __cdecl cmpistring(const DirItem &elem1, const DirItem &elem2)
{
	if (int cmp = _tcsicoll(elem1.filename.c_str(), elem2.filename.c_str()))
		return cmp < 0;
	if (elem1.size.int64 != elem2.size.int64)
		return elem1.size.int64 < elem2.size.int64;
	if (elem1.mtime != elem2.mtime)
		return elem1.mtime < elem2.mtime;
	return _tcsicoll(elem1.path.c_str(), elem2.path.c_str()) < 0;
}

/**
 * @brief sort specified array
 */
void CDiffContext::Sort(DirItemArray *dirs) const
{
	stl::sort(dirs->begin(), dirs->end(),
		m_piFilterGlobal->isCaseSensitive() ? cmpstring : cmpistring);
}
