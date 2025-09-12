#include "./includes/fsc.hh"

/*

----- SYNTAX -----
==================

// Deterministic Symbols
[[ NUMBER ]] [[ IDENTIFIER ]] [[ STRING ]] [[ CHAR ]]
' "
( ) [ ] { } , ;
+ - / * % .. = += -= /= *= %= ..= -> => && || < > <= >= == != : . |
++ -- !
\\ \n \t \r \0

// Ambiguous Syntax (resolved later)
-(unary)

// "Unfinished" Symbols('~' is the wildcard)
+~ -~ /~ *~ %~ ..~ =* |~ !~ .~ <~ >~ \~


// Keywords (resolved via unordered hash-map)
Number Int Float Natural String Char Boolean // Primitive Types
Unit Nil True False // Atomic(Constants) Primitives (Note: "Unit" is both an atom and type)
array("TYPE[]") fn("TYPE -> ... -> TYPE") struct interface class type data object record namespace // Derived types

// Structuring Keywords
for in while repeat until if elif else return expose

// Compiler-Interfaces (CIs)
@use @using @require

*/

constexpr Lexer::TransitionTable Lexer::buildTransitions() {
  TransitionTable transitions = { };
  
  for (size_t i = 0; i < (size_t) State::NUM_STATES; i++) {
    for (size_t j = 0; j < NUM_ASCII; j++) {
      transitions[i][j] = State::ERROR;
    }
  }

  for (size_t i = (size_t) 'A'; i <= (size_t) 'Z'; i++) {
    transitions[(size_t) State::START][i] = State::IDENTIFIER;
    transitions[(size_t) State::IDENTIFIER][i] = State::IDENTIFIER;
  }

  for (size_t i = (size_t) 'a'; i <= (size_t) 'z'; i++) {
    transitions[(size_t) State::START][i] = State::IDENTIFIER;
    transitions[(size_t) State::IDENTIFIER][i] = State::IDENTIFIER;
  }

  for (size_t i = (size_t) '0'; i <= (size_t) '9'; i++) {
    transitions[(size_t) State::IDENTIFIER][i] = State::IDENTIFIER;
    transitions[(size_t) State::START][i] = State::NUMBER;
    transitions[(size_t) State::NUMBER][i] = State::NUMBER;
  }

  for (size_t i = NORM_CHAR_LOWER; i < NORM_CHAR_UPPER; i++) {
    transitions[(size_t) State::STRING_START][i] = State::STRING_BODY;
    transitions[(size_t) State::STRING_BODY][i] = State::STRING_BODY;
    transitions[(size_t) State::STRING_ESCAPE_BODY][i] = State::STRING_BODY;
    transitions[(size_t) State::CHAR_START][i] = State::CHAR_BODY;
  }

  transitions[(size_t) State::START][(size_t) '"'] = State::STRING_START;
  transitions[(size_t) State::STRING_BODY][(size_t) '"'] = State::STRING_END;
  transitions[(size_t) State::STRING_ESCAPE_BODY][(size_t) '"'] = State::STRING_END;

  transitions[(size_t) State::START][(size_t) '\''] = State::CHAR_START;
  transitions[(size_t) State::CHAR_BODY][(size_t) '\''] = State::CHAR_END;
  transitions[(size_t) State::CHAR_ESCAPE_BODY][(size_t) '\''] = State::CHAR_END;

  transitions[(size_t) State::STRING_START][(size_t) '\\'] = State::STRING_ESCAPE_START;
  transitions[(size_t) State::STRING_BODY][(size_t) '\\'] = State::STRING_ESCAPE_START;
  transitions[(size_t) State::CHAR_START][(size_t) '\\'] = State::CHAR_ESCAPE_START;

  // Escapes for Strings
  transitions[(size_t) State::STRING_ESCAPE_START][(size_t) '\\'] = State::STRING_ESCAPE_BODY;
  transitions[(size_t) State::STRING_ESCAPE_START][(size_t) 'n'] = State::STRING_ESCAPE_BODY;
  transitions[(size_t) State::STRING_ESCAPE_START][(size_t) 't'] = State::STRING_ESCAPE_BODY;
  transitions[(size_t) State::STRING_ESCAPE_START][(size_t) 'r'] = State::STRING_ESCAPE_BODY;
  transitions[(size_t) State::STRING_ESCAPE_START][(size_t) '\0'] = State::STRING_ESCAPE_BODY;
  transitions[(size_t) State::STRING_ESCAPE_START][(size_t) '\''] = State::STRING_ESCAPE_BODY;
  transitions[(size_t) State::STRING_ESCAPE_START][(size_t) '"'] = State::STRING_ESCAPE_BODY;

  // Escapes for Chars
  transitions[(size_t) State::CHAR_ESCAPE_START][(size_t) '\\'] = State::CHAR_ESCAPE_BODY;
  transitions[(size_t) State::CHAR_ESCAPE_START][(size_t) 'n'] = State::CHAR_ESCAPE_BODY;
  transitions[(size_t) State::CHAR_ESCAPE_START][(size_t) 't'] = State::CHAR_ESCAPE_BODY;
  transitions[(size_t) State::CHAR_ESCAPE_START][(size_t) 'r'] = State::CHAR_ESCAPE_BODY;
  transitions[(size_t) State::CHAR_ESCAPE_START][(size_t) '0'] = State::CHAR_ESCAPE_BODY;
  transitions[(size_t) State::CHAR_ESCAPE_START][(size_t) '\''] = State::CHAR_ESCAPE_BODY;
  transitions[(size_t) State::CHAR_ESCAPE_START][(size_t) '"'] = State::CHAR_ESCAPE_BODY;

  transitions[(size_t) State::START][(size_t) '('] = State::LPAREM;
  transitions[(size_t) State::START][(size_t) ')'] = State::RPAREM;
  transitions[(size_t) State::START][(size_t) '['] = State::LBRACKET;
  transitions[(size_t) State::START][(size_t) ']'] = State::RBRACKET;
  transitions[(size_t) State::START][(size_t) '{'] = State::LBRACE;
  transitions[(size_t) State::START][(size_t) '}'] = State::RBRACE;

  transitions[(size_t) State::START][(size_t) ','] = State::COMMA;
  transitions[(size_t) State::START][(size_t) ';'] = State::SEMICOLON;

  for (size_t i = UNFINISHED_LOWER; i < UNFINISHED_UPPER; i++) {
    for (size_t j = 0; j < NUM_ASCII; j++) {
      transitions[i][j] = (State) (i + UNFINISHED_DIFFERENCE);
    }
  }

  // Maybe the code up-above will work; test later
  // make a for-loop for handling "unfinishes" to go to their primitive symbol, instead of "ERROR," if char is not found
  // maybe use a double-for-loop with bounds, then just have the states which need it be next to each other
  // or just make some kind-of higher-level fallback where the last state of an unfinished is its complemenet
  // or make a trie substrate

  transitions[(size_t) State::START][(size_t) '+'] = State::PLUS_UNFINISHED;
  transitions[(size_t) State::START][(size_t) '-'] = State::MINUS_UNFINISHED;
  transitions[(size_t) State::START][(size_t) '/'] = State::DIV_UNFINISHED;
  transitions[(size_t) State::START][(size_t) '*'] = State::MULT_UNFINISHED;
  transitions[(size_t) State::START][(size_t) '%'] = State::MOD_UNFINISHED;
  transitions[(size_t) State::START][(size_t) '.'] = State::ACCESS_UNFINISHED;
  transitions[(size_t) State::ACCESS_UNFINISHED][(size_t) '.'] = State::CONCAT_UNFINISHED;
  transitions[(size_t) State::START][(size_t) '='] = State::ASSIGN_UNFINISHED;
  transitions[(size_t) State::START][(size_t) '|'] = State::PIPE_UNFINISHED;
  transitions[(size_t) State::START][(size_t) '!'] = State::BANG_UNFINISHED;
  transitions[(size_t) State::START][(size_t) '<'] = State::LESS_UNFINISHED;
  transitions[(size_t) State::START][(size_t) '>'] = State::GREATER_UNFINISHED;

  transitions[(size_t) State::PLUS_UNFINISHED][(size_t) '='] = State::PLUS_ASSIGN;
  transitions[(size_t) State::MINUS_UNFINISHED][(size_t) '='] = State::MINUS_ASSIGN;
  transitions[(size_t) State::DIV_UNFINISHED][(size_t) '='] = State::DIV_ASSIGN;
  transitions[(size_t) State::MULT_UNFINISHED][(size_t) '='] = State::MULT_ASSIGN;
  transitions[(size_t) State::MOD_UNFINISHED][(size_t) '='] = State::MOD_ASSIGN;
  transitions[(size_t) State::CONCAT_UNFINISHED][(size_t) '='] = State::CONCAT_ASSIGN;

  transitions[(size_t) State::MINUS_UNFINISHED][(size_t) '>'] = State::TYPE_ARROW;
  transitions[(size_t) State::ASSIGN_UNFINISHED][(size_t) '>'] = State::FUNCTION_ARROW;
  transitions[(size_t) State::PIPE_UNFINISHED][(size_t) '|'] = State::OR;
  transitions[(size_t) State::GREATER_UNFINISHED][(size_t) '='] = State::GREATER_EQUALS;
  transitions[(size_t) State::LESS_UNFINISHED][(size_t) '='] = State::LESS_EQUALS;
  transitions[(size_t) State::ASSIGN_UNFINISHED][(size_t) '='] = State::EQUALS;
  transitions[(size_t) State::BANG_UNFINISHED][(size_t) '='] = State::NOT_EQUALS;

  transitions[(size_t) State::PLUS_UNFINISHED][(size_t) '+'] = State::INCREMENT;
  transitions[(size_t) State::MINUS_UNFINISHED][(size_t) '-'] = State::DECREMENT;
  
  return transitions;
}

constexpr Lexer::AcceptingsTable Lexer::buildAcceptings() {
  AcceptingsTable acceptings = { };
  
  for (size_t i = 0; i < (size_t) State::NUM_STATES; i++) {
    acceptings[i] = false;
  }

  acceptings[(size_t) State::IDENTIFIER] = true;
  acceptings[(size_t) State::NUMBER] = true;

  acceptings[(size_t) State::STRING_END] = true;
  acceptings[(size_t) State::CHAR_END] = true;

  acceptings[(size_t) State::LPAREM] = true;
  acceptings[(size_t) State::RPAREM] = true;
  acceptings[(size_t) State::LBRACKET] = true;
  acceptings[(size_t) State::RBRACKET] = true;
  acceptings[(size_t) State::LBRACE] = true;
  acceptings[(size_t) State::RBRACE] = true;

  acceptings[(size_t) State::COMMA] = true;
  acceptings[(size_t) State::SEMICOLON] = true;

  acceptings[(size_t) State::AT] = true;

  acceptings[(size_t) State::PLUS] = true;
  acceptings[(size_t) State::MINUS] = true;
  acceptings[(size_t) State::DIV] = true;
  acceptings[(size_t) State::MULT] = true;
  acceptings[(size_t) State::MOD] = true;
  acceptings[(size_t) State::CONCAT] = true;
  acceptings[(size_t) State::ASSIGN] = true;
  acceptings[(size_t) State::PIPE] = true;
  acceptings[(size_t) State::BANG] = true;
  acceptings[(size_t) State::ACCESS] = true;
  acceptings[(size_t) State::GREATER] = true;
  acceptings[(size_t) State::LESS] = true;

  acceptings[(size_t) State::PLUS_ASSIGN] = true;
  acceptings[(size_t) State::MINUS_ASSIGN] = true;
  acceptings[(size_t) State::DIV_ASSIGN] = true;
  acceptings[(size_t) State::MULT_ASSIGN] = true;
  acceptings[(size_t) State::MOD_ASSIGN] = true;
  acceptings[(size_t) State::CONCAT_ASSIGN] = true;

  acceptings[(size_t) State::TYPE_ARROW] = true;
  acceptings[(size_t) State::FUNCTION_ARROW] = true;
  acceptings[(size_t) State::AND] = true;
  acceptings[(size_t) State::OR] = true;
  acceptings[(size_t) State::GREATER_EQUALS] = true;
  acceptings[(size_t) State::LESS_EQUALS] = true;
  acceptings[(size_t) State::EQUALS] = true;
  acceptings[(size_t) State::NOT_EQUALS] = true;
  acceptings[(size_t) State::COLON] = true;

  acceptings[(size_t) State::INCREMENT] = true;
  acceptings[(size_t) State::DECREMENT] = true;

  /*
    // Keywords
    KEYWORD_NUMBER, INT, FLOAT, NATURAL, STRING, CHAR, BOOLEAN,
    UNIT, NIL, TRUE, FALSE,
    STRUCT, INTERFACE, CLASS, TYPE, DATA,
    OBJECT, RECORD, NAMESPACE,

    FOR, IN, WHILE, REPEAT, UNTIL, IF,
    ELIF, ELSE, RETURN, EXPOSE,

    USE, USING, REQUIRE,
    
    ERROR, NUM_STATES
  */
  
  return acceptings;
}

std::expected<std::vector<Token>, Error> Lexer::mainloop() {
  std::vector<Token> tokens = { };
  
  return tokens;
}

const std::function<std::vector<Token>(std::string_view)> Lexer::tokenize = [](std::string_view input) {
  Lexer lexer(input);
  return (lexer.mainloop().has_value()) ? lexer.mainloop().value() : (std::vector<Token>){ }; // write code to handle lexical errors before pushing
};
