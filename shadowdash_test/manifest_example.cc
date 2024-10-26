#include "manifest.h"

manifest() {
	// Define a "compile" rule that complies into object files.
	rule(
		compile,
		bind(command, {"g++", "-c", "in"_v, "-o", "out"_v, "-MD", "-MF", "depfile"_v}),
		bind(depfile, {"out"_v, ".d"}),
		bind(deps, {"gcc"}),
	);

	// Define a "link" rule that links object files into an executable.
	rule(
		link,
		bind(command, {"g++", "in"_v, "-o", "out"_v}),
	);

	// Define a build configuration for compiling "hello.c" into object files.
	auto build1 = build(list(str("hello.o"), str("hello2.o")),
		list(),
		compile,
		list(str("hello.c")),
		list(),
		list(),
		{ bind(test, {"123"}) }
	);

	// Define a build configuration for linking "hello.o" into an executable.
	auto build2 = build(list(str("hello")),
		list(),
		link,
		list(str("hello.o")),
		list(),
		list(),
		{  }
	);

	// Define a build configuration with the special rule "phony"
	auto build3 = build(list(str("hello.d")),
		list(),
		phony,
		list(),
		list(),
		list(),
		{  }
	);
	
	// Define pools with a specified concurrency depth limit.
	new_pool(pool1, 1);
	new_pool(pool2, 1);
	new_pool(pool3, 1);

	 // Define default targets to be built when no specific target is specified.
	default(str("all"), str("test"));

	// Create the ShadowDash manifest with all builds, pools, and default targets defined above.
	ShadowDash({ build1, build2, build3 }, {pool1, pool2, pool3}, default_target);
}