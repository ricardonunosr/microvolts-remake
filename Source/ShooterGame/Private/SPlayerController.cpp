// Fill out your copyright notice in the Description page of Project Settings.

#include "SPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "SHUD.h"

void ASPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ASPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("OptionsMenu", IE_Pressed, this, &ASPlayerController::HandleOptionsMenu);
	InputComponent->BindAction("Fire", IE_Pressed, this, &ASPlayerController::PressedFire);
}

void ASPlayerController::HandleOptionsMenu()
{
	ASHUD* HUD = GetHUD<ASHUD>();

	if (HUD)
	{
		HUD->SHOptionsMenu();
	}
}

void ASPlayerController::StartFire(uint8 FireModeNum)
{
	Super::StartFire(FireModeNum);
}

void ASPlayerController::PressedFire()
{
	StartFire(0);
}

void ASPlayerController::ReleasedFire()
{
}
