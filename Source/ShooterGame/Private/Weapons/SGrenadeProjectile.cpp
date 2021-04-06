// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/SGrenadeProjectile.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void ASGrenadeProjectile::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_GrenadeLifeSpan, this, &ASGrenadeProjectile::Explode, LifeSpan, false);
}

void ASGrenadeProjectile::Explode()
{
	FVector ActorLocation = GetActorLocation();
	AController* InstigatorController = GetInstigatorController();
	TArray<AActor*> IgnoreActors;

	PlayExplosionEffects();
	UGameplayStatics::ApplyRadialDamage(
		GetWorld(), ExplosionDamage, ActorLocation, DamageRadius, nullptr, IgnoreActors, this, InstigatorController, true);

	GetWorldTimerManager().ClearTimer(TimerHandle_GrenadeLifeSpan);

	// UKismetSystemLibrary::DrawDebugSphere(GetWorld(), ActorLocation, DamageRadius, 12, FColor::Red, 2, 2);

	Destroy();
}
