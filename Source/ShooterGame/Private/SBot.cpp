// Fill out your copyright notice in the Description page of Project Settings.

#include "SBot.h"

#include "Components/SHealthComponent.h"

// Sets default values
ASBot::ASBot()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));

	
	bDied = false;
}

// Called when the game starts or when spawned
void ASBot::BeginPlay()
{
	Super::BeginPlay();

	HealthComp->OnHealthChanged.AddDynamic(this, &ASBot::OnHealthChanged);
}

void ASBot::OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta,
	const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Log, TEXT("HealthChangedBot"));

	if (Health <= 0.0f && !bDied)
	{
		bDied = true;

		MeshComp->SetAllBodiesSimulatePhysics(true);
		MeshComp->SetSimulatePhysics(true);
		MeshComp->WakeAllRigidBodies();

		SetLifeSpan(.5f);
	}
}
