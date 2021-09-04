// Copyright 2019, Institute for Artificial Intelligence - University of Bremen
// Author: Andrei Haidu (http://haidu.eu)

using UnrealBuildTool;
using System.IO;
using System;

public class Boost : ModuleRules
{
	public Boost(ReadOnlyTargetRules Target) : base(Target)
	{
		// We are just setting up paths for pre-compiled binaries.
		Type = ModuleType.External;

		// For boost::
		bEnableUndefinedIdentifierWarnings = false;
		bUseRTTI = true;

		string[] dirs = Directory.GetDirectories(Path.Combine(ModuleDirectory, "boost", "libs"), "*", SearchOption.TopDirectoryOnly);
		foreach (string dir in dirs)
		{
			string SubModulePath = Path.Combine(ModuleDirectory, "boost", "libs", dir, "include");
			if (Directory.Exists(SubModulePath)) {
				PublicIncludePaths.Add(SubModulePath);
			}
		}
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "boost", "libs", "numeric", "conversion", "include"));
	}
}
