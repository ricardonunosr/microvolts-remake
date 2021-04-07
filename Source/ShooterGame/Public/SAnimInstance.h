﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimInstance.h"
#include "CoreMinimal.h"
#include "ShooterGame/ShooterGame.h"
#include "ShooterGameWeapons/ShooterGameWeapons.h"

#include "SAnimInstance.generated.h"

UCLASS()
class SHOOTERGAME_API USAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	USAnimInstance();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	FRotator HeadRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	EWeaponType CurrentWeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	float AimYaw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	float AimPitch;
};
