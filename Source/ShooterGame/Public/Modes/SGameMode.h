// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"

#include "SGameMode.generated.h"

UCLASS()
class SHOOTERGAME_API ASGameMode : public AGameMode
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Classes")
	TSubclassOf<UUserWidget> ScoreboardClass;

	UPROPERTY(EditDefaultsOnly, Category = "Parameters")
	int32 MaxPlayers;

	UPROPERTY(EditDefaultsOnly, Category = "Parameters")
	int32 TimeLimit;

	void RegisterKill(AController* Victim, AController* Killer);
};
