// I believe we can safely construe that this software may be released under
// the Free Software Foundation's GPL - I adopted TaeRichEdit for my project
// more than 15 years ago and have made significant changes in various
// places. Below is the original license. (Scott Swift 2015 dxzl@live.com)
//===========================================================================
// Copyright © 1999 Thin Air Enterprises and Robert Dunn.  All rights reserved.
// Free for non-commercial use.  Commercial use requires license agreement.
// See http://home.att.net/~robertdunn/Yacs.html for the most current version.
//===========================================================================
//---------------------------------------------------------------------------
// TaePageLayout.h - header file for TaePageLayout (classes involved
// in page formatting (headers, footers, etc.).
//---------------------------------------------------------------------------
#ifndef YcPageLayoutH
#define YcPageLayoutH

#include "YcRegistry.h"

#include <vector>
//---------------------------------------------------------------------------
// TYcHeaderText class -- used for headers *and * footers
//
class PACKAGE TYcHeaderText : public TPersistent {
protected:
//public:
  TStringList* FLeft;
  TStringList* FCenter;
  TStringList* FRight;
  TFont* FFont;  // TYcHeaderText owns only the font it creates -- others are TFont::Assign()'ed

  void __fastcall SetText(AnsiString text);
  void __fastcall SetLeft(TStringList* stringList) { SetStringList(FLeft, stringList); };
  void __fastcall SetCenter(TStringList* stringList) { SetStringList(FCenter, stringList); };
  void __fastcall SetRight(TStringList* stringList) { SetStringList(FRight, stringList); };
  void __fastcall SetStringList(TStringList* list, TStringList* newList);
  void __fastcall SetFont(TFont* font);

public:
  __fastcall TYcHeaderText(void);
  virtual __fastcall ~TYcHeaderText(void);
  __property AnsiString Text = { read = GetText, write = SetText };

  // assignment operator
  TYcHeaderText& operator=(TYcHeaderText& header);
  // copy constructor
  TYcHeaderText(const TYcHeaderText& header);
  AnsiString __fastcall GetText(void);

  void LoadFromRegistry(TRegistry& reg, AnsiString name);
  void SaveToRegistry(TRegistry&reg, AnsiString name);

__published:
  __property TStringList* Left = { read = FLeft, write = SetLeft, nodefault };
  __property TStringList* Center = { read = FCenter, write = SetCenter, nodefault };
  __property TStringList* Right = { read = FRight, write = SetRight, nodefault };
  __property TFont* Font = { read = FFont, write = SetFont, nodefault };
};

// TYcBorderLine enum -- used to specify borders for headers, footers
// and body text
typedef enum { blHeaderLeft, blHeaderTop, blHeaderRight, blHeaderBottom,
  blBodyLeft, blBodyTop, blBodyRight, blBodyBottom,
  blFooterLeft, blFooterTop, blFooterRight, blFooterBottom
  } TYcBorderLine;
typedef Set<TYcBorderLine, blHeaderLeft, blFooterBottom> TYcBorderLines;

// TYcPageStyle -- aggregates header & footer text, border styles, and
// margins for pages.  used to define attributes for first page and
// all remaining pages.
class PACKAGE TYcPageStyle {
protected:
public:
  AnsiString FName;
  TYcHeaderText* FFirstHeader;
  TYcHeaderText* FFirstFooter;
  TYcHeaderText* FHeader;
  TYcHeaderText* FFooter;
  TYcBorderLines FBorderLines;
  int FBorderWidth;
  int FInsideMargin;
  bool FDifferentFirstPage;

  TYcPageStyle(void);
  ~TYcPageStyle();

  // assignment operator
  TYcPageStyle& operator=(TYcPageStyle& style);
  // copy constructor
  TYcPageStyle(const TYcPageStyle& style);

  void LoadFromRegistry(TRegistry& reg, AnsiString name);
  void SaveToRegistry(TRegistry& reg, AnsiString name);
  static void RemoveFromRegistry(TRegistry& reg, AnsiString name);
};

// TYcPageStyles list typedef -- used to store a list of program-defined or
// user-defined page styles.  typically presented to users for selection of
// style to use for the current document, for example.
typedef std::vector<TYcPageStyle*> TYcPageStyles;

// TYcPageStyleList class -- manages a list of page styles
class PACKAGE TYcPageStyleList {
protected:
  AnsiString FCurrentStyleName;
  TYcPageStyles FPageStyles;

  TYcPageStyle* GetCurrentStyle(void) { return Find(FCurrentStyleName); };
  void SetCurrentStyleName(AnsiString name) {
    if (name == "") return;
    if (Find(name)) FCurrentStyleName = name;
    };

public:
  TYcPageStyleList(void);
  ~TYcPageStyleList();
  void Clear(void);
  void Add(TYcPageStyle& style);
  TYcPageStyle* Find(AnsiString styleName);
  void Change(TYcPageStyle& style, bool add = false);
  void Delete(AnsiString styleName);
  bool StyleExists(AnsiString styleName);
  int Count(void);
  AnsiString StyleName(int index);

  void LoadFromRegistry(TRegistry& reg, AnsiString name);
  void SaveToRegistry(TRegistry& reg, AnsiString name);

  __property AnsiString CurrentStyleName = { read = FCurrentStyleName,
    write = SetCurrentStyleName };
  __property TYcPageStyle* CurrentStyle = { read = GetCurrentStyle };
};
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
