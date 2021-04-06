// Fill out your copyright notice in the Description page of Project Settings.

#include "SCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/SHealthComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SWeapon.h"

#include <GameFramework/CharacterMovementComponent.h>
#include <Kismet/KismetSystemLibrary.h>
#include <SAnimInstance.h>

// Sets default values
ASCharacter::ASCharacter()
{
	SpringComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringComp"));
	SpringComp->bUsePawnControlRotation = true;
	SpringComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringComp);

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	YawRightLimit = 70;
	YawLeftLimit = -70;

	ZoomedFOV = 60;
	ZoomInterSpeed = 20.0f;
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultFOV = CameraComp->FieldOfView;
	DefaultWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
}

void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SpawnLoadout();
	StartEquip(0, EWeaponType::E_Melee);
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
		CurrentWeapon->StartSecondaryFire();
		EWeaponState::Type CurrentState = CurrentWeapon->GetCurrentState();
		if (CurrentWeapon->WeaponType == EWeaponType::E_Sniper && CurrentState == EWeaponState::SecondaryFiring)
		{
			GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed * 0.5;
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
		}
	}
}

void ASCharacter::StopSecondaryFire()
{
	if (CurrentWeapon)
	{
		// CurrentWeapon->StopSecondaryFire();
	}
}

void ASCharacter::UpdateWeaponAnimation(EWeaponType CurrentWeaponType)
{
	USAnimInstance* AnimationBP = Cast<USAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimationBP)
	{
		AnimationBP->CurrentWeaponType = CurrentWeaponType;
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
	if (CurrentWeapon)
	{
		CurrentWeapon->StartReload();
	}
}

void ASCharacter::StopReload()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopReload();
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
			float TargetFOV = bWantsToZoom ? ZoomedFOV : DefaultFOV;
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
			AddWeapon(WeaponSpawn);
		}
	}
}

void ASCharacter::AddWeapon(ASWeapon* NewWeapon)
{
	Loadout.AddUnique(NewWeapon);
}

void ASCharacter::StartEquip(int32 LoadoutIndex, EWeaponType WeaponType)
{
	WeaponType == EWeaponType::E_Melee ? JumpMaxCount = 2 : JumpMaxCount = 1;
	UpdateWeaponAnimation(WeaponType);
	Equip(Loadout[LoadoutIndex]);
	OnEquip.Broadcast(Loadout[LoadoutIndex]);
}

void ASCharacter::Equip(ASWeapon* EquipWeapon)
{
	if (CurrentWeapon)
	{
		if (EquipWeapon != CurrentWeapon)
		{
			CurrentWeapon->SetActorHiddenInGame(true);
			CurrentWeapon->StartReload();
		}

		EquipWeapon->OnUnEquip();
		EquipWeapon->StopReload();
		CurrentWeapon = EquipWeapon;
		CurrentWeapon->SetActorHiddenInGame(false);
	}
	else
	{
		CurrentWeapon = EquipWeapon;
		CurrentWeapon->SetActorHiddenInGame(false);
	}
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

	PlayerInputComponent->BindAction<FEquipActionDelegate>(
		"EquipMelee", IE_Pressed, this, &ASCharacter::StartEquip, 0, EWeaponType::E_Melee);
	PlayerInputComponent->BindAction<FEquipActionDelegate>(
		"EquipRifle", IE_Pressed, this, &ASCharacter::StartEquip, 1, EWeaponType::E_Rifle);
	PlayerInputComponent->BindAction<FEquipActionDelegate>(
		"EquipShotgun", IE_Pressed, this, &ASCharacter::StartEquip, 2, EWeaponType::E_Shotgun);
	PlayerInputComponent->BindAction<FEquipActionDelegate>(
		"EquipSniper", IE_Pressed, this, &ASCharacter::StartEquip, 3, EWeaponType::E_Sniper);
	PlayerInputComponent->BindAction<FEquipActionDelegate>(
		"EquipGatling", IE_Pressed, this, &ASCharacter::StartEquip, 4, EWeaponType::E_Gatling);
	PlayerInputComponent->BindAction<FEquipActionDelegate>(
		"EquipRocketLauncher", IE_Pressed, this, &ASCharacter::StartEquip, 5, EWeaponType::E_RocketLauncher);
	PlayerInputComponent->BindAction<FEquipActionDelegate>(
		"EquipGrenadeLauncher", IE_Pressed, this, &ASCharacter::StartEquip, 6, EWeaponType::E_GrenadeLauncher);

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

void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// UpdateHeadRotation();
	// UpdateRifleRotation();

	UpdateZoom(DeltaTime);
}
