// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"

#include "SGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilled, AActor*, VictimActor, AActor*, KillerActor, AActor*, DamageCauser);

/**
 *
 */
UCLASS()
class SHOOTERGAME_API ASGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ASGameState();

	UPROPERTY(Transient, Replicated)
	int32 RemainingTime;

	UFUNCTION(Unreliable, NetMulticast)
	void MultiCastOnActorKilled(AActor* Victim, AActor* Killer, AActor* DamageCauser);

	UPROPERTY(BlueprintAssignable, Category = "GameMode")
	FOnActorKilled OnActorKilled;
};
