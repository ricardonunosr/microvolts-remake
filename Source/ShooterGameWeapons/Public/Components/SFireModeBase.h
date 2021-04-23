// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "ShooterGameWeapons/ShooterGameWeapons.h"

#include "SFireModeBase.generated.h"

UCLASS(Abstract, Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SHOOTERGAMEWEAPONS_API USFireModeBase : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USFireModeBase();

	UPROPERTY(EditDefaultsOnly, Category = FireMode)
	EWeaponFireMode FireMode;

	// virtual void StartFire();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
