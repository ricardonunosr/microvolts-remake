// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abstract/SHitscanWeapon.h"
#include "CoreMinimal.h"

#include "SSniper.generated.h"

class UUserWidget;

UCLASS()
class SHOOTERGAMEWEAPONS_API ASSniper : public ASHitscanWeapon
{
	GENERATED_BODY()

public:
	virtual void Fire() override;

	virtual void SecondaryFire() override;

	virtual void OnUnEquip() override;

	bool bIsActivated;

	UPROPERTY(EditDefaultsOnly,Category=Weapon)
	TSubclassOf<UUserWidget> ScopeWidgetClass;

	UUserWidget* ScopeWidget;
	
	void ActivateScope();

	void DeactivateScope();
};
