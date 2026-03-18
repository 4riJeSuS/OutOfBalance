// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OutOfBalance : ModuleRules
{
	public OutOfBalance(ReadOnlyTargetRules Target) : base(Target)
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

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"OutOfBalance",
			"OutOfBalance/Variant_Platforming",
			"OutOfBalance/Variant_Platforming/Animation",
			"OutOfBalance/Variant_Combat",
			"OutOfBalance/Variant_Combat/AI",
			"OutOfBalance/Variant_Combat/Animation",
			"OutOfBalance/Variant_Combat/Gameplay",
			"OutOfBalance/Variant_Combat/Interfaces",
			"OutOfBalance/Variant_Combat/UI",
			"OutOfBalance/Variant_SideScrolling",
			"OutOfBalance/Variant_SideScrolling/AI",
			"OutOfBalance/Variant_SideScrolling/Gameplay",
			"OutOfBalance/Variant_SideScrolling/Interfaces",
			"OutOfBalance/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
