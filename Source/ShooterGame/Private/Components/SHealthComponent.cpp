// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/SHealthComponent.h"

#include "Net/UnrealNetwork.h"

#include <Modes/SFreeForAll.h>

USHealthComponent::USHealthComponent()
{
	DefaultHealth = 100;
	CurrentHealth = DefaultHealth;
	bIsDead = false;

	SetIsReplicatedByDefault(true);
}

void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeAnyDamage);
		}
	}
}

void USHealthComponent::OnRep_Health(float OldHealth)
{
	float Damage = CurrentHealth - OldHealth;

	OnHealthChanged.Broadcast(this, CurrentHealth, Damage, nullptr, nullptr, nullptr);
}

void USHealthComponent::HandleTakeAnyDamage(
	AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f && bIsDead)
	{
		return;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, DefaultHealth);

	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s"), *FString::SanitizeFloat(CurrentHealth));
	OnHealthChanged.Broadcast(this, CurrentHealth, Damage, DamageType, InstigatedBy, DamageCauser);

	bIsDead = CurrentHealth <= 0.0f;
}

void USHealthComponent::Heal(float HealAmount)
{
	CurrentHealth = FMath::Min(CurrentHealth + HealAmount, DefaultHealth);

	OnHealthChanged.Broadcast(this, CurrentHealth, 0, nullptr, nullptr, nullptr);
}

float USHealthComponent::GetHealth() const
{
	return CurrentHealth;
}

void USHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USHealthComponent, CurrentHealth);
	DOREPLIFETIME(USHealthComponent, bIsDead);
}
