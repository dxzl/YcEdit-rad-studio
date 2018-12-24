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
// YcParser.h - header file for YcParser.cpp (routines used to substitute
// text in page headers and footers).
//---------------------------------------------------------------------------
#ifndef YcParserH
#define YcParserH

#include <sysdefs.h>
#include <dstring.h>

//---------------------------------------------------------------------------
// TYcParserBase - base class for text substitution parser
//
class PACKAGE TYcParserBase {
protected:
  TStringList* FOptList;
  TStringList* FSubstList;

  bool Find(String option, int& i);

public:
  TYcParserBase(void);
  ~TYcParserBase();
  void Add(String option, String value = "");
  bool Has(String option);
  bool Remove(String option);
  bool Change(String option, String newValue);
  bool Compare(String str, String subStr, bool ignoreCase);
  int CompareFind(String s, bool ignoreCase);
  virtual String Parse(String format, bool ignoreCase = true);
};

// TYcParser class - main text substitution parser class
class PACKAGE TYcParser : public TYcParserBase
{
protected:
  String FDateFormat;
  String FTimeFormat;

  void SetDateTime(String dateOpt, String timeOpt, TDateTime dateTime);

public:
  TYcParser(String dateFormat, String timeFormat);
  void SetName(String name, bool autoInit = true);
  void SetTitle(String title);
  void SetPage(int page);
  void SetPages(int pages);
  void SetCreated(TDateTime dateTime);
  void SetModified(TDateTime dateTime);
  void SetNow(void);
};
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
