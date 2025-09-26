using UnrealBuildTool;
public class ReasoningEngine  : ModuleRules
{
    public ReasoningEngine(ReadOnlyTargetRules Target) : base(Target)
    {
        // NO editor dependencies in runtime module
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "DeveloperSettings"
        });
        
        // Runtime module NEVER includes editor headers
        if (Target.bBuildEditor)
        {
            // This ONLY affects compilation, not runtime
            PublicDefinitions.Add("WITH_SEMANTIC_ENGINE_EDITOR=1");
        }
        else
        {
            PublicDefinitions.Add("WITH_SEMANTIC_ENGINE_EDITOR=0");
        }
    }
}
