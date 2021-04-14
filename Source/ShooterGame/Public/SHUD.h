// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

#include "SHUD.generated.h"

class UUserWidget;
UCLASS(Abstract) class SHOOTERGAME_API ASHUD : public AHUD
{
	GENERATED_BODY()

private:
	void ShowOptionsMenu();

	void HideOptionsMenu();

	void EnableUIInputMode();

	void EnableGameInputMode();

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> MainHUDClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> OptionsMenuClass;

public:
	UPROPERTY(BlueprintReadOnly)
	class UUserWidget* MainHUD;

	UPROPERTY(BlueprintReadOnly)
	class UUserWidget* OptionsMenu;

	void CreateMainHUD();

	void RemoveMainHUD();

	void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	void SHOptionsMenu();
};
