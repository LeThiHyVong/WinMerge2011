///////////////////////////////////////////////////////////////////////////
//  File:    java.cpp
//  Version: 1.1.0.4
//  Updated: 19-Jul-1998
//
//  Copyright:  Ferdinand Prantl, portions by Stcherbatchenko Andrei
//  E-mail:     prantl@ff.cuni.cz
//
//  Java syntax highlighing definition
//
//  You are free to use or modify this code to the following restrictions:
//  - Acknowledge me somewhere in your about box, simple "Parts of code by.."
//  will be enough. If you can't (or don't want to), contact me personally.
//  - LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ccrystaltextview.h"
#include "string_util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using CommonKeywords::IsNumeric;

static BOOL IsJavaKeyword(LPCTSTR pszChars, int nLength)
{
	static LPCTSTR const s_apszJavaKeywordList[] =
	{
		_T("abstract"),
		_T("async") SCRIPT_LEXIS_ONLY,
		_T("await") SCRIPT_LEXIS_ONLY,
		_T("boolean"),
		_T("break"),
		_T("byte"),
		_T("byvalue"),
		_T("case"),
		_T("catch"),
		_T("char"),
		_T("class"),
		_T("const"),
		_T("continue"),
		_T("default"),
		_T("do"),
		_T("double"),
		_T("else"),
		_T("extends"),
		_T("false"),
		_T("final"),
		_T("finally"),
		_T("float"),
		_T("for"),
		_T("function") SCRIPT_LEXIS_ONLY,
		_T("goto"),
		_T("if"),
		_T("implements"),
		_T("import"),
		_T("instanceof"),
		_T("int"),
		_T("interface"),
		_T("long"),
		_T("native"),
		_T("new"),
		_T("null"),
		_T("package"),
		_T("private"),
		_T("protected"),
		_T("public"),
		_T("return"),
		_T("short"),
		_T("static"),
		_T("super"),
		_T("switch"),
		_T("synchronized"),
		_T("this"),
		_T("threadsafe"),
		_T("throw"),
		_T("transient"),
		_T("true"),
		_T("try"),
		_T("undefined") SCRIPT_LEXIS_ONLY,
		_T("var") SCRIPT_LEXIS_ONLY,
		_T("void"),
		_T("while"),
	};
	return xiskeyword<_tcsncmp>(pszChars, nLength, s_apszJavaKeywordList);
}

#define DEFINE_BLOCK pBuf.DefineBlock

#define COOKIE_COMMENT          0x0001
#define COOKIE_PREPROCESSOR     0x0002
#define COOKIE_STRING           0x000C
#define COOKIE_STRING_SINGLE    0x0004
#define COOKIE_STRING_DOUBLE    0x0008
#define COOKIE_STRING_REGEXP    0x000C
#define COOKIE_EXT_COMMENT      0x0010
#define COOKIE_REJECT_REGEXP    0x0020
#define COOKIE_TRANSPARENT      0xFFFFFF00

void CCrystalTextView::ParseLineJava(TextBlock::Cookie &cookie, LPCTSTR const pszChars, int const nLength, int I, TextBlock::Array &pBuf)
{
	DWORD &dwCookie = cookie.m_dwCookie;

	if (nLength == 0)
	{
		dwCookie &= (COOKIE_TRANSPARENT | COOKIE_EXT_COMMENT | COOKIE_REJECT_REGEXP | COOKIE_TEMPLATE_STRING);
		return;
	}

	DWORD const dwParser = dwCookie & COOKIE_PARSER ?
		dwCookie & COOKIE_PARSER : dwCookie >> 4 & COOKIE_PARSER;
	int const nKeywordMask = dwParser == COOKIE_PARSER_JAVA ?
		COMMON_LEXIS | NATIVE_LEXIS : COMMON_LEXIS | SCRIPT_LEXIS;

	BOOL bFirstChar = (dwCookie & ~(COOKIE_TRANSPARENT | COOKIE_EXT_COMMENT)) == 0;
	BOOL bRedefineBlock = TRUE;
	BOOL bDecIndex = FALSE;
	enum { False, Start, End } bWasComment = False;
	int nIdentBegin = -1;
	do
	{
		int const nPrevI = I++;
		if (bRedefineBlock)
		{
			int const nPos = bDecIndex ? nPrevI : I;
			bRedefineBlock = FALSE;
			bDecIndex = FALSE;
			if (dwCookie & (COOKIE_COMMENT | COOKIE_EXT_COMMENT))
			{
				DEFINE_BLOCK(nPos, COLORINDEX_COMMENT);
			}
			else if (dwCookie & COOKIE_PREPROCESSOR)
			{
				DEFINE_BLOCK(nPos, COLORINDEX_PREPROCESSOR);
			}
			else if (dwCookie & (dwParser == COOKIE_PARSER_JSCRIPT ? COOKIE_TEMPLATE_STRING | COOKIE_STRING : COOKIE_STRING))
			{
				DEFINE_BLOCK(nPos, COLORINDEX_STRING);
			}
			else if (xisalnum(pszChars[nPos]) || pszChars[nPos] == '.' && nPos > 0 && (!xisalpha(pszChars[nPos - 1]) && !xisalpha(pszChars[nPos + 1])))
			{
				DEFINE_BLOCK(nPos, COLORINDEX_NORMALTEXT);
			}
			else
			{
				DEFINE_BLOCK(nPos, COLORINDEX_OPERATOR);
				bRedefineBlock = TRUE;
				bDecIndex = TRUE;
			}
		}
		// Can be bigger than length if there is binary data
		// See bug #1474782 Crash when comparing SQL with with binary data
		if (I < nLength)
		{
			if (dwCookie & COOKIE_COMMENT)
			{
				DEFINE_BLOCK(I, COLORINDEX_COMMENT);
				dwCookie |= COOKIE_COMMENT;
				break;
			}

			switch (dwCookie & COOKIE_STRING)
			{
			case COOKIE_STRING_SINGLE:
				if (pszChars[I] == '\'')
				{
					dwCookie &= ~COOKIE_STRING_SINGLE;
					bRedefineBlock = TRUE;
					int nPrevI = I;
					while (nPrevI && pszChars[--nPrevI] == '\\')
					{
						dwCookie ^= COOKIE_STRING_SINGLE;
						bRedefineBlock ^= TRUE;
					}
				}
				continue;
			case COOKIE_STRING_DOUBLE:
				if (pszChars[I] == '"')
				{
					dwCookie &= ~COOKIE_STRING_DOUBLE;
					bRedefineBlock = TRUE;
					int nPrevI = I;
					while (nPrevI && pszChars[--nPrevI] == '\\')
					{
						dwCookie ^= COOKIE_STRING_DOUBLE;
						bRedefineBlock ^= TRUE;
					}
				}
				continue;
			case COOKIE_STRING_REGEXP:
				if (pszChars[I] == '/')
				{
					dwCookie &= ~COOKIE_STRING_REGEXP;
					bRedefineBlock = TRUE;
					int nPrevI = I;
					while (nPrevI && pszChars[--nPrevI] == '\\')
					{
						dwCookie ^= COOKIE_STRING_REGEXP;
						bRedefineBlock ^= TRUE;
					}
					if (!(dwCookie & COOKIE_STRING_REGEXP))
					{
						int nNextI = I;
						while (++nNextI < nLength && xisalnum(pszChars[nNextI]))
							I = nNextI;
						bWasComment = End;
					}
				}
				continue;
			}

			if (dwParser == COOKIE_PARSER_JSCRIPT && (dwCookie & COOKIE_TEMPLATE_STRING))
			{
				if (pszChars[I] == '`')
				{
					dwCookie &= ~COOKIE_TEMPLATE_STRING;
					bRedefineBlock = TRUE;
					int nPrevI = I;
					while (nPrevI && pszChars[--nPrevI] == '\\')
					{
						dwCookie ^= COOKIE_TEMPLATE_STRING;
						bRedefineBlock ^= TRUE;
					}
				}
				else if (pszChars[I] == '$' && pszChars[I + 1] == '{')
				{
					dwCookie &= ~COOKIE_TEMPLATE_STRING;
					bRedefineBlock = TRUE;
					bDecIndex = TRUE;
					cookie.m_dwNesting <<= 4;
					++cookie.m_dwNesting;
				}
				continue;
			}

			//  Extended comment /*....*/
			if (dwCookie & COOKIE_EXT_COMMENT)
			{
				if (I > 0 && pszChars[I] == '/' && pszChars[nPrevI] == '*' && bWasComment != Start)
				{
					dwCookie &= ~COOKIE_EXT_COMMENT;
					bRedefineBlock = TRUE;
					bWasComment = End;
				}
				else
				{
					bWasComment = False;
				}
				continue;
			}

			if (I > 0 && pszChars[I] == '/' && pszChars[nPrevI] == '/' && bWasComment != End)
			{
				DEFINE_BLOCK(nPrevI, COLORINDEX_COMMENT);
				dwCookie |= COOKIE_COMMENT;
				break;
			}

			if (DWORD const dwNesting = cookie.m_dwNesting & 0xF)
			{
				if (pszChars[I] == '{' && pszChars[nPrevI] != '$' && dwNesting != 0xF)
					++cookie.m_dwNesting;
			}

			// Double-quoted text
			if (pszChars[I] == '"')
			{
				DEFINE_BLOCK(I, dwCookie & COOKIE_PREPROCESSOR ? COLORINDEX_PREPROCESSOR : COLORINDEX_STRING);
				dwCookie |= COOKIE_STRING_DOUBLE;
				continue;
			}
			// Single-quoted text
			if (pszChars[I] == '\'' && (I == 0 || !xisxdigit(pszChars[nPrevI])))
			{
				DEFINE_BLOCK(I, dwCookie & COOKIE_PREPROCESSOR ? COLORINDEX_PREPROCESSOR : COLORINDEX_STRING);
				dwCookie |= COOKIE_STRING_SINGLE;
				continue;
			}
			// Regular expression literal
			if (pszChars[I] == '/' && I < nLength - 1 && pszChars[I + 1] != '/' && pszChars[I + 1] != '*')
			{
				if (!(dwCookie & COOKIE_REJECT_REGEXP))
				{
					DEFINE_BLOCK(I, dwCookie & COOKIE_PREPROCESSOR ? COLORINDEX_PREPROCESSOR : COLORINDEX_STRING);
					dwCookie |= COOKIE_STRING_REGEXP | COOKIE_REJECT_REGEXP;
					continue;
				}
			}
			// Template string literal
			if (dwParser == COOKIE_PARSER_JSCRIPT && pszChars[I] == '`')
			{
				DEFINE_BLOCK(I, dwCookie & COOKIE_PREPROCESSOR ? COLORINDEX_PREPROCESSOR : COLORINDEX_STRING);
				dwCookie |= COOKIE_TEMPLATE_STRING;
				continue;
			}

			if (I > 0 && pszChars[I] == '*' && pszChars[nPrevI] == '/' && bWasComment != End)
			{
				DEFINE_BLOCK(nPrevI, COLORINDEX_COMMENT);
				dwCookie |= COOKIE_EXT_COMMENT;
				bWasComment = Start;
				continue;
			}

			bWasComment = False;

			//  Preprocessor directive #....
			if (dwCookie & COOKIE_PREPROCESSOR)
				continue;

			if (bFirstChar)
			{
				if (pszChars[I] == '#')
				{
					DEFINE_BLOCK(I, COLORINDEX_PREPROCESSOR);
					dwCookie |= COOKIE_PREPROCESSOR;
					continue;
				}
				if (!xisspace(pszChars[I]))
					bFirstChar = FALSE;
			}

			if (xisalnum(pszChars[I]) || pszChars[I] == '.' && I > 0 && (!xisalpha(pszChars[nPrevI]) && !xisalpha(pszChars[I + 1])))
			{
				if (nIdentBegin == -1)
					nIdentBegin = I;
				dwCookie |= COOKIE_REJECT_REGEXP;
				continue;
			}

			if ((dwCookie & (COOKIE_STRING | COOKIE_COMMENT | COOKIE_EXT_COMMENT)) == 0 && !xisspace(pszChars[I]))
			{
				if (I + 1 < nLength)
				{
					switch (pszChars[I])
					{
					case '/': case '*':
						switch (pszChars[I + 1])
						{
						case '/': case '*':
							continue;
						}
					}
				}
				dwCookie &= ~COOKIE_REJECT_REGEXP;
				switch (TCHAR peer = pszChars[I])
				{
				case '+':
				case '-':
					if (I == 0 || pszChars[nPrevI] != peer)
						break;
					// fall through
				case ')':
				case ']':
					dwCookie |= COOKIE_REJECT_REGEXP;
					break;
				}
			}
		}
		if (nIdentBegin >= 0)
		{
			if (!pBuf.m_bRecording)
			{
				// No need to extract keywords, so skip rest of loop
			}
			else if (IsNumeric(pszChars + nIdentBegin, I - nIdentBegin))
			{
				DEFINE_BLOCK(nIdentBegin, COLORINDEX_NUMBER);
			}
			else if (IsJavaKeyword(pszChars + nIdentBegin, I - nIdentBegin) & nKeywordMask)
			{
				DEFINE_BLOCK(nIdentBegin, COLORINDEX_KEYWORD);
			}
			else
			{
				for (int j = I; j < nLength; j++)
				{
					if (!xisspace(pszChars[j]))
					{
						if (pszChars[j] == '(')
						{
							DEFINE_BLOCK(nIdentBegin, COLORINDEX_FUNCNAME);
						}
						break;
					}
				}
			}
			nIdentBegin = -1;
		}
		bRedefineBlock = TRUE;
		bDecIndex = TRUE;
		if ((nKeywordMask & SCRIPT_LEXIS) && (cookie.m_dwNesting & 0xF) && I < nLength && pszChars[I] == '}')
		{
			--cookie.m_dwNesting;
			if (!(cookie.m_dwNesting & 0xF))
			{
				DEFINE_BLOCK(I, COLORINDEX_OPERATOR);
				cookie.m_dwNesting >>= 4;
				dwCookie |= COOKIE_TEMPLATE_STRING;
				bDecIndex = FALSE;
			}
		}
	} while (I < nLength);

	if (pszChars[nLength - 1] != '\\')
		dwCookie &= (COOKIE_TRANSPARENT | COOKIE_EXT_COMMENT | COOKIE_REJECT_REGEXP | COOKIE_TEMPLATE_STRING);
}

TESTCASE
{
	int count = 0;
	BOOL (*pfnIsKeyword)(LPCTSTR, int) = NULL;
	FILE *file = fopen(__FILE__, "r");
	assert(file);
	TCHAR text[1024];
	while (_fgetts(text, _countof(text), file))
	{
		TCHAR c, *p, *q;
		if (pfnIsKeyword && (p = _tcschr(text, '"')) != NULL && (q = _tcschr(++p, '"')) != NULL)
			VerifyKeyword<_tcsncmp>(pfnIsKeyword, p, static_cast<int>(q - p));
		else if (_stscanf(text, _T(" static BOOL IsJavaKeyword %c"), &c) == 1 && c == '(')
			pfnIsKeyword = IsJavaKeyword;
		else if (pfnIsKeyword && _stscanf(text, _T(" } %c"), &c) == 1 && (c == ';' ? ++count : 0))
			VerifyKeyword<_tcsncmp>(pfnIsKeyword = NULL, NULL, 0);
	}
	fclose(file);
	assert(count == 1);
	return count;
}
