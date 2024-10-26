#include "manifest.h"

manifest() {
	rule(
		compile,
		bind(command, {"g++", "-c", "in"_v, "-o", "out"_v, "-MD", "-MF", "depfile"_v}),
		bind(depfile, {"out"_v, ".d"}),
		bind(deps, {"gcc"}),
	);

	rule(
		link,
		bind(command, {"g++", "in"_v, "-o", "out"_v}),
	);

	auto build1 = build(list(str("hello.o"), str("hello2.o")),
		list(),
		compile,
		list(str("hello.c")),
		list(),
		list(),
		{ bind(test, {"123"}) }
	);

	auto build2 = build(list(str("hello")),
		list(),
		link,
		list(str("hello.o")),
		list(),
		list(),
		{  }
	);

	auto build3 = build(list(str("hello.d")),
		list(),
		phony,
		list(),
		list(),
		list(),
		{  }
	);

	new_pool(pool1, 1);
	new_pool(pool2, 1);
	new_pool(pool3, 1);

	default(str("all"), str("test"));

	ShadowDash({ build1, build2, build3 }, {pool1, pool2, pool3}, default_target);
}