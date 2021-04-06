// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SProjectile.h"

#include "SGrenadeProjectile.generated.h"

/**
 *
 */
UCLASS()
class SHOOTERGAME_API ASGrenadeProjectile : public ASProjectile
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

protected:
	virtual void Explode() override;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	float LifeSpan;

	FTimerHandle TimerHandle_GrenadeLifeSpan;
};
