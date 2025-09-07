#ifndef __LEXER_FCS__
#define __LEXER_FCS__

#include "./farSide.hh"

class Fsc::Lexer {
  private:
  using TransitionTable = std::array<std::array<State, NUM_ASCII>, (size_t) State::NUM_STATES>;
  using AcceptingsTable = std::array<bool, (size_t) State::NUM_STATES>;

  constexpr TransitionTable buildTransitions();
  constexpr AcceptingsTable buildAcceptings();

  const std::string_view source;
  const TransitionTable table = buildTransitions();
  const AcceptingsTable acceptings = buildAcceptings();
  State last = State::START;
  std::optional<State> lastAccepted;
  char c;
  size_t pos;
  size_t initial;

  public:
  static const std::function<std::vector<Token>(std::string_view)> tokanize;

  Lexer(std::string_view source) : source(source) { };
  ~Lexer() = default;
};

#endif
