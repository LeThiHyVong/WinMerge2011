// CrystalEditViewEx.h: Schnittstelle f�r die Klasse CCrystalEditViewEx.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "ccrystaleditview.h"
#include "crystalparser.h"

class EDITPADC_CLASS CCrystalEditViewEx : public CCrystalEditView
{

protected:
	friend class CCrystalParser;

/** @construction/destruction */
public:
	CCrystalEditViewEx(size_t);

/** @operations */
public:
	/**
	Attaches a syntax parser to this view.

	@param pParser
		The syntax parser to attach to this view. The Parser must be derived
		from CCrystalParser.

	@return
		The syntax parser that was used until the call to this function or
		NULL if no one was attached to this view.
	*/
	CCrystalParser *SetSyntaxParser( CCrystalParser *pParser );

/** @overridables */
protected:
	/**
	@description
		Is called by tht view, when there is a Line to parse.

	@param dwCookie
		The result of parsing the previous line. This parameter contains flags,
		you set while parsing the previous line. Normaly this will be flags that
		indicate a region that was still open at the end of the last line. You
		set those flags for a line by returning them ored (|).
	@param nLineIndex
		The zero-based index of the line to parse.
	@param pBuf
		You have to split the lines in to parts which are specified by there
		beginning (zero-based index of the character in this line) and there text
		color. You have to specifie each one of this blocks in this Buffer.
		You can define a new Block with the macro DEFINE_BLOCK( pos, colorindex ).

		When the function ist called with this parameter set zu NULL, you only
		have to calculate the cookies.
	@param nActualItems
		Used by the macro DEFINE_BLOCK.

	@return 
		The calculated flags for this line (see dwCookie).
	*/
	virtual DWORD ParseLine( DWORD dwCookie, int nLineIndex, 
		TEXTBLOCK *pBuf, int &nActualItems );
	
/** @attributes */
protected:
	/**
	@description
		The parser used to parse the Text or NULL if no parser is used.
	*/
	CCrystalParser *m_pSyntaxParser;
};
