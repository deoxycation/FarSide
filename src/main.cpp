#include "./includes/farSide.hh"

int main() {
  Compiler<std::string_view> initial("");

  auto final = initial | Fsc::Lexer::tokanize;

  // testing cases
  for (const auto& N : final.get()) {
    std::cout << N.lexeme; // lexer
  }

  return 0;
}
