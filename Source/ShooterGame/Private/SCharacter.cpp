// Fill out your copyright notice in the Description page of Project Settings.

#include "SCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SHealthComponent.h"
#include "Components/SLoadoutComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "ShooterGameWeapons/Public/Abstract/SWeapon.h"

#include <GameFramework/CharacterMovementComponent.h>
#include <Kismet/KismetSystemLibrary.h>
#include <SAnimInstance.h>

ASCharacter::ASCharacter()
{
	class USkeletalMeshComponent* MeshComp = GetMesh();

	MeshComp->SetCollisionObjectType(ECC_Pawn);
	MeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	MeshComp->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);
	MeshComp->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Block);
	MeshComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	SpringComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringComp"));
	SpringComp->bUsePawnControlRotation = true;
	SpringComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringComp);

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));

	LoadoutComp = CreateDefaultSubobject<USLoadoutComponent>(TEXT("LoadoutComp"));

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	RifleZoomedFOV = 60;
	SniperZoomedFOV = 20;
	ZoomInterSpeed = 20.0f;

	bIsDead = false;
}

void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultFOV = CameraComp->FieldOfView;
	DefaultWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	HealthComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
}

void ASCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
}

void ASCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}

void ASCharacter::DoJump()
{
	Jump();
}

void ASCharacter::BeginCrouch()
{
	Crouch();
}

void ASCharacter::EndCrouch()
{
	UnCrouch();
}

void ASCharacter::StartFire()
{
	if (LoadoutComp->GetCurrentWeapon())
	{
		LoadoutComp->GetCurrentWeapon()->StartFire();
	}
}

void ASCharacter::StopFire()
{
	if (LoadoutComp->GetCurrentWeapon())
	{
		LoadoutComp->GetCurrentWeapon()->StopFire();
	}
}

void ASCharacter::StartSecondaryFire()
{
	if (LoadoutComp->GetCurrentWeapon())
	{
		EWeaponState CurrentState = LoadoutComp->GetCurrentWeapon()->GetCurrentState();
		if (LoadoutComp->GetCurrentWeapon()->WeaponType == EWeaponType::E_Sniper && CurrentState == EWeaponState::Idle)
		{
			LoadoutComp->GetCurrentWeapon()->StartSecondaryFire();
			CameraComp->SetFieldOfView(SniperZoomedFOV);
			GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed * 0.5;
		}
		else
		{
			LoadoutComp->GetCurrentWeapon()->StartSecondaryFire();
		}
	}
}

void ASCharacter::StopSecondaryFire(bool IsActive)
{
	if (!IsActive)
	{
		CameraComp->SetFieldOfView(DefaultFOV);
		GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
	}
}

void ASCharacter::ResetWeapons()
{
	for (int32 i = 0; i < LoadoutComp->GetLoadout().Num(); i++)
	{
		LoadoutComp->GetLoadout()[i]->ResetWeapon();
	}
}

void ASCharacter::StartReload()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerStartReload();
	}
	else
	{
		if (LoadoutComp->GetCurrentWeapon())
		{
			LoadoutComp->GetCurrentWeapon()->StartReload();
		}
	}
}

void ASCharacter::ServerStartReload_Implementation()
{
	StartReload();
}

bool ASCharacter::ServerStartReload_Validate()
{
	return true;
}

void ASCharacter::StopReload()
{
	if (LoadoutComp->GetCurrentWeapon())
	{
		LoadoutComp->GetCurrentWeapon()->StopReload();
	}
}

void ASCharacter::OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta,
	const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f && !bIsDead)
	{
		bIsDead = true;

		OnDeath.Broadcast();

		GetMovementComponent()->StopMovementImmediately();

		DetachFromControllerPendingDestroy();

		SetLifeSpan(.2f);
	}
}

void ASCharacter::StartZoom()
{
	bWantsToZoom = true;
}

void ASCharacter::EndZoom()
{
	bWantsToZoom = false;
}

void ASCharacter::UpdateZoom(float DeltaTime)
{
	if (LoadoutComp->GetCurrentWeapon())
	{
		if (LoadoutComp->GetCurrentWeapon()->WeaponType == EWeaponType::E_Rifle)
		{
			float TargetFOV = bWantsToZoom ? RifleZoomedFOV : DefaultFOV;
			float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterSpeed);

			CameraComp->SetFieldOfView(NewFOV);
		};
	}
}

void ASCharacter::StartEquip(int32 LoadoutNumber)
{
	ASWeapon* EquipWeapon = LoadoutComp->GetLoadout()[LoadoutNumber];

	OnEquip.Broadcast(EquipWeapon);

	if (GetLocalRole() == ROLE_Authority)
	{
		LoadoutComp->Equip(EquipWeapon);
	}
	else
	{
		ServerStartEquipWeapon(LoadoutNumber);
	}
}

void ASCharacter::ServerStartEquipWeapon_Implementation(int32 LoadoutNumber)
{
	StartEquip(LoadoutNumber);
}

bool ASCharacter::ServerStartEquipWeapon_Validate(int32 LoadoutNumber)
{
	return true;
}

void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Forward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Right", this, &ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &ASCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookSide", this, &ASCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::DoJump);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASCharacter::EndCrouch);

	PlayerInputComponent->BindAction<FEquipActionDelegate>("EquipMelee", IE_Pressed, this, &ASCharacter::StartEquip, 0);
	PlayerInputComponent->BindAction<FEquipActionDelegate>("EquipRifle", IE_Pressed, this, &ASCharacter::StartEquip, 1);
	PlayerInputComponent->BindAction<FEquipActionDelegate>("EquipShotgun", IE_Pressed, this, &ASCharacter::StartEquip, 2);
	PlayerInputComponent->BindAction<FEquipActionDelegate>("EquipSniper", IE_Pressed, this, &ASCharacter::StartEquip, 3);
	PlayerInputComponent->BindAction<FEquipActionDelegate>("EquipGatling", IE_Pressed, this, &ASCharacter::StartEquip, 4);
	PlayerInputComponent->BindAction<FEquipActionDelegate>("EquipRocketLauncher", IE_Pressed, this, &ASCharacter::StartEquip, 5);
	PlayerInputComponent->BindAction<FEquipActionDelegate>("EquipGrenadeLauncher", IE_Pressed, this, &ASCharacter::StartEquip, 6);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASCharacter::StopFire);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ASCharacter::StartReload);

	PlayerInputComponent->BindAction("SecondaryFire", IE_Pressed, this, &ASCharacter::StartSecondaryFire);
}

FVector ASCharacter::GetPawnViewLocation() const
{
	if (CameraComp)
	{
		return CameraComp->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}

class TArray<ASWeapon*> ASCharacter::GetLoadout()
{
	return LoadoutComp->GetLoadout();
}

class ASWeapon*
ASCharacter::GetCurrentWeapon()
{
	return LoadoutComp->GetCurrentWeapon();
}

bool ASCharacter::GetPawnDied() const
{
	return bIsDead;
}

void ASCharacter::Destroyed()
{
	Super::Destroyed();

	HealthComp->DestroyComponent();
	LoadoutComp->DestroyComponent();
}

void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// CHANGE THIS
	if (LoadoutComp->GetCurrentWeapon())
	{
		LoadoutComp->GetCurrentWeapon()->WeaponType == EWeaponType::E_Melee ? JumpMaxCount = 2 : JumpMaxCount = 1;
	}

	UpdateZoom(DeltaTime);
}

void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASCharacter, bIsDead);
}
