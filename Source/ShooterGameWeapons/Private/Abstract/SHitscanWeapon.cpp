// Fill out your copyright notice in the Description page of Project Settings.

#include "Abstract/SHitscanWeapon.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

void ASHitscanWeapon::Fire()
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
			const float ConeHalfAngle = FMath::DegreesToRadians(CurrentSpread * 1.5f);
			const FVector ShootDir = WeaponRandomStream.VRandCone(ShotDirection, ConeHalfAngle, ConeHalfAngle);

			FVector TraceEnd = EyeLocation + (ShootDir * 100000);
			FVector TracerEndPoint = TraceEnd;

			EPhysicalSurface SurfaceType = SurfaceType_Default;
			CurrentSpread = FMath::Min(WeaponConfig.MaxSpread, CurrentSpread + WeaponConfig.SpreadIncrement);

			if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECC_Visibility, QueryParams))
			{
				UGameplayStatics::ApplyPointDamage(Hit.GetActor(), WeaponConfig.HitDamage, ShotDirection, Hit,
					MyOwner->GetInstigatorController(), this, DamageType);

				SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
				PlayImpactEffects(SurfaceType, Hit.ImpactPoint);
				TracerEndPoint = Hit.ImpactPoint;
			}
			PlayFireEffects(TracerEndPoint);

			if (GetLocalRole() == ROLE_Authority)
			{
				HitScanTrace.SurfaceType = SurfaceType;
				HitScanTrace.TraceTo = TracerEndPoint;
			}
		}

		UseAmmo();
		PlaySounds(MyOwner->GetActorLocation());
	}
}

void ASHitscanWeapon::OnRep_HitScanTrace()
{
	PlayFireEffects(HitScanTrace.TraceTo);
	PlayImpactEffects(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
}

void ASHitscanWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASHitscanWeapon, HitScanTrace, COND_SkipOwner);
}
