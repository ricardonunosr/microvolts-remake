// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "SPracticeRange.generated.h"

/**
 *
 */
UCLASS()
class SHOOTERGAME_API ASPracticeRange : public AGameModeBase
{
	GENERATED_BODY()
public:
	ASPracticeRange();

	virtual void StartPlay() override;

	virtual void Tick(float DeltaSeconds) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GameMode")
	TArray<FVector> SpawnPoints;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	int32 NrOfBotsToSpawn;

	int32 NumberOfBots;

	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void SpawnBot();

	void CheckBots();
};
