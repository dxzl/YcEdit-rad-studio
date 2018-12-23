// I believe we can safely construe that this software may be released under
// the Free Software Foundation's GPL - I adopted YcEdit for my project
// more than 15 years ago and have made significant changes in various
// places. Below is the original license. (Scott Swift 2015 dxzl@live.com)
//===========================================================================
// Copyright © 1999 Thin Air Enterprises and Robert Dunn.  All rights reserved.
// Free for non-commercial use.  Commercial use requires license agreement.
// See http://home.att.net/~robertdunn/Yacs.html for the most current version.
//===========================================================================
//---------------------------------------------------------------------------
// YcPrint.h - header file for YcPrint.cpp (print and
// print preview support).
//
// note:  the TRichEditPrint class works in TWIPs, not pixels or some other
// unit of measure.  the class includes methods to convert between TWIPs and
// other units. 
//---------------------------------------------------------------------------
#ifndef YcCustomPrintH
#define YcCustomPrintH

#include <ComCtrls.hpp>
#include <Printers.hpp>
#include <Math.hpp>
#include <vector>
#include <values.h>
//---------------------------------------------------------------------------
//typedef enum _Process_DPI_Awareness {
//  Process_DPI_Unaware            = 0,
//  Process_System_DPI_Aware       = 1,
//  Process_Per_Monitor_DPI_Aware  = 2
//} Process_DPI_Awareness;

typedef struct tagTYcPageOffset
{
  long int Start;
  long int End;
} TYcPageOffsets;

typedef struct tagTYcInfoDC
{
  long int xLogPix;
  long int yLogPix;
  long int xPhysPage;
  long int yPhysPage;
  long int xMinMargin;
  long int yMinMargin;
} TYcInfoDC;

typedef struct tagTPageOffset
{
  long int Start;
  long int End;
  RECT rendRect;
} TPageOffsets;

typedef TYcPageOffsets TYcPageRange;

typedef void __fastcall (__closure *TOnPageEvent)(TObject* Sender, HDC hdc, int page);

class TYcCustomPrint : public TPersistent
{
friend class PACKAGE TYcEdit;

private:
  TYcEdit* FRichEdit;  // the rich edit control associated with this instance
  TRect FMargins;        // requested margins in twips
  TRect FRendRect;      // render rect in twips
  TRect FPageRect;      // page rect in twips
  TPoint FPrinterOffset;    // offset of printed page in printer device units
  std::vector<TYcPageOffsets> FPageOffsets;
  HDC FRendDC;        // rendering hdc
  bool FPrinting, FShowMargins;
  int FRendWidth, FRendHeight; // in device units (pixels)
  int FScaleX, FScaleY;
  bool FAbortPrint;
  TYcInfoDC FInfo;

//  int __fastcall GetPrinterPageCount(void);
//  HDC GetTargDC(void);
  void __fastcall SetMargins(TRect margins);
  void __fastcall SetRendDC(HDC rendDC);
  void __fastcall SetShowMargins(bool showMargins);
  TRect __fastcall GetRendRect(HDC hdc, TRect rMargins, int &w, int &h);
  HDC __fastcall CreatePrinterDC(void);
  TYcInfoDC __fastcall GetInfo(void);
  TYcInfoDC __fastcall GetInfo(HDC hdc);
  int __fastcall GetPageCount(void);
  CHARRANGE __fastcall GetSelRange(void);
  int __fastcall GetSelStartPage(void);
  int __fastcall GetSelEndPage(void);
  TOnPageEvent FOnBeforePage;
  TOnPageEvent FOnAfterPage;
  TNotifyEvent FOnStartPrint;
  TNotifyEvent FOnEndPrint;

protected:
  virtual void DrawMargins(HDC hdc, int page, TRect margins);
  virtual void CalcRects(void);
  virtual TRect __fastcall GetRendRect(HDC hdc, int page);
  virtual long int FormatRange(TFormatRange& formatRange, bool render);
  virtual void ClearRenderingCache(void);
  virtual void BeforePage(HDC hdc, int page);
  virtual void AfterPage(HDC hdc, int page);
  virtual void StartPrint(void);
  virtual void EndPrint(void);

public:
  __fastcall TYcCustomPrint(TYcEdit* Owner);
  __fastcall ~TYcCustomPrint(void);
  virtual bool RenderPage(HDC hdc, int w, int h, int page);
  virtual bool PaginateTo(int page);
  ::TYcPageRange __fastcall GetOffsetPages(CHARRANGE chrg);
  ::TYcPageRange __fastcall GetOffsetPages(int begOffset = 1, int endOffset = MAXINT);

  virtual void BeginRender(int maxPages);
  virtual void EndRender(void);
  bool PrintToPrinter(WideString spoolTitle = "", int startPage = 1,
    int endPage = MAXINT, int copies = 1, bool collate = false);

  TRect TwipsToRenderRect(TRect rect);
  TRect TwipsToTargetRect(HDC hdc, TRect rect);

  __property TYcEdit* YcEdit = { read = FRichEdit };
  __property TRect Margins = { read = FMargins, write = SetMargins, nodefault };
  __property HDC RendDC = { read = FRendDC, write = SetRendDC, nodefault };
  __property TRect TargetRendRect = { read = FRendRect };
  __property TRect TargetPageRect = { read = FPageRect };
  __property int PageCount = { read = GetPageCount };
//  __property int PrinterPageCount = { read = GetPrinterPageCount };
  __property int RendWidth = { read = FRendWidth, write = FRendWidth, nodefault };
  __property int RendHeight = { read = FRendHeight, write = FRendHeight, nodefault };
  __property int SelStartPage = { read = GetSelStartPage };
  __property int SelEndPage = { read = GetSelEndPage };
  __property int ScaleX = { read = FScaleX, write = FScaleX };
  __property int ScaleY = { read = FScaleY, write = FScaleY };
  __property bool AbortPrint = { read = FAbortPrint, write = FAbortPrint, nodefault };
  __property bool Printing = { read = FPrinting };

__published:
  __property bool ShowMargins = { read = FShowMargins, write = SetShowMargins, nodefault };
  __property TOnPageEvent OnBeforePage = { read = FOnBeforePage, write = FOnBeforePage, nodefault };
  __property TOnPageEvent OnAfterPage = { read = FOnAfterPage, write = FOnAfterPage, nodefault };
  __property TNotifyEvent OnStartPrint = { read = FOnStartPrint, write = FOnStartPrint, nodefault };
  __property TNotifyEvent OnEndPrint = { read = FOnEndPrint, write = FOnEndPrint, nodefault };
};
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
