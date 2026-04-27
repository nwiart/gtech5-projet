// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GT5_Projet : ModuleRules
{
	public GT5_Projet(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "Paper2D", "PaperZD", "Niagara" });

		// MetaSound support for advanced music system
		PublicDependencyModuleNames.AddRange(new string[] { "MetasoundEngine", "MetasoundFrontend", "AudioMixer" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Making custom assets.
		if (Target.bBuildEditor) {
			PublicDependencyModuleNames.AddRange(new[] { "AssetDefinition", "UnrealEd", });
			PrivateDefinitions.Add("EDITOR_BUILD");
		}

		// UMG UI support
		PublicDependencyModuleNames.AddRange(new string[] { "UMG", "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
