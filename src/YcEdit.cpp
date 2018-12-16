//---------------------------------------------------------------------------
// YahCoLoRiZe - Edit, process and send colorized text into IRC chat-rooms
// via various chat-clients such as mIRC, IceCHat and LeafCHat
//
// Author: Scott Swift
//
// Released to GitHub under GPL v3 October, 2016
//
//---------------------------------------------------------------------------
#include <vcl.h>
//#include <Controls.hpp>
//#include "Main.h"
// Include header files common to all files before this directive!
#pragma hdrstop

#include "YcEdit.h"
#include "YcPrint.h"

#pragma package(smart_init)

//TCustomRichEdit introduces new properties and methods to expand the
//capabilities of TCustomMemo for rich text.
//Do not create instances of TCustomRichEdit. Use TCustomRichEdit as a base
//class when declaring control objects that implement rich text capabilities
//in an edit control. Properties and methods of TCustomRichEdit provide basic
//behavior that descendant classes inherit as well as behavior that components
//can override to customize their behavior.
//
//See Also
//TConversion
//TParaAttributes
//TRichEdit
//TTextAttributes
//---------------------------------------------------------------------------
// YcEdit RichEdit derived from TCustomRichEdit
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
static inline void ValidCtrCheck(TYcEdit *)
{
  new TYcEdit((TComponent*)NULL);
}
//---------------------------------------------------------------------------
namespace Ycedit
{
  void __fastcall PACKAGE Register()
  {
    TComponentClass classes[1] = {__classid(TYcEdit)};
    RegisterComponents("YcEdit", classes, 0);
  }
}
//---------------------------------------------------------------------------
//__fastcall TYcEdit::TYcEdit(TComponent* Owner) : TCustomRichEdit(Owner)
__fastcall TYcEdit::TYcEdit(TComponent* Owner) : TCustomRichEdit(Owner)
// constructor
{
//  this->dts = static_cast<TDTSColor*>(Owner);

  FHideSelection = true;
  FHideScrollBars = true;
  FInsertMode = true;
  FRestoreSel = false;
  FUndoLimit = 100;
  FUndoLimitActual = -1;

  FFileName = "";

  FYcPrint = NULL;
  SetPrintSupport(FPrintSupport); // instantiate TYcPrint if needed
}
//---------------------------------------------------------------------------
__fastcall TYcEdit::~TYcEdit()
// destructor
{
  SetPrintSupport(false); // destroy TYcPrint
}
//---------------------------------------------------------------------------
// scroll window to display the caret (useful if cursor is programmatically
// moved when the control does not have focus)
//
void TYcEdit::ScrollToCaret(void)
{
  ::SendMessage(Handle, EM_SCROLLCARET, 0, 0L);
}
//---------------------------------------------------------------------------
// set number of undo's stored by control. returns number actually set (may
// be smaller than request).
//
void __fastcall TYcEdit::SetUndoLimit(int value)
{
  FUndoLimit = value;
  FUndoLimitActual = ::SendMessage(Handle, EM_SETUNDOLIMIT, value, 0);
}
//---------------------------------------------------------------------------
// get number of undo's stored by control.  we do an odd thing here... we
// want the design-mode requested undo limit to be whatever the programmer
// requested and we want that value to be stored.  however, if the programmer
// asks for the value at runtime, we want to return the true value that the
// control is using.
//
int __fastcall TYcEdit::GetUndoLimit(void)
{
  if (FUndoLimitActual == -1) return FUndoLimit;
  return FUndoLimitActual;
}
//---------------------------------------------------------------------------
// get screen position from characterscroll position
//
TPoint __fastcall TYcEdit::GetCharCoordinates(long Index)
// S.S. 4/2014 Send the 0-based index of the character (SelStart)
// Returns the x,y coordinates relative to the upper-left of the edit-control
// (in screen units)
{
  POINTL pt;
  ::SendMessage(Handle, EM_POSFROMCHAR, (WPARAM)&pt, (LPARAM)Index);
  TPoint newPt;
  newPt.x = pt.x;
  newPt.y = pt.y;
  return newPt;
}
//---------------------------------------------------------------------------
// get the current SelLine (in y) and corrected SelStart (in x) at the caret
//
TPoint __fastcall TYcEdit::GetCaretPos(void)
{
  CHARRANGE CharRange;
  TPoint pt;

  ::SendMessage(Handle, EM_EXGETSEL, 0, (LPARAM)&CharRange);
  pt.x = CharRange.cpMax;
  pt.y = GetLineByIndex(pt.x);
  pt.x -= ::SendMessage(Handle, EM_LINEINDEX, -1, 0);
  return pt;
}
//---------------------------------------------------------------------------
// get the current Line (in y) and corrected Column (in x) at Index
//
TPoint __fastcall TYcEdit::GetPosByIndex(long Index)
{
  long x = 0;
  long y = 0;

  if (Index >= 0)
  {
    // Get the line at the index
    if ((y = ::SendMessage(Handle, EM_EXLINEFROMCHAR, 0, (LPARAM)Index)) >= 0)
      // Column = Index - index at the start of the line
      x = Index - ::SendMessage(Handle, EM_LINEINDEX, (WPARAM)y, 0);
  }

  TPoint pt;
  pt.x = x;
  pt.y = y;
  
  return pt;
}
//---------------------------------------------------------------------------
// get scroll position
//
TPoint __fastcall TYcEdit::GetScrollPos(void)
// S.S. 8/2013
{
  TPoint pt;
  ::SendMessage(Handle, EM_GETSCROLLPOS, 0, (LPARAM)&pt);
  return pt;
}
//---------------------------------------------------------------------------
// set scroll position
//
void __fastcall TYcEdit::SetScrollPos(TPoint pt)
// S.S. 8/2013
{
  ::SendMessage(Handle, EM_SETSCROLLPOS, 0, (LPARAM)&pt);
}
//---------------------------------------------------------------------------
// get the column number of the cursor
//
long __fastcall TYcEdit::GetColumn(void)
{
  CHARRANGE chrg;
  ::SendMessage(Handle, EM_EXGETSEL, 0, (LPARAM) &chrg);
  return chrg.cpMin - ::SendMessage(Handle, EM_LINEINDEX, (WPARAM)-1, 0); // Changed to -1 5/2015
}
//---------------------------------------------------------------------------
// get the column at a specific character index
//
long __fastcall TYcEdit::GetColumnByIndex(long Index)
{
  long retVal = 0;

  if (Index >= 0)
  {
    // Get the line at the index
    long lineIdx = ::SendMessage(Handle, EM_EXLINEFROMCHAR, 0, (LPARAM)Index);

    if (lineIdx >= 0)
    {
      // Get index at the start of the line
      long charIdx = ::SendMessage(Handle, EM_LINEINDEX, (WPARAM)lineIdx, 0);

      retVal = Index - charIdx;
    }
  }
  // Subtract the index at the start of the line from Value to get the column
  return retVal;
}
//---------------------------------------------------------------------------
// get the line number at the caret OR at the start of the selection
//
long __fastcall TYcEdit::GetLine(void)
{
  return GetLineByIndex(-1);
}
//---------------------------------------------------------------------------
// get the line number given a character-index
//
long __fastcall TYcEdit::GetLineByIndex(long Index)
{
  return ::SendMessage(Handle, EM_EXLINEFROMCHAR, 0, (LPARAM)Index);
}
//---------------------------------------------------------------------------
// move the cursor to a specific line
//
void __fastcall TYcEdit::SetLine(long Value)
{
  CHARRANGE chrg;
  chrg.cpMin = chrg.cpMax = ::SendMessage(Handle, EM_LINEINDEX,
                                                     (WPARAM) Value, 0);
  if (chrg.cpMin == -1)  // past end of text so find last line
    chrg.cpMin = chrg.cpMax = TextLength; // Property that calls GetTextLength() S.S.
  ::SendMessage(Handle, EM_EXSETSEL, 0, (LPARAM) &chrg);
}
//---------------------------------------------------------------------------
// get the character's index at Line
//
long int __fastcall TYcEdit::GetCharIdx(long Line)
{
  return ::SendMessage(Handle, EM_LINEINDEX, (WPARAM) Line, 0);
}
//---------------------------------------------------------------------------
// move the cursor to a specific column on the current line
//
void __fastcall TYcEdit::SetColumn(long column)
{
  CHARRANGE chrg;
  long currLine = GetLine();
  long ndx = ::SendMessage(Handle, EM_LINEINDEX, (WPARAM) currLine, 0);
  long cols = ::SendMessage(Handle, EM_LINEINDEX, (WPARAM) currLine + 1, 0) -
    ndx - 1;
  if (column > cols) column = cols;
  chrg.cpMin = chrg.cpMax = ndx + column;
  ::SendMessage(Handle, EM_EXSETSEL, 0, (LPARAM) &chrg);
}
//---------------------------------------------------------------------------
// S.S. Added 5/28/15
//
WideString __fastcall TYcEdit::GetStringW(long LineIndex)
{
  // Get char-index from LineIndex
  long CharIndex = ::SendMessage(Handle, EM_LINEINDEX, (WPARAM)LineIndex, 0);

  // Get length of line at CharIndex
  DWORD len = ::SendMessageW(Handle, EM_LINELENGTH, CharIndex, 0);

  if (!len) return "";

  WideString wText;

  // leave more than enough room for size or null-terminator...
  wchar_t* buf = new wchar_t[len + sizeof(DWORD) + sizeof(wchar_t)];

  ZeroMemory(buf, sizeof(DWORD) + sizeof(wchar_t));
  *((DWORD*)buf) = len; // put # chars to copy at top of buffer

  if (len == (DWORD)::SendMessageW(Handle, EM_GETLINE, LineIndex, (LPARAM)buf))
  {
    // Strip any cr/lf chars
    if (len && (buf[len - 1] == 0x0d || buf[len - 1] == 0x0a)) len--;
    if (len && (buf[len - 1] == 0x0d || buf[len - 1] == 0x0a)) len--;

    buf[len] = '\0'; // terminate

    wText = WideString(buf);
  }

  delete [] buf;

  return wText;
}
//---------------------------------------------------------------------------
// S.S. Added 11/16/15 fixed 11/21/2015
//
void __fastcall TYcEdit::SetStringW(WideString wText, long LineIndex)
{
  // Get char-index from LineIndex
  long iStart = ::SendMessage(Handle, EM_LINEINDEX, (WPARAM)LineIndex, 0);
  long lineLen = ::SendMessageW(Handle, EM_LINELENGTH, iStart, 0);
  SetSelFirstLast(iStart, iStart+lineLen);
  ::SendMessageW(Handle, EM_REPLACESEL, FALSE, (LPARAM)wText.c_bstr());
}
//---------------------------------------------------------------------------
// returns first and last as 0-based char-indices from a selection.
// both return char-index corresponding to the caret-position if no selection.
// (full 32-bits each) S.S.
void __fastcall TYcEdit::GetSelFirstLast(long &first, long &last)
{
  ::SendMessage(Handle, EM_GETSEL, (WPARAM)&first , (LPARAM)&last);
}
//---------------------------------------------------------------------------
// sets select-zone from first and last as 0-based char-indices
// S.S.
void __fastcall TYcEdit::SetSelFirstLast(long first, long last)
{
  ::SendMessage(Handle, EM_SETSEL, (WPARAM)&first , (LPARAM)&last);
}
//---------------------------------------------------------------------------
// S.S. Added 11/16/15
long __fastcall TYcEdit::GetLineCount(void)
{
//  return ::SendMessageW(Handle, EM_GETLINECOUNT, 0, 0);
  return this->Lines->Count;
}
//---------------------------------------------------------------------------
// get the text as an WideString (plaintext)
//
WideString __fastcall TYcEdit::GetTextW(void)
{
  WideString sResult;
  wchar_t* pBuf = NULL;

  try
  {
    pBuf = this->GetRangeBufW(0, -1);
  }
  __finally
  {
    if (pBuf != NULL)
    {
      sResult = WideString(pBuf);
      delete [] pBuf;
    }
  }

  return sResult;
}
//---------------------------------------------------------------------------
// set the text as an WideString (plaintext)
//
void __fastcall TYcEdit::SetTextW(WideString wText)
{
  SETTEXTEX st;
  st.flags = ST_UNICODE;
  st.codepage = 1200; // UTF-16 (UTF-8 is 65001, ANSI is 1252)

  ::SendMessage(Handle, EM_SETTEXTEX, (WPARAM)&st, (LPARAM)wText.c_bstr());
}
//---------------------------------------------------------------------------
// get the text from the control (as plaintext).
//
// Returns the number of wide-chars copied not including the null;
long __fastcall TYcEdit::GetTextBufW(wchar_t* Buffer, long BufSize)
{
  if (Buffer == NULL || BufSize < (long)sizeof(wchar_t))
    return 0;

  long MaxChars = BufSize/2; // how many wide-chars can this buffer hold?

  // Only room for the null?
  if (MaxChars <= 1)
  {
    Buffer[0] = '\0';
    return 0;
  }

  // have room for at least one wide-char...
  WideString S;
  long BytesToCopy;

  S = GetTextW();
  BytesToCopy = Min((int)(S.Length()*(long)sizeof(wchar_t)),
                                      (int)((MaxChars-1)*(long)sizeof(wchar_t)));
  ::memcpy(Buffer, S.c_bstr(), BytesToCopy);
  Buffer[BytesToCopy/2] = '\0';
  return (BytesToCopy/2)-1;
}
//---------------------------------------------------------------------------
// get the selected text as an WideString (plaintext) - if SelLength is 0,
// this function returns the character at the caret!
//
WideString __fastcall TYcEdit::GetSelTextW(void)
{
  WideString sResult;
  wchar_t* pBuf = NULL;

  try
  {
    pBuf = this->GetRangeBufW(-1, 0);
  }
  __finally
  {
    if (pBuf != NULL)
    {
      sResult = WideString(pBuf);
      delete [] pBuf;
    }
  }

  return sResult;
}
//---------------------------------------------------------------------------
// replace the selected text
//
void __fastcall TYcEdit::SetSelTextW(WideString wSelText)
{
  SETTEXTEX st;
  st.flags = ST_SELECTION | ST_UNICODE;
  st.codepage = 1200; // UTF-16 (UTF-8 is 65001, ANSI is 1252)

  ::SendMessage(Handle, EM_SETTEXTEX, (WPARAM)&st, (LPARAM)wSelText.c_bstr());
//  ::SendMessage(Handle, EM_REPLACESEL, FALSE, (LPARAM)wSelText.c_bstr());
}
//---------------------------------------------------------------------------
// get the selected text from the control (as plaintext).
//
// Returns the number of wide-chars copied not including the null;
long __fastcall TYcEdit::GetSelTextBufW(wchar_t* Buffer, long BufSize)
{
  if (Buffer == NULL || BufSize < (long)sizeof(wchar_t))
    return 0;

  long MaxChars = BufSize/2; // how many wide-chars can this buffer hold?

  // Only room for the null?
  if (MaxChars <= 1)
  {
    Buffer[0] = '\0';
    return 0;
  }

  // have room for at least one wide-char...
  WideString S;
  long BytesToCopy;

  S = GetSelTextW();
  BytesToCopy = Min((int)(S.Length()*(long)sizeof(wchar_t)), (int)((MaxChars-1)*(long)sizeof(wchar_t)));
  ::memcpy(Buffer, S.c_bstr(), BytesToCopy);
  Buffer[BytesToCopy/2] = '\0';
  return (BytesToCopy/2)-1;
}
//---------------------------------------------------------------------------
// GetRangeW() - use this method for faster access to the control's text
// (this should be much faster than the SelText property...)
//
WideString __fastcall TYcEdit::GetRangeW(long startPos, long endPos)
{
  WideString sResult;
  wchar_t* pBuf = GetRangeBufW(startPos, endPos);
  WideString Result(pBuf);
  delete[] pBuf;
  return Result;
}
//---------------------------------------------------------------------------
// GetTextRangeW() - use this method for faster access to the control's text
// (this should be much faster than the SelTextW property...)
// (by S.S. 2015)
//
// Set startPos 0 and endPos -1 to return all text.
// Set startPos -1 and endPos 0 to return selected text or char at the caret
// if nothing is selected.
wchar_t* __fastcall TYcEdit::GetRangeBufW(long startPos, long endPos)
{
  if (endPos < 0) // get all text
  {
    startPos = 0;
    endPos = this->TextLength;
  }
  else if (startPos < 0) // get selected text
  {
    int selStart = this->SelStart;
    startPos = selStart;
    endPos = selStart + this->SelLength;
  }

  int dwNum = endPos-startPos;

  wchar_t* pwText = new wchar_t[dwNum+1];

  if (pwText)
  {
    pwText[dwNum] = '\0';

    if (dwNum > 0)
    {
      TEXTRANGEW chrg = { { startPos,  endPos },  pwText };
      // get the text
      ::SendMessageW(Handle, EM_GETTEXTRANGE, 0, (LPARAM) &chrg);
    }
  }

  // return wchar_t string (can be just 1 null char! Never should be
  // a NULL pointer!)
  return pwText;
}
//---------------------------------------------------------------------------
// get the currently selected text complete with RTF codes
//
AnsiString __fastcall TYcEdit::GetTextRtf(void)
{
  return GetRtf(false); // clear selectionOnly flag
}
//---------------------------------------------------------------------------
AnsiString __fastcall TYcEdit::GetSelTextRtf(void)
{
  return GetRtf(true); // set selectionOnly flag
}
//---------------------------------------------------------------------------
// copy RTF encoded text into an AnsiString. note that this function is not
// particularly efficient, but I do not know of any way to determine in
// advance how large a buffer to allocate, so I have to use a stream as
// an intermediate buffer.  I would suggest that you not use this to get
// large blocks of text; there may be size limitations in AnsiString, also.
//
//   selectionOnly - copy only selected text into string; if false, copy
//     entire contents of control into string (not recommended for very
//     large files)
//
AnsiString __fastcall TYcEdit::GetRtf(bool selectionOnly)
{
  char* buf = NULL;
  TMemoryStream* memStream = NULL;
  AnsiString s;

  try
  {
    try
    {
      // create a memory stream and get the text
      memStream = new TMemoryStream();
      CopyToStream(memStream, selectionOnly);
      memStream->Seek(0, soFromBeginning);

      // allocate a buffer of sufficient size and copy the rtf into it
      buf = new char[memStream->Size];
      memStream->ReadBuffer(buf, memStream->Size);

      // insert text into an AnsiString, free buffer, and return
      s = AnsiString(buf);
    }
    catch(...)
    {
      s = "exception thrown in TYcEdit::GetRtf()!";
    }
  }
  __finally
  {
    if (buf)
      delete[] buf;
    if (memStream)
      delete memStream;
  }
  return s;
}
//---------------------------------------------------------------------------
// replace the current selection with RTF encoded text.  note that the
// rtfText value must be equivalent to a *complete* RTF file, i.e., it
// must include the prefixing "{\rtf" and the final closing "}".  if this
// is unclear, use GetSelTextRtf or the SelTextRtf property to fetch
// various selections of text from the control and look at the returned
// values.
//
void __fastcall TYcEdit::SetSelTextRtf(AnsiString rtfText)
{
  PutRtf(rtfText, true); // set replaceSelection flag
}
//---------------------------------------------------------------------------
void __fastcall TYcEdit::SetTextRtf(AnsiString rtfText)
{
  PutRtf(rtfText, false); // clear replaceSelection flag
}
//---------------------------------------------------------------------------
// copy RTF encoded text from an AnsiString into the control. note that this
// function is not particularly efficient -- it could be made more efficient
// by writing a version of StreamInCallback() modified to work directly with
// a buffer.  if you are using this for large blocks of text, consider
// writing such a version yourself.
//
//   replaceSelection - replace selected text (if selection empty, inserts
//     text; if false, replaces entire contents of control)
//
// note: inserting invalid RTF codes may not raise an exception -- my tests
// indicate that the control simply interprets the RTF as best it can....
//
// replaceSelection defaults true
void __fastcall TYcEdit::PutRtf(AnsiString rtfText, bool replaceSelection)
{
  // create a memory stream and put the text into it
  TMemoryStream* memStream = new TMemoryStream();
  memStream->WriteBuffer(rtfText.c_str(), rtfText.Length() + 1);

  // rewind stream and insert into control
  memStream->Seek(0, soFromBeginning);
  PasteFromStream(memStream, replaceSelection);

  // discard stream
  delete memStream;
}
//---------------------------------------------------------------------------
// insert text from a stream into the control
//
//   stream - exisiting stream; note that stream is not rewound prior to
//     inserting the text into the control
//   selectionOnly - replace current selection (if selection empty, inserts
//      text; if false, replaces entire contents of control)
//   plainText - convert to plain text
//   plainRtf - ignore language-specific RTF codes
//
// note: if you CopyToStream(..., plainText = true...) and then paste back
//   from the stream with PasteFromStream(..., plainText = false...), the
//   WinAPI will return an error condition -- that is, you must paste valid
//   RTF when calling this function with plainText = false.
//
// defaults: true, false, false
void __fastcall TYcEdit::PasteFromStream(TStream* stream, bool selectionOnly,
                                                      bool plainText, bool plainRtf)
{
  TEditStream editStream;
  WPARAM format = 0;

  if (selectionOnly) format |= SFF_SELECTION;
  if (plainRtf) format |= SFF_PLAINRTF;
  format |= (plainText) ? SF_TEXT : SF_RTF; // SF_UNICODE can be combined with SF_TEXT

  editStream.dwCookie = (DWORD) stream;
  editStream.dwError = 0;
  editStream.pfnCallback = StreamInCallback;

  ::SendMessage(Handle, EM_STREAMIN, format, (LPARAM) &editStream);

  if (editStream.dwError) throw EOutOfResources("Failed to load stream.");
}
//---------------------------------------------------------------------------
// This counts cr/lfs as 2 chars each! (SelStart and SelLength count newlines
// as 1 char)
long __fastcall TYcEdit::GetTextLength(void)
{
  //return ::SendMessageW(Handle, WM_GETTEXTLENGTH, 0, 0);

  // The message returns the number of TCHARs in the edit control, depending on
  // the setting of the flags in the GETTEXTLENGTHEX structure. If incompatible
  // flags were set in the flags member, the message returns E_INVALIDARG .
  //typedef struct _gettextlengthex {
  //DWORD flags;
  //UINT  codepage;
  //}
  //GTL_DEFAULT Returns the number of characters. This is the default.
  //GTL_USECRLF Computes the answer by using CR/LFs at the end of paragraphs.
  //GTL_PRECISE Computes a precise answer. This approach could necessitate a
  // conversion and thereby take longer. This flag cannot be used with the
  //GTL_CLOSE flag. E_INVALIDARG will be returned if both are used.
  //GTL_CLOSE	Computes an approximate (close) answer. It is obtained quickly
  // and can be used to set the buffer size. This flag cannot be used with the
  //GTL_PRECISE flag. E_INVALIDARG will be returned if both are used.
  //GTL_NUMCHARS Returns the number of characters. This flag cannot be used with
  // the GTL_NUMBYTES flag. E_INVALIDARG will be returned if both are used.
  //GTL_NUMBYTES Returns the number of bytes. This flag cannot be used with the
  //GTL_NUMCHARS flag. E_INVALIDARG will be returned if both are used.
  //
  // Code page used in the translation. It is CP_ACP for ANSI Code Page and
  // 1200 for Unicode.
  GETTEXTLENGTHEX gtl;
  gtl.flags = GTL_DEFAULT;
  gtl.codepage = 1200; // Unicode
  return ::SendMessageW(Handle, EM_GETTEXTLENGTHEX, (WPARAM)&gtl, (LPARAM)0);
}
//---------------------------------------------------------------------------
// Property setter for TabCount (S.S.)
// -1 is MAX_TAB_COUNT
void __fastcall TYcEdit::SetTabCount(int tabCount)
{
  if (FTabCount != tabCount)
  {
    FTabCount = tabCount < 0 ? MAX_TAB_STOPS : tabCount;

    if (FTabCount > 0)
      SetTabStops();
  }
}
//---------------------------------------------------------------------------
// Property setter for TabWidth (S.S.)
void __fastcall TYcEdit::SetTabWidth(int tabWidth)
{
  if (FTabWidth != tabWidth)
  {
    FTabWidth = tabWidth;

    if (FTabCount > 0)
      SetTabStops();
  }
}
//---------------------------------------------------------------------------
void __fastcall TYcEdit::SetTabStops(bool entireDocument)
{
  SetTabStops(this->TabWidth, Font, entireDocument);
}
//---------------------------------------------------------------------------
void __fastcall TYcEdit::SetTabStops(int tabWidth, TFont* font, bool entireDocument)
{
  int tabSizeTwips = GetTabWidthTwipsEx(tabWidth, font);

  if (tabSizeTwips)
  {
    // hide and save selection
    CHARRANGE chrg, chrgAll;
    ::SendMessage(Handle, EM_EXGETSEL, 0, (LPARAM) &chrg);

    // if entireDocument, set selection to all
    if (entireDocument)
    {
      chrgAll.cpMin = 0;
      chrgAll.cpMax = -1;
      ::SendMessage(Handle, EM_EXSETSEL, 0, (LPARAM) &chrgAll);
    }

    // set tab size
    SetTabStops(abs(tabSizeTwips)); // no error on failure...

    // restore selection
    ::SendMessage(Handle, EM_EXSETSEL, 0, (LPARAM) &chrg);
  }
}
//---------------------------------------------------------------------------
// Set tab-stops equal-size and in twips
void __fastcall TYcEdit::SetTabStops(int tabStopTwips)
{
  TTaeTabStops tabs;

  int ii;

  for (ii = 0; ii < FTabCount && ii < MAX_TAB_STOPS; ii++)
    tabs[ii] = (ii + 1) * tabStopTwips;

  SetTabStops(ii, tabs);
}
//---------------------------------------------------------------------------
// Set X tab-stops via an array
void __fastcall TYcEdit::SetTabStops(int tabCount, TTaeTabStops &tabStops)
{
  PARAFORMAT2 pfmt;
  pfmt.cbSize = sizeof(pfmt);

  pfmt.dwMask = PFM_TABSTOPS;

  int ii;

  for (ii = 0; ii < tabCount && ii < MAX_TAB_STOPS; ii++) {
    pfmt.rgxTabs[ii] = tabStops[ii];
    FTabStops[ii] = tabStops[ii];
    }

  pfmt.cTabCount = (SHORT)ii;
  FTabCount = ii;

  ::SendMessage(Handle, EM_SETPARAFORMAT, 0, (LPARAM) &pfmt);
}
//---------------------------------------------------------------------------
// Pass in the font and tab width in chars
int __fastcall TYcEdit::GetTabWidthTwips(void)
{
  return GetTabWidthTwipsEx(FTabWidth, Font);
}

int __fastcall TYcEdit::GetTabWidthTwipsEx(int tabWidth, TFont* font)
{
  HDC hdc = ::GetDC(Handle);
  ::SaveDC(hdc);

  ::SetMapMode(hdc, MM_TEXT);

  TFont* afont = new TFont();
  afont->Assign(font);
  ::SelectObject(hdc, afont->Handle);

  WideString s = WideString::StringOfChar('X', tabWidth);
  SIZE sz;
  int cx = GetDeviceCaps(hdc, LOGPIXELSX);

  int tabSizeTwips;

  if (::GetTextExtentPoint32W(hdc, s.c_bstr(), tabWidth, &sz))
    tabSizeTwips = ::MulDiv(sz.cx, 1440, cx);
  else
    tabSizeTwips = 0;

  ::RestoreDC(hdc, -1);
  ::ReleaseDC(Handle, hdc);

  delete afont;

  return tabSizeTwips;
}
//---------------------------------------------------------------------------
// set the maximum length of text in the control.  note:  my understanding is
// that setting the max length while the control is not empty has no effect.
// if this turns out to be true, then this method should be recoded to
// recreate the window.
//
void __fastcall TYcEdit::DoSetMaxLength(int Value)
{
  ::SendMessage(Handle, EM_EXLIMITTEXT, 0, Value);
}
//---------------------------------------------------------------------------
// get the length of the selected text
//
int __fastcall TYcEdit::GetSelLength(void)
{
  TCharRange CharRange;
  ::SendMessage(Handle, EM_EXGETSEL, 0, (LPARAM) &CharRange);

  int len = CharRange.cpMax - CharRange.cpMin;

  // Need to check for selection at end of document"no text"
  if (len == 1 && CharRange.cpMax >= this->TextLength - this->LineCount)
    len = 0;

  return len;
}
//---------------------------------------------------------------------------
// set the length of the selection
//
void __fastcall TYcEdit::SetSelLength(int Value)
{
  TCharRange CharRange;

  ::SendMessage(Handle, EM_EXGETSEL, 0, (LPARAM) &CharRange);
  CharRange.cpMax = CharRange.cpMin + Value;
  ::SendMessage(Handle, EM_EXSETSEL, 0, (LPARAM) &CharRange);
  ::SendMessage(Handle, EM_SCROLLCARET, 0, 0);
}
//---------------------------------------------------------------------------
// get the line and offset at the selection start
//
TPoint __fastcall TYcEdit::GetSelStartPos(void)
{
  return GetPosByIndex(SelStart);
}
//---------------------------------------------------------------------------
// get the line and offset at the selection start
//
TPoint __fastcall TYcEdit::GetSelEndPos(void)
{
  return GetPosByIndex(SelStart+SelLength);
}
//---------------------------------------------------------------------------
// get the start offset of the cursor
//
int __fastcall TYcEdit::GetSelStart(void)
{
  TCharRange CharRange;

  ::SendMessage(Handle, EM_EXGETSEL, 0, (LPARAM) &CharRange);
  return CharRange.cpMin;
}
//---------------------------------------------------------------------------
// move the start of the selection
//
void __fastcall TYcEdit::SetSelStart(int Value)
{
  TCharRange CharRange;

  CharRange.cpMin = Value;
  CharRange.cpMax = Value;
  ::SendMessage(Handle, EM_EXSETSEL, 0, (LPARAM) &CharRange);
}
//---------------------------------------------------------------------------
// copy the text to a stream
//
//   stream - existing TStream (usually a TMemoryStream)
//   selectionOnly - copy only selected text
//   plainText - convert to plain text
//   noObjects - insert spaces in place of OLE objects
//   plainRtf - ignore language-specific RTF codes
//
// note: text is appended to the stream at the current stream position
//
void __fastcall TYcEdit::CopyToStream(TStream* stream, bool selectionOnly,
                      bool plainText, bool noObjects, bool plainRtf)
{
  TEditStream editStream;
  WPARAM format = 0;

  if (selectionOnly) format |= SFF_SELECTION;
  if (plainRtf) format |= SFF_PLAINRTF;
  if (plainText) format |= (noObjects) ? SF_TEXT : SF_TEXTIZED;
  else format |= (noObjects) ? SF_RTFNOOBJS : SF_RTF;

  editStream.dwCookie = (DWORD) stream;
  editStream.dwError = 0;
  editStream.pfnCallback = StreamOutCallback;

  ::SendMessage(Handle, EM_STREAMOUT, format, (LPARAM) &editStream);

  if (editStream.dwError) throw EOutOfResources("Failed to save stream.");
}
//---------------------------------------------------------------------------
// WinAPI callback for auto-correction S.S.
//
//int WINAPI AutoCorrectProc(
//  LANGID langid,
//  const WCHAR *pszBefore,
//  WCHAR *pszAfter,
//  LONG cchAfter,
//  LONG *pcchReplaced
//);
//langid
//Type: LANGID
//Language ID that identifies the autocorrect file to use for automatic
// correcting.
//pszBefore
//Type: const WCHAR*
//Autocorrect candidate string.
//pszAfter
//Type: WCHAR*
//Resulting autocorrect string, if the return value is not ATP_NOCHANGE.
//cchAfter
//Type: LONG
//Count of characters in pszAfter.
//pcchReplaced
//Type: LONG*
//Count of trailing characters in pszBefore to replace with pszAfter.
//Return value
//Type: int
//Returns one or more of the following values.
//ATP_NOCHANGE 0 No change.
//ATP_CHANGE 1 Change but don’t replace most delimiters, and don’t replace
// a span of unchanged trailing characters (preserves their formatting).
//ATP_NODELIMITER 2 Change but don’t replace a span of unchanged trailing
// characters.
//ATP_REPLACEALLTEXT 4 Replace trailing characters even if they are not
// changed (uses the same formatting for the entire replacement string).
//int CALLBACK TTaeRichEdit::AutoCorrectProc(LANGID langid, const WCHAR *pszBefore,
//           WCHAR *pszAfter, LONG cchAfter, LONG *pcchReplaced)
//{
//  return 0;
//}
//---------------------------------------------------------------------------
// WinAPI callback for streaming text into the control
//
DWORD CALLBACK TYcEdit::StreamInCallback(DWORD dwCookie, LPBYTE pbBuff,
  LONG cb, LONG FAR *pcb)
{
  TStream* stream = reinterpret_cast<TStream*>(dwCookie);

  try
  {
    *pcb = stream->Read(pbBuff, cb);
    return 0;
  }
  catch (EReadError& e)
  {
    *pcb = 0;
    return 1;
  }
}
//---------------------------------------------------------------------------
// WinAPI callback for streaming text out of the control
//
DWORD CALLBACK TYcEdit::StreamOutCallback(DWORD dwCookie, LPBYTE pbBuff,
  LONG cb, LONG FAR *pcb)
{
  TStream* stream = reinterpret_cast<TStream*>(dwCookie);

  try
    {
    *pcb = stream->Write(pbBuff, cb);
    return 0;
    }
  catch (EWriteError& e)
    {
    *pcb = 0;
    return 1;
    }
}
//---------------------------------------------------------------------------
// note: SetInsertMode() does not trigger an OnInsertChange event
//
void __fastcall TYcEdit::SetInsertMode(bool value)
{
  if (FInsertMode != value) return;
  FInsertMode = value;
  ToggleInsertMode();
}
//---------------------------------------------------------------------------
// note: ToggleInsertMode() does not trigger an OnInsertChange event --
// the FInsertMode member shadow variable gets flipped in the KeyDown()
// handler
//
void __fastcall TYcEdit::ToggleInsertMode(void)
{
  // synthesize an insert keystroke (cannot use keybd_event() because
  // there is no window associated with the API function)
  if (!Handle) return;
  // save and clear the event handler
//  TNotifyEvent event = FOnInsertModeChange;
//  FOnInsertModeChange = 0;
  // the following was glommed from a Micro$oft VB example
  ::SendMessage(Handle, WM_KEYDOWN, VK_INSERT, 0x00510001);
  ::SendMessage(Handle, WM_KEYUP, VK_INSERT, 0xC0510001);
  // restore the event handler
//  FOnInsertModeChange = event;
}
//---------------------------------------------------------------------------
void __fastcall TYcEdit::SetLocation(int Left, int Top, int Width, int Height)
{
  this->Left = Left;
  this->Top = Top;
  this->Width = Width;
  this->Height = Height;
}
//---------------------------------------------------------------------------
YCPOSITION __fastcall TYcEdit::GetPosition(void)
{
  YCPOSITION pos;

  // get the line offset in pos.y and the x-offset into the line in pos.x
  TCharRange CharRange;
  ::SendMessage(Handle, EM_EXGETSEL, 0, (LPARAM)&CharRange);

  pos.p.x = CharRange.cpMax - ::SendMessage(Handle, EM_LINEINDEX, -1, 0);
  pos.p.y = SendMessage(Handle, EM_LINEFROMCHAR, -1, 0);

  pos.start = CharRange.cpMin;
  pos.end = CharRange.cpMax;
  pos.firstVisLine = SendMessage(Handle, EM_GETFIRSTVISIBLELINE, 0, 0);
  FSavePos = pos; // Save it for SetPosition to use...

  return pos;
}
//---------------------------------------------------------------------------
void __fastcall TYcEdit::SetPosition(YCPOSITION pos)
{
  // Don't want to restore SelStart and SelLength if it's become "unknown",
  // so the invoking class clears this flag...
  if ( !FRestoreSel )
  {
    // Set SelStart to beginning of the line we were on... no selection
    pos.start = SendMessage(Handle, EM_LINEINDEX, (WPARAM)FSavePos.p.y, 0);
    pos.end = pos.start;
  }

  TCharRange CharRange;
  CharRange.cpMax = pos.end;
  CharRange.cpMin = pos.start;
  ::SendMessage(Handle, EM_EXSETSEL, 0, (LPARAM)&CharRange);

  int newPos = SendMessage(Handle, EM_GETFIRSTVISIBLELINE, 0, 0);
  ::SendMessage(Handle, EM_LINESCROLL, 0, FSavePos.firstVisLine-newPos);
}
//---------------------------------------------------------------------------
// locate text in the control
//
/*
int __fastcall TTaeRichEdit::FindText(AnsiString text, int StartPos,
  int Length, TSearchTypes2 searchOptions)
{
#if __BORLANDC__ < 0x0550
  FINDTEXTEX ft;
#else
  Richedit::FINDTEXTEX ft;
#endif

  ft.chrg.cpMin = StartPos;
  ft.chrg.cpMax = ft.chrg.cpMin + Length;

  /////////// S.S. Modified 5/2014 /////////////////////
  int destSize = text.Length()*sizeof(wchar_t) + sizeof(wchar_t);
  wchar_t* destBuf = new wchar_t[destSize];
  ft.lpstrText = (char *)StringToWideChar(text, destBuf, destSize);
  ///////////////////////////////////////////////////////

  WPARAM flags = 0;
  if (!searchOptions.Contains(st2Backward)) flags |= FR_DOWN;    // and this is documented WHERE???
  if (searchOptions.Contains(st2MatchCase)) flags |= FR_MATCHCASE;  // apparently could use FR_MATCHCASE
  if (searchOptions.Contains(st2WholeWord)) flags |= FR_WHOLEWORD;  // apparently could use FR_WHOLEWORD

  ft.lpstrText = text.c_str();

  int pos = ::SendMessage(Handle, EM_FINDTEXT, flags, (LPARAM) &ft);

  delete [] destBuf; // S.S. 5/2014

  return pos;
}
*/
//---------------------------------------------------------------------------
// locate text in the control
//
//FR_DOWN
//If set, the operation searches from the end of the current selection to the
// end of the document. If not set, the operation searches from the end of the
// current selection to the beginning of the document.
//FR_MATCHALEFHAMZA
//By default, Arabic and Hebrew alefs with different accents are all matched by
// the alef character. Set this flag if you want the search to differentiate
// between alefs with different accents.
//FR_MATCHCASE
//If set, the search operation is case-sensitive. If not set, the search
// operation is case-insensitive.
//FR_MATCHDIAC
//By default, Arabic and Hebrew diacritical marks are ignored. Set this flag
// if you want the search operation to consider diacritical marks.
//FR_MATCHKASHIDA
//By default, Arabic and Hebrew kashidas are ignored. Set this flag if you want
// the search operation to consider kashidas.
//FR_WHOLEWORD
//If set, the operation searches only for whole words that match the search
// string. If not set, the operation also searches for word fragments that
// match the search string.
int __fastcall TYcEdit::FindTextW(const WideString SearchStr, int StartPos,
  int Length, TSearchTypes Options)
{
  FINDTEXTW Find;
  int Flags = 0;

  Find.chrg.cpMin = StartPos;
  Find.chrg.cpMax = Find.chrg.cpMin + Length;

  if (Options.Contains(stWholeWord)) Flags |= FT_WHOLEWORD;
  if (Options.Contains(stMatchCase)) Flags |= FT_MATCHCASE;

  // have to cast to char* but it's realy wchar_t*
  Find.lpstrText = SearchStr.c_bstr();

  return (::SendMessageW(Handle, EM_FINDTEXTW, Flags, (LPARAM) &Find));
}

// Keep this for backward compatibility with old versions of YahCoLoRiZe
int __fastcall TYcEdit::FindText(const String SearchStr, int StartPos,
  int Length, TSearchTypes Options)
{
  return FindTextW(WideString(SearchStr), StartPos, Length, Options);
}
//---------------------------------------------------------------------------
// enhanced search function -- supports backwards searching.
// note: although RE 1.0 supports the EM_FINDTEXTEX message, it always
// searches forward.
//
bool __fastcall TYcEdit::FindTextExW(WideString text, int startPos,
            TSearchTypes2 searchOptions, int& foundPos, int& foundLength)
{
  FINDTEXTEXW ft;

  /////////// S.S. Modified 5/2014 and 6/2015 /////////////////////
  ft.lpstrText = text.c_bstr();
  ft.chrg.cpMin = startPos;

  if (foundLength == -1)
    ft.chrg.cpMax = -1; // Search all...
  else
  {
    if (searchOptions.Contains(st2Backward))
      ft.chrg.cpMax = startPos-foundLength;
    else
      ft.chrg.cpMax = startPos+foundLength;
  }

  // This is returned with the range of text found
  ft.chrgText.cpMin = -1;
  ft.chrgText.cpMax = -1;

  ///////////////////////////////////////////////////////

  WPARAM flags = 0;
  if (!searchOptions.Contains(st2Backward)) flags |= FR_DOWN;    // and this is documented WHERE???
  if (searchOptions.Contains(st2MatchCase)) flags |= FR_MATCHCASE;  // apparently could use FR_MATCHCASE
  if (searchOptions.Contains(st2WholeWord)) flags |= FR_WHOLEWORD;  // apparently could use FR_WHOLEWORD

  int pos = ::SendMessageW(Handle, EM_FINDTEXTEXW, flags, (LPARAM) &ft);

  if (pos != -1)
  {
    foundPos = ft.chrgText.cpMin;
    foundLength = ft.chrgText.cpMax - foundPos;
  }

  return pos != -1;
}

// Keep this for backward compatibility with old versions of YahCoLoRiZe
bool __fastcall TYcEdit::FindTextEx(String text, int startPos,
  TSearchTypes2 searchOptions, int& foundPos, int& foundLength)
{
  return FindTextExW(WideString(text), startPos, searchOptions,
                                                  foundPos, foundLength);
}
//---------------------------------------------------------------------------
// set word wrap to none, window, or printer page -- note that there is no way
// for SetWordWrapToPrinter() to handle pages of different widths so
// applications that require pages to have different rendering widths should
// not support SetWordWrapToPrinter()
//
void TYcEdit::SetWordWrapToPrinter(void)
{
  if (FYcPrint)
  {
     TRect rect = FYcPrint->TargetRendRect;
     ::SendMessage(Handle, EM_SETTARGETDEVICE, (WPARAM) Printer()->Handle,
                                          (LPARAM) (rect.Right - rect.Left));
  }
}
//---------------------------------------------------------------------------
// set word wrap to none, window, or printer page
//
void __fastcall TYcEdit::SetWordWrapTo(TWordWrapTo wwtType)
{
  // if trying to wrap to printer width, must have a printer...
  if (wwtType == wwtPrinter && !Printer()->Handle) wwtType = wwtNone;

  // if wrapping to printer width, set the wrapping mode, and redraw
  if (wwtType == wwtPrinter)
    SetWordWrapToPrinter();
  // otherwise, clear the wrapping mode
  else
    ::SendMessage(Handle, EM_SETTARGETDEVICE, 0, wwtType == wwtWindow ? 0 : 1);

  // only need horizontal scroll bars if wrapping to window
  if (FScrollBars == ssNone)
    ScrollBars = ssNone;
  else if (FScrollBars == ssVertical)
    ScrollBars = ssVertical;
  else if (FScrollBars == Vcl::Stdctrls::ssHorizontal)
    ScrollBars = (wwtType == wwtWindow) ? ssVertical : ssNone;
  else if (FScrollBars == ssBoth)
    ScrollBars = (wwtType == wwtWindow) ? ssVertical : ssBoth;

  FWordWrapTo = wwtType;
}
//---------------------------------------------------------------------------
// set ScrollBars
//void __fastcall TYcEdit::SetScrollBars(TScrollStyle Value)
//{
//  FScrollBars = Value;
//  ScrollBars = FScrollBars;
//}
//---------------------------------------------------------------------------
void __fastcall TYcEdit::SetPrintSupport(bool Value)
{
  if (Value == true)
  {
    if (!FYcPrint)
      FYcPrint = new TYcPrint(this);
  }
  else if (FYcPrint)
  {
    delete FYcPrint;
    FYcPrint = NULL;
  }
}
//---------------------------------------------------------------------------
// hide or show the scrollbars
//
void __fastcall TYcEdit::SetHideScrollBars(bool Value)
{
  if (HideScrollBars == Value) return;
  FHideScrollBars = Value;
//  RecreateWnd();
}
//---------------------------------------------------------------------------
// hide or show the selection (if true, the selection highlight disappears
// when the focus is moved to another control or window; if false, the
// selection remains highlighted even if focus is removed.
//
void __fastcall TYcEdit::SetHideSelection(bool Value)
{
  if (HideSelection == Value) return;
  FHideSelection = Value;
  ::SendMessage(Handle, EM_HIDESELECTION, HideSelection, (LPARAM) true);
}
//---------------------------------------------------------------------------


