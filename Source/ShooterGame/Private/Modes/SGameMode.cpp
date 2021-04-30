// Fill out your copyright notice in the Description page of Project Settings.

#include "Modes/SGameMode.h"

#include "Modes/SPlayerState.h"

void ASGameMode::RegisterKill(AController* Victim, AController* Killer)
{
	if (Victim)
	{
		ASPlayerState* VictimPlayerState = Victim->GetPlayerState<ASPlayerState>();

		if (VictimPlayerState)
		{
			VictimPlayerState->ScoreDeath(Killer);
		}

		if (Killer)
		{
			ASPlayerState* KillerPlayerState = Killer->GetPlayerState<ASPlayerState>();

			if (KillerPlayerState)
			{
				KillerPlayerState->ScoreKill(Victim);
			}
		}
	}
}
