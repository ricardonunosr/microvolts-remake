// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abstract/SWeapon.h"
#include "CoreMinimal.h"

#include "SProjectileWeapon.generated.h"

/**
 *
 */
UCLASS()
class SHOOTERGAMEWEAPONS_API ASProjectileWeapon : public ASWeapon
{
	GENERATED_BODY()

protected:
	virtual void Fire() override;

	virtual void SecondaryFire() override;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TSubclassOf<AActor> ProjectileClass;

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerFireProjectile();
};
