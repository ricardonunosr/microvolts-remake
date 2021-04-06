// Fill out your copyright notice in the Description page of Project Settings.

#include "SWeapon.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(
	TEXT("S.DebugWeapons"), DebugWeaponDrawing, TEXT("Draw Debug Lines for Weapons"), ECVF_Cheat);

ASWeapon::ASWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = MeshComp;

	bIsSecundaryFireActive = false;
	bIsReloading = false;
	bIsRefering = false;

	CurrentState = EWeaponState::Idle;
	CurrentTotalAmmo = 0;
	CurrentAmmo = 0;
	LastFireTime = 0.0f;
}

void ASWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	CurrentTotalAmmo = WeaponConfig.TotalAmmo;
	CurrentAmmo = WeaponConfig.MagSize;
}

void ASWeapon::OnEquip()
{
}

void ASWeapon::OnUnEquip()
{
	LastFireTime = 0;
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
	GetWorldTimerManager().ClearTimer(TimerHandle_ReloadTime);
}

void ASWeapon::StartFire()
{
	SetWeaponState(EWeaponState::Firing);
}

void ASWeapon::StopFire()
{
	SetWeaponState(EWeaponState::Idle);
}

void ASWeapon::OnBurstStarted()
{
	const float GameTime = GetWorld()->GetTimeSeconds();
	if (LastFireTime > 0 && LastFireTime + WeaponConfig.TimeBetweenShots > GameTime)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASWeapon::HandleFiring,
			LastFireTime + WeaponConfig.TimeBetweenShots - GameTime, false);
	}
	else
	{
		HandleFiring();
	}
}

void ASWeapon::OnBurstEnded()
{
	CurrentSpread = 0;
	bIsRefering = false;
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

void ASWeapon::HandleFiring()
{
	if (CanFire())
	{
		Fire();
	}
	else if (CanReload())
	{
		StartReload();
	}
	else
	{
		StopFire();
	}

	bIsRefering = CurrentState == EWeaponState::Firing;
	if (bIsRefering)
	{
		GetWorldTimerManager().SetTimer(
			TimerHandle_TimeBetweenShots, this, &ASWeapon::HandleFiring, WeaponConfig.TimeBetweenShots, false);
	}
	LastFireTime = GetWorld()->GetTimeSeconds();
}

void ASWeapon::StartSecondaryFire()
{
	SecondaryFire();
}

void ASWeapon::StartReload()
{
	if (CanReload())
	{
		bIsReloading = true;
		OnReload.Broadcast(this);
		GetWorldTimerManager().SetTimer(TimerHandle_ReloadTime, this, &ASWeapon::Reload, WeaponConfig.ReloadTime, false);
	}
}

void ASWeapon::StopReload()
{
	if (bIsReloading)
	{
		bIsReloading = false;
		GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
		GetWorldTimerManager().ClearTimer(TimerHandle_ReloadTime);
	}
}

void ASWeapon::ResetWeapon()
{
	CurrentTotalAmmo = WeaponConfig.TotalAmmo;
	CurrentAmmo = WeaponConfig.MagSize;
}

EWeaponState::Type ASWeapon::GetCurrentState()
{
	return CurrentState;
}

void ASWeapon::Reload()
{
	int32 AmmoDif = WeaponConfig.MagSize - CurrentAmmo;
	int32 Delta = FMath::Min(CurrentTotalAmmo, AmmoDif);
	CurrentAmmo += Delta;
	CurrentTotalAmmo -= Delta;

	bIsReloading = false;
}

void ASWeapon::SecondaryFire()
{
	bIsSecundaryFireActive = !bIsSecundaryFireActive;
	bIsSecundaryFireActive ? CurrentState = EWeaponState::SecondaryFiring : CurrentState = EWeaponState::Idle;
	OnSecondaryFire.Broadcast(this);
}

void ASWeapon::SetWeaponState(EWeaponState::Type NewState)
{
	const EWeaponState::Type PrevState = CurrentState;

	if (PrevState == EWeaponState::Firing && NewState != EWeaponState::Firing)
	{
		OnBurstEnded();
	}

	CurrentState = NewState;

	if (PrevState != EWeaponState::Firing && NewState == EWeaponState::Firing)
	{
		OnBurstStarted();
	}
}

bool ASWeapon::CanReload()
{
	return CurrentAmmo < WeaponConfig.MagSize && CurrentTotalAmmo > 0;
}

bool ASWeapon::CanFire()
{
	return CurrentAmmo > 0;
}

void ASWeapon::UseAmmo()
{
	CurrentAmmo--;
}

void ASWeapon::PlayImpactEffects(EPhysicalSurface Surface, FVector ImpactPoint)
{
	FVector MuzzleLocation = MeshComp->GetSocketLocation(WeaponMuzzleSocketName);
	FVector ShotDirection = ImpactPoint - MuzzleLocation;
	ShotDirection.Normalize();

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DefaultImpactEffect, ImpactPoint, ShotDirection.Rotation());
}

void ASWeapon::PlayFireEffects(FVector ImpactPoint)
{
	if (TracerEffect)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation(WeaponMuzzleSocketName);

		UNiagaraComponent* TracerSystem = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
		if (TracerSystem)
		{
			TracerSystem->SetVectorParameter("BeamEnd", ImpactPoint);
		}
	}
}

void ASWeapon::PlaySounds(FVector ActorLocation)
{
}
