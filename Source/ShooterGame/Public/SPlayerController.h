// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "SPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPawnChanged, APawn*, NewPawn);

UCLASS()
class SHOOTERGAME_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintAssignable)
	FOnPawnChanged OnPawnChanged;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> MainHUDClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> OptionsMenuClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> ScoreboardClass;

public:
	virtual void SetupInputComponent() override;

	void TogglePauseMenu();

public:
	UPROPERTY(BlueprintReadOnly)
	class UUserWidget* MainHUD;

	UPROPERTY(BlueprintReadOnly)
	class UUserWidget* OptionsMenu;

	UPROPERTY(BlueprintReadOnly)
	class UUserWidget* Scoreboard;

	void ShowScoreboard();

	void HideScoreboard();

	virtual void SetPawn(APawn* InPawn) override;

	/* Called when player controller is ready to begin playing, good moment to initialize things like UI which might be too early in
	   BeginPlay (esp. in multiplayer clients where not all data such as PlayerState may have been received yet) */
	virtual void BeginPlayingState() override;

	UFUNCTION(BlueprintImplementableEvent)
	void BlueprintBeginPlayingState();
};
