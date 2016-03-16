/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include "StdH.h"
#include <Engine/Base/KeyNames.h>
#include <Engine/CurrentVersion.h>
#include "MenuPrinting.h"
#include <GameMP/LCDDrawing.h>
#include "MGChangePlayer.h"

extern CSoundData *_psdPress;


void CMGChangePlayer::OnActivate(void)
{
	PlayMenuSound(_psdPress);
	IFeel_PlayEffect("Menu_press");
	_iLocalPlayer = mg_iLocalPlayer;
	if (_pGame->gm_aiMenuLocalPlayers[mg_iLocalPlayer] < 0)
		_pGame->gm_aiMenuLocalPlayers[mg_iLocalPlayer] = 0;
	gmPlayerProfile.gm_piCurrentPlayer = &_pGame->gm_aiMenuLocalPlayers[mg_iLocalPlayer];
	gmPlayerProfile.gm_pgmParentMenu = &gmSelectPlayersMenu;
	extern BOOL _bPlayerMenuFromSinglePlayer;
	_bPlayerMenuFromSinglePlayer = FALSE;
	ChangeToMenu(&gmPlayerProfile);
}

void CMGChangePlayer::SetPlayerText(void)
{
	INDEX iPlayer = _pGame->gm_aiMenuLocalPlayers[mg_iLocalPlayer];
	CPlayerCharacter &pc = _pGame->gm_apcPlayers[iPlayer];
	if (iPlayer<0 || iPlayer>7) {
		mg_strText = "????";
	}
	else {
		mg_strText.PrintF(TRANS("Player %d: %s\n"), mg_iLocalPlayer + 1, pc.GetNameForPrinting());
	}
}