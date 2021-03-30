// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "SPickup.generated.h"

class USphereComponent;

UCLASS()
class SHOOTERGAME_API ASPickup : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASPickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UDecalComponent* DecalComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnActivated(AActor* ActiveFor);

	float CooldownDurantion;

	void Respawn();

public:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
