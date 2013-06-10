#include "CodeGeneration.hxx"

void CodeGeneration::runPass(Design *design) {
  design->writeEngineFiles("build/engine");
}

string CodeGeneration::logPass() {
  return "MaxJ Code Generation";
}
