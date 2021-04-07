// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "SProjectile.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExplodeSignature);

class UProjectileMovementComponent;
class UParticleSystem;
class URadialForceComponent;

UCLASS() class SHOOTERGAMEWEAPONS_API ASProjectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASProjectile();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UProjectileMovementComponent* ProjetileMovementComp;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	URadialForceComponent* RadialForceComp;

	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnExplodeSignature OnExplode;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	UParticleSystem* ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	float DamageRadius;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	float ExplosionDamage;

	virtual void Explode() PURE_VIRTUAL(ASProjectile::Explode, );

	void PlayExplosionEffects();
};
