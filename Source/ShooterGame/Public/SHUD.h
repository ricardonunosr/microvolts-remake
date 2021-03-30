// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

#include "SHUD.generated.h"

/**
 *
 */
UCLASS(Abstract)
class SHOOTERGAME_API ASHUD : public AHUD
{
	GENERATED_BODY()

private:
	void ShowOptionsMenu();

	void HideOptionsMenu();

	void EnableUIInputMode();

	void EnableGameInputMode();

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> OptionsMenuClass;

	UPROPERTY()
	class UUserWidget* OptionsMenu;

	TArray<class UUserWidget*> PageStack;

public:
	void SHOptionsMenu();
};
