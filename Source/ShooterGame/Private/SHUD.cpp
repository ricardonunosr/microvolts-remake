// Fill out your copyright notice in the Description page of Project Settings.

#include "SHUD.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "SCharacter.h"
#include "SPlayerController.h"

#include <Blueprint/UserWidget.h>
#include <GameFramework/PlayerController.h>
#include <Kismet/GameplayStatics.h>

ASHUD::ASHUD()
{
	// CreateMainHUD();
}

void ASHUD::CreateMainHUD()
{
	if (MainHUDClass)
	{
		ASPlayerController* PC = Cast<ASPlayerController>(GetOwningPlayerController());

		if (PC)
		{
			MainHUD = CreateWidget<UUserWidget>(PC, MainHUDClass);
		}
	}
}

void ASHUD::RemoveMainHUD()
{
	MainHUD->RemoveFromViewport();
}

void ASHUD::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASHUD::BeginPlay()
{
	Super::BeginPlay();
}

void ASHUD::SHOptionsMenu()
{
	if (OptionsMenu)
	{
		HideOptionsMenu();
		return;
	}
	ShowOptionsMenu();
}

void ASHUD::ShowOptionsMenu()
{
	ASPlayerController* PC = Cast<ASPlayerController>(GetOwner());
	OptionsMenu = CreateWidget<UUserWidget>(PC, OptionsMenuClass);

	OptionsMenu->AddToViewport();
	EnableUIInputMode();
}

void ASHUD::HideOptionsMenu()
{
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD<ASHUD>();

	if (OptionsMenu)
	{
		OptionsMenu->RemoveFromViewport();
		OptionsMenu = nullptr;
	}
	EnableGameInputMode();
}

void ASHUD::EnableUIInputMode()
{
	ASPlayerController* PC = Cast<ASPlayerController>(GetOwner());

	PC->SetIgnoreLookInput(true);
	PC->SetIgnoreMoveInput(true);
	PC->SetShowMouseCursor(true);
}

void ASHUD::EnableGameInputMode()
{
	ASPlayerController* PC = Cast<ASPlayerController>(GetOwner());

	PC->ResetIgnoreInputFlags();
	PC->SetShowMouseCursor(false);
}
