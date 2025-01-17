// Ninja Bear Studio Inc. 2022, all rights reserved.
using UnrealBuildTool;

public class NinjaInput : ModuleRules
{
	public NinjaInput(ReadOnlyTargetRules target) : base(target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(new string[]
		{
		});
		
		PrivateIncludePaths.AddRange(new string[]
		{
		});

		PublicDependencyModuleNames.AddRange(new [] 
		{
			"Core",
			"DeveloperSettings",
			"InputCore", 
			"EnhancedInput",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks"
		});
		
		PrivateDependencyModuleNames.AddRange(new [] 
		{
			"CoreUObject",
			"Engine"
		});
		
		DynamicallyLoadedModuleNames.AddRange(new string[] 
		{
		});
	}
}
