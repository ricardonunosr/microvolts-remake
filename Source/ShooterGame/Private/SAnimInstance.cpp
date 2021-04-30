// Fill out your copyright notice in the Description page of Project Settings.

#include "SAnimInstance.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SCharacter.h"
#include "SPlayerController.h"
#include "ShooterGameWeapons/Public/Abstract/SWeapon.h"

USAnimInstance::USAnimInstance()
{
	HeadRotation = FRotator::ZeroRotator;
}

void USAnimInstance::UpdateAnimationProperties()
{
	APawn* Pawn = TryGetPawnOwner();

	ASCharacter* Character;

	if (Pawn && (Character = Cast<ASCharacter>(Pawn)) != nullptr)
	{
		Speed = Character->GetVelocity().Size();
		Direction = CalculateDirection(Character->GetVelocity(), Character->GetActorRotation());
		bIsCrouching = Character->bIsCrouched;
		bIsJumping = Character->bWasJumping;
		JumpCurrentCount = Character->JumpCurrentCount;
		bIsFalling = Character->GetCharacterMovement()->IsFalling();
		bIsDead = Character->GetPawnDied();

		/*if (Pawn->GetVelocity().Equals(FVector::ZeroVector))
		{
			FRotator PCRotation = Pawn->GetControlRotation();
			FRotator PawnRotation = Pawn->GetActorRotation();

			FRotator Current = FRotator(AimPitch, AimYaw, 0);
			Current.Normalize();
			FRotator Target = UKismetMathLibrary::NormalizedDeltaRotator(PCRotation, PawnRotation);

			FRotator Returned = UKismetMathLibrary::RInterpTo(Current, Target, GetWorld()->DeltaTimeSeconds, 15);

			float AimPitchC = UKismetMathLibrary::ClampAngle(Returned.Pitch, -90, 90);
			float AimYawC = UKismetMathLibrary::ClampAngle(Returned.Yaw, -90, 90);

			UKismetSystemLibrary::PrintString(
				GetWorld(), "Target Pitch:" + FString::SanitizeFloat(Target.Pitch), true, true, FColor::Red, 0);
			UKismetSystemLibrary::PrintString(
				GetWorld(), "Target Yaw:" + FString::SanitizeFloat(Target.Yaw), true, true, FColor::Red, 0);
			UKismetSystemLibrary::PrintString(
				GetWorld(), "Current Pitch:" + FString::SanitizeFloat(Current.Pitch), true, true, FColor::Red, 0);
			UKismetSystemLibrary::PrintString(
				GetWorld(), "Current Yaw:" + FString::SanitizeFloat(Current.Yaw), true, true, FColor::Red, 0);

			AimPitch = AimPitchC;
			AimYaw = AimYawC;
			Pawn->bUseControllerRotationYaw = false;

			float LocalActorYawnOffset = 0.0f;

			if (AimYaw >= 90)
			{
				LocalActorYawnOffset = 90;
				DesiredYawRotation = PawnRotation.Yaw + LocalActorYawnOffset;
			}
			else if (AimYaw <= -90)
			{
				LocalActorYawnOffset = -90;
				DesiredYawRotation = PawnRotation.Yaw + LocalActorYawnOffset;
			}
			FRotator TargetYaw = FRotator(0, DesiredYawRotation, 0);
			if (TargetYaw.Yaw != PawnRotation.Yaw)
			{
				Pawn->SetActorRotation(UKismetMathLibrary::RInterpTo(PawnRotation, TargetYaw, GetWorld()->DeltaTimeSeconds, 5));
			}
		}
		else
		{
			AimPitch = 0;
			AimYaw = 0;
			Pawn->bUseControllerRotationYaw = true;
		}*/

		if (Character->GetCurrentWeapon() != nullptr)
		{
			CurrentWeaponType = Character->GetCurrentWeapon()->WeaponType;
		}
	}
}
