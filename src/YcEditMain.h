//---------------------------------------------------------------------------
#ifndef YcEditMainH
#define YcEditMainH

#include <Classes.hpp>
#include <StdCtrls.hpp>
#include <Math.hpp>

#include "YcPrint.h"
#include "YcPosition.h"

enum TSearchType2 { st2WholeWord, st2MatchCase, st2Backward };
typedef Set<TSearchType2, st2WholeWord, st2Backward> TSearchTypes2;

typedef int TYcTabStops[MAX_TAB_STOPS];
typedef enum { wwtNone, wwtWindow, wwtPrinter } TWordWrapTo;

typedef void __fastcall (__closure *TYcEditUrlClick)(TObject* Sender, String urlText);
typedef void __fastcall (__closure *TYcEditProtectEvent)(TObject* Sender, TWMNotify& Message);

class PACKAGE TYcEdit : public TCustomRichEdit
{
  // forward declarations
//  friend class PACKAGE TYcPrint;
//  friend class TYcPrint;
  private:
    AnsiString __fastcall GetRtf(bool selectionOnly);
    void __fastcall PutRtf(AnsiString rtfText, bool replaceSelection);

    AnsiString __fastcall GetTextRtf(void);
    void __fastcall SetTextRtf(AnsiString rtfText);
    AnsiString __fastcall GetSelTextRtf(void);
    void __fastcall SetSelTextRtf(AnsiString rtfText);
    WideString __fastcall GetTextW(void);
    void __fastcall SetTextW(WideString wText);
    WideString __fastcall GetSelTextW(void);
    void __fastcall SetSelTextW(WideString wSelText);
    int __fastcall GetTabStops(TYcTabStops& tabStops);
    void __fastcall SetTabStops(bool entireDocument = true); // S.S. (must set FTabWidth first!)
    void __fastcall SetTabStops(int tabCount, TYcTabStops &tabStops);
    void __fastcall SetTabStops(int tabStopTwips);
    void __fastcall SetTabStops(int tabWidth, TFont* font, bool entireDocument = true);
    int __fastcall GetUndoLimit(void);
    void __fastcall SetUndoLimit(int value);
    int __fastcall GetSelStart(void);
    void __fastcall SetSelStart(int Value);
    long __fastcall GetColumn(void);
    void __fastcall SetColumn(long column);
    long __fastcall GetLine(void);
    void __fastcall SetLine(long Value);
    TPoint __fastcall GetScrollPos(void);
    void __fastcall SetScrollPos(TPoint pt);
    void __fastcall SetHideSelection(bool Value);
    void __fastcall SetHideScrollBars(bool Value);
    void __fastcall SetScrollBars(TScrollStyle Value);
    TPoint __fastcall GetCaretPos(void);
    int __fastcall GetTabWidthTwips(void);
    void __fastcall SetTabWidth(int tabWidth);
    long __fastcall GetTextLength(void);
    void __fastcall SetSelLength(int Value);
    int __fastcall GetSelLength(void);
    void __fastcall SetTabCount(int tabCount);
    void __fastcall SetPosition(YCPOSITION pos);
    YCPOSITION __fastcall GetPosition(void);
    void SetWordWrapToPrinter(void);
    void __fastcall SetWordWrapTo(TWordWrapTo wwtType);
    TWordWrapTo __fastcall GetWordWrapTo(void);
    void __fastcall SetPrintSupport(bool Value);
    void __fastcall Init(TComponent* Owner, String sName);
    void __fastcall ToggleInsertMode(void);
    void __fastcall SetInsertMode(bool value);
    void __fastcall SetAutoUrlDetect(bool value);
    void __fastcall SetOnUrlClick(TNotifyEvent value);
    void __fastcall SetEnableNotifications(bool enable);
    void __fastcall ExecuteUrl(TObject* Sender, String urlText);

    TYcEditProtectEvent FOnLinkEvent;
    TYcEditUrlClick FOnUrlClick;
    TWordWrapTo FWordWrapTo;
    TYcPrint* FYcPrint;
    bool FAutoUrlExecute;

    int FUndoLimit, FUndoLimitActual;
    int FTabCount, FTabWidth; // with in chars
    int FLockCounter, FView;
    long FOldLineCount, FOldLength;
    bool FInsertMode, FRestoreSel, FAutoUrlDetect;
    bool FPrintSupport, FHideScrollBars, FHideSelection;
    bool FEnableNotifications;

    Classes::TNotifyEvent FOnInsertModeChange;
    Classes::TNotifyEvent FSaveOnChange;

    TScrollStyle FScrollBars;
    YCPOSITION FSavePos;
    TYcTabStops FTabStops;
    String FFileName; // storage space for the file being edited (if any)

  protected:
    static DWORD CALLBACK StreamOutCallback(DWORD dwCookie, LPBYTE pbBuff,
                                                LONG cb, LONG FAR *pcb);
    static DWORD CALLBACK StreamInCallback(DWORD dwCookie, LPBYTE pbBuff,
                                                LONG cb, LONG FAR *pcb);

  DYNAMIC void __fastcall KeyDown(Word &Key, Classes::TShiftState Shift);

  MESSAGE int __fastcall CNNotify(Messages::TWMNotify &Message);

  //virtual void __fastcall CreateWnd(void);

  // many days were wasted before I realized that I had to add the following
  // message maps.  perhaps it was obvious, but I incorrectly assumed that
  // the message handlers would be used if declared in the base....
  BEGIN_MESSAGE_MAP
//    VCL_MESSAGE_HANDLER(WM_NCDESTROY, TWMNCDestroy, WMNCDestroy)
//    VCL_MESSAGE_HANDLER(CM_BIDIMODECHANGED, TMessage, CMBiDiModeChanged)
//    VCL_MESSAGE_HANDLER(CM_FONTCHANGED, TMessage, CMFontChanged)
//    VCL_MESSAGE_HANDLER(CM_COLORCHANGED, TMessage, CMColorChanged)
//    VCL_MESSAGE_HANDLER(CM_TEXTCHANGED, TMessage, CMTextChanged)
//    VCL_MESSAGE_HANDLER(WM_COMMAND, TWMCommand, WMCommand)
    VCL_MESSAGE_HANDLER(CN_NOTIFY, TWMNotify, CNNotify)
//    VCL_MESSAGE_HANDLER(WM_SETCURSOR, TWMSetCursor, WMSetCursor)
//    VCL_MESSAGE_HANDLER(WM_PAINT, TWMPaint, WMPaint)
//    VCL_MESSAGE_HANDLER(WM_SETFONT, TWMSetFont, WMSetFont)
  END_MESSAGE_MAP(TCustomRichEdit)

  public:
    __fastcall TYcEdit(TComponent* Owner); // constructor
    __fastcall TYcEdit(TComponent* Owner, String sName); // constructor
    __fastcall ~TYcEdit();

    // static class members
    static const AnsiString Untitled;

    int __fastcall GetTabWidthTwipsEx(int tabWidth, TFont* font);
    long __fastcall GetLineByIndex(long Index);
    long __fastcall GetColumnByIndex(long Index);
    TPoint __fastcall GetCharCoordinates(long Index);
    TPoint __fastcall GetPosByIndex(long Index);
    long int __fastcall GetCharIdx(long Line);
    void __fastcall SetSelFirstLast(long first, long last);
    void __fastcall GetSelFirstLast(long &first, long &last);
    wchar_t* __fastcall GetRangeBufW(long startPos, long endPos);
    WideString __fastcall GetRangeW(long startPos, long endPos);
    long __fastcall GetSelTextBufW(wchar_t* Buffer, long BufSize);
    long __fastcall GetTextBufW(wchar_t* Buffer, long BufSize);
    void __fastcall SetStringW(WideString wText, long LineIndex);
    WideString __fastcall GetStringW(long LineIndex);
    long __fastcall GetLineCount(void);
    void ScrollToCaret(void);
    TPoint __fastcall GetSelEndPos(void);
    TPoint __fastcall GetSelStartPos(void);
    void __fastcall DoSetMaxLength(int Value);
    void __fastcall PasteFromStream(TStream* stream, bool selectionOnly = true,
                bool plainText = false, bool plainRtf = false);
    void __fastcall CopyToStream(TStream* stream, bool selectionOnly = true,
        bool plainText = false, bool noObjects = false, bool plainRtf = false);
    void __fastcall SetLocation(int Left, int Top, int Width, int Height);
    int __fastcall FindTextW(const WideString SearchStr, int StartPos,
                                             int Length, TSearchTypes Options);
    int __fastcall FindText(const String SearchStr, int StartPos,
                                  int Length, TSearchTypes Options);
    bool __fastcall FindTextExW(WideString text, int startPos,
            TSearchTypes2 searchOptions, int& foundPos, int& foundLength);
    bool __fastcall FindTextEx(String text, int startPos,
            TSearchTypes2 searchOptions, int& foundPos, int& foundLength);

  __property TWordWrapTo WordWrapTo = { read = GetWordWrapTo, write = SetWordWrapTo };
  __property int TabWidthTwips = { read = GetTabWidthTwips };
  __property int SelLength = { read = GetSelLength, write = SetSelLength };
  __property int SelStart = { read = GetSelStart, write = SetSelStart };
  __property AnsiString SelTextRtf = { read = GetSelTextRtf, write = SetSelTextRtf };
  __property AnsiString TextRtf = { read = GetTextRtf, write = SetTextRtf };
  __property WideString SelTextW = { read = GetSelTextW, write = SetSelTextW };
  __property WideString TextW = { read = GetTextW, write = SetTextW };
  __property int UndoLimit  = { read = GetUndoLimit, write = SetUndoLimit };
  __property long TextLength = { read = GetTextLength }; // S.S. 5/28/15
  __property long LineCount = { read = GetLineCount };
  __property TPoint ScrollPos = { read = GetScrollPos, write = SetScrollPos };
  __property TPoint CaretPos = { read = GetCaretPos, write = SetCaretPos };
  __property long Line = { read = GetLine, write = SetLine };
  __property long Column = { read = GetColumn, write = SetColumn };
  __property YCPOSITION Position = {read = GetPosition, write = SetPosition};
  // Don't want to restore SelStart and SelLength if it's become "unknown",
  // so the invoking class clears this flag...
  __property int TabWidth = { read = FTabWidth, write = SetTabWidth };
  __property int TabCount = { read = FTabCount, write = SetTabCount };
  __property YCPOSITION SavePos = {read = FSavePos, write = FSavePos};
  __property long OldLineCount = { read = FOldLineCount, write = FOldLineCount };
  __property long OldLength = { read = FOldLength, write = FOldLength };
  __property int LockCounter = { read = FLockCounter, write = FLockCounter };
  __property int View = { read = FView, write = FView };
  __property bool RestoreSel = {write = FRestoreSel};
  __property Classes::TNotifyEvent SaveOnChange = { read = FSaveOnChange, write = FSaveOnChange, default = 0 };
  __property TWordWrapTo WordWrapMode = { read = FWordWrapTo, write = SetWordWrapTo, default = wwtNone };
//  __property TYcPrint* YcPrint = { read = FYcPrint, default = 0, stored = false };
  __property TYcPrint* YcPrint = { read = FYcPrint, default = 0 };
  __property String FileName = { read = FFileName,  write = FFileName };
  __property bool EnableNotifications = { read = FEnableNotifications, write = SetEnableNotifications, default = true, stored = false };

__published:

//
// New custom properties
//
  __property bool PrintSupport  = { read = FPrintSupport, write = SetPrintSupport, default = false };
  __property bool InsertMode  = { read = FInsertMode, write = SetInsertMode, default = true };
  __property bool AutoUrlDetect  = { read = FAutoUrlDetect, write = SetAutoUrlDetect, default = false };
  __property bool AutoUrlExecute = { read = FAutoUrlExecute, write = FAutoUrlExecute, default = true };

  __property TNotifyEvent OnInsertModeChange  = { read = FOnInsertModeChange, write = FOnInsertModeChange, default = 0 };
  __property TYcEditUrlClick OnUrlClick = { read = FOnUrlClick, write = FOnUrlClick,  default = 0 };
//------------------------------------
// publish inherited properties
//
  __property Align;
  __property Alignment;
  __property Anchors;
  __property BiDiMode;
  __property BorderStyle;
  __property BorderWidth;
  __property CanUndo;
  __property Color;
  __property Constraints;
  __property Ctl3D;
  __property DragCursor;
  __property DragKind;
  __property DragMode;
  __property Enabled;
  __property Font;
  __property HideScrollBars;
  __property ScrollBars;
  __property HideSelection;
  __property ImeMode;
  __property ImeName;
  __property Lines;
  __property MaxLength;
  __property Name;
  __property ParentBiDiMode;
  __property ParentColor;
  __property ParentCtl3D;
  __property ParentFont;
  __property ParentShowHint;
  __property PlainText;
  __property PopupMenu;
  __property ReadOnly;
  __property ShowHint;
  __property TabOrder;
  __property TabStop;
  __property Visible;
  __property WantTabs;
  __property WantReturns;
  __property WordWrap;
  __property OnChange; // used
  __property OnDragDrop;
  __property OnDragOver;
  __property OnEndDock;
  __property OnEndDrag;
  __property OnEnter;
  __property OnExit;
  __property OnKeyDown; // used
  __property OnKeyPress; // used
  __property OnKeyUp; // used
  __property OnMouseDown;
  __property OnMouseMove;
  __property OnMouseUp;
  __property OnMouseWheel;
  __property OnMouseWheelDown;
  __property OnMouseWheelUp;
  __property OnStartDock;
  __property OnStartDrag;
};
//---------------------------------------------------------------------------
#endif
