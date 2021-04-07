// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abstract/SWeapon.h"
#include "CoreMinimal.h"

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
class SHOOTERGAMEWEAPONS_API ASHitscanWeapon : public ASWeapon
{
	GENERATED_BODY()

protected:
	virtual void Fire() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FHitscanConfig HitScanConfig;
};
