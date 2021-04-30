// Fill out your copyright notice in the Description page of Project Settings.

#include "SPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "SHUD.h"

void ASPlayerController::TogglePauseMenu()
{
	if (OptionsMenu && OptionsMenu->IsInViewport())
	{
		OptionsMenu->RemoveFromParent();
		OptionsMenu = nullptr;

		bShowMouseCursor = false;
		SetInputMode(FInputModeGameOnly());
		return;
	}

	OptionsMenu = CreateWidget<UUserWidget>(this, OptionsMenuClass);
	if (OptionsMenu)
	{
		OptionsMenu->AddToViewport(100);

		bShowMouseCursor = true;
		DisableInput(this);
		// SetInputMode(FInputModeUIOnly());
	}
}

void ASPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("OptionsMenu", IE_Pressed, this, &ASPlayerController::TogglePauseMenu);
	InputComponent->BindAction("Scoreboard", IE_Pressed, this, &ASPlayerController::ShowScoreboard);
	InputComponent->BindAction("Scoreboard", IE_Released, this, &ASPlayerController::HideScoreboard);
}

void ASPlayerController::ShowScoreboard()
{
	Scoreboard = CreateWidget<UUserWidget>(this, ScoreboardClass);
	if (Scoreboard)
	{
		Scoreboard->AddToViewport(100);
	}
}

void ASPlayerController::HideScoreboard()
{
	if (Scoreboard && Scoreboard->IsInViewport())
	{
		Scoreboard->RemoveFromParent();
		Scoreboard = nullptr;
		SetInputMode(FInputModeGameOnly());
		return;
	}
}

void ASPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	OnPawnChanged.Broadcast(InPawn);
}

void ASPlayerController::BeginPlayingState()
{
	BlueprintBeginPlayingState();
}
