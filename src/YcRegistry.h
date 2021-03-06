// I believe we can safely construe that this software may be released under
// the Free Software Foundation's GPL - I adopted TaeRichEdit for my project
// more than 15 years ago and have made significant changes in various
// places. Below is the original license. (Scott Swift 2015 dxzl@live.com)
//===========================================================================
// Copyright � 1999 Thin Air Enterprises and Robert Dunn.  All rights reserved.
// Free for non-commercial use.  Commercial use requires license agreement.
// See http://home.att.net/~robertdunn/Yacs.html for the most current version.
//===========================================================================
//---------------------------------------------------------------------------
// TaeRegistry.h - header file for TaeRegistry.cpp (routines for storing and
// loading registry information).
//---------------------------------------------------------------------------
#ifndef YcRegistryH
#define YcRegistryH

#include <registry.hpp>
#include <printers.hpp>
//---------------------------------------------------------------------------
inline TRegistry& RegValueExists(TRegistry& reg, String value)
{
  if (reg.ValueExists(value)) return reg;
  throw ERegistryException("No such value (" + value + ")");
}
void LoadFromRegistry(TRegistry& reg, String name, TFont& font);
void SaveToRegistry(TRegistry& reg, String name, TFont& font);
void LoadFromRegistry(TRegistry& reg, String name, TPrinter& printer);
void SaveToRegistry(TRegistry& reg, String name, TPrinter& printer);
void LoadFromRegistry(TRegistry& reg, String name, TStringList& list);
void SaveToRegistry(TRegistry& reg, String name, TStringList& list);
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
