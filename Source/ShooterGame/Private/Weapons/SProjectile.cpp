// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/SProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"

static int32 DebugProjectiles = 0;
FAutoConsoleVariableRef CVARDebugProjectiles(
	TEXT("S.DebugProjectile"), DebugProjectiles, TEXT("Draw Debug Lines for Projectiles"), ECVF_Cheat);

ASProjectile::ASProjectile()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	ProjetileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));

	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
}

void ASProjectile::PlayExplosionEffects()
{
	RadialForceComp->FireImpulse();
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation(), GetActorRotation());
}
