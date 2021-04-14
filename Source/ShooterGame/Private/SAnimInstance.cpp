// Fill out your copyright notice in the Description page of Project Settings.

#include "SAnimInstance.h"

#include "SCharacter.h"
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
		if (Character->GetCurrentWeapon() != nullptr)
		{
			CurrentWeaponType = Character->GetCurrentWeapon()->WeaponType;
		}
	}
}
