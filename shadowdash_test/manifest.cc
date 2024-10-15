#include "manifest.h"
#include <iostream>

using namespace shadowdash;

extern "C" {
	buildGroup manifest() {
		let(flags, {"-O3"});

		char test[] = "hello.o";

		auto compile = rule{{
			bind(command, {"g++", "flags"_v, "-c", in, "-o", out}),
		}};

		auto link = rule({
			bind(command, {"g++", in, "-o", out}),
		});

		// std::cout << "creating mystr" << std::endl;
		// auto mystr = str {{test}};
		
		// for (const shadowdash::Token& token: mystr.tokens_) {
		// 	std::cout << "token: " << token.value_ << std::endl;
		// }

		// std::cout << "creating mylist" << std::endl;
		// auto mylist = list{ {mystr} };
		// std::cout << "creating build1" << std::endl;

		auto build1 = build(list{ {str {{"hello.o"}}} },
			list{{}},
			compile,
			list{ {str{{"hello.cc"}}} },
			list{{}},
			list{{}},
			{ bind(flags, {"-O2"}) }
		);

		// auto build2 = build(list{ str{ "hello" } },
		// 	{},
		// 	link,
		// 	list{ str{"hello.o"} },
		// 	{},
		// 	{},
		// 	{}
		// );
		
		auto build2 = build(list{ {str {{"hello"}}} },
			list{{}},
			link,
			list{ {str{{"hello.o"}}} },
			list{{}},
			list{{}},
			{ bind(flags, {"-O2"}) }
		);

		// std::cout << "???" << std::endl;
		// std::cout << build1.outputs_ << std::endl;
		// std::cout << build1.inputs_ << std::endl;
		
        // for(const shadowdash::str& str : build1.outputs_.values_) {
        //     std::cout << "outputs" << std::endl;
        //     for (const shadowdash::Token& token: str.tokens_) {
        //         std::cout << "token: " << token.value_ << std::endl;
        //     }
        // }
        // for(const shadowdash::str& str : build1.inputs_.values_) {
        //     std::cout << "inputs" << std::endl;
        //     for (const shadowdash::Token& token: str.tokens_) {
        //         std::cout << "token: " << token << std::endl;
        //     }
        // }
		
		// std::cout << "????" << std::endl;

		return buildGroup({ build1, build2 });
	}
}