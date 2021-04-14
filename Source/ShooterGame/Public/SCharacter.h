// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterGame/ShooterGame.h"
#include "ShooterGameWeapons/ShooterGameWeapons.h"

#include "SCharacter.generated.h"

DECLARE_DELEGATE_OneParam(FEquipActionDelegate, ASWeapon*);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipSignature, ASWeapon*, CurrentWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);

class USpringArmComponent;
class UCameraComponent;
class ASWeapon;
class USHealthComponent;

UCLASS()
class SHOOTERGAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

	void Destroyed() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USpringArmComponent* SpringComp;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UCameraComponent* CameraComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	USHealthComponent* HealthComp;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void DoJump();

	void BeginCrouch();

	void EndCrouch();

	void UpdateHeadRotation();

	void UpdateRifleRotation();

	float DesiredYawRotation;

	UPROPERTY(EditDefaultsOnly, Category = "Character", meta = (ClampMin = 0, ClampMax = 90))
	float YawRightLimit;

	UPROPERTY(EditDefaultsOnly, Category = "Character", meta = (ClampMin = -90, ClampMax = 0))
	float YawLeftLimit;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character")
	TArray<TSubclassOf<ASWeapon>> DefaultLoadoutClasses;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Character")
	TArray<ASWeapon*> Loadout;

	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = "Character")
	ASWeapon* CurrentWeapon;

	float DefaultWalkSpeed;

	void SpawnLoadout();

	void DestroyLoadout();

	void AddWeapon(ASWeapon* NewWeapon);

	void RemoveWeapon(ASWeapon* Weapon);

	void StartEquip(ASWeapon* EquipWeapon);

	UFUNCTION(Reliable, server, WithValidation)
	void ServerStartEquipWeapon(ASWeapon* EquipWeapon);

	void Equip(ASWeapon* EquipWeapon);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnEquipSignature OnEquip;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDeathSignature OnDeath;

	void StartFire();

	void StopFire();

	void StartSecondaryFire();

	UFUNCTION()
	void StopSecondaryFire(bool isActive);

	UFUNCTION(BlueprintCallable)
	void ResetWeapons();

	void StartReload();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerStartReload();

	void StopReload();

	bool bWantsToZoom;

	UPROPERTY(Replicated)
	bool bIsDead;

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType,
		class AController* InstigatedBy, AActor* DamageCauser);

	void StartZoom();

	void EndZoom();

	void UpdateZoom(float DeltaTime);

	float DefaultFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	float RifleZoomedFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	float SniperZoomedFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	float ZoomInterSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	FName WeaponAttachSocketName;

public:
	void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;

	ASWeapon* GetCurrentWeapon() const;

	TArray<ASWeapon*> GetLoadout() const;

	bool GetPawnDied() const;

	void SetPawnDefaults();
};
