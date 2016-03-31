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
#include "MGKeyDefinition.h"

extern CSoundData *_psdSelect;
extern CSoundData *_psdPress;

extern BOOL _bDefiningKey;


CMGKeyDefinition::CMGKeyDefinition(void)
{
  mg_iState = DOING_NOTHING;
}

void CMGKeyDefinition::OnActivate(void)
{
  PlayMenuSound(_psdPress);
  IFeel_PlayEffect("Menu_press");
  SetBindingNames(/*bDefining=*/TRUE);
  mg_iState = RELEASE_RETURN_WAITING;
}

BOOL CMGKeyDefinition::OnKeyDown(int iVKey)
{
  // if waiting for a key definition
  if (mg_iState == PRESS_KEY_WAITING) {
    // do nothing
    return TRUE;
  }

  // if backspace pressed
  if (iVKey == VK_BACK) {
    // clear both keys
    DefineKey(KID_NONE);
    // message is processed
    return TRUE;
  }

  return CMenuGadget::OnKeyDown(iVKey);
}

// set names for both key bindings
void CMGKeyDefinition::SetBindingNames(BOOL bDefining)
{
  // find the button
  INDEX ict = 0;
  INDEX iDik = 0;
  FOREACHINLIST(CButtonAction, ba_lnNode, _pGame->gm_ctrlControlsExtra.ctrl_lhButtonActions, itba) {
    if (ict == mg_iControlNumber) {
      CButtonAction &ba = *itba;
      // get the current bindings and names
      INDEX iKey1 = ba.ba_iFirstKey;
      INDEX iKey2 = ba.ba_iSecondKey;
      BOOL bKey1Bound = iKey1 != KID_NONE;
      BOOL bKey2Bound = iKey2 != KID_NONE;
      CTString strKey1 = _pInput->GetButtonTransName(iKey1);
      CTString strKey2 = _pInput->GetButtonTransName(iKey2);

      // if defining
      if (bDefining) {
        // if only first key is defined
        if (bKey1Bound && !bKey2Bound) {
          // put question mark for second key
          mg_strBinding = strKey1 + TRANS(" or ") + "?";
        // otherwise
        } else {
          // put question mark only
          mg_strBinding = "?";
        }
        // if not defining
      }
      else {
        // if second key is defined
        if (bKey2Bound) {
          // add both
          mg_strBinding = strKey1 + TRANS(" or ") + strKey2;
        // if second key is undefined
        } else {
          // display only first one
          mg_strBinding = strKey1;
        }
      }
      return;
    }
    ict++;
  }

  // if not found, put errorneous string
  mg_strBinding = "???";
}

void CMGKeyDefinition::Appear(void)
{
  SetBindingNames(/*bDefining=*/FALSE);
  CMenuGadget::Appear();
}

void CMGKeyDefinition::Disappear(void)
{
  CMenuGadget::Disappear();
}

void CMGKeyDefinition::DefineKey(INDEX iDik)
{
  // for each button in controls
  INDEX ict = 0;
  FOREACHINLIST(CButtonAction, ba_lnNode, _pGame->gm_ctrlControlsExtra.ctrl_lhButtonActions, itba) {
    CButtonAction &ba = *itba;
    // if it is this one
    if (ict == mg_iControlNumber) {
      // if should clear
      if (iDik == KID_NONE) {
        // unbind both
        ba.ba_iFirstKey = KID_NONE;
        ba.ba_iSecondKey = KID_NONE;
      }
      // if first key is unbound, or both keys are bound
      if (ba.ba_iFirstKey == KID_NONE || ba.ba_iSecondKey != KID_NONE) {
        // bind first key
        ba.ba_iFirstKey = iDik;
        // clear second key
        ba.ba_iSecondKey = KID_NONE;
      // if only first key bound
      } else {
        // bind second key
        ba.ba_iSecondKey = iDik;
      }
      // if it is not this one
    } else {
      // clear bindings that contain this key
      if (ba.ba_iFirstKey == iDik) {
        ba.ba_iFirstKey = KID_NONE;
      }
      if (ba.ba_iSecondKey == iDik) {
        ba.ba_iSecondKey = KID_NONE;
      }
    }
    ict++;
  }

  SetBindingNames(/*bDefining=*/FALSE);
}

void CMGKeyDefinition::Think(void)
{
  if (mg_iState == RELEASE_RETURN_WAITING)
  {
    _bDefiningKey = TRUE;
    extern BOOL _bMouseUsedLast;
    _bMouseUsedLast = FALSE;
    _pInput->SetJoyPolling(TRUE);
    _pInput->GetInput(FALSE);
    if (_pInput->IsInputEnabled() &&
      !_pInput->GetButtonState(KID_ENTER) &&
      !_pInput->GetButtonState(KID_MOUSE1))
    {
      mg_iState = PRESS_KEY_WAITING;
    }
  }
  else if (mg_iState == PRESS_KEY_WAITING)
  {
    _pInput->SetJoyPolling(TRUE);
    _pInput->GetInput(FALSE);
    for (INDEX iDik = 0; iDik<MAX_OVERALL_BUTTONS; iDik++)
    {
      if (_pInput->GetButtonState(iDik))
      {
        // skip keys that cannot be defined
        if (iDik == KID_TILDE) {
          continue;
        }
        // if escape not pressed
        if (iDik != KID_ESCAPE) {
          // define the new key
          DefineKey(iDik);
          // if escape pressed
        } else {
          // undefine the key
          DefineKey(KID_NONE);
        }

        // end defining loop
        mg_iState = DOING_NOTHING;
        _bDefiningKey = FALSE;
        // refresh all buttons
        pgmCurrentMenu->FillListItems();
        break;
      }
    }
  }
}

void CMGKeyDefinition::Render(CDrawPort *pdp)
{
  SetFontMedium(pdp);

  PIXaabbox2D box = FloatBoxToPixBox(pdp, mg_boxOnScreen);
  PIX pixIL = box.Min()(1) + box.Size()(1)*0.45f;
  PIX pixIR = box.Min()(1) + box.Size()(1)*0.55f;
  PIX pixJ = box.Min()(2);

  COLOR col = GetCurrentColor();
  pdp->PutTextR(mg_strLabel, pixIL, pixJ, col);
  pdp->PutText(mg_strBinding, pixIR, pixJ, col);
}