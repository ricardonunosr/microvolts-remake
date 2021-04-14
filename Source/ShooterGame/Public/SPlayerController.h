// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "SPlayerController.generated.h"

UCLASS()
class SHOOTERGAME_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void AcknowledgePossession(class APawn* P);

public:
	virtual void SetupInputComponent() override;

	void HandleOptionsMenu();
};
