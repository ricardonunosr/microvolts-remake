// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abstract/SWeapon.h"
#include "CoreMinimal.h"

#include "SMelee.generated.h"

/**
 *
 */
UCLASS()
class SHOOTERGAMEWEAPONS_API ASMelee : public ASWeapon
{
	GENERATED_BODY()
protected:
	virtual void Fire() override;

	// UFUNCTION(Server, Reliable, WithValidation)
	// void ServerTryToAttack();

	// UFUNCTION(NetMulticast, Reliable, WithValidation)
	// void MultiAttackMontage();

	// UFUNCTION(Server, Reliable, WithValidation)
	// void ServerMeleeAttackDamage();
};
