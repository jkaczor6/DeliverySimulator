// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DeliverySimulator : ModuleRules
{
	public DeliverySimulator(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { "ChaosVehicles" });

		PublicIncludePaths.AddRange(new string[] {
			"DeliverySimulator",
			"DeliverySimulator/Variant_Horror",
			"DeliverySimulator/Variant_Horror/UI",
			"DeliverySimulator/Variant_Shooter",
			"DeliverySimulator/Variant_Shooter/AI",
			"DeliverySimulator/Variant_Shooter/UI",
			"DeliverySimulator/Variant_Shooter/Weapons"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
