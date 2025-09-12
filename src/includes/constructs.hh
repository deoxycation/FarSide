#ifndef __CONSTRUCTS_FCS__
#define __CONSTRUCTS_FCS__

#include "./fsc.hh"

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

namespace Fsc::Constructs {
  enum class ErrorTypes : unsigned short {
    UNKNOWN_TOKEN
  };

  enum class State : unsigned short {
    START = 0,
    
    // Core:
    IDENTIFIER, NUMBER,
    STRING_START, STRING_BODY, STRING_END,
    CHAR_START, CHAR_BODY, CHAR_END,
    
    // Delimiter Symbols
    LPAREM, RPAREM, LBRACKET, RBRACKET,
    LBRACE, RBRACE, 
    COMMA, SEMICOLON,
    AT,
    
    // Non-accepting Symbols ("*_UNFINISHED")
    PLUS_UNFINISHED, MINUS_UNFINISHED,
    DIV_UNFINISHED, MULT_UNFINISHED,
    MOD_UNFINISHED, CONCAT_UNFINISHED,
    ASSIGN_UNFINISHED, PIPE_UNFINISHED,
    BANG_UNFINISHED, ACCESS_UNFINISHED,
    GREATER_UNFINISHED, LESS_UNFINISHED,

    STRING_ESCAPE_START, CHAR_ESCAPE_START, STRING_ESCAPE_BODY, CHAR_ESCAPE_BODY,
    
    // fall-backs (exactly 14 numbers away from unfinished)
    PLUS, MINUS, DIV, MULT, MOD, CONCAT,
    ASSIGN, PIPE, BANG, ACCESS, GREATER, LESS,

    // !IMPORTANT NOTE: If order is changed or altered, without similarily altering the macro for difference
    // undefined behavior will occur within the lexer

    // (Remaining) Binary Ops
    PLUS_ASSIGN, MINUS_ASSIGN, DIV_ASSIGN,
    MULT_ASSIGN, MOD_ASSIGN, CONCAT_ASSIGN,
    TYPE_ARROW, FUNCTION_ARROW, AND, OR,
    GREATER_EQUALS, LESS_EQUALS,
    EQUALS, NOT_EQUALS, COLON,
    
    // (Remaining) Unary Ops
    INCREMENT, DECREMENT,
    
    // Keywords
    KEYWORD_NUMBER, INT, FLOAT, NATURAL, STRING, CHAR, BOOLEAN,
    UNIT, NIL, TRUE, FALSE,
    STRUCT, INTERFACE, CLASS, TYPE, DATA,
    OBJECT, RECORD, NAMESPACE,

    FOR, IN, WHILE, REPEAT, UNTIL, IF,
    ELIF, ELSE, RETURN, EXPOSE,

    USE, USING, REQUIRE,
    
    ERROR, NUM_STATES
  };

  struct Error {
    public:
    const ErrorTypes type;
    const std::string_view lexeme;
    const size_t lineNumber;
    const size_t start;
    const size_t end;
    
    Error(ErrorTypes type, std::string_view lexeme, size_t lineNumber, size_t start, size_t end)
      : type(type), lexeme(lexeme), lineNumber(lineNumber), start(start), end(end) { };
    ~Error() = default;
  };
  
  struct Token {
    public:
    const State type;
    const std::string_view lexeme;
    const size_t lineNumber;
    const size_t start;
    const size_t end;
    
    Token(State type, std::string_view lexeme, size_t lineNumber, size_t start, size_t end)
      : type(type), lexeme(lexeme), lineNumber(lineNumber), start(start), end(end) { };
    ~Token() = default;
  };

  template <typename T>
  class Compiler {
    private:
    const T data;

    public:
    T& get() const {
      return data;
    }

    template <typename U>
    Compiler<U> operator|(const std::function<U(T)> f) {
      return Compiler<U>(f(this->data));
    }

    Compiler(T data) : data(data) { };
    ~Compiler() = default;
  };
}

using ErrorTypes = Fsc::Constructs::ErrorTypes;
using State = Fsc::Constructs::State;
using Error = Fsc::Constructs::Error;
using Token = Fsc::Constructs::Token;
template <typename T>
using Compiler = Fsc::Constructs::Compiler<T>;

#endif
