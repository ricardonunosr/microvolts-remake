// Fill out your copyright notice in the Description page of Project Settings.

#include "Abstract/SWeapon.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "ShooterGameWeapons/ShooterGameWeapons.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(
	TEXT("S.DebugWeapons"), DebugWeaponDrawing, TEXT("Draw Debug Lines for Weapons"), ECVF_Cheat);

ASWeapon::ASWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionObjectType(ECC_WorldDynamic);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	RootComponent = MeshComp;

	bReplicates = true;
	bAlwaysRelevant = true;

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
	if (bIsReloading)
	{
		StopReload();
	}

	GetWorldTimerManager().ClearTimer(TimerHandle_ReloadTime);
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

void ASWeapon::OnUnEquip()
{
	if (CanReload())
	{
		StartReload();
	}

	LastFireTime = 0;
	StopSecondaryFire();
}

void ASWeapon::StartFire()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerStartFire();
	}

	if (!bWantsToFire)
	{
		bWantsToFire = true;
		DetermineWeaponState();
	}
}

void ASWeapon::ServerStartFire_Implementation()
{
	StartFire();
}

bool ASWeapon::ServerStartFire_Validate()
{
	return true;
}

void ASWeapon::StopFire()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerStopFire();
	}

	if (bWantsToFire)
	{
		bWantsToFire = false;
		DetermineWeaponState();
	}
}

void ASWeapon::ServerStopFire_Implementation()
{
	StopFire();
}

bool ASWeapon::ServerStopFire_Validate()
{
	return true;
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
		OnFire.Broadcast(this);
	}
	else if (CanReload())
	{
		StartReload();
	}
	else
	{
		StopFire();
	}

	if (CurrentAmmo <= 0 && CanReload())
	{
		StartReload();
	}

	bIsRefering = CurrentState == EWeaponState::Firing;
	if (bIsRefering)
	{
		GetWorldTimerManager().SetTimer(
			TimerHandle_TimeBetweenShots, this, &ASWeapon::HandleFiring, WeaponConfig.TimeBetweenShots, false);
	}
	LastFireTime = GetWorld()->GetTimeSeconds();
}

void ASWeapon::HandleSecondFiring(bool bWasActive)
{
	if (bIsSecundaryFireActive == bWasActive)
	{
		StopSecondaryFire();
		SecondaryFire();
	}
	else
	{
		SecondaryFire();
	}
}

void ASWeapon::StartSecondaryFire()
{
	bool bWasActive = bIsSecundaryFireActive;
	bIsSecundaryFireActive = true;
	HandleSecondFiring(bWasActive);
}

void ASWeapon::StopSecondaryFire()
{
	bIsSecundaryFireActive = false;
}

void ASWeapon::StartReload(bool bFromReplication)
{
	if (!bFromReplication && GetLocalRole() < ROLE_Authority)
	{
		ServerStartReload();
	}
	else
	{
		if (bFromReplication || CanReload())
		{
			bIsReloading = true;
			DetermineWeaponState();
			OnStartReload.Broadcast(this);
			if (GetLocalRole() == ROLE_Authority)
			{
				GetWorldTimerManager().SetTimer(
					TimerHandle_StopReload, this, &ASWeapon::StopReload, WeaponConfig.ReloadTime, false);
				GetWorldTimerManager().SetTimer(
					TimerHandle_ReloadTime, this, &ASWeapon::Reload, WeaponConfig.ReloadTime - 0.1f, false);
			}
		}
	}
}

void ASWeapon::ServerStartReload_Implementation()
{
	StartReload();
}

bool ASWeapon::ServerStartReload_Validate()
{
	return true;
}

void ASWeapon::StopReload()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerStopReload();
	}
	else
	{
		if (CurrentState == EWeaponState::Reloading)
		{
			bIsReloading = false;
			DetermineWeaponState();
		}
	}
	OnStopReload.Broadcast(this);
}

void ASWeapon::ServerStopReload_Implementation()
{
	StopReload();
}

bool ASWeapon::ServerStopReload_Validate()
{
	return true;
}

void ASWeapon::ResetWeapon()
{
	CurrentTotalAmmo = WeaponConfig.TotalAmmo;
	CurrentAmmo = WeaponConfig.MagSize;
}

EWeaponState ASWeapon::GetCurrentState()
{
	return CurrentState;
}

void ASWeapon::Reload()
{
	int32 AmmoDif = WeaponConfig.MagSize - CurrentAmmo;
	int32 Delta = FMath::Min(CurrentTotalAmmo, AmmoDif);
	CurrentAmmo += Delta;
	CurrentTotalAmmo -= Delta;
}

void ASWeapon::OnRepReload()
{
	if (bIsReloading)
	{
		StartReload(true);
	}
	else
	{
		StopReload();
	}
}

void ASWeapon::SecondaryFire()
{
	OnSecondaryFire.Broadcast(bIsSecundaryFireActive);
}

void ASWeapon::DetermineWeaponState()
{
	EWeaponState NewState = EWeaponState::Idle;

	if (bIsReloading)
	{
		if (CanReload() == false)
		{
			NewState = CurrentState;
		}
		else
		{
			NewState = EWeaponState::Reloading;
		}
	}
	else if ((bIsReloading == false) && (bWantsToFire == true) && (CanFire() == true))
	{
		NewState = EWeaponState::Firing;
	}

	SetWeaponState(NewState);
}

void ASWeapon::SetWeaponState(EWeaponState NewState)
{
	const EWeaponState PrevState = CurrentState;

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
	bool bHasBulletsToReload = CurrentAmmo < WeaponConfig.MagSize && CurrentTotalAmmo > 0;
	bool bStateOkToReload = CurrentState == EWeaponState::Idle || CurrentState == EWeaponState::Firing;
	return bHasBulletsToReload && bStateOkToReload;
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
	if (FireSound)
	{
		UGameplayStatics::SpawnSoundAttached(
			FireSound, MeshComp, WeaponMuzzleSocketName, FVector::ZeroVector, EAttachLocation::KeepRelativeOffset);
	}
}

void ASWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASWeapon, CurrentAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ASWeapon, CurrentTotalAmmo, COND_OwnerOnly);

	DOREPLIFETIME_CONDITION(ASWeapon, bIsReloading, COND_OwnerOnly);
}
