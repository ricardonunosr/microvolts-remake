// Fill out your copyright notice in the Description page of Project Settings.

#include "SPlayerController.h"

#include "SHUD.h"

void ASPlayerController::AcknowledgePossession(class APawn* P)
{
	Super::AcknowledgePossession(P);

	ASHUD* HUD = GetHUD<ASHUD>();

	if (HUD->MainHUD == nullptr)
	{
		HUD->CreateMainHUD();
	}
	else
	{
		HUD->RemoveMainHUD();
		HUD->CreateMainHUD();
	}
}

void ASPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("OptionsMenu", IE_Pressed, this, &ASPlayerController::HandleOptionsMenu);
}

void ASPlayerController::HandleOptionsMenu()
{
	ASHUD* HUD = GetHUD<ASHUD>();

	if (HUD)
	{
		HUD->SHOptionsMenu();
	}
}
