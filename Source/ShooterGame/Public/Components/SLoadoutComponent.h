// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

#include "SLoadoutComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadoutSpawnSignature, USLoadoutComponent*, LoadoutComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnCurrentWeaponChangedSignature, USLoadoutComponent*, Loadout, ASWeapon*, CurrentWeapon);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SHOOTERGAME_API USLoadoutComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USLoadoutComponent();

protected:
	virtual void BeginPlay() override;

	/*Classes of weapons to spawn in loadout*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TArray<TSubclassOf<class ASWeapon>> DefaultLoadoutClasses;

	/*Array of spawned instances of DefaultLoadoutClasses*/
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Loadout")
	TArray<class ASWeapon*> Loadout;

	/*Current equipped weapon*/
	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = "Loadout")
	class ASWeapon* CurrentWeapon;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnLoadoutSpawnSignature OnLoadoutSpawn;

	void SpawnLoadout();

	void DestroyLoadout();

	void AddWeapon(class ASWeapon* NewWeapon);

	void RemoveWeapon(class ASWeapon* Weapon);

public:
	virtual void BeginDestroy() override;

	void Equip(ASWeapon* EquipWeapon);

	ASWeapon* GetCurrentWeapon() const;

	TArray<ASWeapon*> GetLoadout() const;

	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnCurrentWeaponChangedSignature OnCurrentWeaponChanged;
};
