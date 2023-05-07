// Copyright CapralKein. All rights reserved.

using UnrealBuildTool;

public class StateTreeExtension : ModuleRules
{
	public StateTreeExtension(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
			}
		);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
			}
		);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core"
			}
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"AIModule",
				"NavigationSystem",
				"StateTreeModule",
				"GameplayStateTreeModule",
				"GameplayTasks",
				"GameplayTags",
				"StructUtils",
				"GameplayAbilities"
			}
		);
		
		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"StateTreeEditorModule"
				}
			);
		}


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
		);
	}
}
