// Some copyright should be here...

using UnrealBuildTool;

public class VTLearnEd : ModuleRules
{
	public VTLearnEd(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				"VTLearnEd/Public"
			}
			);


		PrivateIncludePaths.AddRange(
			new string[] {
				"VTLearnEd/Private"
			}
			);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore"
			}
			);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"VTLearn",
				"UnrealEd"
			}
			);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
