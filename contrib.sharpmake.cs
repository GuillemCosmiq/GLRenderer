using System.IO; // for Path.Combine
using Sharpmake;

[Generate]
class Contrib : Project
{
    public Contrib()
    {
        Name = "Contrib";

        SourceRootPath = @"[project.SharpmakeCsPath]\contrib";

        AddTargets(new Target(
            Platform.win32 | Platform.win64,
            DevEnv.vs2019,
            Optimization.Debug | Optimization.Release));
    }

    [Configure]
    public void ConfigureAll(Project.Configuration conf, Target target)
    {
        conf.ProjectPath = Path.Combine("[project.SharpmakeCsPath]", "contrib");
        conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\assimp");
        conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\glad");
        conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\glm");
        conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\imgui");
        conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\jsoncpp");
        conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\SDL2");
        conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\stb_image");
    }
}
