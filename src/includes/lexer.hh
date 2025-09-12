#ifndef __LEXER_FCS__
#define __LEXER_FCS__

#include "./fsc.hh"

#define NUM_ASCII 128
#define NORM_CHAR_LOWER 32 // inclusive; lower-Latin chars
#define NORM_CHAR_UPPER 127 // exclusive; upper-Latin chars
#define UNFINISHED_LOWER (size_t) State::PLUS_UNFINISHED // inclusive; lower-unfinished bound
#define UNFINISHED_UPPER (size_t) State::STRING_ESCAPE_START // exclusive; upper-unfinished bound
#define UNFINISHED_DIFFERENCE (size_t) State::PLUS - (size_t) State::PLUS_UNFINISHED // Dif between unfinished and primitive ops

using Lexer = Fsc::Lexer;

class Lexer {
  private:
  using TransitionTable = std::array<std::array<State, NUM_ASCII>, (size_t) State::NUM_STATES>;
  using AcceptingsTable = std::array<bool, (size_t) State::NUM_STATES>;

  constexpr TransitionTable buildTransitions();
  constexpr AcceptingsTable buildAcceptings();
  inline void reset();
  inline void handleWhitespace();
  inline void transition();
  State classify(const std::string_view lexeme);
  Token createToken();
  std::unexpected<Error> createError(ErrorTypes type);
  std::vector<std::expected<Token, Error>> mainloop();

  const std::string_view source;
  const TransitionTable transitions = buildTransitions();
  const AcceptingsTable acceptings = buildAcceptings();
  std::vector<std::expected<Token, Error>> tokens = { };
  State last = State::START;
  std::optional<State> lastAccepted = std::nullopt;
  char c;
  size_t lineNumber = 1;
  size_t pos = 0;
  size_t initial = 0;

  public:
  static const std::function<std::vector<std::expected<Token, Error>>(std::string_view)> tokenize;

  Lexer(std::string_view source) : source(source) { };
  ~Lexer() = default;
};

#endif
