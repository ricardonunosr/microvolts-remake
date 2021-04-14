// Fill out your copyright notice in the Description page of Project Settings.

#include "SCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SHealthComponent.h"
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

// Sets default values
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

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	YawRightLimit = 70;
	YawLeftLimit = -70;

	RifleZoomedFOV = 60;
	SniperZoomedFOV = 20;
	ZoomInterSpeed = 20.0f;

	bIsDead = false;
}

void ASCharacter::Destroyed()
{
	Super::Destroyed();

	DestroyLoadout();
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultFOV = CameraComp->FieldOfView;
	DefaultWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	HealthComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
}

void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (GetLocalRole() == ROLE_Authority)
	{
		SpawnLoadout();
	}
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

void ASCharacter::UpdateHeadRotation()
{
	if (CurrentWeapon->WeaponType == EWeaponType::E_Melee)
	{
		FRotator PCRotation = GetControlRotation();
		FRotator ActorRotation = GetActorRotation();

		FRotator NewHeadRotation = PCRotation - ActorRotation;
		NewHeadRotation.Roll = 0;
		NewHeadRotation.Pitch = 0;
		NewHeadRotation.Yaw = UKismetMathLibrary::ClampAngle(NewHeadRotation.Yaw, -179.9, -180);

		USAnimInstance* AnimationBP = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
		if (AnimationBP)
		{
			AnimationBP->HeadRotation = NewHeadRotation;
		}

		if (GetVelocity().Equals(FVector::ZeroVector))
		{
			bUseControllerRotationYaw = false;
			float LocalActorYawnOffset = 0.0f;

			if (NewHeadRotation.Yaw > YawRightLimit)
			{
				LocalActorYawnOffset = 90;
				DesiredYawRotation = ActorRotation.Yaw + LocalActorYawnOffset;
			}
			else if (NewHeadRotation.Yaw < YawLeftLimit)
			{
				LocalActorYawnOffset = -90;
				DesiredYawRotation = ActorRotation.Yaw + LocalActorYawnOffset;
			}
			FRotator Target = FRotator(0, DesiredYawRotation, 0);
			SetActorRotation(UKismetMathLibrary::RInterpTo(ActorRotation, Target, GetWorld()->DeltaTimeSeconds, 5));
		}
		else
		{
			bUseControllerRotationYaw = true;
			AnimationBP->HeadRotation =
				UKismetMathLibrary::RInterpTo(AnimationBP->HeadRotation, FRotator::ZeroRotator, GetWorld()->DeltaTimeSeconds, 5);
		}
	}
}

void ASCharacter::UpdateRifleRotation()
{
	if (CurrentWeapon->WeaponType == EWeaponType::E_Rifle)
	{
		FRotator PCRotation = GetControlRotation();
		FRotator ActorRotation = GetActorRotation();

		FRotator Current = FRotator(PCRotation.Pitch, PCRotation.Yaw, 0);
		Current.Normalize();
		FRotator Target = UKismetMathLibrary::NormalizedDeltaRotator(PCRotation, ActorRotation);

		FRotator Returned = UKismetMathLibrary::RInterpTo(Current, Target, GetWorld()->DeltaTimeSeconds, 15);

		float AimPitch = UKismetMathLibrary::ClampAngle(Returned.Pitch, -90, 90);
		float AimYaw = UKismetMathLibrary::ClampAngle(Returned.Yaw, -90, 90);

		UKismetSystemLibrary::PrintString(
			GetWorld(), "Target Pitch:" + FString::SanitizeFloat(Target.Pitch), true, true, FColor::Red, 0);
		UKismetSystemLibrary::PrintString(
			GetWorld(), "Target Yaw:" + FString::SanitizeFloat(Target.Yaw), true, true, FColor::Red, 0);
		UKismetSystemLibrary::PrintString(
			GetWorld(), "Current Pitch:" + FString::SanitizeFloat(Current.Pitch), true, true, FColor::Red, 0);
		UKismetSystemLibrary::PrintString(
			GetWorld(), "Current Yaw:" + FString::SanitizeFloat(Current.Yaw), true, true, FColor::Red, 0);

		if (GetVelocity().Equals(FVector::ZeroVector))
		{
			USAnimInstance* AnimationBP = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
			if (AnimationBP)
			{
				AnimationBP->AimPitch = AimPitch;
				AnimationBP->AimYaw = AimYaw;
			}

			bUseControllerRotationYaw = false;
			float LocalActorYawnOffset = 0.0f;

			if (AimYaw > 90)
			{
				LocalActorYawnOffset = 90;
				DesiredYawRotation = ActorRotation.Yaw + LocalActorYawnOffset;
			}
			else if (AimYaw < -90)
			{
				LocalActorYawnOffset = -90;
				DesiredYawRotation = ActorRotation.Yaw + LocalActorYawnOffset;
			}
			FRotator TargetYaw = FRotator(0, DesiredYawRotation, 0);
			SetActorRotation(UKismetMathLibrary::RInterpTo(ActorRotation, TargetYaw, GetWorld()->DeltaTimeSeconds, 5));
		}
		else
		{
			bUseControllerRotationYaw = true;

			// UKismetMathLibrary::RInterpTo(Current, FRotator::ZeroRotator, GetWorld()->DeltaTimeSeconds, 5);
		}
	}
}

void ASCharacter::StartFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
	}
}

void ASCharacter::StopFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}

void ASCharacter::StartSecondaryFire()
{
	if (CurrentWeapon)
	{
		EWeaponState CurrentState = CurrentWeapon->GetCurrentState();
		if (CurrentWeapon->WeaponType == EWeaponType::E_Sniper && CurrentState == EWeaponState::Idle)
		{
			CurrentWeapon->StartSecondaryFire();
			CameraComp->SetFieldOfView(SniperZoomedFOV);
			GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed * 0.5;
		}
		else
		{
			CurrentWeapon->StartSecondaryFire();
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
	for (int32 i = 0; i < Loadout.Num(); i++)
	{
		Loadout[i]->ResetWeapon();
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
		if (CurrentWeapon)
		{
			CurrentWeapon->StartReload();
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
	if (CurrentWeapon)
	{
		CurrentWeapon->StopReload();
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

		Destroy();
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
	if (CurrentWeapon)
	{
		if (CurrentWeapon->WeaponType == EWeaponType::E_Rifle)
		{
			float TargetFOV = bWantsToZoom ? RifleZoomedFOV : DefaultFOV;
			float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterSpeed);

			CameraComp->SetFieldOfView(NewFOV);
		};
	}
}

void ASCharacter::SpawnLoadout()
{
	for (int32 i = 0; i < DefaultLoadoutClasses.Num(); i++)
	{
		if (DefaultLoadoutClasses[i])
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			ASWeapon* WeaponSpawn =
				GetWorld()->SpawnActor<ASWeapon>(DefaultLoadoutClasses[i], FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
			WeaponSpawn->SetActorHiddenInGame(true);
			WeaponSpawn->SetOwner(this);
			WeaponSpawn->AttachToComponent(
				GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSpawn->WeaponAttachSocketName);
			WeaponSpawn->OnSecondaryFire.AddDynamic(this, &ASCharacter::StopSecondaryFire);
			AddWeapon(WeaponSpawn);
		}
	}

	if (Loadout.Num() > 0)
	{
		StartEquip(Loadout[0]);
	}
}

void ASCharacter::DestroyLoadout()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		return;
	}

	// remove all weapons from inventory and destroy them
	for (int32 i = Loadout.Num() - 1; i >= 0; i--)
	{
		ASWeapon* Weapon = Loadout[i];
		if (Weapon)
		{
			RemoveWeapon(Weapon);
			Weapon->Destroy();
		}
	}
}

void ASCharacter::AddWeapon(ASWeapon* NewWeapon)
{
	Loadout.AddUnique(NewWeapon);
}

void ASCharacter::RemoveWeapon(ASWeapon* Weapon)
{
	if (Weapon && GetLocalRole() == ROLE_Authority)
	{
		Loadout.RemoveSingle(Weapon);
	}
}

void ASCharacter::StartEquip(ASWeapon* EquipWeapon)
{
	OnEquip.Broadcast(EquipWeapon);
	EquipWeapon->WeaponType == EWeaponType::E_Melee ? JumpMaxCount = 2 : JumpMaxCount = 1;
	if (GetLocalRole() == ROLE_Authority)
	{
		Equip(EquipWeapon);
	}
	else
	{
		ServerStartEquipWeapon(EquipWeapon);
	}
}

void ASCharacter::Equip(ASWeapon* EquipWeapon)
{
	if (CurrentWeapon)
	{
		ASWeapon* LocalLastCurrent = CurrentWeapon;

		if (EquipWeapon != CurrentWeapon)
		{
			CurrentWeapon->SetActorHiddenInGame(true);
			LocalLastCurrent->OnUnEquip();
		}

		EquipWeapon->OnEquip();
		CurrentWeapon = EquipWeapon;
		CurrentWeapon->SetActorHiddenInGame(false);
	}
	else
	{
		CurrentWeapon = EquipWeapon;
		CurrentWeapon->SetActorHiddenInGame(false);
	}
}

void ASCharacter::ServerStartEquipWeapon_Implementation(ASWeapon* EquipWeapon)
{
	StartEquip(EquipWeapon);
}

bool ASCharacter::ServerStartEquipWeapon_Validate(ASWeapon* EquipWeapon)
{
	return true;
}

// Called to bind functionality to input
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

	PlayerInputComponent->BindAction<FEquipActionDelegate>("EquipMelee", IE_Pressed, this, &ASCharacter::StartEquip, Loadout[0]);
	PlayerInputComponent->BindAction<FEquipActionDelegate>("EquipRifle", IE_Pressed, this, &ASCharacter::StartEquip, Loadout[1]);
	PlayerInputComponent->BindAction<FEquipActionDelegate>("EquipShotgun", IE_Pressed, this, &ASCharacter::StartEquip, Loadout[2]);
	PlayerInputComponent->BindAction<FEquipActionDelegate>("EquipSniper", IE_Pressed, this, &ASCharacter::StartEquip, Loadout[3]);
	PlayerInputComponent->BindAction<FEquipActionDelegate>("EquipGatling", IE_Pressed, this, &ASCharacter::StartEquip, Loadout[4]);
	PlayerInputComponent->BindAction<FEquipActionDelegate>(
		"EquipRocketLauncher", IE_Pressed, this, &ASCharacter::StartEquip, Loadout[5]);
	PlayerInputComponent->BindAction<FEquipActionDelegate>(
		"EquipGrenadeLauncher", IE_Pressed, this, &ASCharacter::StartEquip, Loadout[6]);

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

ASWeapon* ASCharacter::GetCurrentWeapon() const
{
	return CurrentWeapon;
}

TArray<ASWeapon*> ASCharacter::GetLoadout() const
{
	return Loadout;
}

bool ASCharacter::GetPawnDied() const
{
	return bIsDead;
}

void ASCharacter::SetPawnDefaults()
{
	bIsDead = false;
}

void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// UpdateHeadRotation();
	// UpdateRifleRotation();

	UpdateZoom(DeltaTime);
}

void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASCharacter, Loadout);
	DOREPLIFETIME(ASCharacter, CurrentWeapon);
	DOREPLIFETIME(ASCharacter, bIsDead);
}
