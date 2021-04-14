// Fill out your copyright notice in the Description page of Project Settings.

#include "Modes/SGameState.h"

#include "Net/UnrealNetwork.h"

ASGameState::ASGameState()
{
	RemainingTime = 0;
}

void ASGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASGameState, RemainingTime);
}
