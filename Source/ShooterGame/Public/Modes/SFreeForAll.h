// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Modes/SGameMode.h"

#include "SFreeForAll.generated.h"

UCLASS() class SHOOTERGAME_API ASFreeForAll : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASFreeForAll();

	void RegisterKill(AActor* Victim, AActor* Killer, AActor* DamageCauser);

	void Tick(float DeltaSeconds) override;

	void CheckRespawnPlayers();
};
