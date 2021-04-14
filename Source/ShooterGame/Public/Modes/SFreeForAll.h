// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "SFreeForAll.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnActorKilled, AActor*, VictimActor, AActor*, KillerActor, AController*, KillerController);

UCLASS() class SHOOTERGAME_API ASFreeForAll : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASFreeForAll();

	void Tick(float DeltaSeconds) override;

	void CheckRespawnPlayers();

	UPROPERTY(BlueprintAssignable, Category = "GameMode")
	FOnActorKilled OnActorKilled;
};
