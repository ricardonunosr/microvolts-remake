// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterGame/ShooterGame.h"
#include "ShooterGameWeapons/ShooterGameWeapons.h"

#include "SCharacter.generated.h"

DECLARE_DELEGATE_OneParam(FEquipActionDelegate, int32);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipSignature, ASWeapon*, CurrentWeapon);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathSignature, ASCharacter*, DeadPawn);

class ASWeapon;

UCLASS()
class SHOOTERGAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASCharacter();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class USpringArmComponent* SpringComp;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class UCameraComponent* CameraComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	class USHealthComponent* HealthComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	class USLoadoutComponent* LoadoutComp;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void DoJump();

	void BeginCrouch();

	void EndCrouch();

	float DefaultWalkSpeed;

	void StartEquip(int32 LoadoutNumber);

	UFUNCTION(Reliable, server, WithValidation)
	void ServerStartEquipWeapon(int32 LoadoutNumber);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnEquipSignature OnEquip;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDeathSignature OnDeath;

	void StartFire();

	void StopFire();

	void StartSecondaryFire();

	UFUNCTION(BlueprintCallable)
	void ResetWeapons();

	void StartReload();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerStartReload();

	void StopReload();

	bool bWantsToZoom;

	UPROPERTY(Replicated)
	bool bIsAttacking;

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
	float ZoomInterSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	FName WeaponAttachSocketName;

public:
	virtual void Destroyed() override;

	void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;

	UFUNCTION(BlueprintCallable)
	TArray<class ASWeapon*> GetLoadout();

	UFUNCTION(BlueprintCallable)
	class ASWeapon* GetCurrentWeapon();

	bool GetPawnDied() const;
};
