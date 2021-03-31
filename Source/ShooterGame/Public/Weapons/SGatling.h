// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeapon.h"

#include "SGatling.generated.h"

/**
 *
 */
UCLASS()
class SHOOTERGAME_API ASGatling : public ASWeapon
{
	GENERATED_BODY()
protected:
	void Fire();

public:
	void StartFire() override;
};
