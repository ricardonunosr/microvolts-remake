// Fill out your copyright notice in the Description page of Project Settings.

#include "Abstract/SProjectileWeapon.h"

#include "Abstract/SProjectile.h"

void ASProjectileWeapon::Fire()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		ServerFireProjectile();
	}
}

void ASProjectileWeapon::SecondaryFire()
{
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

		ASProjectile* Projectile = GetWorld()->SpawnActor<ASProjectile>(ProjectileClass, MuzzleLocation, EyeRotation, SpawnParams);
		if (Projectile)
		{
			Projectile->SetInstigator(Cast<APawn>(MyOwner));
			Projectile->SetOwner(this);
		}

		UseAmmo();
	}
}

bool ASProjectileWeapon::ServerFireProjectile_Validate()
{
	return true;
}
