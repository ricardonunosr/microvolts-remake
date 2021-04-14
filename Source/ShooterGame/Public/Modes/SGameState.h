// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"

#include "SGameState.generated.h"

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
};
