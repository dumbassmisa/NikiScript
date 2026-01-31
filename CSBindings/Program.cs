class Program
{
	static bool Running = false;

	static void Generate()
	{
		if (CppSharp.ConsoleDriver.Run(new NikiScriptLibrary()))
		{
			File.Copy("GeneratedBindings/NikiScript.cs", "../../../NikiScript.cs", true);
			File.Copy("GeneratedBindings/Std.cs", "../../../Std.cs", true);
		}
	}

	static void Test()
	{
		var ctx = new NikiScript.Context();
		NikiScript.NikiScript.RegisterCommands(ctx);
		var cmdCtx = new NikiScript.CommandContext();
		// Running = true;
		// while (Running)
		// {
		// 	// string? inp = Console.ReadLine();
		// 	NikiScript.Parser.Parse(ctx, true);
		// }
	}

	static void Main()
	{
		Generate();
		// Test();
	}
}