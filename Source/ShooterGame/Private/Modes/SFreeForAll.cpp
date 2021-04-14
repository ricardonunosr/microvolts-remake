// Fill out your copyright notice in the Description page of Project Settings.

#include "Modes/SFreeForAll.h"

#include "Components/SHealthComponent.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Modes/SGameState.h"
#include "Modes/SPlayerState.h"
#include "SCharacter.h"

ASFreeForAll::ASFreeForAll()
{
	GameStateClass = ASGameState::StaticClass();
	PlayerStateClass = ASPlayerState::StaticClass();

	PrimaryActorTick.bCanEverTick = true;
}

void ASFreeForAll::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CheckRespawnPlayers();
}

void ASFreeForAll::CheckRespawnPlayers()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && PC->GetPawn() == nullptr)
		{
			TArray<AActor*> PlayerStarts;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

			int32 RandomInt = FMath::RandRange(0, PlayerStarts.Num() - 1);

			UE_LOG(LogTemp, Log, TEXT("%d"), RandomInt);

			AActor* RandomPlayerStart = PlayerStarts[RandomInt];

			RestartPlayerAtPlayerStart(PC, RandomPlayerStart);
		}
	}
}
