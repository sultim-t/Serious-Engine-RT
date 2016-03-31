/* Copyright (c) 2002-2012 Croteam Ltd. 
This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

#include "StdH.h"
#include <Engine/Base/KeyNames.h>
#include <Engine/CurrentVersion.h>
#include <GameMP/LCDDrawing.h>
#include "MGEdit.h"

extern CSoundData *_psdPress;

extern BOOL _bEditingString;


CMGEdit::CMGEdit(void)
{
  mg_pstrToChange = NULL;
  mg_ctMaxStringLen = 70;
  Clear();
}

void CMGEdit::Clear(void)
{
  mg_iCursorPos = 0;
  mg_bEditing = FALSE;
  _bEditingString = FALSE;
}

void CMGEdit::OnActivate(void)
{
  if (!mg_bEnabled) {
    return;
  }
  ASSERT(mg_pstrToChange != NULL);
  PlayMenuSound(_psdPress);
  IFeel_PlayEffect("Menu_press");
  SetText(mg_strText);
  mg_iCursorPos = strlen(mg_strText);
  mg_bEditing = TRUE;
  _bEditingString = TRUE;
}


// focus lost
void CMGEdit::OnKillFocus(void)
{
  // go out of editing mode
  if (mg_bEditing) {
    OnKeyDown(VK_RETURN);
    Clear();
  }
  // proceed
  CMenuGadget::OnKillFocus();
}

// helper function for deleting char(s) from string
static void Key_BackDel(CTString &str, INDEX &iPos, BOOL bShift, BOOL bRight)
{
  // do nothing if string is empty
  INDEX ctChars = strlen(str);
  if (ctChars == 0) return;
  if (bRight && iPos<ctChars) {  // DELETE key
    if (bShift) {
      // delete to end of line
      str.TrimRight(iPos);
    } else {
      // delete only one char
      str.DeleteChar(iPos);
    }
  }
  if (!bRight && iPos>0) {       // BACKSPACE key
    if (bShift) {
      // delete to start of line
      str.TrimLeft(ctChars - iPos);
      iPos = 0;
    } else {
      // delete only one char
      str.DeleteChar(iPos - 1);
      iPos--;
    }
  }
}

// key/mouse button pressed
BOOL CMGEdit::OnKeyDown(int iVKey)
{
  // if not in edit mode
  if (!mg_bEditing) {
    // behave like normal gadget
    return CMenuGadget::OnKeyDown(iVKey);
  }

  // finish editing?
  BOOL bShift = GetKeyState(VK_SHIFT) & 0x8000;
  switch (iVKey) {
  case VK_UP: case VK_DOWN:
  case VK_RETURN:  case VK_LBUTTON: *mg_pstrToChange = mg_strText;  Clear(); OnStringChanged();  break;
  case VK_ESCAPE:  case VK_RBUTTON:  mg_strText = *mg_pstrToChange; Clear(); OnStringCanceled(); break;
  case VK_LEFT:    if (mg_iCursorPos > 0)                  mg_iCursorPos--;  break;
  case VK_RIGHT:   if (mg_iCursorPos < strlen(mg_strText)) mg_iCursorPos++;  break;
  case VK_HOME:    mg_iCursorPos = 0;                   break;
  case VK_END:     mg_iCursorPos = strlen(mg_strText);  break;
  case VK_BACK:    Key_BackDel(mg_strText, mg_iCursorPos, bShift, FALSE);  break;
  case VK_DELETE:  Key_BackDel(mg_strText, mg_iCursorPos, bShift, TRUE);   break;
  default:  break; // ignore all other special keys
  }

  // key is handled
  return TRUE;
}

// char typed
BOOL CMGEdit::OnChar(MSG msg)
{
  // if not in edit mode
  if (!mg_bEditing) {
    // behave like normal gadget
    return CMenuGadget::OnChar(msg);
  }
  // only chars are allowed
  const INDEX ctFullLen = mg_strText.Length();
  const INDEX ctNakedLen = mg_strText.LengthNaked();
  mg_iCursorPos = Clamp(mg_iCursorPos, 0L, ctFullLen);
  int iVKey = msg.wParam;
  if (isprint(iVKey) && ctNakedLen <= mg_ctMaxStringLen) {
    mg_strText.InsertChar(mg_iCursorPos, (char)iVKey);
    mg_iCursorPos++;
  }
  // key is handled
  return TRUE;
}

void CMGEdit::Render(CDrawPort *pdp)
{
  if (mg_bEditing) {
    mg_iTextMode = -1;
  } else if (mg_bFocused) {
    mg_iTextMode = 0;
  } else {
    mg_iTextMode = 1;
  }

  if (mg_strText == "" && !mg_bEditing) {
    if (mg_bfsFontSize == BFS_SMALL) {
      mg_strText = "*";
    } else {
      mg_strText = TRANS("<none>");
    }
    CMGButton::Render(pdp);
    mg_strText = "";
  } else {
    CMGButton::Render(pdp);
  }
}

void CMGEdit::OnStringChanged(void)
{
}

void CMGEdit::OnStringCanceled(void)
{
}