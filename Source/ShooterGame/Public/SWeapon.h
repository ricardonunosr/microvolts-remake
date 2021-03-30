// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShooterGame/ShooterGame.h"

#include "SWeapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReloadSignature, ASWeapon*, OwningWeapon);

class UDamageType;
class UParticleSystem;
class UTexture2D;
class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FWeaponConfig
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Ammo)
	int32 TotalAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Ammo)
	int32 MagSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = WeaponStat)
	float MaxSpread;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = WeaponStat)
	float SpreadIncrement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = WeaponStat)
	float RateOfFire;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = WeaponStat)
	float ReloadTime;

	FWeaponConfig()
	{
		TotalAmmo = 80;
		MagSize = 40;
		MaxSpread = 3.0f;
		SpreadIncrement = 0.1f;
		RateOfFire = 600.0f;
		ReloadTime = 1;
	}
};

UCLASS()
class SHOOTERGAME_API ASWeapon : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	FTimerHandle TimerHandle_TimeBetweenShots;

	FTimerHandle TimerHandle_ReloadTime;

	float LastFireTime;

	float TimeBetweenShots;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	int32 CurrentTotalAmmo;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	int32 CurrentAmmo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	FName Name;

	void Reload();

	void Fire();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UNiagaraSystem* TracerEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* DefaultImpactEffect;

	void PlayImpactEffects(EPhysicalSurface Surface, FVector ImpactPoint);

	void PlaySounds(FVector ActorLocation);

	void PlayFireEffects(FVector ImpactPoint);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = HUD)
	UTexture2D* PrimaryIcon;

	float CurrentSpread;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FWeaponConfig Config;

	bool bIsReloading;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnReloadSignature OnReload;

	bool CanReload();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void PostInitializeComponents();

	virtual void StartFire();

	void StopFire();

	void StartReload();

	void ResetWeapon();

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponMuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponAttachSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	EWeaponType WeaponType;
};
