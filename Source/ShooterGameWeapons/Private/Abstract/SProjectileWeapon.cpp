// Fill out your copyright notice in the Description page of Project Settings.

#include "Abstract/SProjectileWeapon.h"

void ASProjectileWeapon::Fire()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		ServerFireProjectile();
	}
}

void ASProjectileWeapon::ServerFireProjectile_Implementation()
{
	AActor* MyOwner = GetOwner();
	if (MyOwner && ProjectileClass)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector MuzzleLocation = MeshComp->GetSocketLocation(WeaponMuzzleSocketName);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, EyeRotation, SpawnParams);
	}
}

bool ASProjectileWeapon::ServerFireProjectile_Validate()
{
	return true;
}
