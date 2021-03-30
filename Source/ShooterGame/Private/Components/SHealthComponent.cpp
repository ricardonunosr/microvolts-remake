// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/SHealthComponent.h"

USHealthComponent::USHealthComponent()
{
	DefaultHealth = 100;
	CurrentHealth = DefaultHealth;
}

void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeAnyDamage);
	}
}

void USHealthComponent::HandleTakeAnyDamage(
	AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f)
	{
		return;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, DefaultHealth);

	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s"), *FString::SanitizeFloat(CurrentHealth));
	OnHealthChanged.Broadcast(this, CurrentHealth, Damage, nullptr, nullptr, nullptr);
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
