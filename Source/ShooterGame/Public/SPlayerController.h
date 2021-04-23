// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "SPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPossesionSignature, ASPlayerController*, PlayerController);

UCLASS()
class SHOOTERGAME_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnPossesionSignature OnPossesion;

public:
	void BeginPlay() override;

	virtual void SetupInputComponent() override;

	void HandleOptionsMenu();

	virtual void StartFire(uint8 FireModeNum = 0) override;

	virtual void PressedFire();

	virtual void ReleasedFire();
};
