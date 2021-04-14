// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abstract/SProjectile.h"
#include "CoreMinimal.h"

#include "SRocketProjectile.generated.h"

/**
 *
 */
UCLASS()
class SHOOTERGAMEWEAPONS_API ASRocketProjectile : public ASProjectile
{
	GENERATED_BODY()

public:
	ASRocketProjectile();

	virtual void BeginPlay() override;

	void Tick(float DeltaTime) override;

	virtual void Explode() override;

	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved,
		FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	UParticleSystem* TrailEffect;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	FName RocketTailSocket;

	UPROPERTY(EditDefaultsOnly, Category = Config)
	float ProjectileSpeed;

	UPROPERTY(EditDefaultsOnly, Category = Config)
	float SelfDestructDistance;

	UPROPERTY(VisibleAnywhere, Category = Config)
	float DistanceTraveled;

	void PlayFireEffects();
};
