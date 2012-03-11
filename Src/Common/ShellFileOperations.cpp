/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or (at
//    your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/**
 * @file  ShellFileOperations.cpp
 *
 * @brief Implementation of ShellFileOperations class.
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "StdAfx.h"
#include "paths.h"
#include "ShellFileOperations.h"

using stl::vector;

/**
 * @brief Constructor.
 */
ShellFileOperations::ShellFileOperations()
 : m_bOneToOneMapping(false)
 , m_function(0)
 , m_flags(0)
 , m_parentWindow(NULL)
 , m_isCanceled(false)
{
}

/**
 * @brief Add source- and destination paths.
 * @param [in] source Source path.
 * @param [in] destination Destination path.
 */
void ShellFileOperations::AddSourceAndDestination(LPCTSTR source, LPCTSTR destination)
{
	m_sources.push_back(paths_UndoMagic(&String(source).front()));
	m_destinations.push_back(paths_UndoMagic(&String(destination).front()));
}

/**
 * @brief Add source path.
 * @param [in] source Source path.
 */
void ShellFileOperations::AddSource(LPCTSTR source)
{
	m_sources.push_back(paths_UndoMagic(&String(source).front()));
	m_bOneToOneMapping = false;
}

/**
 * @brief Add destination path.
 * @param [in] destination Destination path.
 */
void ShellFileOperations::SetDestination(LPCTSTR destination)
{
	m_destinations.push_back(paths_UndoMagic(&String(destination).front()));
	m_bOneToOneMapping = false;
}

/**
 * @brief Get a path list as C string with NULLs.
 * This function returns C-string with NULLs between paths and two NULLs
 * at the end of the paths. As the ShellFileOp() requires.
 * @param [in] source If true, return source paths, else return destination
 *   paths.
 * @return C-string of the paths.
 * @note You must free the returned string after using it!
 */
TCHAR* ShellFileOperations::GetPathList(bool source) const
{
	const int len = CountStringSize(source);
	TCHAR *pStr = new TCHAR[len];
	ZeroMemory(pStr, len * sizeof(TCHAR));

	vector<String>::const_iterator iter;
	vector<String>::const_iterator end;
	if (source)
	{
		iter = m_sources.begin();
		end = m_sources.end();
	}
	else
	{
		iter = m_destinations.begin();
		end = m_destinations.end();
	}

	int ind = 0;
	while (iter != end)
	{
		const int slen = (*iter).length();
		memcpy(pStr + ind, (*iter).c_str(), slen * sizeof(TCHAR));
		ind += slen;
		ind++; // NULL between strings
		iter++;
	}
	return pStr;
}

/**
 * @brief Calculate lenght of the C-string required for paths.
 * @param [in] source If true calculate source paths, else calculate
 *   destination paths.
 * @return Lenght of the string.
 */
int ShellFileOperations::CountStringSize(bool source) const
{
	vector<String>::const_iterator iter;
	vector<String>::const_iterator end;
	if (source)
	{
		iter = m_sources.begin();
		end = m_sources.end();
	}
	else
	{
		iter = m_destinations.begin();
		end = m_destinations.end();
	}

	int size = 0;
	while (iter != end)
	{
		size += (*iter).length() * sizeof(TCHAR);
		size += sizeof(TCHAR); // NULL between strings
		iter++;
	}
	size += 2; // Two zeros at end of the string
	return size;
}

/**
 * @brief Set the operation, flags and parent window.
 * @param [in] operation Operation to run (copy/move/delete/rename).
 * @param [in] flags Flags related to the operation.
 * @param [in] parentWindow Window getting notifications.
 */
void ShellFileOperations::SetOperation(UINT operation, FILEOP_FLAGS flags,
		HWND parentWindow /*= NULL*/)
{
	m_function = operation;
	m_flags = flags;
	m_parentWindow = H2O::GetTopLevelParent(parentWindow);
}

/**
 * @brief Run the file operation(s).
 * This method runs the filesystem operation(s) added earlier.
 * @return true if succeeds and user did not cancel, false otherwise.
 */
bool ShellFileOperations::Run()
{
	if (m_function == 0)
		return false; // Operation not set!

	TCHAR *sourceStr = GetPathList(true);
	TCHAR *destStr = NULL;
	if (m_function != FO_DELETE)
		destStr = GetPathList(false);

	SHFILEOPSTRUCT fileop = {m_parentWindow, m_function, sourceStr, destStr,
			m_flags, FALSE, 0, 0};

	EnableWindow(m_parentWindow, FALSE);
	int ret = SHFileOperation(&fileop);
	EnableWindow(m_parentWindow, TRUE);

	if (ret == 0x75) // DE_OPCANCELLED
		m_isCanceled = true;

	delete [] sourceStr;
	sourceStr = NULL;
	delete [] destStr;
	destStr = NULL;

	BOOL anyAborted = fileop.fAnyOperationsAborted;

	// SHFileOperation returns 0 when succeeds
	if (ret == 0 && anyAborted == FALSE)
		return true;
	return false;
}

/**
 * @brief Did the user cancel the operation?
 * @return true if the operation was canceled by the user.
 */
bool ShellFileOperations::IsCanceled() const
{
	return m_isCanceled;
}

/**
 * @brief Reset the class.
 */
void ShellFileOperations::Reset()
{
	m_bOneToOneMapping = false;
	m_function = 0;
	m_flags = 0;
	m_parentWindow = NULL;
	m_isCanceled = false;

	m_sources.clear();
	m_destinations.clear();
}
