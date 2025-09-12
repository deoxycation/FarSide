#include "./includes/fsc.hh"

int main() {
  Compiler<std::string_view> initial("Int");

  auto final = initial | Fsc::Lexer::tokenize;

  // testing cases
  for (const auto& N : final.get()) {
    // std::cout << N.value().lexeme; // lexer test-run "Bad access" ; fix later
  }

  return 0;
}
