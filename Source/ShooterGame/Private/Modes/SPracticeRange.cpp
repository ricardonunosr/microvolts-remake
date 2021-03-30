// Fill out your copyright notice in the Description page of Project Settings.

#include "Modes/SPracticeRange.h"

#include "Components/SHealthComponent.h"
#include "EngineUtils.h"
#include "GameFramework/HUD.h"
#include "SBot.h"

ASPracticeRange::ASPracticeRange()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnOb(TEXT("/Game/Blueprints/BP_PlayerCharacter"));
	DefaultPawnClass = PlayerPawnOb.Class;

	static ConstructorHelpers::FClassFinder<AHUD> HUDOb(TEXT("/Game/Blueprints/BP_HUD"));
	HUDClass = HUDOb.Class;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;
}

void ASPracticeRange::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CheckBots();
}

void ASPracticeRange::StartPlay()
{
	Super::StartPlay();

	for (int32 i = 0; i < NrOfBotsToSpawn; i++)
	{
		SpawnBot();
		NumberOfBots++;
	}
}

void ASPracticeRange::CheckBots()
{
	int32 BotsAlive = 0;
	for (TActorIterator<ASBot> PawnIterator(GetWorld()); PawnIterator; ++PawnIterator)
	{
		BotsAlive++;
	}

	if (BotsAlive < NrOfBotsToSpawn)
	{
		SpawnBot();
	}
}
