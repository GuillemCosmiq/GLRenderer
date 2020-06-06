using Sharpmake; // contains the entire Sharpmake object library.

[module: Sharpmake.Include("glrenderer.sharpmake.cs")]
[module: Sharpmake.Include("contrib.sharpmake.cs")]

[Generate]
class GLRendererSolution : Solution
{
    public GLRendererSolution()
    {
        Name = "GLRenderer";

        AddTargets(new Target(
            Platform.win32 | Platform.win64,
            DevEnv.vs2019,
            Optimization.Debug | Optimization.Release));
    }

    [Configure]
    public void ConfigureAll(Solution.Configuration conf, Target target)
    {
        conf.SolutionPath = @"[solution.SharpmakeCsPath]";

       // conf.AddProject<Contrib>(target);
        conf.AddProject<GLRenderer>(target);
    }

    [Main]
    public static void SharpmakeMain(Arguments sharpmakeArgs)
    {
        sharpmakeArgs.Generate<GLRendererSolution>();
    }
}