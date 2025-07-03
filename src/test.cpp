#include "InstructionInterpreter.hpp"
#include "Instruction.hpp"
#include "core/process/ProcessData.hpp"
#include "core/process/ProcessProgram.hpp"
#include "core/process/ProcessExecutor.hpp"

using namespace csopesy;

int main() {
  auto data = ProcessData("test", 1);

  // Create a flat program with a FOR loop and some basic instructions
  auto& prog = data.get_program();
  auto& insts = prog.get_instructions();

  insts = {
    Instruction{"DECLARE", {"x", "0"}},
    Instruction{"FOR", {"3"}},           // FOR-BEGIN
    Instruction{"ADD", {"x", "x", "1"}},
    Instruction{"PRINT", {"x"}},
    Instruction{"ENDFOR", {}},           // FOR-END
  };

  InstructionInterpreter interpreter = InstructionInterpreter::instance();

  while (true) {
    if (!ProcessExecutor::step(data)) break;
    sleep_for(100ms);  // Optional: slow down for visibility
  }

  for (auto& line : data.get_logs())
    cout << line << '\n';
}
