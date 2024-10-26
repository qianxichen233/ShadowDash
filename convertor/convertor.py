# minimal build.ninja to manifest.cc convertor
import sys

target = "build.ninja"
if len(sys.argv) > 1:
    target = sys.argv[1]

output = "manifest_output.cc"

class FileReader:
    def __init__(self, file_path):
        self.file_path = file_path
        self.file = None
    
    def open(self):
        self.file = open(self.file_path, 'r')
    
    def read_nextline(self):
        if self.file is None:
            raise ValueError("File is not opened yet. Call 'open()' first.")
        
        line = self.file.readline()
        if line:
            return line  # Strip whitespace and newlines
        else:
            return None  # End of file
    
    def close(self):
        if self.file:
            self.file.close()
            self.file = None

class FileWriter:
    def __init__(self, file_path):
        self.file_path = file_path
        self.file = None

        self.begin = """#include "manifest.h"
#include <iostream>

using namespace shadowdash;

extern "C" {
	buildGroup manifest() {
"""

        self.end = """	}
}"""

        self.indent = 2
        self.build_count = 0
    
    def open(self):
        self.file = open(self.file_path, 'w')
        self.__write_start()
    
    def write_line(self, line):
        if self.file is None:
            raise ValueError("File is not opened yet. Call 'open()' first.")
        
        self.file.write("	" * self.indent + line + '\n')
    
    def write_indent(self):
        if self.file is None:
            raise ValueError("File is not opened yet. Call 'open()' first.")
        
        self.file.write("	" * self.indent)

    def write(self, string):
        if self.file is None:
            raise ValueError("File is not opened yet. Call 'open()' first.")
        
        self.file.write(string)
    
    def newline(self, n = 1):
        self.file.write("\n" * n)
    
    def close(self):
        self.__write_end()
        if self.file:
            self.file.close()
            self.file = None
    
    def incr_indent(self):
        self.indent += 1
    
    def decr_indent(self):
        self.indent -= 1
    
    def write_rule(self, name, commands, depfile, deps):
        self.write_line(f"auto {name} = rule{{{{")
        self.incr_indent()
        self.write_line(f"""bind(command, {{{', '.join(commands)}}}),""")
        if len(depfile) > 0:
            self.write_line(f"""bind(depfile, {{{', '.join(depfile)}}}),""")
        if len(deps) > 0:
            self.write_line(f"""bind(deps, {{{', '.join(deps)}}}),""")
        self.decr_indent()
        self.write_line("}};")

        self.newline()
    
    def write_build(self, _in, order_in, phony_in, _out, rule, vars: dict):
        self.build_count += 1

        self.write_line(f"auto build{self.build_count} = build(list{{ {{{', '.join(_out)}}} }},")
        self.incr_indent()
        self.write_line("""list{{}},""")
        if rule == "phony":
            rule = "rule::phony"
        self.write_line(f"{rule},")
        if _in != None:
            self.write_line(f"list{{ {{{', '.join(_in)}}} }},")
            # self.write_line(f"list{{ {{{"str()"}}} }},")
        else:
            self.write_line("""list{{}},""")
        self.write_line("""list{{}},""")
        if order_in != None:
            self.write_line(f"list{{ {{{', '.join(order_in)}}} }},")
            # self.write_line(f"list{{ {{str{{{{{', '.join(order_in)}}}}}}} }},")
        else:
            self.write_line("""list{{}},""")

        transformed = []
        for key in vars:
            val = vars[key].replace("\"", "\\\"")
            transformed.append(f"bind({key}, {{\"{val}\"}})")
        self.write_line(f"{{ {', '.join(transformed)} }}")

        self.decr_indent()
        self.write_line(");")

        self.newline()
    
    def write_define(self, name, val):
        self.write_line(f"let({name}, {{\"{val}\"}});")

        self.newline()

    def __write_start(self):
        self.write(self.begin)

    def __write_end(self):
        builds = ["build" + str(i) for i in range(1, self.build_count + 1)]

        self.write_line(f"return buildGroup({{ {', '.join(builds)} }});")
        self.write(self.end)

writer = FileWriter(output)

def expect_indent(line: str):
    return line != None and line.strip() != "" and line.startswith(" ")

def parseRuleVal(lst, val: str):
    for word in val.split(" "):
        word = word.strip()
        if word == "in":
            lst.append("in")
        elif word == "out":
            lst.append("out")
        elif word.startswith("$"):
            lst.append(f"\"{word[1:]}\"_v")
        else:
            lst.append(f"\"{word}\"")

def parseRule(line: str, reader: FileReader):
    name = line.split(" ")[1]
    commands = []
    depfile = []
    deps = []

    while True:
        line = reader.read_nextline()
        if not expect_indent(line):
            break

        line = line.strip()
        [key, val] = line.split("=", 1)
        key = key.strip()
        val = val.strip()

        if key == "command":
            if len(commands) > 0:
                print("multiple commands found in the rule!")
                exit(1)

            parseRuleVal(commands, val)
            commands = [val for pair in zip(commands, ["\" \""] * (len(commands) - 1)) for val in pair] + [commands[-1]]
        elif key == "depfile":
            if len(depfile) > 0:
                print("multiple depfile found in the rule!")
                exit(1)

            parseRuleVal(depfile, val)
        elif key == "deps":
            if len(deps) > 0:
                print("multiple deps found in the rule!")
                exit(1)

            parseRuleVal(deps, val)

    writer.write_rule(name, commands, depfile, deps)

    return line

def parseBuild(line: str, reader: FileReader):
    [build, target] = line.split(":")
    _out = [f"str{{{{\"{item.strip()}\"}}}}" for item in build.split(" ")[1:]]

    splited = target.strip().split(" ")
    rule = splited[0]

    _in = None
    order_in = None
    phony_in = None
    if len(splited) > 1:
        state = 0
        for i in range(1, len(splited)):
            if splited[i] == "|":
                state = 1
                continue

            if splited[i] == "||":
                state = 2
                continue

            if state == 0:
                if _in == None:
                    _in = []
                _in.append(f"str{{{{\"{splited[i]}\"}}}}")
            elif state == 1:
                if order_in == None:
                    order_in = []
                order_in.append(f"str{{{{\"{splited[i]}\"}}}}")
            else:
                if phony_in == None:
                    phony_in = []
                phony_in.append(f"str{{{{\"{splited[i]}\"}}}}")

    vars = {}
    while True:
        line = reader.read_nextline()
        if not expect_indent(line):
            break
        [key, value] = line.split("=", 1)
        vars[key.strip()] = value.strip()

    # if _in != None:
    #     _in = [val for pair in zip(_in, ["\" \""] * (len(_in) - 1)) for val in pair] + [_in[-1]]
    # if order_in != None:
    #     order_in = [val for pair in zip(order_in, ["\" \""] * (len(order_in) - 1)) for val in pair] + [order_in[-1]]
    writer.write_build(_in, order_in, phony_in, _out, rule, vars)

    return line

def parseDefine(line: str, reader: FileReader):
    [key, value] = line.split("=", 1)
    writer.write_define(key.strip(), value.strip())

if __name__ == "__main__":
    reader = FileReader(target)
    reader.open()
    writer.open()

    skip_read = False

    while True:
        if not skip_read:
            line = reader.read_nextline()

        skip_read = False

        if line == None:
            break

        line = line.strip()

        # empty line
        if line == "":
            continue

        # comment
        if line.startswith("#"):
            continue

        if line.startswith("rule"):
            line = parseRule(line, reader)
            skip_read = True

        elif line.startswith("build"):
            line = parseBuild(line, reader)
            skip_read = True

        elif "=" in line:
            # probably a variable define
            parseDefine(line, reader)

        elif line.startswith("default"):
            # one of the syntax we cannot translate currently
            pass
        
        else:
            # there should definitely be more valid syntax in ninja
            # but for purpose of parsing build.ninja for zlib, there doesn't seem to be more
            print(f"unknown identifier: {line}")
            break
    
    reader.close()
    writer.close()
