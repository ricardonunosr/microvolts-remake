// Fill out your copyright notice in the Description page of Project Settings.

#include "SRocketProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

ASRocketProjectile::ASRocketProjectile()
{
	MeshComp->SetNotifyRigidBodyCollision(true);
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetEnableGravity(false);

	ProjetileMovementComp->InitialSpeed = 2000.0f;
	ProjetileMovementComp->MaxSpeed = 2000.0f;
}

void ASRocketProjectile::BeginPlay()
{
	Super::BeginPlay();
	PlayFireEffects();
}

void ASRocketProjectile::Explode()
{
	FVector ActorLocation = GetActorLocation();
	AController* InstigatorController = GetInstigatorController();
	TArray<AActor*> IgnoreActors;

	PlayExplosionEffects();
	UGameplayStatics::ApplyRadialDamage(
		GetWorld(), ExplosionDamage, ActorLocation, DamageRadius, nullptr, IgnoreActors, this, InstigatorController, true);

	// UKismetSystemLibrary::DrawDebugSphere(GetWorld(), ActorLocation, DamageRadius, 12, FColor::Red, 2, 2);

	Destroy();
}

void ASRocketProjectile::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp,
	bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	Explode();
}

void ASRocketProjectile::PlayFireEffects()
{
	UGameplayStatics::SpawnEmitterAttached(TrailEffect, MeshComp, RocketTailSocket, FVector::ZeroVector, GetActorRotation(),
		EAttachLocation::SnapToTargetIncludingScale);
}
