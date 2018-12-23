//===========================================================================
// Copyright © 1999 Thin Air Enterprises and Robert Dunn.  All rights reserved.
// Free for non-commercial use.  Commercial use requires license agreement.
// See http://home.att.net/~robertdunn/Yacs.html for the most current version.
//===========================================================================
//---------------------------------------------------------------------------
// TaeRichEditAdvPrint.h - header file for TaeRichEditAdvPrint.cpp (advanced
// print and print preview functionality).
//
// note:  the TRichEditAdvPrint class works in TWIPs, not pixels or some other
// unit of measure.  the TRichEditPrint class includes methods to convert
// between TWIPs and other units.
//---------------------------------------------------------------------------
#ifndef YcPrintH
#define YcPrintH

#include "YcCustomPrint.h"
#include "YcPageLayout.h"
//---------------------------------------------------------------------------
typedef struct tagTYcPageRects
{
  TRect HeaderBorder;
  TRect BodyBorder;
  TRect FooterBorder;
  TRect HeaderRender;
  TRect BodyRender;
  TRect FooterRender;
} TYcPageRects;

class PACKAGE TYcPrint : public TYcCustomPrint
{
friend class PACKAGE TYcEdit;

protected:
  TYcHeaderText* FHeader;
  TYcHeaderText* FFooter;
  TYcHeaderText* FFirstHeader;
  TYcHeaderText* FFirstFooter;
  bool FUseFirstHeader;
  bool FNeedToCalcDrawRects;
  TYcBorderLines FBorders;
  int FInsideMargin;
  int FBorderWidth;

  void __fastcall SetHeader(TYcHeaderText* header) { SetAHeader(FHeader, header); };
  void __fastcall SetFooter(TYcHeaderText* footer) { SetAHeader(FFooter, footer); };
  void __fastcall SetFirstHeader(TYcHeaderText* header) { SetAHeader(FFirstHeader, header); };
  void __fastcall SetFirstFooter(TYcHeaderText* footer) { SetAHeader(FFirstFooter, footer); };
  void __fastcall SetAHeader(TYcHeaderText* headerFooter, TYcHeaderText* newHeaderFooter);

  virtual TRect __fastcall GetRendRect(HDC hdc, int page);
  virtual void DrawMargins(HDC hdc, int page, TRect margins);

  virtual void CalcRects(void);
  void CalcDrawRects(HDC hdc);
  void CalcRectsForPage(TYcHeaderText* header, TYcHeaderText* footer,
    TYcPageRects& pageRects, TYcBorderLines borderLines, TRect renderRect);
  int GetTextHeight(TYcHeaderText* header);
  int DrawHeader(HDC hdc, TYcHeaderText* header, TRect rect);
  void DrawBorders(HDC hdc, TYcPageRects& pageRects, TYcBorderLines lines);
  virtual void StartPrint(void);
  virtual void AfterPage(HDC hdc, int page);

public:
  __fastcall TYcPrint(TYcEdit* Owner);
  __fastcall ~TYcPrint();

  void SetPageStyle(TYcPageStyle& style);

__published:
  __property TYcHeaderText* Header = { read = FHeader, write = SetHeader, nodefault };
  __property TYcHeaderText* Footer = { read = FFooter, write = SetFooter, nodefault };
  __property TYcHeaderText* FirstHeader = { read = FFirstHeader, write = SetFirstHeader, nodefault };
  __property TYcHeaderText* FirstFooter = { read = FFirstFooter, write = SetFirstFooter, nodefault };
  __property bool UseFirstHeaderFooter = { read = FUseFirstHeader, write = FUseFirstHeader, default = false };
  __property TYcBorderLines Borders = { read = FBorders, write = FBorders, nodefault };
  __property int InsideMargin = { read = FInsideMargin, write = FInsideMargin, default = 100 };
  __property int BorderWidth = { read = FBorderWidth, write = FBorderWidth, default = 1 };
};
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
