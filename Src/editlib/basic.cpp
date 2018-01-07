///////////////////////////////////////////////////////////////////////////
//  File:    basic.cpp
//  Version: 1.1.0.4
//  Updated: 19-Jul-1998
//
//  Copyright:  Ferdinand Prantl, portions by Stcherbatchenko Andrei
//  E-mail:     prantl@ff.cuni.cz
//
//  Visual Basic syntax highlighing definition
//
//  You are free to use or modify this code to the following restrictions:
//  - Acknowledge me somewhere in your about box, simple "Parts of code by.."
//  will be enough. If you can't (or don't want to), contact me personally.
//  - LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ccrystaltextview.h"
#include "SyntaxColors.h"
#include "string_util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using CommonKeywords::IsNumeric;

static BOOL IsBasicKeyword(LPCTSTR pszChars, int nLength)
{
  //  (Visual) Basic keywords
  static LPCTSTR const s_apszBasicKeywordList[] =
  {
    _T ("Abs"),
    _T ("AddHandler"),
    _T ("AddressOf"),
    _T ("Alias"),
    _T ("And"),
    _T ("AndAlso"),
    _T ("Ansi"),
    _T ("Any"),
    _T ("AppActivate"),
    _T ("As"),
    _T ("Asc"),
    _T ("Assembly"),
    _T ("Atn"),
    _T ("Attribute"),
    _T ("Auto"),
    _T ("Base"),
    _T ("Beep"),
    _T ("Begin"),
    _T ("BeginProperty"),
    _T ("Binary"),
    _T ("Boolean"),
    _T ("ByRef"),
    _T ("Byte"),
    _T ("ByVal"),
    _T ("Call"),
    _T ("Case"),
    _T ("Catch"),
    _T ("CBool"),
    _T ("CByte"),
    _T ("CChar"),
    _T ("CDate"),
    _T ("CDbl"),
    _T ("CDec"),
    _T ("Char"),
    _T ("ChDir"),
    _T ("ChDrive"),
    _T ("CheckBox"),
    _T ("Chr"),
    _T ("CInt"),
    _T ("Class"),
    _T ("CLng"),
    _T ("Close"),
    _T ("CObj"),
    _T ("Compare"),
    _T ("Const"),
    _T ("Continue"),
    _T ("Cos"),
    _T ("CreateObject"),
    _T ("CreateVerifyItem"),
    _T ("CSByte"),
    _T ("CShort"),
    _T ("CSng"),
    _T ("CStr"),
    _T ("CType"),
    _T ("CUInt"),
    _T ("CULng"),
    _T ("CurDir"),
    _T ("Currency"),
    _T ("CUShort"),
    _T ("Custom"),
    _T ("CVar"),
    _T ("Database"),
    _T ("Date"),
    _T ("Decimal"),
    _T ("Declare"),
    _T ("Default"),
    _T ("Delegate"),
    _T ("Dialog"),
    _T ("Dim"),
    _T ("Dir"),
    _T ("DirectCast"),
    _T ("DlgEnable"),
    _T ("DlgText"),
    _T ("DlgVisible"),
    _T ("Do"),
    _T ("Double"),
    _T ("Each"),
    _T ("Else"),
    _T ("ElseIf"),
    _T ("End"),
    _T ("EndIf"),
    _T ("EndProperty"),
    _T ("Enum"),
    _T ("EOF"),
    _T ("Erase"),
    _T ("Error"),
    _T ("Event"),
    _T ("Exit"),
    _T ("Exp"),
    _T ("Explicit"),
    _T ("ExternalSource"),
    _T ("False"),
    _T ("FileCopy"),
    _T ("FileLen"),
    _T ("Finally"),
    _T ("Fix"),
    _T ("For"),
    _T ("Format"),
    _T ("Friend"),
    _T ("Function"),
    _T ("Get"),
    _T ("GetAttrName"),
    _T ("GetAttrType"),
    _T ("GetAttrValBool"),
    _T ("GetAttrValEnumInt"),
    _T ("GetAttrValEnumString"),
    _T ("GetAttrValFloat"),
    _T ("GetAttrValInt"),
    _T ("GetAttrValString"),
    _T ("GetClassId"),
    _T ("GetGeoType"),
    _T ("GetObject"),
    _T ("GetType"),
    _T ("Global"),
    _T ("GoSub"),
    _T ("GoTo"),
    _T ("Handles"),
    _T ("Hex"),
    _T ("Hour"),
    _T ("If"),
    _T ("Implements"),
    _T ("Imports"),
    _T ("In"),
    _T ("Inherits"),
    _T ("Input"),
    _T ("InputBox"),
    _T ("InStr"),
    _T ("Int"),
    _T ("Integer"),
    _T ("Interface"),
    _T ("Is"),
    _T ("IsDate"),
    _T ("IsEmpty"),
    _T ("IsFalse"),
    _T ("IsNot"),
    _T ("IsNull"),
    _T ("IsNumeric"),
    _T ("IsTrue"),
    _T ("Kill"),
    _T ("LBound"),
    _T ("LCase"),
    _T ("Left"),
    _T ("Len"),
    _T ("Let"),
    _T ("Lib"),
    _T ("Like"),
    _T ("Line"),
    _T ("Log"),
    _T ("Long"),
    _T ("Loop"),
    _T ("LTrim"),
    _T ("Me"),
    _T ("Mid"),
    _T ("Minute"),
    _T ("MkDir"),
    _T ("Mod"),
    _T ("Module"),
    _T ("Month"),
    _T ("MsgBox"),
    _T ("MustInherit"),
    _T ("MustOverride"),
    _T ("My"),
    _T ("MyBase"),
    _T ("MyClass"),
    _T ("Name"),
    _T ("Namespace"),
    _T ("Narrowing"),
    _T ("New"),
    _T ("Next"),
    _T ("Not"),
    _T ("Nothing"),
    _T ("NotInheritable"),
    _T ("NotOverridable"),
    _T ("Now"),
    _T ("Object"),
    _T ("Oct"),
    _T ("Of"),
    _T ("Off"),
    _T ("On"),
    _T ("Open"),
    _T ("Operator"),
    _T ("Option"),
    _T ("Optional"),
    _T ("Or"),
    _T ("OrElse"),
    _T ("Overloads"),
    _T ("Overridable"),
    _T ("Overrides"),
    _T ("ParamArray"),
    _T ("Partial"),
    _T ("Preserve"),
    _T ("Print"),
    _T ("Private"),
    _T ("Property"),
    _T ("Protected"),
    _T ("Public"),
    _T ("RaiseEvent"),
    _T ("ReadOnly"),
    _T ("ReDim"),
    _T ("Region"),
    _T ("Rem"),
    _T ("RemoveHandler"),
    _T ("Resume"),
    _T ("Return"),
    _T ("Right"),
    _T ("RmDir"),
    _T ("Rnd"),
    _T ("RTrim"),
    _T ("Sbyte"),
    _T ("Second"),
    _T ("Seek"),
    _T ("Select"),
    _T ("SendKeys"),
    _T ("Set"),
    _T ("SetAttrValBool"),
    _T ("SetAttrValEnumInt"),
    _T ("SetAttrValEnumString"),
    _T ("SetAttrValFloat"),
    _T ("SetAttrValInt"),
    _T ("SetAttrValString"),
    _T ("Shadows"),
    _T ("Shared"),
    _T ("Shell"),
    _T ("Short"),
    _T ("Sin"),
    _T ("Single"),
    _T ("SMDoMenu"),
    _T ("Space"),
    _T ("Sqr"),
    _T ("Static"),
    _T ("Step"),
    _T ("Stop"),
    _T ("Str"),
    _T ("StrComp"),
    _T ("Strict"),
    _T ("String"),
    _T ("StringFunction"),
    _T ("Structure"),
    _T ("Sub"),
    _T ("SyncLock"),
    _T ("Tan"),
    _T ("Text"),
    _T ("TextBox"),
    _T ("Then"),
    _T ("Throw"),
    _T ("Time"),
    _T ("TimeSerial"),
    _T ("TimeValue"),
    _T ("To"),
    _T ("Trim"),
    _T ("True"),
    _T ("Try"),
    _T ("TryCast"),
    _T ("Type"),
    _T ("TypeOf"),
    _T ("UBound"),
    _T ("UCase"),
    _T ("UInteger"),
    _T ("ULong"),
    _T ("Unicode"),
    _T ("Until"),
    _T ("UShort"),
    _T ("Using"),
    _T ("Val"),
    _T ("Variant"),
    _T ("VarType"),
    _T ("VerifyCardinalities"),
    _T ("Version"),
    _T ("Wend"),
    _T ("When"),
    _T ("While"),
    _T ("Widening"),
    _T ("With"),
    _T ("WithEvents"),
    _T ("Write"),
    _T ("WriteOnly"),
    _T ("Xor"),
    _T ("Year"),
  };
  return xiskeyword<_tcsnicmp>(pszChars, nLength, s_apszBasicKeywordList);
}

#define DEFINE_BLOCK(pos, colorindex)   \
ASSERT((pos) >= 0 && (pos) <= nLength);\
if (pBuf != NULL)\
  {\
    if (nActualItems == 0 || pBuf[nActualItems - 1].m_nCharPos <= (pos)){\
        pBuf[nActualItems].m_nCharPos = (pos);\
        pBuf[nActualItems].m_nColorIndex = (colorindex);\
        pBuf[nActualItems].m_nBgColorIndex = COLORINDEX_BKGND;\
        nActualItems ++;}\
  }

#define COOKIE_COMMENT          0x0001
#define COOKIE_PREPROCESSOR     0x0002
#define COOKIE_EXT_COMMENT      0x0004
#define COOKIE_STRING           0x0008
#define COOKIE_CHAR             0x0010

DWORD CCrystalTextView::ParseLineBasic(DWORD dwCookie, int nLineIndex, TEXTBLOCK *pBuf, int &nActualItems)
{
  const int nLength = GetLineLength(nLineIndex);
  if (nLength == 0)
    return dwCookie & COOKIE_EXT_COMMENT;

  LPCTSTR pszChars = GetLineChars(nLineIndex);
  BOOL bFirstChar = (dwCookie & ~COOKIE_EXT_COMMENT) == 0;
  BOOL bRedefineBlock = TRUE;
  BOOL bDecIndex = FALSE;
  int nIdentBegin = -1;
  int nPrevI = -1;
  int I;
  for (I = 0; I <= nLength; nPrevI = I++)
    {
      if (bRedefineBlock)
        {
          int nPos = I;
          if (bDecIndex)
            nPos = nPrevI;
          if (dwCookie & (COOKIE_COMMENT | COOKIE_EXT_COMMENT))
            {
              DEFINE_BLOCK(nPos, COLORINDEX_COMMENT);
            }
          else if (dwCookie & (COOKIE_CHAR | COOKIE_STRING))
            {
              DEFINE_BLOCK(nPos, COLORINDEX_STRING);
            }
          else
            {
              if (xisalnum(pszChars[nPos]) || pszChars[nPos] == '.' && nPos > 0 && (!xisalpha(pszChars[nPos - 1]) && !xisalpha(pszChars[nPos + 1])))
                {
                  DEFINE_BLOCK(nPos, COLORINDEX_NORMALTEXT);
                }
              else
                {
                  DEFINE_BLOCK(nPos, COLORINDEX_OPERATOR);
                  bRedefineBlock = TRUE;
                  bDecIndex = TRUE;
                  goto out;
                }
            }
          bRedefineBlock = FALSE;
          bDecIndex = FALSE;
        }
out:

      // Can be bigger than length if there is binary data
      // See bug #1474782 Crash when comparing SQL with with binary data
      if (I >= nLength)
        break;

      if (dwCookie & COOKIE_COMMENT)
        {
          DEFINE_BLOCK(I, COLORINDEX_COMMENT);
          dwCookie |= COOKIE_COMMENT;
          break;
        }

      //  String constant "...."
      if (dwCookie & COOKIE_STRING)
        {
          if (pszChars[I] == '"')
            {
              dwCookie &= ~COOKIE_STRING;
              bRedefineBlock = TRUE;
            }
          continue;
        }

      if (pszChars[I] == '\'')
        {
          DEFINE_BLOCK(I, COLORINDEX_COMMENT);
          dwCookie |= COOKIE_COMMENT;
          break;
        }

      //  Normal text
      if (pszChars[I] == '"')
        {
          DEFINE_BLOCK(I, COLORINDEX_STRING);
          dwCookie |= COOKIE_STRING;
          continue;
        }

      if (bFirstChar)
        {
          if (!xisspace(pszChars[I]))
            bFirstChar = FALSE;
        }

      if (pBuf == NULL)
        continue;               //  We don't need to extract keywords,
      //  for faster parsing skip the rest of loop

      if (xisalnum(pszChars[I]) || pszChars[I] == '.' && I > 0 && (!xisalpha(pszChars[nPrevI]) && !xisalpha(pszChars[I + 1])))
        {
          if (nIdentBegin == -1)
            nIdentBegin = I;
        }
      else
        {
          if (nIdentBegin >= 0)
            {
              if (IsBasicKeyword(pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK(nIdentBegin, COLORINDEX_KEYWORD);
                }
              else if (IsNumeric(pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK(nIdentBegin, COLORINDEX_NUMBER);
                }
              else
                {
                  bool bFunction = FALSE;

                  for (int j = I; j < nLength; j++)
                    {
                      if (!xisspace(pszChars[j]))
                        {
                          if (pszChars[j] == '(')
                            {
                              bFunction = TRUE;
                            }
                          break;
                        }
                    }
                  if (bFunction)
                    {
                      DEFINE_BLOCK(nIdentBegin, COLORINDEX_FUNCNAME);
                    }
                }
              bRedefineBlock = TRUE;
              bDecIndex = TRUE;
              nIdentBegin = -1;
            }
        }
    }

  if (nIdentBegin >= 0)
    {
      if (IsBasicKeyword(pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK(nIdentBegin, COLORINDEX_KEYWORD);
        }
      else if (IsNumeric(pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK(nIdentBegin, COLORINDEX_NUMBER);
        }
      else
        {
          bool bFunction = FALSE;

          for (int j = I; j < nLength; j++)
            {
              if (!xisspace(pszChars[j]))
                {
                  if (pszChars[j] == '(')
                    {
                      bFunction = TRUE;
                    }
                  break;
                }
            }
          if (bFunction)
            {
              DEFINE_BLOCK(nIdentBegin, COLORINDEX_FUNCNAME);
            }
        }
    }

  dwCookie &= COOKIE_EXT_COMMENT;
  return dwCookie;
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
			assert(pfnIsKeyword(p, static_cast<int>(q - p)));
		else if (_stscanf(text, _T(" static BOOL IsBasicKeyword %c"), &c) == 1 && c == '(')
			pfnIsKeyword = IsBasicKeyword;
		else if (pfnIsKeyword && _stscanf(text, _T(" } %c"), &c) == 1 && (c == ';' ? ++count : 0))
			pfnIsKeyword = NULL;
	}
	fclose(file);
	assert(count == 1);
	return count;
}
