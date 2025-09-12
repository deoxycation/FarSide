#ifndef __FAR_SIDE__
#define __FAR_SIDE__

#include <array>
#include <vector>
#include <memory>
#include <expected>
#include <functional>
#include <optional>
#include <iostream>
#include <fstream>

namespace Fsc { // "Far-Side Code"
  class Lexer;
  class Parser;
}

// using Lexer = Fsc::Lexer;
// using Parser = Fsc::Parser;

#include "./constructs.hh"
#include "./ast.hh"
#include "./lexer.hh"
#include "./parser.hh"

#endif
