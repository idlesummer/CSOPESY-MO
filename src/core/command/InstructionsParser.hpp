#pragma once

#include "core/common/imports/_all.hpp"
#include "core/instruction/Instruction.hpp"

struct ParsedProcess {
    str process_name;
    uint memory_size;
    vec<Instruction> program;
};

inline Instruction parse_instruction(const str& line) {
    Instruction instr;
    isstream ss(line);
    ss >> instr.opcode;

    str token;
    bool inside_quote = false;
    str quoted;

    while (ss >> token) {
        if (!inside_quote && token.front() == '"') {
            inside_quote = true;
            quoted = token;
        } else if (inside_quote) {
            quoted += " " + token;
            if (token.back() == '"') {
                instr.args.push_back(quoted);
                inside_quote = false;
            }
        } else {
            instr.args.push_back(token);
        }
    }

    if (inside_quote) {
        instr.args.push_back(quoted);
    }

    return instr;
}
