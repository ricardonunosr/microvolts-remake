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

// Sets default values
ASWeapon::ASWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = MeshComp;

	TimeBetweenShots = 60 / Config.RateOfFire;
	CurrentTotalAmmo = 0;
	CurrentAmmo = 0;
}

void ASWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	CurrentTotalAmmo = Config.TotalAmmo;
	CurrentAmmo = Config.MagSize;
}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void ASWeapon::Reload()
{
	if (CurrentTotalAmmo <= 0)
		return;
	int32 AmmoDif = Config.MagSize - CurrentAmmo;
	int32 Delta = FMath::Min(CurrentTotalAmmo, AmmoDif);
	CurrentAmmo += Delta;
	CurrentTotalAmmo -= Delta;

	bIsReloading = false;
	GetWorldTimerManager().ClearTimer(TimerHandle_ReloadTime);
}

void ASWeapon::Fire()
{
	AActor* MyOwner = GetOwner();

	if (MyOwner && !bIsReloading)
	{
		FVector EyeLocation;
		FRotator EyeRotation;

		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();

		const int32 RandomSeed = FMath::Rand();
		FRandomStream WeaponRandomStream(RandomSeed);
		const float ConeHalfAngle = FMath::DegreesToRadians(CurrentSpread * 0.5f);

		const FVector ShootDir = WeaponRandomStream.VRandCone(ShotDirection, ConeHalfAngle, ConeHalfAngle);

		FVector TraceEnd = EyeLocation + (ShootDir * 100000);

		FHitResult Hit;

		UKismetSystemLibrary::PrintString(GetWorld(), "CurrentSpread" + FString::SanitizeFloat(CurrentSpread));

		CurrentSpread = FMath::Min(Config.MaxSpread, CurrentSpread + Config.SpreadIncrement);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;

		FVector TracerEndPoint = TraceEnd;

		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECC_Visibility, QueryParams))
		{
			UGameplayStatics::ApplyPointDamage(
				Hit.GetActor(), 20.0f, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);
			if (CurrentAmmo == 0)
				StartReload();
			if (CurrentAmmo > 0)
				CurrentAmmo--;

			EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
			PlayImpactEffects(SurfaceType, Hit.ImpactPoint);
			TracerEndPoint = Hit.ImpactPoint;
		}

		PlayFireEffects(TracerEndPoint);
		PlaySounds(MyOwner->GetActorLocation());

		if (DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Red, false, 1.0f, 0, 1.0f);
		}
	}
}

void ASWeapon::PlayImpactEffects(EPhysicalSurface Surface, FVector ImpactPoint)
{
	FVector MuzzleLocation = MeshComp->GetSocketLocation(WeaponMuzzleSocketName);
	FVector ShotDirection = ImpactPoint - MuzzleLocation;
	ShotDirection.Normalize();

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DefaultImpactEffect, ImpactPoint, ShotDirection.Rotation());
}

void ASWeapon::PlaySounds(FVector ActorLocation)
{
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

bool ASWeapon::CanReload()
{
	return CurrentAmmo < Config.MagSize && CurrentTotalAmmo > 0;
}

// Called every frame
void ASWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASWeapon::StartFire()
{
	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);

	if (CurrentAmmo <= 0)
		return;
	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASWeapon::Fire, TimeBetweenShots, true, FirstDelay);
}

void ASWeapon::StopFire()
{
	CurrentSpread = 0;
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

void ASWeapon::StartReload()
{
	if (CanReload())
	{
		bIsReloading = true;
		OnReload.Broadcast(this);
		GetWorldTimerManager().SetTimer(TimerHandle_ReloadTime, this, &ASWeapon::Reload, Config.ReloadTime, true);
	}
}

void ASWeapon::ResetWeapon()
{
	CurrentTotalAmmo = Config.TotalAmmo;
	CurrentAmmo = Config.MagSize;
}
