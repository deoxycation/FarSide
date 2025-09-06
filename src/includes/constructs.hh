#ifndef __CONSTRUCTS_FCS__
#define __CONSTRUCTS_FCS__

#include "./farSide.hh"

namespace Fsc::Constructs {
  enum class State : unsigned short {
    START = 0,
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

using State = Fsc::Constructs::State;
using Token = Fsc::Constructs::Token;
template <typename T>
using Compiler = Fsc::Constructs::Compiler<T>;

#endif
