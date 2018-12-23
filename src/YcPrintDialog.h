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
// TaePrintDialog.h - header file for TYcRichEdit (class-specific print dialog).
//---------------------------------------------------------------------------
#ifndef YcPrintDialogH
#define YcPrintDialogH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>

#include "YcEditMain.h"
//---------------------------------------------------------------------------
// declaration for the TOnPageEvent type
typedef void __fastcall (__closure *TOnPageEvent)(TObject* Sender, HDC hdc,
  int page);

// forward declaration for the TYcPrintCancelDialog class
class PACKAGE TYcPrintCancelDialog;

// TYcPrintDialog class declaration -- this is a specialization of the
// VCL TPrintDialog component that initializes the print dialog with page
// ranges (start and end pages) that are correct for the associated
// TYcRichEdit control
//
class PACKAGE TYcPrintDialog : public TPrintDialog
{
  private:
    WideString FTitle;

protected:
  TYcEdit* FRichEdit;
  TYcPrint* FRichEditPrint;
  TOnPageEvent FOnAfterPage;
  TNotifyEvent FOnEndPrint;
  TYcPrintCancelDialog* FPrintCancelDlg;

  void __fastcall PageEnded(TObject* Sender, HDC hdc, int page);
  void __fastcall PrintingEnded(TObject* Sender);
  void __fastcall AbortPrint(TObject* Sender);
  virtual void __fastcall Notification(TComponent* AComponent,
    TOperation Operation);

public:
  __fastcall TYcPrintDialog(TYcEdit* Owner);
  __fastcall ~TYcPrintDialog(void);
  virtual bool __fastcall TaeExecute(WideString wTitle = "");

__published:
  __property TYcEdit* TaeRichEdit = { read = FRichEdit, write = FRichEdit,
    default = 0 };
  __property WideString Title = { read = FTitle, write = FTitle };
};
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
