
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include <algorithm>
#include <cstdlib>

#include <getopt.h>
#include <unistd.h>

#include "browse.h"
#include "build.h"
#include "build_log.h"
#include "deps_log.h"
#include "clean.h"
#include "debug_flags.h"
#include "depfile_parser.h"
#include "disk_interface.h"
#include "graph.h"
#include "graphviz.h"
#include "json.h"
#include "manifest_parser.h"
#include "metrics.h"
#include "missing_deps.h"
#include "state.h"
#include "status.h"
#include "util.h"
#include "version.h"

extern void compile_hello(int argc, char** argv);  // Declare the new function

int main(int argc, char** argv) {
    compile_hello(argc, argv);  // Call the separated compile_hello function
    return 0;
}