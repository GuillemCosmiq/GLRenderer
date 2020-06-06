using System.IO; // for Path.Combine
using Sharpmake;

[Generate]
class GLRenderer : Project
{
    public string ContribPath = @"[project.SharpmakeCsPath]\contrib\";

    public GLRenderer()
    {
        Name = "GLRenderer";

        SourceRootPath = @"[project.SharpmakeCsPath]\source";
        AdditionalSourceRootPaths.Add(ContribPath);

        AddTargets(new Target(
            Platform.win32 | Platform.win64,
            DevEnv.vs2019,
            Optimization.Debug | Optimization.Release));
    }

    [Configure]
    public void ConfigureAll(Project.Configuration conf, Target target)
    {
        conf.ProjectPath = Path.Combine("[project.SharpmakeCsPath]", "source");

        conf.PrecompHeader = "stdafx.h";
        conf.PrecompSource = "stdafx.cpp";

        conf.IncludePaths.Add(@"[project.ContribPath]");
        conf.LibraryPaths.Add(@"[project.SharpmakeCsPath]\bin");

        if (target.Platform == Platform.win32)
        {
            conf.LibraryPaths.Add(@"[project.ContribPath]\assimp\libx86");
            conf.LibraryPaths.Add(@"[project.ContribPath]\SDL2\lib\x86");
        }
        else if (target.Platform == Platform.win64)
        {
            conf.LibraryPaths.Add(@"[project.ContribPath]\assimp\libx64");
            conf.LibraryPaths.Add(@"[project.ContribPath]\SDL2\lib\x64");
        }

        conf.TargetCopyFiles.Add(@"[project.SharpmakeCsPath]\bin\assimp-vc142-mt.dll");
        conf.LibraryFiles.Add("assimp-vc142-mt");

        conf.TargetCopyFiles.Add(@"[project.SharpmakeCsPath]\bin\SDL2.dll");
        conf.LibraryFiles.Add("SDL2");
        conf.LibraryFiles.Add("SDL2main");
    }
}
