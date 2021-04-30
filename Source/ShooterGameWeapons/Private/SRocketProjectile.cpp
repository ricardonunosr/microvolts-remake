// Fill out your copyright notice in the Description page of Project Settings.

#include "SRocketProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Log.h"
#include "PhysicsEngine/RadialForceComponent.h"

ASRocketProjectile::ASRocketProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComp->SetNotifyRigidBodyCollision(true);
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetEnableGravity(false);

	ProjectileSpeed = 2000.0f;

	ProjetileMovementComp->InitialSpeed = ProjectileSpeed;
	ProjetileMovementComp->MaxSpeed = ProjectileSpeed;

	bReplicates = true;
}

void ASRocketProjectile::BeginPlay()
{
	Super::BeginPlay();

	PlayFireEffects();
}

void ASRocketProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float OffsetTraveled = ProjectileSpeed * DeltaTime;
	DistanceTraveled += OffsetTraveled;
	if (DistanceTraveled > SelfDestructDistance)
	{
		Explode();
	}
}

void ASRocketProjectile::Explode()
{
	AActor* MyOwner = GetOwner();
	FVector ActorLocation = GetActorLocation();
	AController* InstigatorController = GetInstigatorController();
	TArray<AActor*> IgnoreActors;

	PlayExplosionEffects();
	RadialForceComp->FireImpulse();
	UGameplayStatics::ApplyRadialDamage(GetWorld(), ExplosionDamage, ActorLocation, DamageRadius, nullptr, IgnoreActors, MyOwner,
		MyOwner->GetInstigatorController(), true);

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
