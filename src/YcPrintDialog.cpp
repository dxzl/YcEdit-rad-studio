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
// TaePrintDialog.cpp - implementation file for TYcRichEdit class-specific
// print dialog.
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <values.h>
#include "YcPrintDialog.h"
#include "YcPrintCancelDlg.h"
#include "YcPrint.h"
//#include "TaeRichEditStrings.h"

#pragma package(smart_init)
//---------------------------------------------------------------------------
// build a string of "Page #" or "Pages #-##"
//
String PagesString(int startPage, int endPage)
{
  if (startPage >= endPage)
    return String::Format("Page %d", OPENARRAY(TVarRec, (startPage)));

  return String::Format("Pages %d-%d",
    OPENARRAY(TVarRec, (startPage, endPage)));
}
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
static inline void ValidCtrCheck(TYcPrintDialog *)
{
  new TYcPrintDialog(NULL);
}
//---------------------------------------------------------------------------
namespace Ycprintdialog
{
  void __fastcall PACKAGE Register()
  {
    TComponentClass classes[1] = {__classid(TYcPrintDialog)};
    RegisterComponents("YcEdit", classes, 0);
  }
}
//---------------------------------------------------------------------------
// TYcPrintDialog constructor
//
__fastcall TYcPrintDialog::TYcPrintDialog(TYcEdit* Owner)
  : TPrintDialog(Owner)
{
  FRichEdit = Owner;
  FOnAfterPage = 0;
  FOnEndPrint = 0;
  FPrintCancelDlg = new TYcPrintCancelDialog(this);
}
//---------------------------------------------------------------------------
__fastcall TYcPrintDialog::~TYcPrintDialog(void)
{
  // S.S.
  if (FPrintCancelDlg)
    delete FPrintCancelDlg;
}
//---------------------------------------------------------------------------
// override the TPrintDialog::Execute() function to add the TYcRichEdit
// specifics
//
bool __fastcall TYcPrintDialog::TaeExecute(UnicodeString uTitle)
{
  // verify that we have a TYcRichEdit to print and that it, in turn,
  // has a valid FRichEditPrint member.  simply return on failure.
  if (!FRichEdit || !FRichEdit->YcPrint) return false;

  FTitle = uTitle;
  FRichEditPrint = FRichEdit->YcPrint;

  // make sure that we have a print cancel dialog
  if (!FPrintCancelDlg) return false;

  // do this before invoking PageCount below - this clears the page-offsets
  FRichEditPrint->RendDC = Printer()->Handle;

  // fill in dialog page range stuff - note that this overrides
  // some properties that may have been set in the IDE (probably
  // should try to hide those properties)
  FromPage = 1;
  MinPage = 1;
  Copies = 1;
  Collate = false;
  MaxPage = FRichEditPrint->PageCount; // invokes GetPageCount
  ToPage = MaxPage;

  Options.Clear();

  if (FRichEdit->SelLength)
  {
    Options << poSelection << poPageNums; // Enable page-nums range and select
    PrintRange << prSelection;
  }
  else if (MaxPage == 1)
    PrintRange << prAllPages;
  else
  {
    Options << poPageNums; // Enable page-nums range
    PrintRange << prAllPages;
  }

  // if selection range active in rich edit, allow printing of selection
  // if (???) Options = Options << poSelection;
  // verify that margins do not exceed printable page here???
  // fill selection and page ranges here???
  if (TPrintDialog::Execute())
  {
    if (!FRichEdit->LineCount)
      return false;


    int startPage, endPage;
    String s;

    switch (PrintRange)
    {
      case prSelection:
        // In TaeRichEditPrint.cpp, calls GetSelStartPage() which
        // calls GetOffsetPages() (S.S.)
        startPage = FRichEditPrint->SelStartPage;
        endPage = FRichEditPrint->SelEndPage;
        s = "Selection (" + PagesString(startPage, endPage) + ")";
        break;
      case prPageNums:
        startPage = FromPage;
        endPage = ToPage;
        s = "Range (" + PagesString(startPage, endPage) + ")";
        break;
      case prAllPages:
        startPage = 1;
        endPage = MaxPage;
        s = "All (" + PagesString(startPage, endPage) + ")";
        break;
    }

    // set up the cancel/status dialog
    FPrintCancelDlg->DocName = FRichEdit->FileName;
    FPrintCancelDlg->DocFile = FRichEdit->FileName;
    FPrintCancelDlg->PrintRange = s;
    FPrintCancelDlg->PagesToPrint = (endPage-startPage)+1;
    FPrintCancelDlg->PagesPrinted = 0;
    FPrintCancelDlg->TargetPrinter = Printer();
    FPrintCancelDlg->OnAbortPrint = AbortPrint;

    // set up events
    FOnAfterPage = FRichEditPrint->OnAfterPage;
    FOnEndPrint = FRichEditPrint->OnEndPrint;
    FRichEditPrint->OnAfterPage = PageEnded;
    FRichEditPrint->OnEndPrint = PrintingEnded;

    // bring up the cancel/status dialog
    bool aborted = false;

    try
    {
      FPrintCancelDlg->Show();

      // and kick off the printing
      aborted = FRichEditPrint->PrintToPrinter(FTitle, startPage,
                                            endPage, Copies, Collate);
    }
    catch (EExternalException& e)
    {
      Application->MessageBox(e.Message.w_str(), L"External Exception",
        MB_OK | MB_ICONSTOP);
    }
    catch (EPrinter& e)
    {
      Application->MessageBox(e.Message.w_str(), L"Printer Exception",
        MB_OK | MB_ICONSTOP);
    }
    catch (...)
    {
      Application->MessageBox(L"Got no idea what went wrong...",
        L"Unknown Exception", MB_OK | MB_ICONSTOP);
    }

    // if aborted, remind user that he did it
    if (aborted) Application->MessageBox(L"Print cancelled by user",
      Application->Title.w_str(), MB_OK | MB_ICONINFORMATION);

    // restore events
    FOnAfterPage = 0;
    FOnEndPrint = 0;
    FRichEditPrint->OnAfterPage = FOnAfterPage;
    FRichEditPrint->OnEndPrint = FOnEndPrint;

    FPrintCancelDlg->Hide();

    return true;
  }

  return false;
}
//---------------------------------------------------------------------------
// tell FRichEditPrint to stop printing
//
void __fastcall TYcPrintDialog::AbortPrint(TObject* Sender)
{
  FRichEditPrint->AbortPrint = true;
}
//---------------------------------------------------------------------------
// tell FPrintCancelDlg to update pages printed and give the OnAfterPage
// handler a chance to cancel remaining print
//
void __fastcall TYcPrintDialog::PageEnded(TObject* Sender, HDC hdc, int page)
{
  FPrintCancelDlg->PagesPrinted = page;

  if (FOnAfterPage)
    FOnAfterPage(Sender, hdc, page);
}
//---------------------------------------------------------------------------
// printing has ended -- notify the OnEndPrint handler (if any) and re-enable
// the main form
//
void __fastcall TYcPrintDialog::PrintingEnded(TObject* Sender)
{
  if (FOnEndPrint)
    FOnEndPrint(Sender);

  TCustomForm* form = GetParentForm(FRichEdit);

  if (form)
    form->Enabled = true;

  FPrintCancelDlg->Hide();
}
//---------------------------------------------------------------------------
// apparently the IDE does not automatically fix up pointers to other
// components when they become invalid (deleted from the IDE) so handle it
// here...
//
void __fastcall TYcPrintDialog::Notification(TComponent* AComponent,
  TOperation Operation)
{
  if (Operation == opRemove && AComponent == FRichEdit)
    FRichEdit = 0;
}
//---------------------------------------------------------------------------

