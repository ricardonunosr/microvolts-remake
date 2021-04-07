// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShooterGameWeapons/ShooterGameWeapons.h"

#include "SWeapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReloadSignature, ASWeapon*, OwningWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSecondaryFireSignature);

class UDamageType;
class UParticleSystem;
class UTexture2D;
class UNiagaraSystem;
class USoundCue;

namespace EWeaponState
{
enum Type
{
	Idle,
	Firing,
	Reloading,
	Equipping,
	SecondaryFiring,
};
}

USTRUCT(BlueprintType)
struct FWeaponConfig
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Ammo)
	int32 TotalAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Ammo)
	int32 MagSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = WeaponStat)
	float HitDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = WeaponStat)
	float MaxSpread;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = WeaponStat)
	float SpreadIncrement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = WeaponStat)
	float TimeBetweenShots;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = WeaponStat)
	float ReloadTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = WeaponStat)
	bool bHasAutomaticFiring;

	FWeaponConfig()
	{
		TotalAmmo = 80;
		MagSize = 40;
		HitDamage = 20;
		MaxSpread = 3.0f;
		SpreadIncrement = 0.1f;
		TimeBetweenShots = 0.2f;
		ReloadTime = 1;
		bHasAutomaticFiring = false;
	}
};

UCLASS()
class SHOOTERGAMEWEAPONS_API ASWeapon : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASWeapon();

	virtual void PostInitializeComponents() override;

	virtual void OnEquip();

	virtual void OnUnEquip();

	virtual void StartFire();

	virtual void StopFire();

	virtual void OnBurstStarted();

	virtual void OnBurstEnded();

	virtual void HandleFiring();

	virtual void StartSecondaryFire();

	virtual void StopSecondaryFire();

	virtual void StartReload();

	virtual void StopReload();

	virtual void ResetWeapon();

	EWeaponState::Type GetCurrentState();

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponMuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponAttachSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	EWeaponType WeaponType;

	// Events
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnReloadSignature OnReload;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSecondaryFireSignature OnSecondaryFire;

protected:
	UPROPERTY(EditAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	FName Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	int32 CurrentTotalAmmo;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	int32 CurrentAmmo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effects)
	UNiagaraSystem* TracerEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effects)
	UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundBase* FireSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = HUD)
	UTexture2D* PrimaryIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = HUD)
	UTexture2D* Crosshair;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Config)
	FWeaponConfig WeaponConfig;

	EWeaponState::Type CurrentState;

	float CurrentSpread;

	bool bIsRefering;

	bool bIsReloading;

	bool bIsSecundaryFireActive;

	float LastFireTime;

	float TimeBetweenShots;

	// Timers
	FTimerHandle TimerHandle_TimeBetweenShots;

	FTimerHandle TimerHandle_ReloadTime;

	virtual void Fire() PURE_VIRTUAL(ASWeapon::Fire, );

	virtual void SecondaryFire();

	void WeaponTrace();

	void SetWeaponState(EWeaponState::Type NewState);

	bool CanReload();

	bool CanFire();

	void UseAmmo();

	void Reload();

	void PlayImpactEffects(EPhysicalSurface Surface, FVector ImpactPoint);

	void PlayFireEffects(FVector ImpactPoint);

	void PlaySounds(FVector ActorLocation);
};
