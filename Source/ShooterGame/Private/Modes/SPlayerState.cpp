// Fill out your copyright notice in the Description page of Project Settings.

#include "Modes/SPlayerState.h"

#include "Net/UnrealNetwork.h"

ASPlayerState::ASPlayerState()
{
	NumKills = 0;
	NumDeaths = 0;
}

void ASPlayerState::ScoreKill(AActor* Victim)
{
	NumKills++;
	OnPlayerScoreKill.Broadcast(this, NumKills);
}

void ASPlayerState::ScoreDeath(AActor* KilledBy)
{
	NumDeaths++;
	OnPlayerScoreDeath.Broadcast(this, NumDeaths);
}

int32 ASPlayerState::GetKills() const
{
	return NumKills;
}

int32 ASPlayerState::GetDeaths() const
{
	return NumDeaths;
}

void ASPlayerState::OnRepKills()
{
	OnPlayerScoreKill.Broadcast(this, NumKills);
}

void ASPlayerState::OnRepDeaths()
{
	OnPlayerScoreKill.Broadcast(this, NumKills);
}

void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPlayerState, NumKills);
	DOREPLIFETIME(ASPlayerState, NumDeaths);
}
