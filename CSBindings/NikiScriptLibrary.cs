using CppSharp;
using CppSharp.AST;

class NikiScriptLibrary : ILibrary
{
	public void Setup(Driver driver)
	{
		var options = driver.Options;
		var module = options.AddModule("NikiScript");
		options.OutputDir = "GeneratedBindings";

		var includeDir = Path.Combine(Directory.GetCurrentDirectory(), "..", "..", "..", "..", "include", "NikiScript");
		driver.ParserOptions.AddIncludeDirs(includeDir);

		string[] files = Directory.GetFiles(includeDir);
		module.Headers.AddRange(files);

		// Optional: specify platform/architecture if needed for accurate parsing
		// driver.ParserOptions.TargetTriple = "x86_64-pc-win32-msvc";
		driver.ParserOptions.AddDefines("NS_CS_API");
	}

	public void SetupPasses(Driver driver) {}

	public void Preprocess(Driver driver, ASTContext ctx)
	{
		// Iterate all translation units
		foreach (var unit in ctx.TranslationUnits)
		{
			// Capture things to modify to avoid modifying during enumeration
			var @namespaces = unit.Namespaces
				.Where(n => n.Name == "ns")
				.ToList();

			foreach (var @namespace in @namespaces)
			{
				@namespace.IsInline = true;
			}
		}
	}

	public void Postprocess(Driver driver, ASTContext ctx) {}
}