// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CH3_Task8 : ModuleRules
{
	public CH3_Task8(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] {
				"Core", 
				"CoreUObject", 
				"Engine", 
				"InputCore", 
				"EnhancedInput",
				"UMG"
			}
		);

		PrivateDependencyModuleNames.AddRange(new string[] {  });
	}
}
