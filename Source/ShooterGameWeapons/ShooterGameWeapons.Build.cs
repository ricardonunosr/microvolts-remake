// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class ShooterGameWeapons : ModuleRules
{
    public ShooterGameWeapons(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[]
            {
                Path.Combine(ModuleDirectory, "Public")
            }
            );


        PrivateIncludePaths.AddRange(
            new string[]
            {

            }
            );


        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core","Niagara","PhysicsCore","UMG"
            }
            );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine"
            }
            );


        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {

            }
            );
    }
}
