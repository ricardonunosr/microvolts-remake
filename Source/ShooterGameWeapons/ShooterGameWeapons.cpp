// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGameWeapons.h"
#include "Logging.h"

#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FShooterGameWeapons"

void FShooterGameWeapons::StartupModule()
{
}

void FShooterGameWeapons::ShutdownModule()
{
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FShooterGameWeapons, ShooterGameWeapons);