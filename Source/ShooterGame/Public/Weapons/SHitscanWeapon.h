// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeapon.h"

#include "SHitscanWeapon.generated.h"

USTRUCT(BlueprintType)
struct FHitscanConfig
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	int32 BurstCount;

	FHitscanConfig()
	{
		BurstCount = 1;
	}
};

UCLASS()
class SHOOTERGAME_API ASHitscanWeapon : public ASWeapon
{
	GENERATED_BODY()

protected:
	virtual void Fire() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FHitscanConfig HitScanConfig;
};
