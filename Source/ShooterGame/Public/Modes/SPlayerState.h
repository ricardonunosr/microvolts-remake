// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"

#include "SPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerScoreKill, ASPlayerState*, PlayerState, int32, NewScoreKill);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerScoreDeath, ASPlayerState*, PlayerState, int32, NewScoreDeath);

/**
 *
 */
UCLASS()
class SHOOTERGAME_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ASPlayerState();

	void ScoreKill(AActor* Victim);

	void ScoreDeath(AActor* KilledBy);

	UFUNCTION(BlueprintCallable)
	int32 GetKills() const;

	UFUNCTION(BlueprintCallable)
	int32 GetDeaths() const;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPlayerScoreKill OnPlayerScoreKill;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPlayerScoreDeath OnPlayerScoreDeath;

protected:
	UPROPERTY(Transient, ReplicatedUsing = OnRepKills)
	int32 NumKills;

	UPROPERTY(Transient, ReplicatedUsing = OnRepDeaths)
	int32 NumDeaths;

	UFUNCTION()
	void OnRepKills();

	UFUNCTION()
	void OnRepDeaths();
};
