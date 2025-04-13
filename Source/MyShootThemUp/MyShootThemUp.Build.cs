// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MyShootThemUp : ModuleRules
{
	public MyShootThemUp(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		PublicIncludePaths.AddRange(new string[] 
		{ 
			"MyShootThemUp/Public/Player",
			"MyShootThemUp/Public/Components", 
			"MyShootThemUp/Public/Dev", 
			"MyShootThemUp/Public/Weapon" ,
			"MyShootThemUp/Public/UI", 
			"MyShootThemUp/Public/Animations",
			"MyShootThemUp/Public/Pickups"

		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
