/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  MainFrm.cpp
 *
 * @brief Implementation of the CMainFrame class
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "StdAfx.h"
#include "DiffWrapper.h"
#include "VSSHelper.h"
#include "Merge.h"
#define operator(args) args
#include "OptionsDef.h"
#include <htmlhelp.h>  // From HTMLHelp Workshop (incl. in Platform SDK)
#include "Constants.h"
#include "Common/RegKey.h"
#include "Common/SettingStore.h"
#include "Common/WindowPlacement.h"
#include "AboutDlg.h"
#include "MainFrm.h"
#include "Splash.h"
#include "DirFrame.h"		// Include type information
#include "ChildFrm.h"
#include "HexMergeFrm.h"
#include "DirView.h"
#include "MergeEditView.h"
#include "MergeDiffDetailView.h"
#include "LocationView.h"
#include "SyntaxColors.h"
#include "LineFiltersList.h"
#include "ConflictFileParser.h"
#include "coretools.h"
#include "LineFiltersDlg.h"
#include "LogFile.h"
#include "paths.h"
#include "Environment.h"
#include "PatchTool.h"
#include "ConfigLog.h"
#include "7zCommon.h"
#include "FileFiltersDlg.h"
#include "OptionsMgr.h"
#include "LanguageSelect.h"
#include "codepage_detect.h"
#include "VSSHelper.h"
#include "codepage.h"
#include "ProjectFile.h"
#include "PreferencesDlg.h"
#include "ProjectFilePathsDlg.h"
#include "MergeCmdLineInfo.h"
#include "FileOrFolderSelect.h"
#include "stringdiffs.h"
#include "MergeCmdLineInfo.h"
#include "OpenDlg.h"
#include <MyCom.h>
#include <mlang.h>

using stl::vector;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static DWORD dwOsVer = GetVersion();

/**
 * @brief A table associating menuitem id, icon and menus to apply.
 */
const CMainFrame::MENUITEM_ICON CMainFrame::m_MenuIcons[] = {
	{ ID_FILE_OPENCONFLICT,			IDB_FILE_OPENCONFLICT			},
	{ ID_EDIT_COPY,					IDB_EDIT_COPY,					},
	{ ID_EDIT_CUT,					IDB_EDIT_CUT,					},
	{ ID_EDIT_PASTE,				IDB_EDIT_PASTE,					},
	{ ID_EDIT_FIND,					IDB_EDIT_SEARCH,				},
	{ ID_WINDOW_CASCADE,			IDB_WINDOW_CASCADE,				},
	{ ID_WINDOW_TILE_HORZ,			IDB_WINDOW_HORIZONTAL,			},
	{ ID_WINDOW_TILE_VERT,			IDB_WINDOW_VERTICAL,			},
	{ ID_FILE_CLOSE,				IDB_WINDOW_CLOSE,				},
	{ ID_WINDOW_CHANGE_PANE,		IDB_WINDOW_CHANGEPANE,			},
	{ ID_EDIT_WMGOTO,				IDB_EDIT_GOTO,					},
	{ ID_EDIT_REPLACE,				IDB_EDIT_REPLACE,				},
	{ ID_VIEW_LANGUAGE,				IDB_VIEW_LANGUAGE,				},
	{ ID_VIEW_SELECTFONT,			IDB_VIEW_SELECTFONT,			},
	{ ID_APP_EXIT,					IDB_FILE_EXIT,					},
	{ ID_HELP_CONTENTS,				IDB_HELP_CONTENTS,				},
	{ ID_EDIT_SELECT_ALL,			IDB_EDIT_SELECTALL,				},
	{ ID_TOOLS_FILTERS,				IDB_TOOLS_FILTERS,				},
	{ ID_TOOLS_CUSTOMIZECOLUMNS,	IDB_TOOLS_COLUMNS,				},
	{ ID_TOOLS_GENERATEPATCH,		IDB_TOOLS_GENERATEPATCH,		},
	{ ID_FILE_PRINT,				IDB_FILE_PRINT,					},
	{ ID_TOOLS_GENERATEREPORT,		IDB_TOOLS_GENERATEREPORT,		},
	{ ID_EDIT_TOGGLE_BOOKMARK,		IDB_EDIT_TOGGLE_BOOKMARK,		},
	{ ID_EDIT_GOTO_NEXT_BOOKMARK,	IDB_EDIT_GOTO_NEXT_BOOKMARK,	},
	{ ID_EDIT_GOTO_PREV_BOOKMARK,	IDB_EDIT_GOTO_PREV_BOOKMARK,	},
	{ ID_EDIT_CLEAR_ALL_BOOKMARKS,	IDB_EDIT_CLEAR_ALL_BOOKMARKS,	},
	{ ID_VIEW_ZOOMIN,				IDB_VIEW_ZOOMIN,				},
	{ ID_VIEW_ZOOMOUT,				IDB_VIEW_ZOOMOUT,				},
	{ ID_MERGE_COMPARE,				IDB_MERGE_COMPARE,				},
	{ ID_MERGE_DELETE,				IDB_MERGE_DELETE,				},
	{ ID_TOOLS_GENERATEREPORT,		IDB_TOOLS_GENERATEREPORT,		},
	{ ID_DIR_COPY_LEFT_TO_RIGHT,	IDB_LEFT_TO_RIGHT,				},
	{ ID_DIR_COPY_RIGHT_TO_LEFT,	IDB_RIGHT_TO_LEFT,				},
	{ ID_DIR_COPY_LEFT_TO_BROWSE,	IDB_LEFT_TO_BROWSE,				},
	{ ID_DIR_COPY_RIGHT_TO_BROWSE,	IDB_RIGHT_TO_BROWSE,			},
	{ ID_DIR_MOVE_LEFT_TO_BROWSE,	IDB_MOVE_LEFT_TO_BROWSE,		},
	{ ID_DIR_MOVE_RIGHT_TO_BROWSE,	IDB_MOVE_RIGHT_TO_BROWSE,		},
	{ ID_DIR_DEL_LEFT,				IDB_LEFT,						},
	{ ID_DIR_DEL_RIGHT,				IDB_RIGHT,						},
	{ ID_DIR_DEL_BOTH,				IDB_BOTH,						},
	{ ID_DIR_COPY_PATHNAMES_LEFT,	IDB_LEFT,						},
	{ ID_DIR_COPY_PATHNAMES_RIGHT,	IDB_RIGHT,						},
	{ ID_DIR_COPY_PATHNAMES_BOTH,	IDB_BOTH,						},
	{ ID_DIR_ZIP_LEFT,				IDB_LEFT,						},
	{ ID_DIR_ZIP_RIGHT,				IDB_RIGHT,						},
	{ ID_DIR_ZIP_BOTH,				IDB_BOTH,						}
};


/////////////////////////////////////////////////////////////////////////////
// CMainFrame

/** @brief Timer ID for window flashing timer. */
static const UINT ID_TIMER_FLASH = 1;

/** @brief Timeout for window flashing timer, in milliseconds. */
static const UINT WINDOW_FLASH_TIMEOUT = 500;

/** @brief Timeout for window flashing timer, in milliseconds. */
static const UINT FILES_MRU_CAPACITY = 8;

/** @brief Backup file extension. */
static const TCHAR BACKUP_FILE_EXT[] = _T("bak");

static int GetMenuBitmapExcessWidth()
{
	int cxExcess = 16 - GetSystemMetrics(SM_CXMENUCHECK);
	return max(cxExcess, 0);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

/**
 * @brief MainFrame constructor. Loads settings from registry.
 * @todo Preference for logging?
 */
CMainFrame::CMainFrame(HWindow *pWnd)
: m_pWndMDIClient(NULL)
, m_hMenuDefault(NULL)
, m_hAccelTable(NULL)
, m_bFlashing(false)
, m_bEscShutdown(false)
, m_bClearCaseTool(false)
, m_bExitIfNoDiff(MergeCmdLineInfo::Disabled)
, m_CheckOutMulti(FALSE)
, m_bVCProjSync(FALSE)
, m_bVssSuppressPathCheck(FALSE)
, m_pLineFilters(new LineFiltersList())
, m_wndToolBar(NULL)
, m_wndStatusBar(NULL)
, m_wndTabBar(NULL)
, m_pScriptMenu(NULL)
, m_imlMenu(NULL)
, m_imlToolbarEnabled(NULL)
, m_imlToolbarDisabled(NULL)
, m_sourceType(0)
{
	SubclassWindow(pWnd);

	SyntaxColors.LoadFromRegistry();

	theApp.m_pMainWnd = this;

	UpdateCodepageModule();

	InitializeSourceControlMembers();

	// If there are no filters loaded, and there is filter string in previous
	// option string, import old filters to new place.
	if (m_pLineFilters->GetCount() == 0)
	{
		String oldFilter = SettingStore.GetProfileString(_T("Settings"), _T("RegExps"));
		if (!oldFilter.empty())
			m_pLineFilters->Import(oldFilter.c_str());
	}

	LoadFilesMRU();

	sd_SetBreakChars(COptionsMgr::Get(OPT_BREAK_SEPARATORS).c_str());

	if (HICON hMergeIcon = LanguageSelect.LoadIcon(IDR_MAINFRAME))
		SetIcon(hMergeIcon, TRUE);

	GetMrgViewFontProperties();
	GetDirViewFontProperties();
	
	CreateToobar();

	m_wndTabBar = HTabCtrl::Create(
		WS_CHILD | WS_VISIBLE | TCS_FOCUSNEVER | TCS_HOTTRACK | TCS_TOOLTIPS, 0, 0, 0, 0, m_pWnd, 0xC001);
	m_wndTabBar->SetFont(static_cast<HFont *>(HGdiObj::GetStockObject(DEFAULT_GUI_FONT)));

	if (!COptionsMgr::Get(OPT_SHOW_TABBAR))
		m_wndTabBar->ShowWindow(SW_HIDE);

	m_wndStatusBar = HStatusBar::Create(
		WS_CHILD | WS_VISIBLE | CCS_BOTTOM, 0, 0, 0, 0, m_pWnd, 0x6000);

	if (!COptionsMgr::Get(OPT_SHOW_STATUSBAR))
		m_wndStatusBar->ShowWindow(SW_HIDE);

	InitCmdUI();

	m_hMenuDefault = LanguageSelect.LoadMenu(IDR_MAINFRAME)->m_hMenu;
	m_hAccelTable = LanguageSelect.LoadAccelerators(IDR_MAINFRAME);
	::SetMenu(m_hWnd, m_hMenuDefault);

	m_pWndMDIClient = (new CSplashWnd(m_pWnd))->m_pWnd;

	m_bAutoDelete = true;
}

CMainFrame::~CMainFrame()
{
	// Stop handling status messages from CustomStatusCursors
	WaitStatusCursor::SetCursor(NULL);

	LogFile.EnableLogging(FALSE);

	// Delete all temporary folders belonging to this process
	GetClearTempPath(NULL, NULL);

	// Remove files from temp file list.
	while (!m_tempFiles.empty())
	{
		TempFile *temp = m_tempFiles.back();
		temp->Delete();
		delete temp;
		m_tempFiles.pop_back();
	}

	sd_SetBreakChars(NULL);

	if (m_imlMenu)
		m_imlMenu->Destroy();
	if (m_imlToolbarEnabled)
		m_imlToolbarEnabled->Destroy();
	if (m_imlToolbarDisabled)
		m_imlToolbarDisabled->Destroy();

	theApp.m_pMainWnd = NULL;
}

HRESULT CMainFrame::FindBehavior(BSTR bstrBehavior, BSTR bstrBehaviorUrl, IElementBehaviorSite *pSite, IElementBehavior **ppBehavior)
{
	if (EatPrefix(bstrBehaviorUrl, L"#WinMerge#"))
	{
		if (StrCmpIW(bstrBehavior, L"MergeApp") == 0)
		{
			*ppBehavior = this;
			return S_OK;
		}
	}
	return E_INVALIDARG;
}

HRESULT CMainFrame::put_StatusText(BSTR bsStatusText)
{
	HWindow *const pWndOwner = GetLastActivePopup();
	if (pWndOwner == NULL)
		return E_UNEXPECTED;
	if (HWindow *const pWnd = pWndOwner->FindWindowEx(NULL, STATUSCLASSNAME))
	{
		DWORD elapsed = GetTickCount() - GetMessageTime();
		string_format text(_T("%ls (%s)"), bsStatusText,
			static_cast<LPCTSTR>(LanguageSelect.Format(IDS_ELAPSED_TIME, elapsed)));
		pWnd->SetWindowText(text.c_str());
		//pWndOwner->UpdateWindow();
	}
	if (::GetAsyncKeyState(VK_PAUSE) & 0x8001)
	{
		// Trigger abortion only if window is in foreground, or processing of
		// outstanding messages takes the effect of moving it to foreground.
		while (pWndOwner != HWindow::GetForegroundWindow())
		{
			MSG msg;
			if (!::PeekMessage(&msg, pWndOwner->m_hWnd, 0, 0, PM_REMOVE))
				return S_OK;
			::DispatchMessage(&msg);
		}
		return E_ABORT;
	}
	return S_OK;
}

HRESULT CMainFrame::get_StatusText(BSTR *pbsStatusText)
{
	return E_NOTIMPL;
}

HRESULT CMainFrame::Strings::Invoke(DISPID dispid, REFIID, LCID, WORD wFlags,
	DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	HRESULT hr = DISP_E_EXCEPTION;
	try
	{
		String s = LanguageSelect.LoadString(dispid);
		V_VT(pVarResult) = VT_BSTR;
		V_BSTR(pVarResult) = ::SysAllocStringLen(s.c_str(), s.length());
		hr = V_BSTR(pVarResult) ? S_OK : E_OUTOFMEMORY;
	}
	catch (OException *e)
	{
		pExcepInfo->wCode = 1001;
		::SysReAllocString(&pExcepInfo->bstrDescription, e->msg);
		m_spTypeInfo->GetDocumentation(-1, &pExcepInfo->bstrSource, NULL, NULL, NULL);
		delete e;
	}
	return hr;
}

HRESULT CMainFrame::get_Strings(IDispatch **ppDispatch)
{
	*ppDispatch = &m_Strings;
	return S_OK;
}

HRESULT CMainFrame::ShowHTMLDialog(BSTR url, VARIANT *arguments, BSTR features, VARIANT *ret)
{
	String moniker = url;
	CMyComPtr<IMoniker> spMoniker;
	HRESULT hr = URLMON->CreateURLMoniker(NULL, env_ResolveMoniker(moniker), &spMoniker);
	if (FAILED(hr))
		return hr;
	DWORD flags = HTMLDLG_MODAL | HTMLDLG_VERIFY | HTMLDLG_PRINT_TEMPLATE;
	if (EatPrefix(url, L"modal:"))
		flags = HTMLDLG_MODAL | HTMLDLG_VERIFY;
	else if (EatPrefix(url, L"modeless:"))
		flags = HTMLDLG_MODELESS | HTMLDLG_VERIFY;
	else if (!EatPrefix(url, L"print_template:"))
		return E_INVALIDARG;
	return MSHTML->ShowHTMLDialogEx(m_hWnd, spMoniker, flags, arguments, features, ret);
}

void CMainFrame::LoadFilesMRU()
{
	if (HKEY hKey = SettingStore.GetSectionKey(_T("Recent File List")))
	{
		DWORD index = 0;
		DWORD type;
		TCHAR name[256];
		BYTE data[1024];
		DWORD cch, cb;
		while (RegEnumValue(hKey, index,
			name, &(cch = _countof(name)),
			NULL, &type,
			data, &(cb = sizeof data)) == ERROR_SUCCESS)
		{
			if (type == REG_SZ)
			{
				String s = reinterpret_cast<LPTSTR>(data);
				m_FilesMRU.push_back(s);
			}
			++index;
		}
		RegCloseKey(hKey);
	}
}

void CMainFrame::SaveFilesMRU()
{
	if (HKEY hKey = SettingStore.GetSectionKey(_T("Recent File List"), CREATE_ALWAYS))
	{
		size_t i = 0;
		while (i < m_FilesMRU.size())
		{
			TCHAR name[20];
			const String &value = m_FilesMRU[i];
			wsprintf(name, _T("File%u"), ++i);
			RegSetValueEx(hKey, name, 0, REG_SZ,
				reinterpret_cast<const BYTE *>(value.c_str()),
				(value.length() + 1) * sizeof(TCHAR));
		}
		RegCloseKey(hKey);
	}
}

void CMainFrame::InitCmdUI()
{
	memset(&m_cmdState, MF_GRAYED, sizeof m_cmdState);
	m_wndToolBar->EnableButton(ID_FILE_SAVE, FALSE);
	m_wndToolBar->EnableButton(ID_EDIT_UNDO, FALSE);
	m_wndToolBar->EnableButton(ID_EDIT_REDO, FALSE);
	m_wndToolBar->EnableButton(ID_L2R, FALSE);
	m_wndToolBar->EnableButton(ID_R2L, FALSE);
	m_wndToolBar->EnableButton(ID_L2RNEXT, FALSE);
	m_wndToolBar->EnableButton(ID_R2LNEXT, FALSE);
	m_wndToolBar->EnableButton(ID_ALL_LEFT, FALSE);
	m_wndToolBar->EnableButton(ID_ALL_RIGHT, FALSE);
	m_wndToolBar->EnableButton(ID_REFRESH, FALSE);
	m_wndToolBar->EnableButton(ID_PREVDIFF, FALSE);
	m_wndToolBar->EnableButton(ID_NEXTDIFF, FALSE);
	m_wndToolBar->EnableButton(ID_FIRSTDIFF, FALSE);
	m_wndToolBar->EnableButton(ID_LASTDIFF, FALSE);
	m_wndToolBar->EnableButton(ID_CURDIFF, FALSE);
	m_wndToolBar->EnableButton(ID_SELECTLINEDIFF, FALSE);
	// Clear the merge mode indicator in the main status pane
	m_wndStatusBar->SetPartText(1, NULL);
	// Clear the item count in the main status pane
	m_wndStatusBar->SetPartText(2, NULL);
}

template<>
void CMainFrame::UpdateCmdUI<ID_FILE_LEFT_READONLY>(BYTE uFlags)
{
	m_cmdState.LeftReadOnly = uFlags;
}

template<>
void CMainFrame::UpdateCmdUI<ID_FILE_RIGHT_READONLY>(BYTE uFlags)
{
	m_cmdState.RightReadOnly = uFlags;
}

template<>
void CMainFrame::UpdateCmdUI<ID_REFRESH>(BYTE uFlags)
{
	m_cmdState.Refresh = uFlags;
	m_wndToolBar->EnableButton(ID_REFRESH, !(uFlags & MF_GRAYED));
}

template<>
void CMainFrame::UpdateCmdUI<ID_FILE_ENCODING>(BYTE uFlags)
{
	m_cmdState.FileEncoding = uFlags;
}

template<>
void CMainFrame::UpdateCmdUI<ID_MERGE_COMPARE>(BYTE uFlags)
{
	m_cmdState.MergeCompare = uFlags;
}

template<>
void CMainFrame::UpdateCmdUI<ID_L2R>(BYTE uFlags)
{
	m_cmdState.LeftToRight = uFlags;
	m_wndToolBar->EnableButton(ID_L2R, !(uFlags & MF_GRAYED));
	m_wndToolBar->EnableButton(ID_L2RNEXT, !(uFlags & MF_GRAYED));
}

template<>
void CMainFrame::UpdateCmdUI<ID_R2L>(BYTE uFlags)
{
	m_cmdState.RightToLeft = uFlags;
	m_wndToolBar->EnableButton(ID_R2L, !(uFlags & MF_GRAYED));
	m_wndToolBar->EnableButton(ID_R2LNEXT, !(uFlags & MF_GRAYED));
}

template<>
void CMainFrame::UpdateCmdUI<ID_ALL_LEFT>(BYTE uFlags)
{
	m_cmdState.AllLeft = uFlags;
	m_wndToolBar->EnableButton(ID_ALL_LEFT, !(uFlags & MF_GRAYED));
}

template<>
void CMainFrame::UpdateCmdUI<ID_ALL_RIGHT>(BYTE uFlags)
{
	m_cmdState.AllRight = uFlags;
	m_wndToolBar->EnableButton(ID_ALL_RIGHT, !(uFlags & MF_GRAYED));
}

template<>
void CMainFrame::UpdateCmdUI<ID_PREVDIFF>(BYTE uFlags)
{
	m_cmdState.PrevDiff = uFlags;
	m_wndToolBar->EnableButton(ID_PREVDIFF, !(uFlags & MF_GRAYED));
	m_wndToolBar->EnableButton(ID_FIRSTDIFF, !(uFlags & MF_GRAYED));
}

template<>
void CMainFrame::UpdateCmdUI<ID_NEXTDIFF>(BYTE uFlags)
{
	m_cmdState.NextDiff = uFlags;
	m_wndToolBar->EnableButton(ID_NEXTDIFF, !(uFlags & MF_GRAYED));
	m_wndToolBar->EnableButton(ID_LASTDIFF, !(uFlags & MF_GRAYED));
}

template<>
void CMainFrame::UpdateCmdUI<ID_CURDIFF>(BYTE uFlags)
{
	m_cmdState.CurDiff = uFlags;
	m_wndToolBar->EnableButton(ID_CURDIFF, !(uFlags & MF_GRAYED));
}

template<>
void CMainFrame::UpdateCmdUI<ID_FILE_SAVE>(BYTE uFlags)
{
	m_cmdState.Save = uFlags;
	m_wndToolBar->EnableButton(ID_FILE_SAVE, !(uFlags & MF_GRAYED));
}

template<>
void CMainFrame::UpdateCmdUI<ID_FILE_SAVE_LEFT>(BYTE uFlags)
{
	m_cmdState.SaveLeft = uFlags;
}

template<>
void CMainFrame::UpdateCmdUI<ID_FILE_SAVE_RIGHT>(BYTE uFlags)
{
	m_cmdState.SaveRight = uFlags;
}

template<>
void CMainFrame::UpdateCmdUI<ID_EDIT_UNDO>(BYTE uFlags)
{
	m_cmdState.Undo = uFlags;
	m_wndToolBar->EnableButton(ID_EDIT_UNDO, !(uFlags & MF_GRAYED));
}

template<>
void CMainFrame::UpdateCmdUI<ID_EDIT_REDO>(BYTE uFlags)
{
	m_cmdState.Redo = uFlags;
	m_wndToolBar->EnableButton(ID_EDIT_REDO, !(uFlags & MF_GRAYED));
}

template<>
void CMainFrame::UpdateCmdUI<ID_EDIT_CUT>(BYTE uFlags)
{
	m_cmdState.Cut = uFlags;
}

template<>
void CMainFrame::UpdateCmdUI<ID_EDIT_COPY>(BYTE uFlags)
{
	m_cmdState.Copy = uFlags;
}

template<>
void CMainFrame::UpdateCmdUI<ID_EDIT_PASTE>(BYTE uFlags)
{
	m_cmdState.Paste = uFlags;
}

template<>
void CMainFrame::UpdateCmdUI<ID_EDIT_REPLACE>(BYTE uFlags)
{
	m_cmdState.Replace = uFlags;
}

template<>
void CMainFrame::UpdateCmdUI<ID_SELECTLINEDIFF>(BYTE uFlags)
{
	m_cmdState.SelectLineDiff = uFlags;
	m_wndToolBar->EnableButton(ID_SELECTLINEDIFF, !(uFlags & MF_GRAYED));
}

template<>
void CMainFrame::UpdateCmdUI<ID_MERGE_DELETE>(BYTE uFlags)
{
	m_cmdState.Delete = uFlags;
}

template<>
void CMainFrame::UpdateCmdUI<ID_VIEW_TREEMODE>(BYTE uFlags)
{
	m_cmdState.TreeMode = uFlags;
}

template<>
void CMainFrame::UpdateCmdUI<ID_VIEW_SHOWHIDDENITEMS>(BYTE uFlags)
{
	m_cmdState.ShowHiddenItems = uFlags;
}

template<>
void CMainFrame::UpdateCmdUI<ID_VIEW_EXPAND_ALLSUBDIRS>(BYTE uFlags)
{
	m_cmdState.ExpandAllSubdirs = uFlags;
}

template<>
void CMainFrame::UpdateCmdUI<ID_EOL_TO_DOS>(BYTE uFlags)
{
	m_cmdState.EolToDos = uFlags;
}

template<>
void CMainFrame::UpdateCmdUI<ID_EOL_TO_UNIX>(BYTE uFlags)
{
	m_cmdState.EolToUnix = uFlags;
}

template<>
void CMainFrame::UpdateCmdUI<ID_EOL_TO_MAC>(BYTE uFlags)
{
	m_cmdState.EolToMac = uFlags;
}

template<>
void CMainFrame::UpdateCmdUI<ID_TOOLS_GENERATEREPORT>(BYTE uFlags)
{
	m_cmdState.GenerateReport = uFlags;
}

template<>
void CMainFrame::UpdateCmdUI<ID_TOOLS_COMPARE_SELECTION>(BYTE uFlags)
{
	m_cmdState.CompareSelection = uFlags;
}

template<>
void CMainFrame::UpdateCmdUI<ID_EDIT_TOGGLE_BOOKMARK>(BYTE uFlags)
{
	m_cmdState.ToggleBookmark = uFlags;
}

template<>
void CMainFrame::UpdateCmdUI<ID_EDIT_GOTO_NEXT_BOOKMARK>(BYTE uFlags)
{
	m_cmdState.NavigateBookmarks = uFlags;
}

template<>
LRESULT CMainFrame::OnWndMsg<WM_DRAWITEM>(WPARAM, LPARAM lParam)
{
	DRAWITEMSTRUCT *const lpdis = reinterpret_cast<DRAWITEMSTRUCT *>(lParam);
	if (CDirView::IsShellMenuCmdID(lpdis->itemID))
	{
		CDocFrame *pDocFrame = GetActiveDocFrame();
		if (pDocFrame->GetFrameType() == FRAME_FOLDER)
		{
			CDirView *pView = static_cast<CDirFrame *>(pDocFrame)->m_pDirView;
			pView->HandleMenuMessage(WM_DRAWITEM, lpdis->itemID, lParam);
		}
		return 0;
	}
	UINT fStyle = ILD_TRANSPARENT;
	UINT uStateMask = LOBYTE(LOWORD(dwOsVer)) < 6 ? ODS_GRAYED : ODS_SELECTED | ODS_GRAYED;
	if ((lpdis->itemState & uStateMask) == uStateMask)
		fStyle |= ILD_BLEND;
	if (lpdis->itemData != ~0)
	{
		m_imlMenu->Draw(lpdis->itemData, lpdis->hDC,
			lpdis->rcItem.left + GetMenuBitmapExcessWidth() - 16,
			lpdis->rcItem.top + (lpdis->rcItem.bottom - lpdis->rcItem.top - 16) / 2,
			fStyle);
	}
	return 0;
}

template<>
LRESULT CMainFrame::OnWndMsg<WM_MEASUREITEM>(WPARAM, LPARAM lParam)
{
	MEASUREITEMSTRUCT *const lpmis = reinterpret_cast<MEASUREITEMSTRUCT *>(lParam);
	if (CDirView::IsShellMenuCmdID(lpmis->itemID))
	{
		CDocFrame *pDocFrame = GetActiveDocFrame();
		if (pDocFrame->GetFrameType() == FRAME_FOLDER)
		{
			CDirView *pView = static_cast<CDirFrame *>(pDocFrame)->m_pDirView;
			pView->HandleMenuMessage(WM_MEASUREITEM, lpmis->itemID, lParam);
		}
		return 0;
	}
	lpmis->itemWidth += 5;
	if (lpmis->itemData != ~0)
	{
		lpmis->itemWidth += GetMenuBitmapExcessWidth();
		if (lpmis->itemHeight < 16)
			lpmis->itemHeight = 16;
	}
	return 0;
}

void CMainFrame::SetBitmaps(HMENU hMenu)
{
	MENUITEMINFO mii;
	mii.cbSize = sizeof mii;
	mii.fMask = MIIM_BITMAP | MIIM_DATA;
	mii.hbmpItem = HBMMENU_CALLBACK;
	// Load bitmaps to menuitems
	int i = _countof(m_MenuIcons);
	int j = m_imlMenu->GetImageCount();
	while (i--)
	{
		mii.dwItemData = --j;
		::SetMenuItemInfo(hMenu, m_MenuIcons[i].menuitemID, FALSE, &mii);
	}
	i = m_wndToolBar->GetButtonCount();
	while (i--)
	{
		TBBUTTONINFO buttonInfo;
		buttonInfo.cbSize = sizeof buttonInfo;
		buttonInfo.dwMask = TBIF_BYINDEX | TBIF_IMAGE | TBIF_STYLE | TBIF_COMMAND;
		m_wndToolBar->GetButtonInfo(i, &buttonInfo);
		if (buttonInfo.fsStyle & BTNS_SEP)
			continue;
		mii.dwItemData = buttonInfo.iImage;
		::SetMenuItemInfo(hMenu, buttonInfo.idCommand, FALSE, &mii);
	}

	mii.fMask = MIIM_FTYPE;
	mii.fType = MFT_RADIOCHECK;
	mii.hbmpChecked = NULL;
	::SetMenuItemInfo(hMenu, ID_EOL_TO_DOS, FALSE, &mii);
	::SetMenuItemInfo(hMenu, ID_EOL_TO_UNIX, FALSE, &mii);
	::SetMenuItemInfo(hMenu, ID_EOL_TO_MAC, FALSE, &mii);
}

const BYTE *CMainFrame::CmdState::Lookup(UINT id)
{
	switch (id)
	{
	case ID_FILE_SAVE:
		return &Save;
	case ID_FILE_SAVE_LEFT:
		return &SaveLeft;
	case ID_FILE_SAVE_RIGHT:
		return &SaveRight;
	case ID_FILE_LEFT_READONLY:
		return &LeftReadOnly;
	case ID_FILE_RIGHT_READONLY:
		return &RightReadOnly;
	case ID_REFRESH:
		return &Refresh;
	case ID_FILE_ENCODING:
		return &FileEncoding;
	case ID_EDIT_UNDO:
		return &Undo;
	case ID_EDIT_REDO:
		return &Redo;
	case ID_EDIT_COPY:
		return &Copy;
	case ID_EDIT_CUT:
	case ID_EDIT_CAPITALIZE:
	case ID_EDIT_SWAPCASE:
	case ID_EDIT_UPPERCASE:
	case ID_EDIT_LOWERCASE:
	case ID_EDIT_SENTENCE:
		return &Cut;
	case ID_EDIT_PASTE:
		return &Paste;
	case ID_EDIT_REPLACE:
		return &Replace;
	case ID_MERGE_COMPARE:
	case ID_MERGE_COMPARE_XML:
	case ID_MERGE_COMPARE_HEX:
		return &MergeCompare;
	case ID_CURDIFF:
		return &CurDiff;;
	case ID_PREVDIFF:
	case ID_FIRSTDIFF:
		return &PrevDiff;
	case ID_NEXTDIFF:
	case ID_LASTDIFF:
		return &NextDiff;
	case ID_L2R:
	case ID_L2RNEXT:
		return &LeftToRight;
	case ID_R2L:
	case ID_R2LNEXT:
		return &RightToLeft;
	case ID_MERGE_DELETE:
		return &Delete;
	case ID_ALL_LEFT:
		return &AllLeft;
	case ID_ALL_RIGHT:
		return &AllRight;
	case ID_VIEW_TREEMODE:
		return &TreeMode;
	case ID_VIEW_SHOWHIDDENITEMS:
		return &ShowHiddenItems;
	case ID_VIEW_EXPAND_ALLSUBDIRS:
	case ID_VIEW_COLLAPSE_ALLSUBDIRS:
		return &ExpandAllSubdirs;
	case ID_EOL_TO_DOS:
		return &EolToDos;
	case ID_EOL_TO_UNIX:
		return &EolToUnix;
	case ID_EOL_TO_MAC:
		return &EolToMac;
	case ID_TOOLS_GENERATEREPORT:
		return &GenerateReport;
	case ID_TOOLS_COMPARE_SELECTION:
		return &CompareSelection;
	case ID_EDIT_TOGGLE_BOOKMARK:
		return &ToggleBookmark;
	case ID_EDIT_GOTO_NEXT_BOOKMARK:
	case ID_EDIT_GOTO_PREV_BOOKMARK:
	case ID_EDIT_CLEAR_ALL_BOOKMARKS:
		return &NavigateBookmarks;
	// TODO: Remove this once MFC has gone.
	/*case ID_FILE_OPEN_REGISTERED:
	case ID_FILE_OPEN_WITHEDITOR:
	case ID_FILE_OPEN_WITH:
	case ID_FILE_SAVEAS_LEFT:
	case ID_FILE_SAVEAS_RIGHT:
	case ID_WINDOW_CHANGE_PANE:
	case ID_VIEW_LANGUAGE:
	case ID_APP_ABOUT:
	case ID_EDIT_WMGOTO:
	case ID_EDIT_COPY_LINENUMBERS:
	case ID_FILE_NEW:
	case ID_FILE_OPEN:
	case ID_FILE_OPENPROJECT:
	case ID_FILE_SAVEPROJECT:
		static const BYTE enabled = 0;
		return &enabled;*/
	}
	return NULL;
}

static COptionDef<bool> *LookupOption(UINT id)
{
	switch (id)
	{
	case ID_OPTIONS_SHOWDIFFERENT:
		return &OPT_SHOW_DIFFERENT;
	case ID_OPTIONS_SHOWIDENTICAL:
		return &OPT_SHOW_IDENTICAL;
	case ID_OPTIONS_SHOWUNIQUELEFT:
		return &OPT_SHOW_UNIQUE_LEFT;
	case ID_OPTIONS_SHOWUNIQUERIGHT:
		return &OPT_SHOW_UNIQUE_RIGHT;
	case ID_OPTIONS_SHOWBINARIES:
		return &OPT_SHOW_BINARIES;
	case ID_OPTIONS_SHOWSKIPPED:
		return &OPT_SHOW_SKIPPED;
	case ID_VIEW_LINEDIFFS:
		return &OPT_WORDDIFF_HIGHLIGHT;
	case ID_VIEW_LINENUMBERS:
		return &OPT_VIEW_LINENUMBERS;
	case ID_VIEW_FILEMARGIN:
		return &OPT_VIEW_FILEMARGIN;
	case ID_VIEW_WORDWRAP:
		return &OPT_WORDWRAP;
	case ID_VIEW_WHITESPACE:
		return &OPT_VIEW_WHITESPACE;
	case ID_VIEW_TAB_BAR:
		return &OPT_SHOW_TABBAR;
	case ID_VIEW_STATUS_BAR:
		return &OPT_SHOW_STATUSBAR;
	case ID_VIEW_LOCATION_BAR:
		return &OPT_SHOW_LOCATIONBAR;
	case ID_VIEW_DETAIL_BAR:
		return &OPT_SHOW_DIFFVIEWBAR;
	case ID_VIEW_RESIZE_PANES:
		return &OPT_RESIZE_PANES;
	case ID_FILE_MERGINGMODE:
		return &OPT_MERGE_MODE;
	}
	return NULL;
}

/**
 * @brief This handler updates the menus from time to time.
 */
template<>
LRESULT CMainFrame::OnWndMsg<WM_INITMENUPOPUP>(WPARAM wParam, LPARAM lParam) 
{
	if (HIWORD(lParam)) // bSysMenu
		return 0;
	HMenu *const pMenu = reinterpret_cast<HMenu *>(wParam);
	CDocFrame *const pDocFrame = GetActiveDocFrame();
	if (pDocFrame->GetFrameType() == FRAME_FOLDER)
	{
		CDirView *pView = static_cast<CDirFrame *>(pDocFrame)->m_pDirView;
		pView->HandleMenuMessage(WM_INITMENUPOPUP, wParam, lParam);
	}
	MENUITEMINFO mii;
	mii.cbSize = sizeof mii;
	mii.fMask = MIIM_STATE | MIIM_ID;
	UINT i = pMenu->GetMenuItemCount();
	while (i)
	{
		--i;
		pMenu->GetMenuItemInfo(i, TRUE, &mii);
		// Handle the exceptional cases
		switch (mii.wID)
		{
		case ID_TOOLBAR_NONE:
			pMenu->CheckMenuRadioItem(ID_TOOLBAR_NONE, ID_TOOLBAR_BIG,
				!COptionsMgr::Get(OPT_SHOW_TOOLBAR) ? ID_TOOLBAR_NONE :
				!COptionsMgr::Get(OPT_TOOLBAR_SIZE) ? ID_TOOLBAR_SMALL :
				ID_TOOLBAR_BIG, MF_BYCOMMAND);
			continue;
		case ID_FILE_NEW:
		case ID_FILE_OPEN:
		case ID_FILE_OPENPROJECT:
		case ID_FILE_OPENCONFLICT:
			mii.fState = m_wndTabBar->IsWindowEnabled() ? MF_ENABLED : MF_GRAYED;
			pMenu->EnableMenuItem(mii.wID, mii.fState);
			continue;
		case ID_FILE_MRU_FILE1:
			if (mii.fState & MF_DISABLED)
			{
				TCHAR text[128];
				pMenu->GetMenuString(mii.wID, text, _countof(text));
				m_TitleMRU = text;
			}
			do
			{
				pMenu->DeleteMenu(mii.wID);
			} while (++mii.wID <= ID_FILE_MRU_FILE16);
			if (size_t n = m_FilesMRU.size())
			{
				mii.fState = m_wndTabBar->IsWindowEnabled() ? MF_ENABLED : MF_GRAYED;
				do
				{
					size_t u = n--;
					string_format text(_T("&%u %s"), u, m_FilesMRU[n].c_str());
					pMenu->InsertMenu(i, MF_BYPOSITION | mii.fState, ID_FILE_MRU_FILE1 + n, text.c_str());
				} while (n);
			}
			else
			{
				pMenu->InsertMenu(i, MF_BYPOSITION | MF_GRAYED, ID_FILE_MRU_FILE1, m_TitleMRU.c_str());
			}
			continue;
		case ID_COLORSCHEME_FIRST:
			pMenu->CheckMenuRadioItem(ID_COLORSCHEME_FIRST, ID_COLORSCHEME_LAST, ID_COLORSCHEME_FIRST + m_sourceType);
			mii.fState = COptionsMgr::Get(OPT_SYNTAX_HIGHLIGHT) ? MF_ENABLED : MF_GRAYED;
			do
			{
				pMenu->EnableMenuItem(mii.wID, mii.fState);
			} while (++mii.wID <= ID_COLORSCHEME_LAST);
			continue;
		case ID_VIEW_CONTEXT_0:
			pMenu->CheckMenuRadioItem(ID_VIEW_CONTEXT_0, ID_VIEW_CONTEXT_UNLIMITED,
				pDocFrame->GetFrameType() == FRAME_FILE ? static_cast<CChildFrame *>(pDocFrame)->m_idContextLines : 0);
			// Limiting context is only allowed when there are no unsaved changes.
			mii.fState = m_cmdState.Save & MF_GRAYED ? MF_ENABLED : MF_GRAYED;
			do
			{
				pMenu->EnableMenuItem(mii.wID, mii.fState);
			} while (++mii.wID <= ID_VIEW_CONTEXT_UNLIMITED);
			continue;
		case ID_FILE_CLOSE:
		case ID_WINDOW_CLOSEALL:
		case ID_WINDOW_CHANGE_PANE:
		case ID_WINDOW_TILE_HORZ:
		case ID_WINDOW_TILE_VERT:
		case ID_WINDOW_CASCADE:
			pMenu->EnableMenuItem(mii.wID, pDocFrame ? MF_ENABLED : MF_GRAYED);
			continue;
		case ID_HELP_MERGE7ZMISMATCH:
			if (HasZipSupport())
				pMenu->RemoveMenu(i, MF_BYPOSITION);
			continue;
		case ID_VIEW_LOCATION_BAR:
		case ID_VIEW_DETAIL_BAR:
			if (HWindow *pBar = pDocFrame ?
				reinterpret_cast<HWindow *>(pDocFrame->m_hWnd)->GetDlgItem(mii.wID) : NULL)
			{
				pMenu->EnableMenuItem(mii.wID, MF_ENABLED);
				pMenu->CheckMenuItem(mii.wID, pBar->IsWindowVisible() ? MF_CHECKED : MF_UNCHECKED);
			}
			else
			{
				pMenu->EnableMenuItem(mii.wID, MF_DISABLED);
			}
			continue;
		}
		// Handle the CmdState driven cases
		if (const BYTE *state = m_cmdState.Lookup(mii.wID))
		{
			mii.fState = *state;
		}
		// Handle the option driven cases
		else if (COptionDef<bool> *option = LookupOption(mii.wID))
		{
			mii.fState = COptionsMgr::Get(*option) ? MF_CHECKED : MF_UNCHECKED;
		}
		pMenu->SetMenuItemInfo(i, TRUE, &mii);
	}
	return 0;
}

template<>
LRESULT CMainFrame::OnWndMsg<WM_MENUCHAR>(WPARAM wParam, LPARAM lParam)
{
	CDocFrame *pDocFrame = GetActiveDocFrame();
	if (pDocFrame->GetFrameType() == FRAME_FOLDER)
	{
		CDirView *pView = static_cast<CDirFrame *>(pDocFrame)->m_pDirView;
		if (LRESULT res = pView->HandleMenuMessage(WM_MENUCHAR, wParam, lParam))
			return res;
	}
	return 0;
}

static HMenu *pCurrentMenu = NULL;

template<>
LRESULT CMainFrame::OnWndMsg<WM_MENUSELECT>(WPARAM wParam, LPARAM lParam)
{
	CDocFrame *pDocFrame = GetActiveDocFrame();
	if (pDocFrame->GetFrameType() == FRAME_FOLDER)
	{
		CDirView *pView = static_cast<CDirFrame *>(pDocFrame)->m_pDirView;
		pView->HandleMenuSelect(wParam, lParam);
	}
	if (HIWORD(wParam) == 0xFFFF && lParam == 0)
	{
		SetStatus(0U);
	}
	else if (HIWORD(wParam) & MF_POPUP)
	{
		SetStatus(_T(""));
		pCurrentMenu = reinterpret_cast<HMenu *>(lParam);
	}
	else
	{
		String text = LanguageSelect.LoadString(LOWORD(wParam));
		String::size_type length = text.find(_T('\n'));
		if (length != String::npos)
			text.resize(length);
		SetStatus(text.c_str());
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnFileOpen() 
{
	FileLocation filelocLeft, filelocRight;
	DoFileOpen(filelocLeft, filelocRight);
}

void CMainFrame::OnFileClose()
{
	if (CDocFrame *pDocFrame = GetActiveDocFrame())
		pDocFrame->SendMessage(WM_CLOSE);
}

/**
 * @brief Check for BOM, and also, if bGuessEncoding, try to deduce codepage
 *
 * Unpacks info from FileLocation & delegates all work to codepage_detect module
 */
static void
FileLocationGuessEncodings(FileLocation & fileloc, BOOL bGuessEncoding)
{
	GuessCodepageEncoding(fileloc.filepath.c_str(), &fileloc.encoding,
		bGuessEncoding);
}

/**
 * @brief Creates new MergeDoc instance and shows documents.
 * @param [in] pDirDoc Dir compare document to create a new Merge document for.
 * @param [in] ifilelocLeft Left side file location info.
 * @param [in] ifilelocRight Right side file location info.
 * @param [in] dwLeftFlags Left side flags.
 * @param [in] dwRightFlags Right side flags.
 * @param [in] infoUnpacker Plugin info.
 * @return OPENRESULTS_TYPE for success/failure code.
 */
void CMainFrame::ShowMergeDoc(CDirFrame *pDirDoc,
	FileLocation & filelocLeft, FileLocation & filelocRight,
	DWORD dwLeftFlags /*=0*/, DWORD dwRightFlags /*=0*/,
	PackingInfo * infoUnpacker /*= NULL*/)
{
	// detect codepage
	BOOL bGuessEncoding = COptionsMgr::Get(OPT_CP_DETECT);
	if (filelocLeft.encoding.m_unicoding == -1)
		filelocLeft.encoding.m_unicoding = NONE;
	if (filelocLeft.encoding.m_unicoding == NONE && filelocLeft.encoding.m_codepage == -1)
	{
		FileLocationGuessEncodings(filelocLeft, bGuessEncoding);
	}
	if (filelocRight.encoding.m_unicoding == -1)
		filelocRight.encoding.m_unicoding = NONE;
	if (filelocRight.encoding.m_unicoding == NONE && filelocRight.encoding.m_codepage == -1)
	{
		FileLocationGuessEncodings(filelocRight, bGuessEncoding);
	}

	BOOL bLeftRO = (dwLeftFlags & FFILEOPEN_READONLY) > 0;
	BOOL bRightRO = (dwRightFlags & FFILEOPEN_READONLY) > 0;

	if ((dwLeftFlags & FFILEOPEN_DETECTBIN) && filelocLeft.encoding.m_binary ||
		(dwRightFlags & FFILEOPEN_DETECTBIN) && filelocRight.encoding.m_binary)
	{
		ShowHexMergeDoc(pDirDoc, filelocLeft, filelocRight, bLeftRO, bRightRO);
		return;
	}

	CChildFrame *const pMergeDoc = GetMergeDocToShow(pDirDoc);

	ASSERT(pMergeDoc);		// must ASSERT to get an answer to the question below ;-)
	if (!pMergeDoc)
		return; // when does this happen ?

	// if an unpacker is selected, it must be used during LoadFromFile
	// MergeDoc must memorize it for SaveToFile
	// Warning : this unpacker may differ from the pDirDoc one
	// (through menu : "Plugins"->"Open with unpacker")
	pMergeDoc->SetUnpacker(infoUnpacker);
	// Note that OpenDocs() takes care of closing compare window when needed.
	OPENRESULTS_TYPE openResults = pMergeDoc->OpenDocs(filelocLeft, filelocRight,
			bLeftRO, bRightRO);

	if (openResults == OPENRESULTS_SUCCESS)
	{
		if (dwLeftFlags & FFILEOPEN_MODIFIED)
		{
			pMergeDoc->m_ptBuf[0]->SetModified(TRUE);
			pMergeDoc->UpdateHeaderPath(0);
		}
		if (dwRightFlags & FFILEOPEN_MODIFIED)
		{
			pMergeDoc->m_ptBuf[1]->SetModified(TRUE);
			pMergeDoc->UpdateHeaderPath(1);
		}
	}
}

void CMainFrame::ShowHexMergeDoc(CDirFrame *pDirDoc,
	const FileLocation &left, const FileLocation &right, BOOL bLeftRO, BOOL bRightRO)
{
	if (CHexMergeFrame *pHexMergeDoc = GetHexMergeDocToShow(pDirDoc))
		pHexMergeDoc->OpenDocs(left, right, bLeftRO, bRightRO);
}

void CMainFrame::RedisplayAllDirDocs()
{
	HWindow *pMDIClient = reinterpret_cast<HWindow *>(m_pWndMDIClient);
	HWindow *pChild = NULL;
	while ((pChild = pMDIClient->FindWindowEx(pChild, WinMergeWindowClass)) != NULL)
	{
		CDocFrame *pDocFrame = static_cast<CDocFrame *>(CDocFrame::FromHandle(pChild));
		if (pDocFrame->GetFrameType() == FRAME_FOLDER)
			static_cast<CDirFrame *>(pDocFrame)->Redisplay();
	}
}

/**
 * @brief Show/Hide different files/directories
 */
void CMainFrame::OnOptionsShowDifferent() 
{
	bool val = COptionsMgr::Get(OPT_SHOW_DIFFERENT);
	COptionsMgr::SaveOption(OPT_SHOW_DIFFERENT, !val); // reverse
	RedisplayAllDirDocs();
}

/**
 * @brief Show/Hide identical files/directories
 */
void CMainFrame::OnOptionsShowIdentical() 
{
	bool val = COptionsMgr::Get(OPT_SHOW_IDENTICAL);
	COptionsMgr::SaveOption(OPT_SHOW_IDENTICAL, !val); // reverse
	RedisplayAllDirDocs();
}

/**
 * @brief Show/Hide left-only files/directories
 */
void CMainFrame::OnOptionsShowUniqueLeft() 
{
	bool val = COptionsMgr::Get(OPT_SHOW_UNIQUE_LEFT);
	COptionsMgr::SaveOption(OPT_SHOW_UNIQUE_LEFT, !val); // reverse
	RedisplayAllDirDocs();
}

/**
 * @brief Show/Hide right-only files/directories
 */
void CMainFrame::OnOptionsShowUniqueRight() 
{
	bool val = COptionsMgr::Get(OPT_SHOW_UNIQUE_RIGHT);
	COptionsMgr::SaveOption(OPT_SHOW_UNIQUE_RIGHT, !val); // reverse
	RedisplayAllDirDocs();
}

/**
 * @brief Show/Hide binary files
 */
void CMainFrame::OnOptionsShowBinaries()
{
	bool val = COptionsMgr::Get(OPT_SHOW_BINARIES);
	COptionsMgr::SaveOption(OPT_SHOW_BINARIES, !val); // reverse
	RedisplayAllDirDocs();
}

/**
 * @brief Show/Hide skipped files/directories
 */
void CMainFrame::OnOptionsShowSkipped()
{
	bool val = COptionsMgr::Get(OPT_SHOW_SKIPPED);
	COptionsMgr::SaveOption(OPT_SHOW_SKIPPED, !val); // reverse
	RedisplayAllDirDocs();
}

/**
 * @brief Show GNU licence information in notepad (local file) or in Web Browser
 */
void CMainFrame::OnHelpGnulicense() 
{
	const String spath = GetModulePath() + LicenseFile;
	OpenFileOrUrl(spath.c_str(), LicenceUrl);
}

/**
 * @brief Checks if path (file/folder) is read-only and asks overwriting it.
 *
 * @param strSavePath [in,out] Path where to save (file or folder)
 * @param bMultiFile [in] Single file or multiple files/folder
 * @param bApplyToAll [in,out] Apply last user selection for all items?
 * @return Users selection:
 * - IDOK: Item was not readonly, no actions
 * - IDYES/IDYESTOALL: Overwrite readonly item
 * - IDNO: User selected new filename (single file) or user wants to skip
 * - IDCANCEL: Cancel operation
 * @sa CMainFrame::SyncFileToVCS()
 * @sa CChildFrame::DoSave()
 */
int CMainFrame::HandleReadonlySave(String &strSavePath, BOOL bMultiFile,
		BOOL &bApplyToAll)
{
	UINT userChoice = 0;
	int nRetVal = IDOK;
	String s;
	DWORD attr = GetFileAttributes(strSavePath.c_str());
	if ((attr != INVALID_FILE_ATTRIBUTES) && ((attr & FILE_ATTRIBUTE_READONLY) != 0))
	{
		// Version control system used?
		// Checkout file from VCS and modify, don't ask about overwriting
		// RO files etc.
		int nVerSys = COptionsMgr::Get(OPT_VCS_SYSTEM);
		if (nVerSys != VCS_NONE)
		{
			BOOL bRetVal = SaveToVersionControl(strSavePath);
			if (bRetVal)
				return IDYES;
			else
				return IDCANCEL;
		}
		
		// Don't ask again if its already asked
		if (bApplyToAll)
			userChoice = IDYES;
		else
		{
			// Prompt for user choice
			if (bMultiFile)
			{
				// Multiple files or folder
				userChoice = LanguageSelect.FormatMessage(
					IDS_SAVEREADONLY_MULTI, strSavePath.c_str()
				).MsgBox(MB_YESNOCANCEL | MB_ICONWARNING | MB_DEFBUTTON3 | MB_DONT_ASK_AGAIN | MB_YES_TO_ALL);
			}
			else
			{
				// Single file
				userChoice = LanguageSelect.FormatMessage(
					IDS_SAVEREADONLY_FMT, strSavePath.c_str()
				).MsgBox(MB_YESNOCANCEL | MB_ICONWARNING | MB_DEFBUTTON2 | MB_DONT_ASK_AGAIN);
			}
		}
		switch (userChoice)
		{
		// Overwrite read-only file
		case IDYESTOALL:
			bApplyToAll = TRUE;  // Don't ask again (no break here)
		case IDYES:
			SetFileAttributes(strSavePath.c_str(), attr & ~FILE_ATTRIBUTE_READONLY);
			nRetVal = IDYES;
			break;
		
		// Save to new filename (single) /skip this item (multiple)
		case IDNO:
			if (!bMultiFile)
			{
				if (SelectFile(m_hWnd, s, strSavePath.c_str(), IDS_SAVE_AS_TITLE, NULL, FALSE))
				{
					strSavePath = s;
					nRetVal = IDNO;
				}
				else
					nRetVal = IDCANCEL;
			}
			else
				nRetVal = IDNO;
			break;

		// Cancel saving
		case IDCANCEL:
			nRetVal = IDCANCEL;
			break;
		}
	}
	return nRetVal;
}

/// Wrapper to set the global option 'm_bAllowMixedEol'
void CMainFrame::SetEOLMixed(BOOL bAllow)
{
	COptionsMgr::SaveOption(OPT_ALLOW_MIXED_EOL, bAllow == TRUE);
	ApplyViewWhitespace();
}

/**
 * @brief Opens Options-dialog and saves changed options
 */
void CMainFrame::OnOptions() 
{
	// Using singleton shared syntax colors
	CPreferencesDlg dlg;
	int rv = LanguageSelect.DoModal(dlg);

	if (rv == IDOK)
	{
		// Set new filterpath
		String filterPath = COptionsMgr::Get(OPT_SUPPLEMENT_FOLDER);
		globalFileFilter.SetUserFilterPath(filterPath.c_str());

		UpdateCodepageModule();
		// Call the wrapper to set m_bAllowMixedEol (the wrapper updates the registry)
		SetEOLMixed(COptionsMgr::Get(OPT_ALLOW_MIXED_EOL));

		sd_SetBreakChars(COptionsMgr::Get(OPT_BREAK_SEPARATORS).c_str());

		GetDirViewFontProperties();
		UpdateDirViewFont();

		GetMrgViewFontProperties();
		UpdateMrgViewFont();

		// make an attempt at rescanning any open diff sessions
		HWindow *pMDIClient = reinterpret_cast<HWindow *>(m_pWndMDIClient);
		HWindow *pChild = NULL;
		while ((pChild = pMDIClient->FindWindowEx(pChild, WinMergeWindowClass)) != NULL)
		{
			CDocFrame *pDocFrame = static_cast<CDocFrame *>(CDocFrame::FromHandle(pChild));
			switch (pDocFrame->GetFrameType())
			{
			case FRAME_FILE:
				static_cast<CChildFrame *>(pDocFrame)->RefreshOptions();
				static_cast<CChildFrame *>(pDocFrame)->FlushAndRescan(TRUE);
				break;
			case FRAME_FOLDER:
				break;
			}
		}
	}
}

/**
 * @brief Begin a diff: open dirdoc if it is directories, else open a mergedoc for editing.
 * @param [in] pszLeft Left-side path.
 * @param [in] pszRight Right-side path.
 * @param [in] dwLeftFlags Left-side flags.
 * @param [in] dwRightFlags Right-side flags.
 * @param [in] bRecurse Do we run recursive (folder) compare?
 * @param [in] pDirDoc Dir compare document to use.
 * @return TRUE if opening files and compare succeeded, FALSE otherwise.
 */
bool CMainFrame::DoFileOpen(FileLocation &filelocLeft, FileLocation &filelocRight,
	DWORD dwLeftFlags, DWORD dwRightFlags, BOOL bRecurse, CDirFrame *pDirDoc)
{
	// If the dirdoc we are supposed to use is busy doing a diff, bail out
	if (IsComparing())
		return false;

	bool bROLeft = (dwLeftFlags & FFILEOPEN_READONLY) != 0;
	bool bRORight = (dwRightFlags & FFILEOPEN_READONLY) != 0;

	UINT idCompareAs = 0;
	PackingInfo packingInfo;

	// pop up dialog unless arguments exist (and are compatible)
	PATH_EXISTENCE pathsType = DOES_NOT_EXIST;
	const DWORD attrLeft = GetFileAttributes(filelocLeft.filepath.c_str());
	const DWORD attrRight = GetFileAttributes(filelocRight.filepath.c_str());
	if (attrLeft == INVALID_FILE_ATTRIBUTES || attrRight == INVALID_FILE_ATTRIBUTES)
	{
		COpenDlg dlg;
		dlg.m_strLeft = filelocLeft.filepath;
		dlg.m_strRight = filelocRight.filepath;
		dlg.m_bRecurse = bRecurse;
		dlg.m_bLeftReadOnly = bROLeft;
		dlg.m_bRightReadOnly = bRORight;

		if (dwLeftFlags & FFILEOPEN_PROJECT || dwRightFlags & FFILEOPEN_PROJECT)
			dlg.m_bOverwriteRecursive = true; // Use given value, not previously used value
		if (dwLeftFlags & FFILEOPEN_CMDLINE || dwRightFlags & FFILEOPEN_CMDLINE)
			dlg.m_bOverwriteRecursive = true; // Use given value, not previously used value

		if (LanguageSelect.DoModal(dlg, m_hWnd) != IDOK)
			return false;

		filelocLeft.filepath = dlg.m_strLeft;
		filelocRight.filepath = dlg.m_strRight;
		bRecurse = dlg.m_bRecurse;
		bROLeft = dlg.m_bLeftReadOnly;
		bRORight = dlg.m_bRightReadOnly;
		idCompareAs = dlg.m_idCompareAs;
		// TODO: add codepage options to open dialog ?
		if (idCompareAs != 0)
		{
			dwLeftFlags &= ~FFILEOPEN_DETECT;
			dwRightFlags &= ~FFILEOPEN_DETECT;
			if (idCompareAs >= IDC_SCRIPT_FIRST && idCompareAs <= IDC_SCRIPT_LAST)
				packingInfo.SetPlugin(dlg.m_szCompareAs);
			pathsType = IS_EXISTING_FILE;
		}
		else
		{
			pathsType = GetPairComparability(
				filelocLeft.filepath.c_str(), filelocRight.filepath.c_str());
		}
	}
	else
	{
		// Add trailing '\' for directories if its missing
		if (attrLeft & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (!paths_EndsWithSlash(filelocLeft.filepath.c_str()))
				filelocLeft.filepath += '\\';
			dwLeftFlags &= ~FFILEOPEN_DETECT;
		}
		if (attrRight & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (!paths_EndsWithSlash(filelocRight.filepath.c_str()))
				filelocRight.filepath += '\\';
			dwRightFlags &= ~FFILEOPEN_DETECT;
		}
		pathsType = attrLeft & attrRight & FILE_ATTRIBUTE_DIRECTORY ? IS_EXISTING_DIR : IS_EXISTING_FILE;
		//save the MRU left and right files.
		if (!(dwLeftFlags & FFILEOPEN_NOMRU))
			addToMru(filelocLeft.filepath.c_str(), _T("Files\\Left"));
		if (!(dwRightFlags & FFILEOPEN_NOMRU))
			addToMru(filelocRight.filepath.c_str(), _T("Files\\Right"));
	}

	if (1)
	{
		LogFile.Write(0,
			_T("### Begin Comparison Parameters #############################\r\n")
			_T("\tLeft: %s\r\n")
			_T("\tRight: %s\r\n")
			_T("\tRecurse: %d\r\n")
			_T("### End Comparison Parameters #############################\r\n"),
			filelocLeft.filepath.c_str(),
			filelocRight.filepath.c_str(),
			bRecurse);
	}

	CTempPathContext *pTempPathContext = NULL;
	if (pathsType == IS_EXISTING_FILE)
	{
		switch (idCompareAs)
		{
		case 0:
			// User is inspecific about how to open files
		case ID_MERGE_COMPARE_TEXT:
			break;
		case ID_MERGE_COMPARE_XML:
			packingInfo.SetXML();
			break;
		case ID_MERGE_COMPARE_ZIP:
			dwLeftFlags |= FFILEOPEN_DETECTZIP;
			dwRightFlags |= FFILEOPEN_DETECTZIP;
			break;
		case ID_MERGE_COMPARE_ZIP_LEFT:
			dwLeftFlags |= FFILEOPEN_DETECTZIP;
			break;
		case ID_MERGE_COMPARE_ZIP_RIGHT:
			dwRightFlags |= FFILEOPEN_DETECTZIP;
			break;
		case ID_MERGE_COMPARE_HEX:
			// Open files in binary editor
			ShowHexMergeDoc(pDirDoc, filelocLeft, filelocRight, bROLeft, bRORight);
			return true;
		default:
			// Open files with specified file transform method
			ShowMergeDoc(pDirDoc, filelocLeft, filelocRight, dwLeftFlags, dwRightFlags, &packingInfo);
			return true;
		}
		try
		{
			// Handle archives using 7-zip
			if (dwLeftFlags & dwRightFlags & FFILEOPEN_DETECTZIP)
			{
				if (Merge7z::Format *piHandler = ArchiveGuessFormat(filelocLeft.filepath.c_str()))
				{
					pTempPathContext = new CTempPathContext;
					String path = GetClearTempPath(pTempPathContext, _T("0"));
					pTempPathContext->m_strLeftDisplayRoot = filelocLeft.filepath;
					pTempPathContext->m_strRightDisplayRoot = filelocRight.filepath;
					pathsType = IS_EXISTING_DIR;
					globalFileFilter.SetFilter(_T("*.*"));
					if (filelocLeft.filepath == filelocRight.filepath)
					{
						filelocRight.filepath.clear();
					}
					do
					{
						if FAILED(piHandler->DeCompressArchive(m_hWnd, filelocLeft.filepath.c_str(), path.c_str()))
							break;
						if (filelocLeft.filepath.find(path) == 0)
						{
							VERIFY(::DeleteFile(filelocLeft.filepath.c_str()) ||
								LogFile.Write(LogFile.LERROR|LogFile.LOSERROR,
								_T("DeleteFile(%s) failed: "), filelocLeft.filepath.c_str()));
						}
						SysFreeString(Assign(filelocLeft.filepath,
							piHandler->GetDefaultName(m_hWnd, filelocLeft.filepath.c_str())));
						filelocLeft.filepath.insert(0U, 1, _T('\\'));
						filelocLeft.filepath.insert(0U, path);
					} while (piHandler = ArchiveGuessFormat(filelocLeft.filepath.c_str()));
					filelocLeft.filepath = path;
					if (Merge7z::Format *piHandler = ArchiveGuessFormat(filelocRight.filepath.c_str()))
					{
						path = GetClearTempPath(pTempPathContext, _T("1"));
						do
						{
							if FAILED(piHandler->DeCompressArchive(m_hWnd, filelocRight.filepath.c_str(), path.c_str()))
								break;
							if (filelocRight.filepath.find(path) == 0)
							{
								VERIFY(::DeleteFile(filelocRight.filepath.c_str()) ||
									LogFile.Write(LogFile.LERROR|LogFile.LOSERROR,
									_T("DeleteFile(%s) failed: "), filelocRight.filepath.c_str()));
							}
							SysFreeString(Assign(filelocRight.filepath,
								piHandler->GetDefaultName(m_hWnd, filelocRight.filepath.c_str())));
							filelocRight.filepath.insert(0U, 1, _T('\\'));
							filelocRight.filepath.insert(0U, path);
						} while (piHandler = ArchiveGuessFormat(filelocRight.filepath.c_str()));
						filelocRight.filepath = path;
					}
					else if (filelocRight.filepath.empty())
					{
						// assume Perry style patch
						filelocRight.filepath = path;
						filelocLeft.filepath += _T("\\ORIGINAL");
						filelocRight.filepath += _T("\\ALTERED");
						if (GetPairComparability(filelocLeft.filepath.c_str(), filelocRight.filepath.c_str()) != IS_EXISTING_DIR)
						{
							// not a Perry style patch: diff with itself...
							filelocLeft.filepath = filelocRight.filepath = path;
						}
						else
						{
							pTempPathContext->m_strLeftDisplayRoot += _T("\\ORIGINAL");
							pTempPathContext->m_strRightDisplayRoot += _T("\\ALTERED");
						}
					}
				}
			}
			else if (dwLeftFlags & FFILEOPEN_DETECTZIP)
			{
				if (Merge7z::Format *piHandler = ArchiveGuessFormat(filelocLeft.filepath.c_str()))
				{
					pTempPathContext = new CTempPathContext;
					String path = GetClearTempPath(pTempPathContext, _T("0"));
					pTempPathContext->m_strLeftDisplayRoot = filelocLeft.filepath;
					pTempPathContext->m_strRightDisplayRoot = filelocRight.filepath;
					pathsType = IS_EXISTING_DIR;
					do
					{
						if FAILED(piHandler->DeCompressArchive(m_hWnd, filelocLeft.filepath.c_str(), path.c_str()))
							break;
						if (filelocLeft.filepath.find(path) == 0)
						{
							VERIFY(::DeleteFile(filelocLeft.filepath.c_str()) ||
								LogFile.Write(LogFile.LERROR|LogFile.LOSERROR,
								_T("DeleteFile(%s) failed: "), filelocLeft.filepath.c_str()));
						}
						SysFreeString(Assign(filelocLeft.filepath,
							piHandler->GetDefaultName(m_hWnd, filelocLeft.filepath.c_str())));
						filelocLeft.filepath.insert(0U, 1, _T('\\'));
						filelocLeft.filepath.insert(0U, path);
					} while (piHandler = ArchiveGuessFormat(filelocLeft.filepath.c_str()));
					filelocLeft.filepath = path;
				}
			}
			else if (dwRightFlags & FFILEOPEN_DETECTZIP)
			{
				if (Merge7z::Format *piHandler = ArchiveGuessFormat(filelocRight.filepath.c_str()))
				{
					pTempPathContext = new CTempPathContext;
					String path = GetClearTempPath(pTempPathContext, _T("1"));
					pTempPathContext->m_strLeftDisplayRoot = filelocLeft.filepath;
					pTempPathContext->m_strRightDisplayRoot = filelocRight.filepath;
					pathsType = IS_EXISTING_DIR;
					do
					{
						if FAILED(piHandler->DeCompressArchive(m_hWnd, filelocRight.filepath.c_str(), path.c_str()))
							break;
						if (filelocRight.filepath.find(path) == 0)
						{
							VERIFY(::DeleteFile(filelocRight.filepath.c_str()) ||
								LogFile.Write(LogFile.LERROR|LogFile.LOSERROR,
								_T("DeleteFile(%s) failed: "), filelocRight.filepath.c_str()));
						}
						SysFreeString(Assign(filelocRight.filepath,
							piHandler->GetDefaultName(m_hWnd, filelocRight.filepath.c_str())));
						filelocRight.filepath.insert(0U, 1, _T('\\'));
						filelocRight.filepath.insert(0U, path);
					} while (piHandler = ArchiveGuessFormat(filelocRight.filepath.c_str()));
					filelocRight.filepath = path;
				}
			}
		}
		catch (OException *e)
		{
			e->ReportError(m_hWnd, MB_ICONSTOP);
			delete e;
		}
		catch (C7ZipMismatchException *e)
		{
			e->ReportError(m_hWnd, MB_ICONSTOP);
			delete e;
		}
	}
	// Determine if we want new a dirview open now that we know if it was
	// and archive. Don't open new dirview if we are comparing files.
	// open the diff
	if (pathsType == IS_EXISTING_DIR)
	{
		// TODO: Is there a good point in closing merge docs in this place?
		if (pDirDoc && !pDirDoc->CloseMergeDocs())
			return false;
		if (pDirDoc != NULL || (pDirDoc = GetDirDocToShow()) != NULL)
		{
			// Anything that can go wrong inside InitCompare() will yield an
			// exception. There is no point in checking return value.
			pDirDoc->InitCompare(
				filelocLeft.filepath.c_str(), filelocRight.filepath.c_str(),
				bRecurse, pTempPathContext);
			LogFile.Write(CLogFile::LNOTICE, _T("Open dirs: Left: %s\n\tRight: %s."),
				filelocLeft.filepath.c_str(), filelocRight.filepath.c_str());

			pDirDoc->SetLeftReadOnly(bROLeft);
			pDirDoc->SetRightReadOnly(bRORight);
			pDirDoc->SetDescriptions(filelocLeft.description, filelocRight.description);
			pDirDoc->ActivateFrame();

			pDirDoc->Rescan();
		}
	}
	else
	{
		if (!COptionsMgr::Get(OPT_MULTIDOC_MERGEDOCS))
		{
			if (!pDirDoc->CloseMergeDocs())
				return false;
		}
		LogFile.Write(CLogFile::LNOTICE, _T("Open files: Left: %s\n\tRight: %s."),
			filelocLeft.filepath.c_str(), filelocRight.filepath.c_str());
		ShowMergeDoc(pDirDoc, filelocLeft, filelocRight, dwLeftFlags, dwRightFlags);
	}
	return true;
}

/**
 * @brief Creates backup before file is saved or copied over.
 * This function handles formatting correct path and filename for
 * backup file. Formatting is done based on several options available
 * for users in Options/Backups dialog. After path is formatted, file
 * is simply just copied. Not much error checking, just if copying
 * succeeded or failed.
 * @param [in] bFolder Are we creating backup in folder compare?
 * @param [in] pszPath Full path to file to backup.
 * @return TRUE if backup succeeds, or isn't just done.
 */
BOOL CMainFrame::CreateBackup(BOOL bFolder, LPCTSTR pszPath)
{
	// If user doesn't want backups in given context, return success
	// so operations don't abort.
	if (!COptionsMgr::Get(bFolder ? OPT_BACKUP_FOLDERCMP : OPT_BACKUP_FILECMP))
		return TRUE;

	// create backup copy of file if destination file exists
	if (!paths_DoesPathExist(pszPath) == IS_EXISTING_FILE)
		return TRUE;

	String bakPath;
	String path;
	String filename;
	String ext;

	SplitFilename(pszPath, &path, &filename, &ext);

	// Determine backup folder
	switch (COptionsMgr::Get(OPT_BACKUP_LOCATION))
	{
	case PropBackups::FOLDER_GLOBAL: 
		// Put backups to global folder defined in options
		bakPath = COptionsMgr::Get(OPT_BACKUP_GLOBALFOLDER);
		if (!bakPath.empty())
			break;
		// fall through
	case PropBackups::FOLDER_ORIGINAL:
		// Put backups to same folder than original file
		bakPath = path;
		break;
	default:
		ASSERT(FALSE);
	}

	if (COptionsMgr::Get(OPT_BACKUP_ADD_BAK))
	{
		// Don't add dot if there is no existing extension
		if (ext.size() > 0)
			ext += _T(".");
		ext += BACKUP_FILE_EXT;
	}

	// Append time to filename if wanted so
	// NOTE just adds timestamp at the moment as I couldn't figure out
	// nice way to add a real time (invalid chars etc).
	if (COptionsMgr::Get(OPT_BACKUP_ADD_TIME))
	{
		time_t curtime = 0;
		time(&curtime);
		TCHAR timestr[24];
		filename += _T("-");
		filename += _ui64tot(curtime, timestr, 10);
	}
	filename += _T(".");
	filename += ext;
	// Append filename and extension (+ optional .bak) to path
	bakPath = paths_ConcatPath(bakPath, filename);

	if (!CopyFile(pszPath, bakPath.c_str(), FALSE))
	{
		int response = LanguageSelect.MsgBox(IDS_BACKUP_FAILED_PROMPT, pszPath,
			MB_YESNO | MB_ICONWARNING | MB_DONT_ASK_AGAIN);
		if (response != IDYES)
			return FALSE;
	}
	return TRUE;
}

/**
 * @brief Sync file to Version Control System
 * @param pszDest [in] Where to copy (incl. filename)
 * @param bApplyToAll [in,out] Apply user selection to all items
 * @param psError [out] Error string that can be shown to user in caller func
 * @return User selection or -1 if error happened
 * @sa CMainFrame::HandleReadonlySave()
 * @sa CDirView::PerformActionList()
 */
int CMainFrame::SyncFileToVCS(LPCTSTR pszDest, BOOL &bApplyToAll,
	String *psError)
{
	String strSavePath(pszDest);
	int nVerSys = 0;

	nVerSys = COptionsMgr::Get(OPT_VCS_SYSTEM);
	
	int nRetVal = HandleReadonlySave(strSavePath, TRUE, bApplyToAll);
	if (nRetVal == IDCANCEL || nRetVal == IDNO)
		return nRetVal;
	
	// If VC project opened from VSS sync and version control used
	if ((nVerSys == VCS_VSS4 || nVerSys == VCS_VSS5) && m_bVCProjSync)
	{
		if (!m_vssHelper.ReLinkVCProj(strSavePath.c_str(), psError))
			nRetVal = -1;
	}
	return nRetVal;
}

void CMainFrame::UpdateDirViewFont()
{
	// Update document fonts
	if (HFont *pOldFont = CDirView::ReplaceFont(&m_lfDir))
	{
		HWindow *pMDIClient = reinterpret_cast<HWindow *>(m_pWndMDIClient);
		HWindow *pChild = NULL;
		while ((pChild = pMDIClient->FindWindowEx(pChild, WinMergeWindowClass)) != NULL)
		{
			CDocFrame *pAbstract = static_cast<CDocFrame *>(CDocFrame::FromHandle(pChild));
			if (pAbstract->GetFrameType() == FRAME_FOLDER)
			{
				CDirFrame *pSpecific = static_cast<CDirFrame *>(pAbstract);
				pSpecific->m_pDirView->UpdateFont();
			}
		}
		pOldFont->DeleteObject();
	}
}

void CMainFrame::UpdateMrgViewFont()
{
	// Update document fonts
	HWindow *pMDIClient = reinterpret_cast<HWindow *>(m_pWndMDIClient);
	HWindow *pChild = NULL;
	while ((pChild = pMDIClient->FindWindowEx(pChild, WinMergeWindowClass)) != NULL)
	{
		CDocFrame *pAbstract = static_cast<CDocFrame *>(CDocFrame::FromHandle(pChild));
		if (pAbstract->GetFrameType() == FRAME_FILE)
		{
			CChildFrame *pSpecific = static_cast<CChildFrame *>(pAbstract);
			if (CMergeEditView *pView = pSpecific->GetLeftView())
			{
				pView->SetFont(m_lfDiff);
				if (CMergeDiffDetailView *pView = pSpecific->GetLeftDetailView())
				{
					pView->SetFont(m_lfDiff);
				}
			}
			if (CMergeEditView *pView = pSpecific->GetRightView())
			{
				pView->SetFont(m_lfDiff);
				if (CMergeDiffDetailView *pView = pSpecific->GetRightDetailView())
				{
					pView->SetFont(m_lfDiff);
				}
			}
		}
	}
}

/**
 * @brief Select font for Merge/Dir view
 * 
 * Shows font selection dialog to user, sets current font and saves
 * selected font properties to registry. Selects fon type to active
 * view (Merge/Dir compare). If there is no open views, then font
 * is selected for Merge view (for example user may want to change to
 * unicode font before comparing files).
 */
void CMainFrame::OnViewSelectfont() 
{
	CHOOSEFONT cf;
	ZeroMemory(&cf, sizeof cf);
	cf.lStructSize = sizeof cf;
	cf.Flags = CF_INITTOLOGFONTSTRUCT | CF_FORCEFONTEXIST | CF_SCREENFONTS;
	cf.hwndOwner = m_hWnd;
	switch (GetActiveDocFrame()->GetFrameType())
	{
	case FRAME_FOLDER:
		cf.lpLogFont = &m_lfDir;
		if (ChooseFont(&cf))
		{
			COptionsMgr::SaveOption(OPT_FONT_DIRCMP_LOGFONT, m_lfDir);
			UpdateDirViewFont();
		}
		break;
	case FRAME_FILE:
	default:
		cf.Flags |= CF_FIXEDPITCHONLY; // Only fixed-width fonts for merge view
		cf.lpLogFont = &m_lfDiff;
		if (ChooseFont(&cf))
		{
			COptionsMgr::SaveOption(OPT_FONT_FILECMP_LOGFONT, m_lfDiff);
			UpdateMrgViewFont();
		}
		break;
	}
}

/**
 * @brief Selects font for Merge view.
 *
 * Cheks if default font or user-selected font should be used in
 * Merge or dir -view and sets correct font properties. Loads user-selected
 * font properties from registry if needed.
 */
void CMainFrame::GetMrgViewFontProperties()
{
	m_lfDiff = COptionsMgr::Get(OPT_FONT_FILECMP_LOGFONT);
	if (OPT_FONT_FILECMP_LOGFONT.IsDefault())
	{
		m_lfDiff.lfHeight = -16;
		m_lfDiff.lfWidth = 0;
		m_lfDiff.lfEscapement = 0;
		m_lfDiff.lfOrientation = 0;
		m_lfDiff.lfWeight = FW_NORMAL;
		m_lfDiff.lfItalic = 0;
		m_lfDiff.lfUnderline = 0;
		m_lfDiff.lfStrikeOut = 0;
		m_lfDiff.lfCharSet = ANSI_CHARSET;
		m_lfDiff.lfOutPrecision = OUT_STRING_PRECIS;
		m_lfDiff.lfClipPrecision = CLIP_STROKE_PRECIS;
		m_lfDiff.lfQuality = DRAFT_QUALITY;
		m_lfDiff.lfPitchAndFamily = FF_MODERN | FIXED_PITCH;
		lstrcpy(m_lfDiff.lfFaceName, _T("Courier New"));
		// Adjust defaults according to MLANG
		MIMECPINFO cpi;
		ZeroMemory(&cpi, sizeof cpi);
		CMyComPtr<IMultiLanguage> pMLang;
		if (SUCCEEDED(pMLang.CoCreateInstance(CLSID_CMultiLanguage, IID_IMultiLanguage)) &&
			SUCCEEDED(pMLang->GetCodePageInfo(GetACP(), &cpi)))
		{
			m_lfDiff.lfCharSet = cpi.bGDICharset;
			lstrcpyn(m_lfDiff.lfFaceName, cpi.wszFixedWidthFont, LF_FACESIZE);
		}
	}
}

void CMainFrame::GetDirViewFontProperties()
{
	m_lfDir = COptionsMgr::Get(OPT_FONT_DIRCMP_LOGFONT);
}

/**
 * @brief Use default font for active view type
 *
 * Disable user-selected font for active view type (Merge/Dir compare).
 * If there is no open views, then Merge view font is changed.
 */
void CMainFrame::OnViewUsedefaultfont() 
{
	switch (GetActiveDocFrame()->GetFrameType())
	{
	case FRAME_FOLDER:
		OPT_FONT_DIRCMP_LOGFONT.Reset();
		OPT_FONT_DIRCMP_LOGFONT.SaveOption();
		GetDirViewFontProperties();
		UpdateDirViewFont();
		break;
	case FRAME_FILE:
		OPT_FONT_FILECMP_LOGFONT.Reset();
		OPT_FONT_FILECMP_LOGFONT.SaveOption();
		GetMrgViewFontProperties();
		UpdateMrgViewFont();
		break;
	}
}

/**
 * @brief Update any resources necessary after a GUI language change
 */
void CMainFrame::UpdateResources()
{
	m_wndStatusBar->SetPartText(0, LanguageSelect.LoadString(IDS_IDLEMESSAGE).c_str());
}

BOOL CMainFrame::IsComparing()
{
	return waitStatusCursor.GetMsgId() == IDS_STATUS_RESCANNING;
}

/**
 * @brief Open file, if it exists, else open url
 */
void CMainFrame::OpenFileOrUrl(LPCTSTR szFile, LPCTSTR szUrl)
{
	if (paths_DoesPathExist(szFile) == IS_EXISTING_FILE)
		ShellExecute(m_hWnd, _T("open"), _T("notepad.exe"), szFile, NULL, SW_SHOWNORMAL);
	else
		ShellExecute(NULL, _T("open"), szUrl, NULL, NULL, SW_SHOWNORMAL);
}

/**
 * @brief Open WinMerge help.
 *
 * If local HTMLhelp file is found, open it, otherwise open HTML page from web.
 */
void CMainFrame::OnHelpContents()
{
	String sPath = GetModulePath(0) + DocsPath;
	if (paths_DoesPathExist(sPath.c_str()) == IS_EXISTING_FILE)
		::HtmlHelp(NULL, sPath.c_str(), HH_DISPLAY_TOC, NULL);
	else
		ShellExecute(NULL, _T("open"), DocsURL, NULL, NULL, SW_SHOWNORMAL);
}

void CMainFrame::InitialActivate(int nCmdShow) 
{
	CWindowPlacement wp;
	CRegKeyEx rk = SettingStore.GetSectionKey(_T("Settings"));
	if (!wp.RegQuery(rk.GetKey(), _T("MainWindowPlacement")))
		GetWindowPlacement(&wp);
	if (nCmdShow == SW_MINIMIZE)
		wp.showCmd = nCmdShow;
	// Ensure top-left corner is on visible area,
	// 20 points margin is added to prevent "lost" window
	RECT dsk_rc =
	{
		::GetSystemMetrics(SM_XVIRTUALSCREEN) - 20,
		::GetSystemMetrics(SM_YVIRTUALSCREEN) - 20,
		dsk_rc.left + ::GetSystemMetrics(SM_CXVIRTUALSCREEN),
		dsk_rc.top + ::GetSystemMetrics(SM_CYVIRTUALSCREEN)
	};
	if (!::IsRectEmpty(&wp.rcNormalPosition) &&
		::PtInRect(&dsk_rc, reinterpret_cast<POINT &>(wp.rcNormalPosition)))
	{
		SetWindowPlacement(&wp);
	}
	else
	{
		ShowWindow(nCmdShow);
	}
	SetStatus(0U);
}

void CMainFrame::EnableModeless(BOOL bEnable)
{
	// Modal operation prevents switching between windows, and opening new ones.
	m_wndTabBar->EnableWindow(bEnable);
	m_wndToolBar->EnableButton(ID_FILE_NEW, bEnable);
	m_wndToolBar->EnableButton(ID_FILE_OPEN, bEnable);
}

void CMainFrame::SaveWindowPlacement()
{
	// save main window position
	CWindowPlacement wp;
	if (GetWindowPlacement(&wp))
	{
		CRegKeyEx rk = SettingStore.GetSectionKey(_T("Settings"));
		wp.RegWrite(rk.GetKey(), _T("MainWindowPlacement"));
	}
}

/**
 * @brief Called when mainframe is about to be closed.
 * This function is called when mainframe is to be closed (not for
 * file/compare windows.
 */
bool CMainFrame::PrepareForClosing()
{
	if (theApp.GetActiveOperations())
		return false;

	// Check if there are multiple windows open and ask for closing them
	if (COptionsMgr::Get(OPT_ASK_MULTIWINDOW_CLOSE) && !AskCloseConfirmation())
		return false;

	OnWindowCloseAll();
	if (GetActiveDocFrame())
		return false;

	return true;
}

/**
 * @brief Utility function to update CSuperComboBox format MRU
 */
void CMainFrame::addToMru(LPCTSTR szItem, LPCTSTR szRegSubKey, UINT nMaxItems)
{
	if (HKEY hKey = SettingStore.GetSectionKey(szRegSubKey))
	{
		DWORD n = 0;
		DWORD bufsize = 0;
		RegQueryInfoKey(hKey, NULL, NULL, NULL, NULL, NULL, NULL, &n, NULL, &bufsize, NULL, NULL);
		if (n > nMaxItems)
			n = nMaxItems;
		TCHAR *const s = reinterpret_cast<TCHAR *>(_alloca(bufsize));
		String strItem = szItem;
		szItem = paths_UndoMagic(&strItem.front());
		UINT i = n;
		// if item is already in MRU, exclude subsequent items from rotation
		while (i != 0)
		{
			TCHAR name[20];
			wsprintf(name, _T("Item_%d"), --i);
			DWORD cb = bufsize;
			DWORD type = REG_NONE;
			if (RegQueryValueEx(hKey, name, NULL, &type,
				reinterpret_cast<BYTE *>(s), &cb) == ERROR_SUCCESS &&
				_tcscmp(s, szItem) == 0)
			{
				n = i;
			}
		}
		// move items down a step
		for (i = n ; i != 0 ; --i)
		{
			TCHAR name[20];
			wsprintf(name, _T("Item_%d"), i - 1);
			DWORD cb = bufsize;
			DWORD type = REG_NONE;
			if (RegQueryValueEx(hKey, name, NULL, &type,
				reinterpret_cast<BYTE *>(s), &cb) == ERROR_SUCCESS)
			{
				wsprintf(name, _T("Item_%d"), i);
				RegSetValueEx(hKey, name, 0L, REG_SZ,
					reinterpret_cast<const BYTE *>(s), cb);
			}
		}
		// add most recent item
		RegSetValueEx(hKey, _T("Item_0"), 0L, REG_SZ,
			reinterpret_cast<const BYTE *>(szItem),
			(_tcslen(szItem) + 1) * sizeof(TCHAR));
	}
}

/**
 * @brief Apply tabs and eols settings to all merge documents
 */
void CMainFrame::ApplyViewWhitespace() 
{
	HWindow *pMDIClient = reinterpret_cast<HWindow *>(m_pWndMDIClient);
	HWindow *pChild = NULL;
	bool opt_view_whitespace = COptionsMgr::Get(OPT_VIEW_WHITESPACE);
	bool opt_allow_mixed_eol = COptionsMgr::Get(OPT_ALLOW_MIXED_EOL);
	while ((pChild = pMDIClient->FindWindowEx(pChild, WinMergeWindowClass)) != NULL)
	{
		CDocFrame *pAbstract = static_cast<CDocFrame *>(CDocFrame::FromHandle(pChild));
		if (pAbstract->GetFrameType() == FRAME_FILE)
		{
			CChildFrame *pSpecific = static_cast<CChildFrame *>(pAbstract);
			if (CMergeEditView *pView = pSpecific->GetLeftView())
			{
				bool view_eols = opt_allow_mixed_eol || pView->LocateTextBuffer()->IsMixedEOL();
				pView->SetViewTabs(opt_view_whitespace);
				pView->SetViewEols(opt_view_whitespace, view_eols);
				if (CMergeDiffDetailView *pView = pSpecific->GetLeftDetailView())
				{
					pView->SetViewTabs(opt_view_whitespace);
					pView->SetViewEols(opt_view_whitespace, view_eols);
				}
			}
			if (CMergeEditView *pView = pSpecific->GetRightView())
			{
				bool view_eols = opt_allow_mixed_eol || pView->LocateTextBuffer()->IsMixedEOL();
				pView->SetViewTabs(opt_view_whitespace);
				pView->SetViewEols(opt_view_whitespace, view_eols);
				if (CMergeDiffDetailView *pView = pSpecific->GetRightDetailView())
				{
					pView->SetViewTabs(opt_view_whitespace);
					pView->SetViewEols(opt_view_whitespace, view_eols);
				}
			}
		}
	}
}

void CMainFrame::OnViewWhitespace() 
{
	bool bViewWhitespace = COptionsMgr::Get(OPT_VIEW_WHITESPACE);
	COptionsMgr::SaveOption(OPT_VIEW_WHITESPACE, !bViewWhitespace);
	ApplyViewWhitespace();
}

CDocFrame *CMainFrame::FindFrameOfType(FRAMETYPE frameType)
{
	HWindow *pMDIClient = reinterpret_cast<HWindow *>(m_pWndMDIClient);
	HWindow *pChild = NULL;
	while ((pChild = pMDIClient->FindWindowEx(pChild, WinMergeWindowClass)) != NULL)
	{
		CDocFrame *pDocFrame = static_cast<CDocFrame *>(CDocFrame::FromHandle(pChild));
		if (pDocFrame->GetFrameType() == frameType)
			return pDocFrame;
	}
	return NULL;
}

HWindow *CMainFrame::CreateChildHandle() const
{
	MDICREATESTRUCT mcs;
	mcs.szClass = WinMergeWindowClass;
	mcs.szTitle = NULL;
	mcs.hOwner = GetModuleHandle(0);
	mcs.x = CW_USEDEFAULT;
	mcs.y = CW_USEDEFAULT;
	mcs.cx = CW_USEDEFAULT;
	mcs.cy = CW_USEDEFAULT;
	mcs.style = WS_OVERLAPPEDWINDOW | WS_CHILD | WS_CLIPCHILDREN;
	mcs.lParam = 0;
	return reinterpret_cast<HWindow *>(
		m_pWndMDIClient->SendMessage(WM_MDICREATE, 0, (LPARAM)&mcs));
}

/**
 * @brief Obtain a merge doc to display a difference in files.
 * This function (usually) uses DirDoc to determine if new or existing
 * MergeDoc is used. However there is exceptional case when DirDoc does
 * not contain diffs. Then we have only file compare, and if we also have
 * limited file compare windows, we always reuse existing MergeDoc.
 * @param [in] pDirDoc Dir compare document.
 * @param [out] pNew Did we create a new document?
 * @return Pointer to merge document.
 */
CChildFrame *CMainFrame::GetMergeDocToShow(CDirFrame *pDirDoc)
{
	if (pDirDoc)
	{
		return pDirDoc->GetMergeDocForDiff();
	}
	if (!COptionsMgr::Get(OPT_MULTIDOC_MERGEDOCS))
	{
		if (CDocFrame *pFrame = FindFrameOfType(FRAME_FILE))
		{
			CChildFrame *pDoc = static_cast<CChildFrame *>(pFrame);
			if (pDoc->SaveModified())
				pFrame->SendMessage(WM_CLOSE);
		}
	}
	// Create a new merge doc
	return new CChildFrame(GetMainFrame());
}

/**
 * @brief Obtain a hex merge doc to display a difference in files.
 * This function (usually) uses DirDoc to determine if new or existing
 * MergeDoc is used. However there is exceptional case when DirDoc does
 * not contain diffs. Then we have only file compare, and if we also have
 * limited file compare windows, we always reuse existing MergeDoc.
 * @param [in] pDirDoc Dir compare document.
 * @param [out] pNew Did we create a new document?
 * @return Pointer to merge document.
 */
CHexMergeFrame *CMainFrame::GetHexMergeDocToShow(CDirFrame *pDirDoc)
{
	if (pDirDoc)
	{
		return pDirDoc->GetHexMergeDocForDiff();
	}
	if (!COptionsMgr::Get(OPT_MULTIDOC_MERGEDOCS))
	{
		if (CDocFrame *pFrame = FindFrameOfType(FRAME_BINARY))
		{
			CHexMergeFrame *pDoc = static_cast<CHexMergeFrame *>(pFrame);
			if (pDoc->SaveModified())
				pFrame->SendMessage(WM_CLOSE);
		}
	}
	// Create a new merge doc
	return new CHexMergeFrame(GetMainFrame());
}

/// Get pointer to a dir doc for displaying a scan
CDirFrame *CMainFrame::GetDirDocToShow()
{
	if (!COptionsMgr::Get(OPT_MULTIDOC_DIRDOCS))
	{
		if (CDocFrame *pFrame = FindFrameOfType(FRAME_FOLDER))
		{
			CDirFrame *pDoc = static_cast<CDirFrame *>(pFrame);
			if (!pDoc->CloseMergeDocs())
				return NULL;
			pFrame->SendMessage(WM_CLOSE);
		}
	}
	return new CDirFrame(this);
}

// Set status in the main status pane
void CMainFrame::SetStatus(UINT status)
{
	SetStatus(LanguageSelect.LoadString(status ? status : IDS_IDLEMESSAGE).c_str());
}

void CMainFrame::SetStatus(LPCTSTR status)
{
	m_wndStatusBar->SetPartText(0, status);
}

void CMainFrame::UpdateIndicators()
{
	if (m_wndStatusBar == NULL)
		return;
	if (UINT key = MapVirtualKey(VK_CAPITAL, MAPVK_VK_TO_VSC))
	{
		TCHAR text[80];
		text
		[
			GetKeyState(VK_CAPITAL) & 1 ?
			GetKeyNameText(key << 16, text, _countof(text)) :
			0
		] = '\0';
		m_wndStatusBar->SetPartText(3, text);
	}
	if (UINT key = MapVirtualKey(VK_NUMLOCK, MAPVK_VK_TO_VSC))
	{
		TCHAR text[80];
		text
		[
			GetKeyState(VK_NUMLOCK) & 1 ?
			GetKeyNameText((key | 0x100) << 16, text, _countof(text)) :
			0
		] = '\0';
		m_wndStatusBar->SetPartText(4, text);
	}
}

/**
 * @brief Generate patch from files selected.
 *
 * Creates a patch from selected files in active directory compare, or
 * active file compare. Files in file compare must be saved before
 * creating a patch.
 */
void CMainFrame::OnToolsGeneratePatch()
{
	CPatchTool patcher;
	CDocFrame *const pFrame = GetActiveDocFrame();
	FRAMETYPE frame = pFrame->GetFrameType();
	// Mergedoc active?
	if (frame == FRAME_FILE)
	{
		CChildFrame *pDoc = static_cast<CChildFrame *>(pFrame);
		// If there are changes in files, tell user to save them first
		if (pDoc->m_ptBuf[0]->IsModified() || pDoc->m_ptBuf[1]->IsModified())
		{
			LanguageSelect.MsgBox(IDS_SAVEFILES_FORPATCH, MB_ICONSTOP);
			return;
		}
		patcher.AddFiles(
			pDoc->m_strPath[0].c_str(), pDoc->m_wndFilePathBar.GetTitle(0).c_str(),
			pDoc->m_strPath[1].c_str(), pDoc->m_wndFilePathBar.GetTitle(1).c_str());
	}
	// Dirview active
	else if (frame == FRAME_FOLDER)
	{
		CDirFrame *pDoc = static_cast<CDirFrame *>(pFrame);
		CDirView *pView = pDoc->m_pDirView;

		// Get selected items from folder compare
		BOOL bValidFiles = TRUE;
		int ind = pView->GetFirstSelectedInd();
		while (ind != -1 && bValidFiles)
		{
			const DIFFITEM &item = pView->GetItemAt(ind);
			if (item.diffcode.isBin())
			{
				LanguageSelect.MsgBox(IDS_CANNOT_CREATE_BINARYPATCH, MB_ICONWARNING |
					MB_DONT_DISPLAY_AGAIN);
				bValidFiles = FALSE;
			}
			else if (item.diffcode.isDirectory())
			{
				LanguageSelect.MsgBox(IDS_CANNOT_CREATE_DIRPATCH, MB_ICONWARNING |
					MB_DONT_DISPLAY_AGAIN);
				bValidFiles = FALSE;
			}

			if (bValidFiles)
			{
				// Format full paths to files (leftFile/rightFile)
				String leftFile = item.GetLeftFilepath(pDoc->GetLeftBasePath());
				if (!leftFile.empty())
					leftFile = paths_ConcatPath(leftFile, item.left.filename);
				String rightFile = item.GetRightFilepath(pDoc->GetRightBasePath());
				if (!rightFile.empty())
					rightFile = paths_ConcatPath(rightFile, item.right.filename);

				// Format relative paths to files in folder compare
				String leftpatch = item.left.path;
				if (!leftpatch.empty())
					leftpatch += _T('/');
				leftpatch += item.left.filename;
				String rightpatch = item.right.path;
				if (!rightpatch.empty())
					rightpatch += _T('/');
				rightpatch += item.right.filename;
				patcher.AddFiles(
					leftFile.c_str(), leftpatch.c_str(),
					rightFile.c_str(), rightpatch.c_str());
				pView->GetNextSelectedInd(ind);
			}
		}
	}
	if (patcher.CreatePatch() && patcher.GetOpenToEditor())
		OpenFileToExternalEditor(patcher.GetPatchFile().c_str());
}

/////////////////////////////////////////////////////////////////////////////
//
//	OnDropFiles code from CDropEdit
//	Copyright 1997 Chris Losinger
//
//	shortcut expansion code modified from :
//	CShortcut, 1996 Rob Warner
//
template<>
LRESULT CMainFrame::OnWndMsg<WM_DROPFILES>(WPARAM wParam, LPARAM)
{
	HDROP dropInfo = reinterpret_cast<HDROP>(wParam);
	// Get the number of pathnames that have been dropped
	UINT fileCount = DragQueryFile(dropInfo, 0xFFFFFFFF, NULL, 0);
	if (fileCount > 2)
		fileCount = 2;
	stl::vector<String> files(2);
	UINT i;
	// get all file names. but we'll only need the first one.
	for (i = 0; i < fileCount; i++)
	{
		// Get the number of bytes required by the file's full pathname
		if (UINT len = DragQueryFile(dropInfo, i, NULL, 0))
		{
			files[i].resize(len);
			DragQueryFile(dropInfo, i, &files[i].front(), len + 1);
		}
	}
	// Free the memory block containing the dropped-file information
	DragFinish(dropInfo);

	for (i = 0; i < fileCount; i++)
	{
		if (paths_IsShortcut(files[i].c_str()))
		{
			// if this was a shortcut, we need to expand it to the target path
			String expandedFile = ExpandShortcut(files[i].c_str());
			// if that worked, we should have a real file name
			if (!expandedFile.empty())
				files[i] = expandedFile;
		}
	}

	// If Ctrl pressed, do recursive compare
	BOOL ctrlKey = ::GetAsyncKeyState(VK_CONTROL);

	// If user has <Shift> pressed with one file selected,
	// assume it is an archive and set filenames to same
	if (::GetAsyncKeyState(VK_SHIFT) < 0 && fileCount == 1)
	{
		files[1] = files[0];
	}

	LogFile.Write(CLogFile::LNOTICE, _T("D&D open: Left: %s\n\tRight: %s."),
		files[0].c_str(), files[1].c_str());

	// Check if they dropped a project file
	if (fileCount == 1)
	{
		if (ProjectFile::IsProjectFile(files[0].c_str()))
		{
			LoadAndOpenProjectFile(files[0].c_str());
			return 0;
		}
		if (IsConflictFile(files[0].c_str()))
		{
			DoOpenConflict(files[0].c_str());
			return 0;
		}
	}

	FileLocation filelocLeft, filelocRight;
	filelocLeft.filepath = files[0];
	filelocRight.filepath = files[1];
	DoFileOpen(filelocLeft, filelocRight, FFILEOPEN_DETECT, FFILEOPEN_DETECT, ctrlKey);
	return 0;
}

/**
 * @brief Open given file to external editor specified in options.
 * @param [in] file Full path to file to open.
 */
void CMainFrame::OpenFileToExternalEditor(LPCTSTR file)
{
	String sExtEditor = COptionsMgr::Get(OPT_EXT_EDITOR_CMD);
	String sCmd, sExecutable;
	GetDecoratedCmdLine(sExtEditor, sCmd, sExecutable);
	if (paths_PathIsExe(sExecutable.c_str()))
	{
		sCmd += _T(" \"");
		sCmd += file;
		sCmd += _T("\"");

		STARTUPINFO startupInfo;
		ZeroMemory(&startupInfo, sizeof startupInfo);
		startupInfo.cb = sizeof startupInfo;
		PROCESS_INFORMATION processInfo;

		BOOL retVal = CreateProcess(NULL, const_cast<LPTSTR>(sCmd.c_str()),
			NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL,
			&startupInfo, &processInfo);

		if (!retVal)
		{
			// Error invoking external editor
			LanguageSelect.FormatMessage(IDS_ERROR_EXECUTE_FILE, sExtEditor.c_str()).MsgBox(MB_ICONSTOP);
		}
	}
	else
	{
		// Don't know how to invoke external editor (it doesn't end with
		// an obvious executable extension)
		LanguageSelect.FormatMessage(IDS_UNKNOWN_EXECUTE_FILE, sExtEditor.c_str()).MsgBox(MB_ICONSTOP);
	}
}

void CMainFrame::OpenFileWith(LPCTSTR file) const
{
	if (HINSTANCE hinst = LoadLibrary(_T("shell32")))
	{
		typedef void (CALLBACK*EntryPoint)(HWND, HINSTANCE, char *, int);
		if (FARPROC fp = GetProcAddress(hinst, "OpenAs_RunDLL"))
		{
			if (HString *cmd = HString::Uni(file)->Oct())
			{
				reinterpret_cast<EntryPoint>(fp)(m_hWnd, hinst, cmd->A, SW_SHOWNORMAL);
				cmd->Free();
			}
		}
		FreeLibrary(hinst);
	}
}

typedef enum { ToConfigLog, FromConfigLog } ConfigLogDirection;

/**
 * @brief Copy one piece of data from options object to config log, or vice-versa
 */
static void LoadConfigIntSetting(int &cfgval, COptionDef<int> &name, ConfigLogDirection cfgdir)
{
	if (cfgdir == ToConfigLog)
		cfgval = COptionsMgr::Get(name);
	else
		COptionsMgr::Set(name, cfgval);
}

/**
 * @brief Copy one piece of data from options object to config log, or vice-versa
 */
static void LoadConfigBoolSetting(bool &cfgval, COptionDef<bool> &name, ConfigLogDirection cfgdir)
{
	if (cfgdir == ToConfigLog)
		cfgval = COptionsMgr::Get(name);
	else
		COptionsMgr::Set(name, cfgval);
}

/**
 * @brief Pass options settings from options manager object to config log, or vice-versa
 */
static void LoadConfigLog(CConfigLog &configLog, LOGFONT &lfDiff, ConfigLogDirection cfgdir)
{
	LoadConfigIntSetting(configLog.m_diffOptions.nIgnoreWhitespace, OPT_CMP_IGNORE_WHITESPACE, cfgdir);
	LoadConfigBoolSetting(configLog.m_diffOptions.bIgnoreBlankLines, OPT_CMP_IGNORE_BLANKLINES, cfgdir);
	LoadConfigBoolSetting(configLog.m_diffOptions.bFilterCommentsLines, OPT_CMP_FILTER_COMMENTLINES, cfgdir);
	LoadConfigBoolSetting(configLog.m_diffOptions.bIgnoreCase, OPT_CMP_IGNORE_CASE, cfgdir);
	LoadConfigBoolSetting(configLog.m_diffOptions.bIgnoreEol, OPT_CMP_IGNORE_EOL, cfgdir);
	LoadConfigIntSetting(configLog.m_compareSettings.nCompareMethod, OPT_CMP_METHOD, cfgdir);
	LoadConfigBoolSetting(configLog.m_compareSettings.bStopAfterFirst, OPT_CMP_STOP_AFTER_FIRST, cfgdir);

	LoadConfigBoolSetting(configLog.m_viewSettings.bShowIdent, OPT_SHOW_IDENTICAL, cfgdir);
	LoadConfigBoolSetting(configLog.m_viewSettings.bShowDiff, OPT_SHOW_DIFFERENT, cfgdir);
	LoadConfigBoolSetting(configLog.m_viewSettings.bShowUniqueLeft, OPT_SHOW_UNIQUE_LEFT, cfgdir);
	LoadConfigBoolSetting(configLog.m_viewSettings.bShowUniqueRight, OPT_SHOW_UNIQUE_RIGHT, cfgdir);
	LoadConfigBoolSetting(configLog.m_viewSettings.bShowBinaries, OPT_SHOW_BINARIES, cfgdir);
	LoadConfigBoolSetting(configLog.m_viewSettings.bShowSkipped, OPT_SHOW_SKIPPED, cfgdir);
	LoadConfigBoolSetting(configLog.m_viewSettings.bTreeView, OPT_TREE_MODE, cfgdir);

	LoadConfigBoolSetting(configLog.m_miscSettings.bPreserveFiletimes, OPT_PRESERVE_FILETIMES, cfgdir);

	LoadConfigBoolSetting(configLog.m_miscSettings.bAutomaticRescan, OPT_AUTOMATIC_RESCAN, cfgdir);
	LoadConfigBoolSetting(configLog.m_miscSettings.bAllowMixedEol, OPT_ALLOW_MIXED_EOL, cfgdir);
	LoadConfigBoolSetting(configLog.m_miscSettings.bScrollToFirst, OPT_SCROLL_TO_FIRST, cfgdir);
	LoadConfigBoolSetting(configLog.m_miscSettings.bViewWhitespace, OPT_VIEW_WHITESPACE, cfgdir);
	LoadConfigBoolSetting(configLog.m_miscSettings.bMovedBlocks, OPT_CMP_MOVED_BLOCKS, cfgdir);
	LoadConfigBoolSetting(configLog.m_miscSettings.bShowLinenumbers, OPT_VIEW_LINENUMBERS, cfgdir);
	LoadConfigBoolSetting(configLog.m_miscSettings.bWrapLines, OPT_WORDWRAP, cfgdir);
	LoadConfigBoolSetting(configLog.m_miscSettings.bMergeMode, OPT_MERGE_MODE, cfgdir);
	LoadConfigBoolSetting(configLog.m_miscSettings.bSyntaxHighlight, OPT_SYNTAX_HIGHLIGHT, cfgdir);
	LoadConfigIntSetting(configLog.m_miscSettings.nInsertTabs, OPT_TAB_TYPE, cfgdir);
	LoadConfigIntSetting(configLog.m_miscSettings.nTabSize, OPT_TAB_SIZE, cfgdir);
	LoadConfigBoolSetting(configLog.m_miscSettings.bMatchSimilarLines, OPT_CMP_MATCH_SIMILAR_LINES, cfgdir);	

	LoadConfigIntSetting(configLog.m_cpSettings.nDefaultMode, OPT_CP_DEFAULT_MODE, cfgdir);
	LoadConfigIntSetting(configLog.m_cpSettings.nDefaultCustomValue, OPT_CP_DEFAULT_CUSTOM, cfgdir);
	LoadConfigBoolSetting(configLog.m_cpSettings.bDetectCodepage, OPT_CP_DETECT, cfgdir);

	if (cfgdir == ToConfigLog)
	{
		configLog.m_fontSettings.nCharset = lfDiff.lfCharSet;
		configLog.m_fontSettings.sFacename = lfDiff.lfFaceName;
	}
	else
	{
		lfDiff.lfCharSet = configLog.m_fontSettings.nCharset;
		lstrcpyn(lfDiff.lfFaceName, configLog.m_fontSettings.sFacename.c_str(), _countof(lfDiff.lfFaceName));
	}
}

/**
 * @brief Save WinMerge configuration and info to file
 */
void CMainFrame::OnSaveConfigData()
{
	CConfigLog configLog;

	LoadConfigLog(configLog, m_lfDiff, ToConfigLog);

	String sError;
	bool ok = configLog.WriteLogFile(sError);
	String sFileName = configLog.GetFileName();
	if (ok)
	{
		OpenFileToExternalEditor(sFileName.c_str());
	}
	else
	{
		LanguageSelect.FormatMessage(
			IDS_ERROR_FILEOPEN, sFileName.c_str(), sError.c_str()
		).MsgBox(MB_ICONSTOP);
	}
}

/**
 * @brief Open two new empty docs, 'Scratchpads'
 * 
 * Allows user to open two empty docs, to paste text to
 * compare from clipboard.
 * @note File filenames are set emptys and filedescriptors
 * are loaded from resource.
 * @sa CChildFrame::OpenDocs()
 * @sa CChildFrame::TrySaveAs()
 */
void CMainFrame::OnFileNew() 
{
	// If the dirdoc we are supposed to use is busy doing a diff, bail out
	if (IsComparing())
		return;

	// Load emptyfile descriptors and open empty docs
	// Use default codepage
	FileLocation filelocLeft, filelocRight; // empty, unspecified (so default) encoding
	filelocLeft.description = LanguageSelect.LoadString(IDS_EMPTY_LEFT_FILE);
	filelocRight.description = LanguageSelect.LoadString(IDS_EMPTY_RIGHT_FILE);
	ShowMergeDoc(NULL, filelocLeft, filelocRight);
}

/**
 * @brief Open Filters dialog
 */
void CMainFrame::OnToolsFilters()
{
	OPropertySheet sht;
	sht.m_caption = LanguageSelect.LoadString(IDS_FILTER_TITLE);
	sht.m_psh.dwFlags |= PSH_NOAPPLYNOW | PSH_NOCONTEXTHELP;
	LineFiltersDlg lineFiltersDlg(m_pLineFilters.get());
	FileFiltersDlg fileFiltersDlg;
	LineFiltersList lineFilters(m_pLineFilters.get());
	const String origFilter = globalFileFilter.GetFilterNameOrMask();
	if (PROPSHEETPAGE *psp = sht.AddPage(fileFiltersDlg))
	{
		psp->dwFlags = PSP_USETITLE | PSP_USEHICON;
		psp->hInstance = LanguageSelect.FindResourceHandle(fileFiltersDlg.m_idd, RT_DIALOG);
		psp->pszTitle = fileFiltersDlg.m_strCaption.c_str();
		psp->hIcon = LanguageSelect.LoadIcon(IDI_FILEFILTER);
	}
	if (PROPSHEETPAGE *psp = sht.AddPage(lineFiltersDlg))
	{
		psp->dwFlags = PSP_USETITLE | PSP_USEHICON;
		psp->hInstance = LanguageSelect.FindResourceHandle(lineFiltersDlg.m_idd, RT_DIALOG);
		psp->pszTitle = lineFiltersDlg.m_strCaption.c_str();
		psp->hIcon = LanguageSelect.LoadIcon(IDI_LINEFILTER);
	}
	// Make sure all filters are up-to-date
	globalFileFilter.ReloadUpdatedFilters();

	const BOOL lineFiltersEnabledOrig = COptionsMgr::Get(OPT_LINEFILTER_ENABLED);
	lineFiltersDlg.m_bIgnoreRegExp = lineFiltersEnabledOrig;

	if (sht.DoModal(NULL, GetLastActivePopup()->m_hWnd) == IDOK)
	{
		String strNone = LanguageSelect.LoadString(IDS_USERCHOICE_NONE);
		String path = fileFiltersDlg.GetSelected();
		if (path.find(strNone.c_str()) != String::npos)
		{
			// Don't overwrite mask we already have
			if (!globalFileFilter.IsUsingMask())
			{
				String sFilter(_T("*.*"));
				globalFileFilter.SetFilter(sFilter);
				COptionsMgr::SaveOption(OPT_FILEFILTER_CURRENT, sFilter);
			}
		}
		else
		{
			globalFileFilter.SetFileFilterPath(path.c_str());
			globalFileFilter.UseMask(FALSE);
			String sFilter = globalFileFilter.GetFilterNameOrMask();
			COptionsMgr::SaveOption(OPT_FILEFILTER_CURRENT, sFilter);
		}
		BOOL linefiltersEnabled = lineFiltersDlg.m_bIgnoreRegExp;
		COptionsMgr::SaveOption(OPT_LINEFILTER_ENABLED, linefiltersEnabled != FALSE);

		// Save new filters before (possibly) rescanning
		m_pLineFilters->SaveFilters();

		// Check if compare documents need rescanning
		int idRefreshFiles = IDNO;
		int idRefreshFolders = IDNO;
		if (lineFiltersEnabledOrig != linefiltersEnabled || 
			!lineFilters.Compare(m_pLineFilters.get()))
		{
			idRefreshFiles = IDYES; // Start without asking
			idRefreshFolders = 0; // Ask before starting
		}
		else if (origFilter != globalFileFilter.GetFilterNameOrMask())
		{
			idRefreshFolders = 0; // Ask before starting
		}
		HWindow *pChild = NULL;
		while ((pChild = m_pWndMDIClient->FindWindowEx(pChild, WinMergeWindowClass)) != NULL)
		{
			CDocFrame *pDocFrame = static_cast<CDocFrame *>(CDocFrame::FromHandle(pChild));
			switch (pDocFrame->GetFrameType())
			{
			case FRAME_FILE:
				if (idRefreshFiles == IDYES)
					static_cast<CChildFrame *>(pDocFrame)->FlushAndRescan(TRUE);
				break;
			case FRAME_FOLDER:
				if (idRefreshFolders == 0)
					idRefreshFolders = LanguageSelect.MsgBox(IDS_FILTERCHANGED,
						MB_ICONWARNING | MB_YESNO | MB_DONT_ASK_AGAIN);
				if (idRefreshFolders == IDYES)
					static_cast<CDirFrame *>(pDocFrame)->Rescan();
				break;
			}
			// Avoid concurrent DirScan threads for now due to issues...
			idRefreshFolders = IDNO;
		}
	}
}

/**
 * @brief Open Filters dialog.
 */
void CMainFrame::SelectFilter()
{
	OnToolsFilters();
}

/**
 * @brief Closes application with ESC.
 *
 * Application is closed if:
 * - 'Close Windows with ESC' option is enabled and
 *    there is no open document windows
 * - '-e' commandline switch is given
 */
BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
	{
		CDocFrame *const pDocFrame = GetActiveDocFrame();
		if (pDocFrame && pDocFrame->PreTranslateMessage(pMsg))
		{
			return TRUE;
		}
		// Check if we got 'ESC pressed' -message
		if (pMsg->message == WM_KEYDOWN)
		{
			switch (pMsg->wParam)
			{
			case VK_ESCAPE:
				if (m_bEscShutdown)
				{
					PostMessage(WM_SYSCOMMAND, SC_CLOSE);
					return TRUE;
				}
				if (m_bClearCaseTool)
				{
					// Close DocFrame, then move MainFrame out of the way.
					if (pDocFrame)
						pDocFrame->SendMessage(WM_CLOSE);
					if (GetActiveDocFrame() != pDocFrame)
					{
						PostMessage(WM_SYSCOMMAND, SC_PREVWINDOW);
						m_bClearCaseTool = false;
					}
					return TRUE;
				}
				if (COptionsMgr::Get(OPT_CLOSE_WITH_ESC))
				{
					if (pDocFrame)
						pDocFrame->SendMessage(WM_CLOSE);
					else
						PostMessage(WM_SYSCOMMAND, SC_CLOSE);
					return TRUE;
				}
				break;
			case VK_CAPITAL:
			case VK_NUMLOCK:
				UpdateIndicators();
				break;
			}
		}
		if (m_hAccelTable != NULL &&
			::TranslateAccelerator(m_hWnd, m_hAccelTable, pMsg))
		{
			return TRUE;
		}
		if ((pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN) &&
			::TranslateMDISysAccel(m_pWndMDIClient->m_hWnd, pMsg))
		{
			return TRUE;
		}
	}
	return FALSE;
}

/**
 * @brief Shows VSS error from exception and writes log.
 */
void CMainFrame::ShowVSSError(HRESULT hr, LPCTSTR strItem)
{
	String errStr = OException(hr).msg;
	String errMsg = LanguageSelect.LoadString(IDS_VSS_ERRORFROM);
	String logMsg = errMsg;
	errMsg += _T("\n");
	errMsg += errStr;
	logMsg += _T(" ");
	logMsg += errStr;
	if (*strItem)
	{
		errMsg += _T("\n\n");
		errMsg += strItem;
		logMsg += _T(": ");
		logMsg += strItem;
	}
	LogErrorString(logMsg.c_str());
	theApp.DoMessageBox(errMsg.c_str(), MB_ICONSTOP);
}

/**
 * @brief Show Help - this is for opening help from outside mainframe.
 * @param [in] helpLocation Location inside help, if NULL main help is opened.
 */
void CMainFrame::ShowHelp(LPCTSTR helpLocation /*= NULL*/)
{
	if (helpLocation == NULL)
	{
		OnHelpContents();
	}
	else
	{
		String sPath = GetModulePath(0) + DocsPath;
		if (paths_DoesPathExist(sPath.c_str()) == IS_EXISTING_FILE)
		{
			sPath += helpLocation;
			::HtmlHelp(NULL, sPath.c_str(), HH_DISPLAY_TOPIC, NULL);
		}
		else if (LPCTSTR helpTarget = EatPrefix(helpLocation, _T("::/htmlhelp/")))
		{
			string_replace(sPath = DocsURL, _T("index.html"), helpTarget);
			ShellExecute(NULL, _T("open"), sPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
		}
	}
}

/**
 * @brief Tell user why archive support is not available.
 */
void CMainFrame::OnHelpMerge7zmismatch() 
{
	Recall7ZipMismatchError(m_hWnd);
}

/**
 * @brief Show/hide statusbar.
 */
void CMainFrame::OnViewStatusBar()
{
	bool bShow = !COptionsMgr::Get(OPT_SHOW_STATUSBAR);
	COptionsMgr::SaveOption(OPT_SHOW_STATUSBAR, bShow);
	m_wndStatusBar->ShowWindow(bShow ? SW_SHOW : SW_HIDE);
	RecalcLayout();
}

/**
 * @brief Show/hide tabbar.
 */
void CMainFrame::OnViewTabBar()
{
	bool bShow = !COptionsMgr::Get(OPT_SHOW_TABBAR);
	COptionsMgr::SaveOption(OPT_SHOW_TABBAR, bShow);
	m_wndTabBar->ShowWindow(bShow ? SW_SHOW : SW_HIDE);
	RecalcLayout();
}

/**
 * @brief Enable/disable automatic pane resizing.
 */
void CMainFrame::OnResizePanes()
{
	bool bResize = !COptionsMgr::Get(OPT_RESIZE_PANES);
	COptionsMgr::SaveOption(OPT_RESIZE_PANES, bResize);
	if (CDocFrame *const pDocFrame = GetActiveDocFrame())
	{
		RECT rect;
		pDocFrame->m_wndFilePathBar.GetWindowRect(&rect);
		pDocFrame->m_wndFilePathBar.SetWindowPos(NULL, 0, 0,
			rect.right - rect.left, rect.bottom - rect.top,
			SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE |
			SWP_FRAMECHANGED | SWP_NOCOPYBITS);
	}
}

/**
 * @brief Open project-file.
 */
void CMainFrame::OnFileOpenProject()
{
	String sFilepath;
	
	// get the default projects path
	String strProjectPath = COptionsMgr::Get(OPT_PROJECTS_PATH);
	if (!SelectFile(m_hWnd, sFilepath, strProjectPath.c_str(), IDS_OPEN_TITLE,
			IDS_PROJECTFILES, TRUE))
		return;
	
	strProjectPath = paths_GetParentPath(sFilepath.c_str());
	// store this as the new project path
	COptionsMgr::SaveOption(OPT_PROJECTS_PATH, strProjectPath);

	LoadAndOpenProjectFile(sFilepath.c_str());
}

/**
 * @brief Receive command line from another instance.
 *
 * This function receives command line when only single-instance
 * is allowed. New instance tried to start sends its command line
 * to here so we can open paths it was meant to.
 */
template<>
LRESULT CMainFrame::OnWndMsg<WM_COPYDATA>(WPARAM, LPARAM lParam)
{
	COPYDATASTRUCT *const pcds = reinterpret_cast<COPYDATASTRUCT *>(lParam);
	LPCTSTR pchData = reinterpret_cast<LPCTSTR>(pcds->lpData);
	// Bail out if data isn't zero-terminated
	DWORD cchData = pcds->cbData / sizeof(TCHAR);
	if (cchData == 0 || pchData[cchData - 1] != _T('\0'))
		return FALSE;
	// Allow calling thread to resume
	ReplyMessage(TRUE);
	// Process command line
	MergeCmdLineInfo cmdInfo = pchData;
	theApp.ParseArgsAndDoOpen(cmdInfo, this);
	return TRUE;
}

/**
 * @brief Prompt user to select configuration file, and then load settings from it
 */
void CMainFrame::OnDebugLoadConfig()
{
	static const TCHAR filetypes[] = _T("WinMerge Config files (*.txt)\0*.txt\0All files (*.*)\0*.*\0");

	OPENFILENAME ofn;
	ZeroMemory(&ofn, OPENFILENAME_SIZE_VERSION_400);
	ofn.lStructSize = OPENFILENAME_SIZE_VERSION_400;
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFilter = filetypes;
	ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOREADONLYRETURN;
	static TCHAR buffer[MAX_PATH];
	ofn.lpstrFile = buffer;
	ofn.nMaxFile = _countof(buffer);

	if (!GetOpenFileName(&ofn))
		return;

	CConfigLog configLog;
	// set configLog settings to current
	LoadConfigLog(configLog, m_lfDiff, ToConfigLog);
	// update any settings found in actual config file
	configLog.ReadLogFile(ofn.lpstrFile);
	// set our current settings from configLog settings
	LoadConfigLog(configLog, m_lfDiff, FromConfigLog);
}

/**
 * @brief Send current option settings into codepage module
 */
void CMainFrame::UpdateCodepageModule()
{
	// Get current codepage settings from the options module
	// and push them into the codepage module
	updateDefaultCodepage(COptionsMgr::Get(OPT_CP_DEFAULT_MODE), COptionsMgr::Get(OPT_CP_DEFAULT_CUSTOM));
}

/**
 * @brief Handle timer events.
 * @param [in] nIDEvent Timer that timed out.
 */
template<>
LRESULT CMainFrame::OnWndMsg<WM_TIMER>(WPARAM wParam, LPARAM)
{
	switch (wParam)
	{
	case WM_NONINTERACTIVE:
		KillTimer(WM_NONINTERACTIVE);
		PostMessage(WM_CLOSE);
		break;
	
	case ID_TIMER_FLASH:
		// This timer keeps running until window is activated
		// See OnActivate()
		FlashWindow(TRUE);
		break;
	}
	return 0;
}

/**
 * @brief Close all open windows.
 * 
 * Asks about saving unsaved files and then closes all open windows.
 */
void CMainFrame::OnWindowCloseAll()
{
	CDocFrame *pDocFrameToClose = NULL;
	do
	{
		CDocFrame *pDocFrame = pDocFrameToClose;
		if (pDocFrame)
			pDocFrame->SendMessage(WM_CLOSE);
		pDocFrameToClose = GetActiveDocFrame();
		if (pDocFrameToClose == pDocFrame)
			pDocFrameToClose = NULL;
	} while (pDocFrameToClose);
}

/**
 * @brief Checkin in file into ClearCase.
 */ 
void CMainFrame::CheckinToClearCase(LPCTSTR strDestinationPath)
{
	String spath, sname;
	SplitFilename(strDestinationPath, &spath, &sname, 0);
	DWORD code;
	// checkin operation
	String args = string_format(_T("checkin -nc \"%s\""), sname.c_str());
	String vssPath = COptionsMgr::Get(OPT_VSS_PATH);
	HANDLE hVss = RunIt(vssPath.c_str(), args.c_str(), TRUE, FALSE);
	if (hVss != INVALID_HANDLE_VALUE)
	{
		WaitForSingleObject(hVss, INFINITE);
		GetExitCodeProcess(hVss, &code);
		CloseHandle(hVss);
		if (code != 0)
		{
			if (LanguageSelect.MsgBox(IDS_VSS_CHECKINERROR, MB_ICONWARNING | MB_YESNO) == IDYES)
			{
				// undo checkout operation
				args = string_format(_T("uncheckout -rm \"%s\""), sname.c_str());
				HANDLE hVss = RunIt(vssPath.c_str(), args.c_str(), TRUE, TRUE);
				if (hVss != INVALID_HANDLE_VALUE)
				{
					WaitForSingleObject(hVss, INFINITE);
					GetExitCodeProcess(hVss, &code);
					CloseHandle(hVss);
					if (code != 0)
					{
						LanguageSelect.MsgBox(IDS_VSS_UNCOERROR, MB_ICONSTOP);
					}
				}
				else
				{
					LanguageSelect.MsgBox(IDS_VSS_RUN_ERROR, MB_ICONSTOP);
				}				
			}
		}
	}
	else
	{
		LanguageSelect.MsgBox(IDS_VSS_RUN_ERROR, MB_ICONSTOP);
	}
}

/**
 * @brief Access to the singleton main frame (where we have some globals)
 */
CMainFrame *GetMainFrame()
{
	return theApp.m_pMainWnd;
}

/** 
 * @brief Opens dialog for user to Load, edit and save project files.
 * This dialog gets current compare paths and filter (+other properties
 * possible in project files) and initializes the dialog with them.
 */
void CMainFrame::OnSaveProject()
{
	OPropertySheet sht;
	sht.m_caption = LanguageSelect.LoadString(IDS_PROJFILEDLG_CAPTION);
	sht.m_psh.dwFlags |= PSH_NOAPPLYNOW; // Hide 'Apply' button since we don't need it
	ProjectFilePathsDlg pathsDlg;
	sht.AddPage(pathsDlg)->pszTitle = pathsDlg.m_strCaption.c_str();

	if (CDocFrame *pFrame = GetActiveDocFrame())
	{
		FRAMETYPE frame = pFrame->GetFrameType();
		pathsDlg.m_sLeftFile = pFrame->m_wndFilePathBar.GetTitle(0);
		pathsDlg.m_sRightFile = pFrame->m_wndFilePathBar.GetTitle(1);
		if (frame == FRAME_FILE)
		{
			CChildFrame *pDoc = static_cast<CChildFrame *>(pFrame);
			pathsDlg.m_bLeftPathReadOnly = pDoc->m_ptBuf[0]->GetReadOnly();
			pathsDlg.m_bRightPathReadOnly = pDoc->m_ptBuf[1]->GetReadOnly();
		}
		else if (frame == FRAME_FOLDER)
		{
			// Get paths currently in compare
			CDirFrame *pDoc = static_cast<CDirFrame *>(pFrame);
			pathsDlg.m_bIncludeSubfolders = pDoc->GetRecursive();
			pathsDlg.m_bLeftPathReadOnly = pDoc->GetLeftReadOnly();
			pathsDlg.m_bRightPathReadOnly = pDoc->GetRightReadOnly();
		}
	}
	String filterNameOrMask = globalFileFilter.GetFilterNameOrMask();
	pathsDlg.m_sFilter = filterNameOrMask.c_str();
	sht.DoModal(NULL, GetLastActivePopup()->m_hWnd);
}

/** 
 * @brief Start flashing window if window is inactive.
 */
void CMainFrame::StartFlashing()
{
	HWindow *const activeWindow = HWindow::GetActiveWindow();
	if (activeWindow != GetLastActivePopup())
	{
		FlashWindow(TRUE);
		m_bFlashing = true;
		SetTimer(ID_TIMER_FLASH, WINDOW_FLASH_TIMEOUT, NULL);
	}
}

/** 
 * @brief Stop flashing window when window is activated.
 *
 * If WinMerge is inactive when compare finishes, we start flashing window
 * to alert user. When user activates WinMerge window we stop flashing.
 * @param [in] nState Tells if window is being activated or deactivated.
 * @param [in] pWndOther Pointer to window whose status is changing.
 * @param [in] Is window minimized?
 */
template<>
LRESULT CMainFrame::OnWndMsg<WM_ACTIVATE>(WPARAM wParam, LPARAM lParam)
{
	MessageReflect_TopLevelWindow<WM_ACTIVATE>(wParam, lParam);
	switch (LOWORD(wParam))
	{
	case WA_ACTIVE:
	case WA_CLICKACTIVE:
		UpdateIndicators();
		if (m_bFlashing)
		{
			m_bFlashing = false;
			FlashWindow(FALSE);
			KillTimer(ID_TIMER_FLASH);
		}
		break;
	}
	return 0;
}

template<>
LRESULT CMainFrame::OnWndMsg<WM_COMMAND>(WPARAM wParam, LPARAM lParam)
{
	switch (const UINT id = lParam ? wParam : LOWORD(wParam))
	{
	case ID_FILE_MRU_FILE1:
	case ID_FILE_MRU_FILE2:
	case ID_FILE_MRU_FILE3:
	case ID_FILE_MRU_FILE4:
	case ID_FILE_MRU_FILE5:
	case ID_FILE_MRU_FILE6:
	case ID_FILE_MRU_FILE7:
	case ID_FILE_MRU_FILE8:
	case ID_FILE_MRU_FILE9:
	case ID_FILE_MRU_FILE10:
	case ID_FILE_MRU_FILE11:
	case ID_FILE_MRU_FILE12:
	case ID_FILE_MRU_FILE13:
	case ID_FILE_MRU_FILE14:
	case ID_FILE_MRU_FILE15:
	case ID_FILE_MRU_FILE16:
		LoadAndOpenProjectFile(m_FilesMRU[id - ID_FILE_MRU_FILE1].c_str());
		break;
	case ID_FILE_NEW:
		OnFileNew();
		break;
	case ID_FILE_OPEN:
		OnFileOpen();
		break;
	case ID_FILE_CLOSE:
		OnFileClose();
		break;
	case ID_FILE_OPENPROJECT:
		OnFileOpenProject();
		break;
	case ID_FILE_SAVEPROJECT:
		OnSaveProject();
		break;
	case ID_FILE_OPENCONFLICT:
		OnFileOpenConflict();
		break;
	case ID_APP_ABOUT:
		LanguageSelect.DoModal(CAboutDlg());
		break;
	case ID_HELP:
		ShowHelp();
		break;
	case ID_VIEW_LANGUAGE:
		if (LanguageSelect.DoModal(LanguageSelect) == IDOK)
			UpdateResources();
		break;
	case ID_VIEW_SELECTFONT:
		OnViewSelectfont();
		break;
	case ID_VIEW_USEDEFAULTFONT:
		OnViewUsedefaultfont();
		break;
	case ID_VIEW_WHITESPACE:
		OnViewWhitespace();
		break;
	case ID_TOOLBAR_NONE:
		OnToolbarNone();
		break;
	case ID_TOOLBAR_SMALL:
		OnToolbarSmall();
		break;
	case ID_TOOLBAR_BIG:
		OnToolbarBig();
		break;
	case ID_VIEW_TAB_BAR:
		OnViewTabBar();
		break;
	case ID_VIEW_STATUS_BAR:
		OnViewStatusBar();
		break;
	case ID_VIEW_LOCATION_BAR:
	case ID_VIEW_DETAIL_BAR:
		if (CDocFrame *pDocFrame = GetActiveDocFrame())
		{
			if (HWindow *pBar = pDocFrame->GetDlgItem(id))
			{
				pBar->ShowWindow(pBar->IsWindowVisible() ? SW_HIDE : SW_SHOW);
				pDocFrame->RecalcLayout();
				if (COptionDef<bool> *opt = LookupOption(id))
					COptionsMgr::SaveOption(*opt, pBar->IsWindowVisible() != FALSE);
			}
		}
		break;
	case ID_VIEW_RESIZE_PANES:
		OnResizePanes();
		break;
	case ID_TOOLS_FILTERS:
		OnToolsFilters();
		break;
	case ID_TOOLS_GENERATEPATCH:
		OnToolsGeneratePatch();
		break;
	case ID_DEBUG_LOADCONFIG:
		OnDebugLoadConfig();
		break;
	case ID_DEBUG_RESETOPTIONS:
		OnDebugResetOptions();
		break;
	case ID_OPTIONS:
		OnOptions();
		break;
	case ID_OPTIONS_SHOWDIFFERENT:
		OnOptionsShowDifferent();
		break;
	case ID_OPTIONS_SHOWIDENTICAL:
		OnOptionsShowIdentical();
		break;
	case ID_OPTIONS_SHOWUNIQUELEFT:
		OnOptionsShowUniqueLeft();
		break;
	case ID_OPTIONS_SHOWUNIQUERIGHT:
		OnOptionsShowUniqueRight();
		break;
	case ID_OPTIONS_SHOWBINARIES:
		OnOptionsShowBinaries();
		break;
	case ID_OPTIONS_SHOWSKIPPED:
		OnOptionsShowSkipped();
		break;
	case ID_WINDOW_ARRANGE:
		m_pWndMDIClient->SendMessage(WM_MDIICONARRANGE);
		break;
	case ID_WINDOW_CASCADE:
		m_pWndMDIClient->SendMessage(WM_MDICASCADE);
		break;
	case ID_WINDOW_TILE_HORZ:
		m_pWndMDIClient->SendMessage(WM_MDITILE, MDITILE_HORIZONTAL);
		break;
	case ID_WINDOW_TILE_VERT:
		m_pWndMDIClient->SendMessage(WM_MDITILE, MDITILE_VERTICAL);
		break;
	case ID_WINDOW_CLOSEALL:
		OnWindowCloseAll();
		break;
	case ID_HELP_CONTENTS:
		OnHelpContents();
		break;
	case ID_HELP_GNULICENSE:
		OnHelpGnulicense();
		break;
	case ID_HELP_RELEASENOTES:
		OnHelpReleasenotes();
		break;
	case ID_HELP_TRANSLATIONS:
		OnHelpTranslations();
		break;
	case ID_HELP_GETCONFIG:
		OnSaveConfigData();
		break;
	case ID_HELP_MERGE7ZMISMATCH:
		OnHelpMerge7zmismatch();
		break;
	case ID_APP_EXIT:
		SendMessage(WM_CLOSE);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

LRESULT CMainFrame::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_ACTIVATE:
		OnWndMsg<WM_ACTIVATE>(wParam, lParam);
		break;
	case WM_COMMAND:
		if (CDocFrame *pDocFrame = GetActiveDocFrame())
			if (LRESULT lResult = pDocFrame->SendMessage(WM_COMMAND, wParam, lParam))
				return lResult;
		if (LRESULT lResult = OnWndMsg<WM_COMMAND>(wParam, lParam))
			return lResult;
		break;
	case WM_SYSCOMMAND:
		::DefFrameProc(m_hWnd, m_pWndMDIClient->m_hWnd, uMsg, wParam, lParam);
		switch (wParam & 0xFFF0)
		{
		case SC_MOVE:
		case SC_SIZE:
		case SC_RESTORE:
		case SC_MAXIMIZE:
			SaveWindowPlacement();
			break;
		}
		return 0;
	case WM_NOTIFY:
		switch (reinterpret_cast<NMHDR *>(lParam)->code)
		{
		case TTN_NEEDTEXT:
			OnToolTipText(reinterpret_cast<NMHDR *>(lParam));
			break;
		case TCN_SELCHANGE:
			int index = m_wndTabBar->GetCurSel();
			TCITEM item;
			item.mask = TCIF_PARAM;
			if (m_wndTabBar->GetItem(index, &item))
				m_pWndMDIClient->SendMessage(WM_MDIACTIVATE, item.lParam);
			break;
		}
		break;
	case WM_INITMENUPOPUP:
		return OnWndMsg<WM_INITMENUPOPUP>(wParam, lParam);
	case WM_MENUCHAR:
		if (LRESULT lResult = OnWndMsg<WM_MENUCHAR>(wParam, lParam))
			return lResult;
		break;
	case WM_MENUSELECT:
		OnWndMsg<WM_MENUSELECT>(wParam, lParam);
		break;
	case WM_MEASUREITEM:
		OnWndMsg<WM_MEASUREITEM>(wParam, lParam);
		break;
	case WM_DRAWITEM:
		OnWndMsg<WM_DRAWITEM>(wParam, lParam);
		break;
	case WM_DROPFILES:
		OnWndMsg<WM_DROPFILES>(wParam, lParam);
		break;
	case WM_CLOSE:
		if (!PrepareForClosing())
			return 0;
		m_wndToolBar = NULL;
		m_wndTabBar = NULL;
		m_wndStatusBar = NULL;
		PostQuitMessage(0);
		break;
	case WM_SETCURSOR:
		if (WaitStatusCursor::SetCursor(this))
			return TRUE;
		break;
	case WM_COPYDATA:
		return OnWndMsg<WM_COPYDATA>(wParam, lParam);
	case WM_TIMER:
		OnWndMsg<WM_TIMER>(wParam, lParam);
		break;
	case WM_SIZE:
		if (wParam != SIZE_MINIMIZED)
			RecalcLayout();
		return 0;
	case WM_ERASEBKGND:
		// Apply background brush only to DocFrames and SubFrames.
		return TRUE;
	case WM_NCDESTROY:
		return OWindow::WindowProc(uMsg, wParam, lParam);
	case WM_HELP:
		ShowHelp();
		return 0;
	}
	return ::DefFrameProc(m_hWnd, m_pWndMDIClient->m_hWnd, uMsg, wParam, lParam);
}

BOOL CMainFrame::CreateToobar()
{
	m_wndToolBar = HToolBar::Create(
		WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS,
		0, 0, 0, 0, m_pWnd, 0xC000);

	H2O::ToolBarButton buttons[] =
	{
		ID_FILE_NEW,
		ID_FILE_OPEN,
		ID_FILE_SAVE,
		0,
		ID_EDIT_UNDO,
		ID_EDIT_REDO,
		0,
		ID_SELECTLINEDIFF,
		0,
		ID_NEXTDIFF,
		ID_PREVDIFF,
		0,
		ID_FIRSTDIFF,
		ID_CURDIFF,
		ID_LASTDIFF,
		0,
		ID_L2R,
		ID_R2L,
		0,
		ID_L2RNEXT,
		ID_R2LNEXT,
		0,
		ID_OPTIONS,
		0,
		ID_ALL_RIGHT,
		ID_ALL_LEFT,
		0,
		ID_REFRESH,
		buttons // NB: This extra entry is there to complete initialization.
	};

	m_wndToolBar->ButtonStructSize();
	m_wndToolBar->AddButtons(_countof(buttons) - 1, buttons);

	LoadToolbarImages();

	if (!COptionsMgr::Get(OPT_SHOW_TOOLBAR))
		m_wndToolBar->ShowWindow(SW_HIDE);

	return TRUE;
}

/** @brief Load toolbar images from the resource. */
void CMainFrame::LoadToolbarImages()
{
	if (m_imlMenu)
		m_imlMenu->Destroy();
	m_imlMenu = LanguageSelect.LoadImageList(IDB_TOOLBAR_ENABLED, 16, _countof(m_MenuIcons));
	int j = m_imlMenu->GetImageCount();
	for (int i = 0 ; i < _countof(m_MenuIcons) ; ++i)
	{
		LPCTSTR lpResID = MAKEINTRESOURCE(m_MenuIcons[i].iconResID);
		HBITMAP hbmImage = (HBITMAP)::LoadImage(
			LanguageSelect.FindResourceHandle(lpResID, RT_BITMAP), lpResID,
			IMAGE_BITMAP, 0, 0, LR_SHARED | LR_VGACOLOR | LR_LOADTRANSPARENT | LR_LOADMAP3DCOLORS);
		m_imlMenu->AddMasked(hbmImage, RGB(255, 0, 255));
		DeleteObject(hbmImage);
	}
	m_wndToolBar->SetImageList(NULL);
	m_wndToolBar->SetDisabledImageList(NULL);
	if (m_imlToolbarEnabled)
		m_imlToolbarEnabled->Destroy();
	if (m_imlToolbarDisabled)
		m_imlToolbarDisabled->Destroy();

	int cxyButton = 0;
	int toolbarSize = COptionsMgr::Get(OPT_TOOLBAR_SIZE);
	if (toolbarSize == 0)
	{
		m_imlToolbarEnabled = LanguageSelect.LoadImageList(IDB_TOOLBAR_ENABLED, 16);
		m_imlToolbarDisabled = LanguageSelect.LoadImageList(IDB_TOOLBAR_DISABLED, 16);
		cxyButton = 24;
	}
	else if (toolbarSize == 1)
	{
		m_imlToolbarEnabled = LanguageSelect.LoadImageList(IDB_TOOLBAR_ENABLED32, 32);
		m_imlToolbarDisabled = LanguageSelect.LoadImageList(IDB_TOOLBAR_DISABLED32, 32);
		cxyButton = 40;
	}

	m_wndToolBar->SetButtonSize(cxyButton, cxyButton);
	m_wndToolBar->SetImageList(m_imlToolbarEnabled->m_hImageList);
	m_wndToolBar->SetDisabledImageList(m_imlToolbarDisabled->m_hImageList);
}

/**
 * @brief Reset all WinMerge options to default values.
 */
void CMainFrame::OnDebugResetOptions()
{
	int response = LanguageSelect.MsgBox(IDS_RESET_OPTIONS_WARNING,
		MB_YESNO | MB_DEFBUTTON2 | MB_ICONWARNING);
	if (response == IDYES)
	{
		theApp.ResetOptions();
	}
}

/** @brief Hide the toolbar. */
void CMainFrame::OnToolbarNone()
{
	COptionsMgr::SaveOption(OPT_SHOW_TOOLBAR, false);
	m_wndToolBar->ShowWindow(SW_HIDE);
	RecalcLayout();
}

/** @brief Show small toolbar. */
void CMainFrame::OnToolbarSmall()
{
	COptionsMgr::SaveOption(OPT_SHOW_TOOLBAR, true);
	COptionsMgr::SaveOption(OPT_TOOLBAR_SIZE, 0);
	LoadToolbarImages();
	m_wndToolBar->ShowWindow(SW_SHOW);
	RecalcLayout();
}

/** @brief Show big toolbar. */
void CMainFrame::OnToolbarBig()
{
	COptionsMgr::SaveOption(OPT_SHOW_TOOLBAR, true);
	COptionsMgr::SaveOption(OPT_TOOLBAR_SIZE, 1);
	LoadToolbarImages();
	m_wndToolBar->ShowWindow(SW_SHOW);
	RecalcLayout();
}

/** @brief Lang aware version of CFrameWnd::OnToolTipText() */
void CMainFrame::OnToolTipText(NMHDR* pNMHDR)
{
	ASSERT(pNMHDR->code == TTN_NEEDTEXT);

	// need to handle both ANSI and UNICODE versions of the message
	TOOLTIPTEXT* pTTT = (TOOLTIPTEXT*)pNMHDR;
	String strFullText;
	LPCTSTR strTipText = _T("");
	UINT_PTR nID = pNMHDR->idFrom;
	if (pNMHDR->code == TTN_NEEDTEXT && (pTTT->uFlags & TTF_IDISHWND))
	{
		// idFrom is actually the HWND of the tool
		nID = ::GetDlgCtrlID((HWND)nID);
	}
	if (nID != 0) // will be zero on a separator
	{
		strFullText = LanguageSelect.LoadString(nID);
		// don't handle the message if no string resource found
		String::size_type i = strFullText.find('\n');
		if (i == String::npos)
			return;
		strTipText = strFullText.c_str() + i + 1;
	}
	lstrcpyn(pTTT->szText, strTipText, _countof(pTTT->szText));
	// bring the tooltip window above other popup windows
	::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,
		SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER);
}

/**
 * @brief Ask user for close confirmation when closing the mainframe.
 * This function asks if user wants to close multiple open windows when user
 * selects (perhaps accidentally) to close WinMerge (application).
 * @return true if user agreeds to close all windows.
 */
bool CMainFrame::AskCloseConfirmation()
{
	CDocFrame *pFrame = GetActiveDocFrame();
	return pFrame == NULL
		|| pFrame->GetWindow(GW_HWNDFIRST) == pFrame->GetWindow(GW_HWNDLAST)
		|| LanguageSelect.MsgBox(IDS_CLOSEALL_WINDOWS, MB_YESNO | MB_ICONWARNING) == IDYES;
}

/**
 * @brief Shows the release notes for user.
 * This function opens release notes HTML document into browser.
 */
void CMainFrame::OnHelpReleasenotes()
{
	String sPath = GetModulePath(0) + RelNotes;
	ShellExecute(NULL, _T("open"), sPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

/**
 * @brief Shows the translations page.
 * This function opens translations page URL into browser.
 */
void CMainFrame::OnHelpTranslations()
{
	ShellExecute(NULL, _T("open"), TranslationsUrl, NULL, NULL, SW_SHOWNORMAL);
}

/**
 * @brief Called when user selects File/Open Conflict...
 */
void CMainFrame::OnFileOpenConflict()
{
	String conflictFile;
	if (SelectFile(m_hWnd, conflictFile))
	{
		if (IsConflictFile(conflictFile.c_str()))
		{
			DoOpenConflict(conflictFile.c_str());
		}
		else
		{
			LanguageSelect.FormatMessage(
				IDS_NOT_CONFLICT_FILE, conflictFile.c_str()
			).MsgBox(MB_ICONSTOP);
		}
	}
}

/**
 * @brief Select and open conflict file for resolving.
 * This function lets user to select conflict file to resolve.
 * Then we parse conflict file to two files to "merge" and
 * save resulting file over original file.
 *
 * Set left-side file read-only as it is the repository file which cannot
 * be modified anyway. Right-side file is user's file which is set as
 * modified by default so user can just save it and accept workspace
 * file as resolved file.
 * @param [in] conflictFile Full path to conflict file to open.
 * @param [in] checked If true, do not check if it really is project file.
 * @return TRUE if conflict file was opened for resolving.
 */
BOOL CMainFrame::DoOpenConflict(LPCTSTR conflictFile)
{
	BOOL conflictCompared = FALSE;

	// Create temp files and put them into the list,
	// from where they get deleted when MainFrame is deleted.
	TempFile *wTemp = new TempFile();
	String workFile = wTemp->Create(_T("confw_"));
	m_tempFiles.push_back(wTemp);
	TempFile *vTemp = new TempFile();
	String revFile = vTemp->Create(_T("confv_"));
	m_tempFiles.push_back(vTemp);

	// Parse conflict file into two files.
	bool inners;
	bool success = ParseConflictFile(conflictFile, workFile.c_str(), revFile.c_str(), inners);

	if (success)
	{
		// Open two parsed files to WinMerge, telling WinMerge to
		// save over original file (given as third filename).
		m_strSaveAsPath = conflictFile;
		FileLocation filelocLeft, filelocRight;
		filelocLeft.filepath = revFile;
		filelocLeft.description = LanguageSelect.LoadString(IDS_CONFLICT_THEIRS_FILE);
		filelocRight.filepath = workFile;
		filelocRight.description = LanguageSelect.LoadString(IDS_CONFLICT_MINE_FILE);
		conflictCompared = DoFileOpen(filelocLeft, filelocRight,
				FFILEOPEN_READONLY | FFILEOPEN_NOMRU,
				FFILEOPEN_NOMRU | FFILEOPEN_MODIFIED);
	}
	else
	{
		LanguageSelect.MsgBox(IDS_ERROR_CONF_RESOLVE, MB_ICONSTOP);
	}
	return conflictCompared;
}

/** 
 * @brief Read project and perform comparison specified
 * @param [in] sProject Full path to project file.
 * @return TRUE if loading project file and starting compare succeeded.
 */
bool CMainFrame::LoadAndOpenProjectFile(LPCTSTR lpProject)
{
	if (*lpProject == '\0')
		return false;

	ProjectFile project;
	String sErr;
	if (!project.Read(lpProject, sErr))
	{
		if (sErr.empty())
			sErr = LanguageSelect.LoadString(IDS_UNK_ERROR_READING_PROJECT);
		LanguageSelect.FormatMessage(
			IDS_ERROR_FILEOPEN, lpProject, sErr.c_str()
		).MsgBox(MB_ICONSTOP);
		return false;
	}

	if (project.HasFilter())
	{
		String filter = project.GetFilter();
		filter = string_trim_ws(filter);
		globalFileFilter.SetFilter(filter);
	}

	bool bRecursive = project.HasSubfolders() && project.GetSubfolders() > 0;

	FileLocation filelocLeft, filelocRight;

	DWORD dwLeftFlags = FFILEOPEN_DETECT;
	filelocLeft.filepath = project.GetLeft();
	if (!filelocLeft.filepath.empty())
	{	
		dwLeftFlags |= FFILEOPEN_PROJECT;
		if (project.GetLeftReadOnly())
			dwLeftFlags |= FFILEOPEN_READONLY;
	}

	DWORD dwRightFlags = FFILEOPEN_DETECT;
	filelocRight.filepath = project.GetRight();
	if (!filelocRight.filepath.empty())
	{	
		dwRightFlags |= FFILEOPEN_PROJECT;
		if (project.GetRightReadOnly())
			dwRightFlags |= FFILEOPEN_READONLY;
	}

	SettingStore.WriteProfileInt(_T("Settings"), _T("Recurse"), bRecursive);

	bool rtn = DoFileOpen(filelocLeft, filelocRight, dwLeftFlags, dwRightFlags, bRecursive);

	String sProject(lpProject); // !!! the erase() below may invalidate lpProject!
	vector<String>::iterator it = find(m_FilesMRU.begin(), m_FilesMRU.end(), sProject);
	if (it != m_FilesMRU.begin())
	{
		if (it != m_FilesMRU.end())
			m_FilesMRU.erase(it);
		m_FilesMRU.insert(m_FilesMRU.begin(), sProject);
		if (m_FilesMRU.size() > FILES_MRU_CAPACITY)
			m_FilesMRU.resize(FILES_MRU_CAPACITY);
		SaveFilesMRU();
	}
	return rtn;
}

/**
 * @brief Get type of frame (File/Folder compare).
 * @param [in] pFrame Pointer to frame to check.
 * @return FRAMETYPE of the given frame.
*/
CDocFrame *CMainFrame::GetActiveDocFrame(BOOL *pfActive)
{
	HWindow *pWnd = (HWindow *)m_pWndMDIClient->SendMessage(
		WM_MDIGETACTIVE, 0, reinterpret_cast<LPARAM>(pfActive));
	return static_cast<CDocFrame *>(CDocFrame::FromHandle(pWnd));
}

void CMainFrame::SetActiveMenu(HMENU hMenu)
{
	if (::GetMenu(m_hWnd) == hMenu)
	{
		m_pWndMDIClient->SendMessage(WM_MDIREFRESHMENU);
	}
	else
	{
	UINT n = GetMenuItemCount(hMenu);
	HMENU hWindowMenu = GetSubMenu(hMenu, n - 2);
	m_pWndMDIClient->SendMessage(WM_MDISETMENU,
		reinterpret_cast<WPARAM>(hMenu),
		reinterpret_cast<LPARAM>(hWindowMenu));
	}
	DrawMenuBar();
}

HMenu *CMainFrame::SetScriptMenu(HMenu *pMenu, LPCSTR section)
{
	char text[128];
	if (section == NULL)
	{
		(pMenu != m_pScriptMenu ? pMenu : m_pScriptMenu) = NULL;
	}
	else if (pMenu && pMenu->GetMenuStringA(IDC_SCRIPT_LAST, text, _countof(text)))
	{
		m_pScriptMenu = HMenu::CreatePopupMenu();
		char path[MAX_PATH];
		GetModuleFileNameA(NULL, path, MAX_PATH);
		PathRemoveFileSpecA(path);
		DWORD cchSupplementFolder = 0;
		do
		{
			PathAppendA(path, text);
			UINT cp = GetPrivateProfileIntA("Locale", "CodePage", CP_ACP, path);
			char buffer[0x8000];
			if (GetPrivateProfileSectionA(section, buffer, _countof(buffer), path))
			{
				char *p = buffer;
				unsigned id = IDC_SCRIPT_FIRST;
				while (char *q = strchr(p, '='))
				{
					const size_t r = strlen(q);
					*q++ = _T('\0');
					if (id < IDC_SCRIPT_LAST)
					{
						pMenu->InsertMenuW(IDC_SCRIPT_LAST, MF_STRING, id, OString(HString::Oct(p)->Uni(cp)).W);
						m_pScriptMenu->AppendMenuW(MF_STRING, id, OString(HString::Oct(q)->Uni(cp)).W);
						++id;
					}
					p = q + r;
				}
			}
			cchSupplementFolder ^= ExpandEnvironmentStringsA("%SupplementFolder%", path, MAX_PATH);
		} while ((cchSupplementFolder > 0) && (cchSupplementFolder < MAX_PATH));
		pMenu->DeleteMenu(IDC_SCRIPT_LAST);
		pMenu = m_pScriptMenu;
	}
	return pMenu;
}

/**
 * @brief Save pane sizes and positions when one of panes requests it.
 */
void CMainFrame::RecalcLayout()
{
	RECT rectClient;
	GetClientRect(&rectClient);
	if (m_wndToolBar && (m_wndToolBar->GetStyle() & WS_VISIBLE))
	{
		m_wndToolBar->MoveWindow(0, 0, 0, 0);
		RECT rect;
		m_wndToolBar->GetWindowRect(&rect);
		rectClient.top += rect.bottom - rect.top;
	}
	if (m_wndTabBar && (m_wndTabBar->GetStyle() & WS_VISIBLE))
	{
		RECT rect = { 0, 0, 0, 0 };
		m_wndTabBar->AdjustRect(TRUE, &rect);
		m_wndTabBar->MoveWindow(
			rectClient.left, rectClient.top,
			rectClient.right, -(rect.bottom + rect.top));
		m_wndTabBar->GetWindowRect(&rect);
		rectClient.top += rect.bottom - rect.top;
	}
	if (m_wndStatusBar && (m_wndStatusBar->GetStyle() & WS_VISIBLE))
	{
		m_wndStatusBar->MoveWindow(0, 0, 0, 0);
		RECT rect;
		m_wndStatusBar->GetWindowRect(&rect);
		rectClient.bottom -= rect.bottom - rect.top;
		m_wndStatusBar->GetClientRect(&rect);
		int parts[5];
		parts[4] = rect.right;
		rect.right -= 100;
		parts[3] = rect.right;
		rect.right -= 100;
		parts[2] = rect.right;
		rect.right -= 150;
		parts[1] = rect.right;
		rect.right -= 100;
		parts[0] = rect.right;
		m_wndStatusBar->SetParts(_countof(parts), parts);
	}
	if (m_pWndMDIClient)
	{
		m_pWndMDIClient->MoveWindow(
			rectClient.left, rectClient.top,
			rectClient.right - rectClient.left,
			rectClient.bottom - rectClient.top);
	}
}
