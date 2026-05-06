using System.IO;
using UnrealBuildTool;

public class Skia : ModuleRules
{
    public Skia(ReadOnlyTargetRules target)
        : base(target)
    {
        Type = ModuleType.External;

        // NOTE: If you want to support other platforms, you will need to compile
        //       Skia for them and use the approriate platform-specific API macro
        //       and paths here
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            // Define the SK_API macro
            PublicDefinitions.Add("SK_API=__declspec(dllimport)");

            // Add the import library
            string configFolder = Target.Configuration switch
            {
                UnrealTargetConfiguration.Debug
                    or UnrealTargetConfiguration.DebugGame
                    or UnrealTargetConfiguration.Development => "Debug",
                _ => "Release",
            };
            PublicAdditionalLibraries.Add(Path.Combine(
                ModuleDirectory,
                "Win64", configFolder, "skia.dll.lib"
            ));

            // Delay-load the DLL, so we can load it in the right place first
            PublicDelayLoadDLLs.Add("skia.dll");

            // Ensure that the DLL is staged along with the executable
            RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/Skia/Win64/skia.dll");
        }

        PublicIncludePaths.AddRange(new[] {
            Path.Combine(ModuleDirectory, "includes"),
        });
    }
}