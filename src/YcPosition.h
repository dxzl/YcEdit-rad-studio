// I believe we can safely construe that this software may be released under
// the Free Software Foundation's GPL - I adopted TaeRichEdit for my project
// more than 15 years ago and have made significant changes in various
// places. Below is the original license. (Scott Swift 2015 dxzl@live.com)
//---------------------------------------------------------------------------
// TaeVerInfo.h - header file for TaeVerInfo.cpp.
// Author: Scott Swift
//---------------------------------------------------------------------------
#ifndef YcPositionH
#define YcPositionH
#include <comctrls.hpp>
//---------------------------------------------------------------------------
class PACKAGE TYcEdit;

#pragma pack(push, 4)
typedef struct
{
  long start;
  long end;
  int firstVisLine;
  TPoint p;

  void YCPOSITION(void)
  {
    start = 0L;
    end = 0L;
    firstVisLine = 0;
    p.y = 0;
    p.x = 0;
  }

} YCPOSITION;
#pragma pack(pop)

#pragma pack(push, 4)
class PACKAGE TYcPosition
{
  friend class PACKAGE TYcEdit;

  protected:
    YCPOSITION savePos;
    HWND re_handle;
    bool bRestoreSel;

    void __fastcall SetPosition(YCPOSITION pos);
    YCPOSITION __fastcall GetPosition(void);

  public:
    __fastcall TYcPosition(TYcEdit* re);

    __property YCPOSITION Position = {read = GetPosition,
                                             write = SetPosition};
    __property YCPOSITION SavePos = {read = savePos, write = savePos};

    // Don't want to restore SelStart and SelLength if it's become "unknown,
    // so the invoking class clears this flag...
    __property bool RestoreSel = {write = bRestoreSel};
};
#pragma pack(pop)

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
