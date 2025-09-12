// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MauriSkate : ModuleRules
{
	public MauriSkate(ReadOnlyTargetRules Target) : base(Target)
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
			"MauriSkate",
			"MauriSkate/Variant_Platforming",
			"MauriSkate/Variant_Platforming/Animation",
			"MauriSkate/Variant_Combat",
			"MauriSkate/Variant_Combat/AI",
			"MauriSkate/Variant_Combat/Animation",
			"MauriSkate/Variant_Combat/Gameplay",
			"MauriSkate/Variant_Combat/Interfaces",
			"MauriSkate/Variant_Combat/UI",
			"MauriSkate/Variant_SideScrolling",
			"MauriSkate/Variant_SideScrolling/AI",
			"MauriSkate/Variant_SideScrolling/Gameplay",
			"MauriSkate/Variant_SideScrolling/Interfaces",
			"MauriSkate/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
