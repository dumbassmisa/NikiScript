using System.Runtime.InteropServices;

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

	static void PrintCallback(nint pData, NikiScript.PrintLevel level, string message)
	{
		ConsoleColor originalFgColor = Console.ForegroundColor;

		switch (level)
		{
		default:
		case NikiScript.PrintLevel.ECHO:
			Console.ForegroundColor = ConsoleColor.Blue;
			Console.WriteLine(message);
			break;
		case NikiScript.PrintLevel.WARNING:
			Console.ForegroundColor = ConsoleColor.Yellow;
			Console.WriteLine(message);
			break;
		case NikiScript.PrintLevel.ERROR:
			Console.ForegroundColor = ConsoleColor.Red;
			Console.WriteLine(message);
			break;
		}

		Console.ForegroundColor = originalFgColor;
	}

	static void test_command(IntPtr _ctx, IntPtr pData)
	{
		var ctx = NikiScript.CommandContext.__GetOrCreateInstance(_ctx);
		NikiScript.Print.Print_(NikiScript.PrintLevel.ECHO, $"TEST: {ctx.Args.GetString(0)}\n");
	}

	static void Test()
	{
		NikiScript.Print.SetPrintCallback(0, PrintCallback);

		var ctx = new NikiScript.Context();
		NikiScript.NikiScript.RegisterCommands(ctx);
		unsafe
		{
			string[] argsDescriptions = [
				"s[variableh]", "description blablabla :rofl: :zzz: :fire:"
			];

			sbyte** nativeArray = (sbyte**)Marshal.AllocHGlobal(
				sizeof(IntPtr) * argsDescriptions.Length
			);

			for (int i = 0; i < argsDescriptions.Length; i++)
			{
				// Convert string → unmanaged ANSI string
				nativeArray[i] = (sbyte*)Marshal.StringToHGlobalAnsi(
					argsDescriptions[i]
				);
			}

			ctx.Commands.Add(new("test", 1, 1, test_command, 0, "This is a test command", nativeArray, (ushort)argsDescriptions.Length));

			// Free pointer array
			Marshal.FreeHGlobal((IntPtr)nativeArray);
		}

		Running = true;
		while (Running)
		{
			NikiScript.CommandContext cctx = new(ctx, Console.ReadLine());
			Console.WriteLine($"Input: {cctx.Lexer.Input}");

			NikiScript.Parser.Parse(cctx, true);
		}
	}

	static void Main()
	{
		// Generate();
		Test();
	}
}