// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"

#include "SPlayerState.generated.h"

/**
 *
 */
UCLASS()
class SHOOTERGAME_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ASPlayerState();

	void ScoreKill(ASPlayerState* Victim);

	void ScoreDeath(ASPlayerState* KilledBy);

	int32 GetKills() const;

	int32 GetDeaths() const;

	void SetMatchId(const FString& CurrentMatchId);

protected:
	UPROPERTY(Transient, Replicated)
	int32 NumKills;

	UPROPERTY(Transient, Replicated)
	int32 NumDeaths;

	UPROPERTY(Replicated)
	FString MatchId;
};
