// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/SLoadoutComponent.h"

#include "Net/UnrealNetwork.h"
#include "ShooterGameWeapons/Public/Abstract/SWeapon.h"

#include <ShooterGame/Public/SCharacter.h>

USLoadoutComponent::USLoadoutComponent()
{
	SetIsReplicatedByDefault(true);
}

void USLoadoutComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->GetLocalRole() == ROLE_Authority)
	{
		SpawnLoadout();
	}
	OnLoadoutSpawn.Broadcast(this);
}

void USLoadoutComponent::SpawnLoadout()
{
	for (int32 i = 0; i < DefaultLoadoutClasses.Num(); i++)
	{
		if (DefaultLoadoutClasses[i])
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			ASWeapon* WeaponSpawn =
				GetWorld()->SpawnActor<ASWeapon>(DefaultLoadoutClasses[i], FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
			WeaponSpawn->SetActorHiddenInGame(true);
			WeaponSpawn->SetOwner(GetOwner());
			WeaponSpawn->AttachToComponent(Cast<ASCharacter>(GetOwner())->GetMesh(),
				FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSpawn->WeaponAttachSocketName);
			AddWeapon(WeaponSpawn);
		}
	}

	if (Loadout.Num() > 0)
	{
		Equip(Loadout[0]);
	}
}

void USLoadoutComponent::DestroyLoadout()
{
	/* Remove all weapons from inventory and destroy them*/
	for (int32 i = Loadout.Num() - 1; i >= 0; i--)
	{
		ASWeapon* Weapon = Loadout[i];
		if (Weapon)
		{
			RemoveWeapon(Weapon);
			Weapon->Destroy();
		}
	}
}

void USLoadoutComponent::AddWeapon(ASWeapon* NewWeapon)
{
	Loadout.AddUnique(NewWeapon);
}

void USLoadoutComponent::RemoveWeapon(ASWeapon* Weapon)
{
	if (Weapon && GetOwner()->GetLocalRole() == ROLE_Authority)
	{
		Loadout.RemoveSingle(Weapon);
	}
}

void USLoadoutComponent::BeginDestroy()
{
	Super::BeginDestroy();

	DestroyLoadout();
}

void USLoadoutComponent::Equip(ASWeapon* EquipWeapon)
{
	if (CurrentWeapon)
	{
		ASWeapon* LocalLastCurrent = CurrentWeapon;

		if (EquipWeapon != CurrentWeapon)
		{
			CurrentWeapon->SetActorHiddenInGame(true);
			LocalLastCurrent->OnUnEquip();
		}

		EquipWeapon->OnEquip();
		CurrentWeapon = EquipWeapon;
		CurrentWeapon->SetActorHiddenInGame(false);
	}
	else
	{
		CurrentWeapon = EquipWeapon;
		CurrentWeapon->SetActorHiddenInGame(false);
	}
	OnCurrentWeaponChanged.Broadcast(this, CurrentWeapon);
}

ASWeapon* USLoadoutComponent::GetCurrentWeapon() const
{
	return CurrentWeapon;
}

TArray<ASWeapon*> USLoadoutComponent::GetLoadout() const
{
	return Loadout;
}

void USLoadoutComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USLoadoutComponent, Loadout);
	DOREPLIFETIME(USLoadoutComponent, CurrentWeapon);
}
