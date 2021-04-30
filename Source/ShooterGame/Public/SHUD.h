// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

#include "SHUD.generated.h"

class UUserWidget;
UCLASS(Abstract) class SHOOTERGAME_API ASHUD : public AHUD
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> MainHUDClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> OptionsMenuClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> ScoreboardClass;

public:
	UPROPERTY(BlueprintReadOnly)
	class UUserWidget* MainHUD;

	UPROPERTY(BlueprintReadOnly)
	class UUserWidget* OptionsMenu;

	UPROPERTY(BlueprintReadOnly)
	class UUserWidget* Scoreboard;

	void TogglePauseMenu();
};
