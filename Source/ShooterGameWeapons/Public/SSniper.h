// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abstract/SHitscanWeapon.h"
#include "CoreMinimal.h"

#include "SSniper.generated.h"

/**
 *
 */
UCLASS()
class SHOOTERGAMEWEAPONS_API ASSniper : public ASHitscanWeapon
{
	GENERATED_BODY()

public:
	virtual void Fire() override;
};
