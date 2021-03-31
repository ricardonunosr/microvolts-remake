// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeapon.h"

#include "SGrenadeLauncher.generated.h"

/**
 *
 */
UCLASS()
class SHOOTERGAME_API ASGrenadeLauncher : public ASWeapon
{
	GENERATED_BODY()
protected:
	void Fire();

public:
	void StartFire() override;
};
