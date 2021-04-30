// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShooterGameWeapons/ShooterGameWeapons.h"

#include "SWeapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFireSignature, ASWeapon*, OwningWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReloadSignature, ASWeapon*, OwningWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReloadedSignature, ASWeapon*, OwningWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSecondaryFireSignature, bool, bIsActive);

class UDamageType;
class UParticleSystem;
class UTexture2D;
class UNiagaraSystem;
class USoundCue;

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

	FWeaponConfig()
	{
		TotalAmmo = 80;
		MagSize = 40;
		HitDamage = 20;
		MaxSpread = 3.0f;
		SpreadIncrement = 0.1f;
		TimeBetweenShots = 0.2f;
		ReloadTime = 1;
	}
};

UCLASS(Abstract, Blueprintable)
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

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerStartFire();

	virtual void StopFire();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerStopFire();

	virtual void OnBurstStarted();

	virtual void OnBurstEnded();

	virtual void HandleFiring();

	virtual void StartSecondaryFire();

	virtual void StartReload(bool bFromReplication = false);

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerStartReload();

	virtual void StopReload();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerStopReload();

	virtual void ResetWeapon();

	EWeaponState GetCurrentState();

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponMuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponAttachSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	EWeaponType WeaponType;

	// Events
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnFireSignature OnFire;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnReloadSignature OnStartReload;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnReloadSignature OnStopReload;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSecondaryFireSignature OnSecondaryFire;

protected:
	UPROPERTY(EditAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = "Weapon")
	int32 CurrentTotalAmmo;

	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = "Weapon")
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

	EWeaponState CurrentState;

	float CurrentSpread;

	bool bWantsToFire;

	bool bIsRefering;

	UPROPERTY(Transient, ReplicatedUsing = OnRepReload, BlueprintReadOnly)
	bool bIsReloading;

	UFUNCTION()
	void OnRepReload();

	bool bIsSecundaryFireActive;

	float LastFireTime;

	float TimeBetweenShots;

	// Timers
	FTimerHandle TimerHandle_TimeBetweenShots;

	FTimerHandle TimerHandle_ReloadTime;

	FTimerHandle TimerHandle_StopReload;

	virtual void Fire() PURE_VIRTUAL(ASWeapon::Fire, );

	virtual void SecondaryFire() PURE_VIRTUAL(ASWeapon::SecondaryFire,);

	void DetermineWeaponState();

	void WeaponTrace();

	void SetWeaponState(EWeaponState NewState);

	bool CanReload();

	bool CanFire();

	void UseAmmo();

	void Reload();

	void PlayImpactEffects(EPhysicalSurface Surface, FVector ImpactPoint);

	void PlayFireEffects(FVector ImpactPoint);

	void PlaySounds(FVector ActorLocation);
};
