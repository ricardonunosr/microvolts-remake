// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abstract/SProjectile.h"
#include "CoreMinimal.h"

#include "SGrenadeProjectile.generated.h"

/**
 *
 */
UCLASS()
class SHOOTERGAMEWEAPONS_API ASGrenadeProjectile : public ASProjectile
{
	GENERATED_BODY()

public:
	ASGrenadeProjectile();

	virtual void BeginPlay() override;

protected:
	virtual void Explode() override;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	float LifeSpan;

	FTimerHandle TimerHandle_GrenadeLifeSpan;
};
