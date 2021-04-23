// Fill out your copyright notice in the Description page of Project Settings.

#include "Modes/SFreeForAll.h"

#include "Blueprint/UserWidget.h"
#include "Components/SHealthComponent.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/SpectatorPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Modes/SGameState.h"
#include "Modes/SPlayerState.h"
#include "SCharacter.h"
#include "SHUD.h"
#include "SPlayerController.h"

ASFreeForAll::ASFreeForAll()
{
	GameStateClass = ASGameState::StaticClass();
	PlayerStateClass = ASPlayerState::StaticClass();

	PrimaryActorTick.bCanEverTick = true;
}

void ASFreeForAll::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// CheckRespawnPlayers();
}
