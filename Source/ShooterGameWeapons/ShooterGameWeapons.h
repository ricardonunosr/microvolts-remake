// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FShooterGameWeapons : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	void StartupModule() override;
	void ShutdownModule() override;
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	E_Melee UMETA(DisplayName = "Melee"),
	E_Rifle UMETA(DisplayName = "Rifle"),
	E_Shotgun UMETA(DisplayName = "Shotgun"),
	E_Sniper UMETA(DisplayName = "Sniper"),
	E_Gatling UMETA(DisplayName = "Gatling"),
	E_RocketLauncher UMETA(DisplayName = "RocketLauncher"),
	E_GrenadeLauncher UMETA(DisplayName = "GrenadeLauncher"),
};
