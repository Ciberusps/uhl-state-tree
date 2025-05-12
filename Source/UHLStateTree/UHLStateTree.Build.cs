// Pavel Penkov 2025 All Rights Reserved.

using UnrealBuildTool;

public class UHLStateTree : ModuleRules
{
	public UHLStateTree(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);


		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",

				"GameplayAbilities",
				"GameplayTags",
				"GameplayTasks",
				
				"AIModule",
				
				// UHL/UHLAI required due to "TurnTo" settings sharing
				"UnrealHelperLibrary", 
				"UHLAI"

				// ... add other public dependencies that you statically link with here ...
			}
			);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"StateTreeModule",
				"GameplayStateTreeModule"
			}
			);
	}
}
