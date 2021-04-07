// Fill out your copyright notice in the Description page of Project Settings.

#include "SSniper.h"

#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

void ASSniper::Fire()
{
	AActor* MyOwner = GetOwner();

	if (CurrentAmmo <= 0)
		StartReload();

	if (MyOwner && !bIsReloading)
	{
		FVector EyeLocation;
		FRotator EyeRotation;

		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;

		for (int32 i = 0; i < HitScanConfig.BurstCount; i++)
		{
			FHitResult Hit;

			const int32 RandomSeed = FMath::Rand();
			FRandomStream WeaponRandomStream(RandomSeed);

			const float ConeHalfAngle = FMath::DegreesToRadians(10.0f);
			FVector ShootDir = WeaponRandomStream.VRandCone(ShotDirection, ConeHalfAngle, ConeHalfAngle);

			if (bIsSecundaryFireActive)
			{
				ShootDir = ShotDirection;
			}

			FVector TraceEnd = EyeLocation + (ShootDir * 100000);
			FVector TracerEndPoint = TraceEnd;

			if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECC_Visibility, QueryParams))
			{
				UGameplayStatics::ApplyPointDamage(Hit.GetActor(), WeaponConfig.HitDamage, ShotDirection, Hit,
					MyOwner->GetInstigatorController(), this, DamageType);

				EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
				PlayImpactEffects(SurfaceType, Hit.ImpactPoint);
				TracerEndPoint = Hit.ImpactPoint;
			}
			PlayFireEffects(TracerEndPoint);
		}
		UseAmmo();
		PlaySounds(MyOwner->GetActorLocation());
		if (bIsSecundaryFireActive)
		{
			SecondaryFire();
		}
	}
}
