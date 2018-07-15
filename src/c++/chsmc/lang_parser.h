/*
**      CHSM Language System
**      src/c++/chsmc/lang_parser.h
**
**      Copyright (C) 2018  Paul J. Lucas
**
**      This program is free software; you can redistribute it and/or modify
**      it under the terms of the GNU General Public License as published by
**      the Free Software Foundation; either version 3 of the License, or
**      (at your option) any later version.
**
**      This program is distributed in the hope that it will be useful,
**      but WITHOUT ANY WARRANTY; without even the implied warranty of
**      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**      GNU General Public License for more details.
**
**      You should have received a copy of the GNU General Public License
**      along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef chsmc_lang_parser_H
#define chsmc_lang_parser_H

// local
#include "options.h"
#include "util.h"

// standard
#include <exception>
#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <utility>                      /* for swap */
#include <vector>

///////////////////////////////////////////////////////////////////////////////

/**
 * A %fn_param is a function parameter.
 * Given a function declaration such as:
 *
 *    T (*const f)( U )
 *
 * we parse out the name:
 *
 *    decl: T (*const $)( U )
 *    name: f
 */
struct fn_param {
  unsigned    line_no_;
  std::string decl_;
  std::string name_;

  fn_param() : line_no_( 0 ) {
  }

  fn_param( unsigned line_no, std::string const &decl,
            std::string const &name ) :
    line_no_( line_no ), decl_( decl ), name_( name )
  {
  }

  void clear() {
    decl_.clear();
    name_.clear();
  }
};

typedef std::vector<fn_param> fn_param_list;

///////////////////////////////////////////////////////////////////////////////

/**
 * A %lang_parser is used to parse parts of a particular programming language.
 * Currently, %lang_parser parses only function parameter lists between
 * parentheses.  For each parameter, it need only parse its name.
 */
class lang_parser {
public:
    /**
     * A %token represents a fundamental grammar building block.
     */
    class token {
    public:
      enum type {
        NONE,
        IDENTIFIER  = 0x81,
        NUMBER      = 0x82,
        AMPER       = '&',
        LPAREN      = '(',
        RPAREN      = ')',
        STAR        = '*',
        COMMA       = ',',
        DOT         = '.',
        SLASH       = '/',
        COLON       = ':',
        LANGLE      = '<',
        EQUAL       = '=',
        RANGLE      = '>',
        LBRACKET    = '[',
        RBRACKET    = ']',
      };

      /**
       * Constructs a %token.
       *
       * @param t The type of %token to construct.
       */
      explicit token( type tt = NONE );

      /**
       * Constructs a %token.
       *
       * @param c The character representing a %token.
       */
      explicit token( char c );

      /**
       * Constructs a %token.
       *
       * @param tt The type of %token to construct.
       * @param s The %token string.
       */
      explicit token( type tt, std::string const &s );

      token( token const &t ) = default;
      token( token &&other );
      token& operator=( token const &t ) = default;
      token& operator=( token &&t );

      /**
       * Gets a string representation of the %token.
       *
       * @return Returns said string.
       */
      std::string as_str() const;

      /**
       * Clears this %token.
       */
      void clear() {
        str_.clear();
        type_ = NONE;
      }

      /**
       * Gets the character value of the %token only when has_char() returns
       * `true`.
       *
       * @return Returns said character.
       */
      char get_char() const;

      /**
       * Gets the string value of the %token only when has_str() returns
       * `true`.
       *
       * @return Returns said string.
       */
      std::string const& get_str() const;

      /**
       * Gets the type of this %token.
       *
       * @return Returns said type.
       */
      type get_type() const {
        return type_;
      }

      /**
       * Gets whether this %token has a character.
       *
       * @return Returns `true` only if the %token type is not #NONE,
       * #IDENTIFIER, or #NUMBER.
       */
      bool has_char() const;

      /**
       * Gets whether this %token has a string.
       *
       * @return Returns `true` only if the %token type is #IDENTIFIER or
       * #NUMBER.
       */
      bool has_str() const;

      /**
       * Checks whether the token represents a punctuation character.
       *
       * @return Returns `true` only if it does.
       */
      bool is_punct() const;

      /**
       * Swaps this %token with another.
       *
       * @param t The other %token to swap with.
       */
      void swap( token &t );

      /**
       * Conversion to `bool`.
       *
       * @return Returns `true` only if the %token's type is not #NONE.
       */
      operator bool() const {
        return type_ != NONE;
      }

      /**
       * Gets whether this %token's type is #NONE.
       *
       * @return Returns `true` only if the %token's type is #NONE.
       */
      bool operator!() const {
        return type_ == NONE;
      }

    private:
      std::string str_;
      type type_;
    };

    /**
     * An %exception is thrown for a parsing error.
     */
    class exception : public std::exception {
    public:
      explicit exception( std::string const &msg ) : msg_( msg ) { }
      virtual ~exception();

      virtual const char* what() const noexcept override;

    private:
      std::string msg_;
    };

    virtual ~lang_parser();

    /**
     * Clears this %lang_parser.
     */
    void clear();

    /**
     * Creates a %lang_parser for the given language.
     *
     * @param l The language to create the %lang_parser for.
     * @param is The input stream to use.
     * @return Returns said %lang_parser.
     */
    static std::unique_ptr<lang_parser> create( lang l, std::istream &is );

    /**
     * Parses a parameter list of a function in a particular language.
     *
     * @return Returns a list of parsed parameters.
     */
    fn_param_list parse_param_list();

    /**
     * Parses (and ignores) and comment.  By default, C/C++/Go/Java comments
     * are recognized.
     *
     * @param c The first character of the comment.
     */
    virtual void parse_comment( char c );

protected:
    /**
     * Constructs a %lang_parser.
     *
     * @param is The input stream to use to get characters.
     */
    lang_parser( std::istream &is );

    ////////// parsing ////////////////////////////////////////////////////////

    /**
     * Parses an identifier.  By default, an identifier starts with a letter or
     * underscore and is followed by zero or more letters, digits, or
     * underscores.
     *
     * @param c The first character of the identifier.
     * @return Returns said identifier.
     */
    virtual std::string parse_identifier_( char c );

    /**
     * Parses a number.
     *
     * @param c The first character of the number.
     * @return Returns said number (as a string).
     */
    virtual std::string parse_number_( char c );

    ////////// token //////////////////////////////////////////////////////////

    /**
     * Gets and consumes the next token.
     *
     * @return Returns the next token.
     */
    virtual token get_token_();

    /**
     * Gets and consumes the next token, but only if its type is \a tt; if not,
     * no token is consumed.
     *
     * @param tt The type of token to request.
     * @return Returns a token of the requested type or the #NONE token if
     * either the token's type is not \a tt or there are no more tokens.
     */
    token get_token_if_type_( token::type tt );

    /**
     * Gets and consumes the next token.  If not #NONE, it \e must be of type
     * \a tt; otherwise, throws an exception.
     *
     * @param tt The type of token to request.
     * @return Returns the next token.
     * @throws exception only if there is a next token and its type is not \a
     * tt.
     */
    token get_token_of_type_( token::type tt );

    /**
     * Gets whether the token represents the end of the current parameter being
     * parsed.
     *
     * @param t The token to check.
     * @return Returns `true` only if it does.
     */
    virtual bool is_param_end_( token const &t );

    /**
     * Gets, but does not consume, the next token.  At most one token can be
     * peeked.
     *
     * @return Returns the next token or the #NONE token if none is available.
     */
    token peek_token_();

    /**
     * Gets and consumes the next token.  If no token is available, throws an
     * exception.
     *
     * @return Returns said token.
     * @throws exception is no token is available.
     */
    token require_token_();

    /**
     * Gets and consumes the next token.  If no token is available, or its type
     * type is not \a tt, throws an exception.
     *
     * @param tt The type of token to require.
     * @return Returns said token.
     * @throws exception only if there is no next token or its type is not \a
     * tt.
     */
    token require_token_of_type_( token::type tt );

    /**
     * Ungets \a t and makes it available for subsequent getting.  At most one
     * token can be "ungot" consecutively.
     *
     * @param t The token to unget.
     */
    void unget_token_( token const &t );

    ////////// character //////////////////////////////////////////////////////

    /**
     * Gets and consumes the next character from the input.
     *
     * @return Returns the character or `\0` if none is available.
     */
    char get_char_();

    /**
     * Gets, but does not consume, the next character from the input.  At most
     * one character can be peeked.
     *
     * @return Returns the character or `\0` if none is available.
     */
    char peek_char_();

    /**
     * Ungets \a c and makes it available for subsequent getting.  At most one
     * character can be "ungot" consecutively.
     *
     * @param c The character to unget.
     */
    void unget_char_( char c );

    ////////// miscellaneous //////////////////////////////////////////////////

    /**
     * Gets whether we're still within parentheses.
     *
     * @return Returns `true` only if we are.
     */
    bool within_parens_() const {
      return parens_.size() - started_with_lparen_ > 0;
    }

private:
    std::istream &is_;
    std::stack<char> parens_;           // stack of "(<[" characters
    token peeked_token_;                // peeked (or ungotten) token
    bool started_with_lparen_;          // did istream start with '('?

    static token const NONE;

    char get_char_from_istream_();

    [[noreturn]] static void unexpected_char( char c );

    [[noreturn]] static void unexpected_token( token const &t = NONE,
                                               token::type tt = token::NONE );

    virtual bool parse_param_( fn_param *param ) = 0;
};

///////////////////////////////////////////////////////////////////////////////

inline bool has_char( lang_parser::token::type tt ) {
  unsigned char const uc = static_cast<unsigned char>( tt );
  return uc > 0 && uc < 0x80;
}

inline bool has_char( lang_parser::token &t ) {
  return has_char( t.get_type() );
}

inline bool lang_parser::token::has_char() const {
  return ::has_char( type_ );
}

inline bool has_str( lang_parser::token::type tt ) {
  return (static_cast<unsigned char>( tt ) & 0x80) != 0;
}

inline bool has_str( lang_parser::token &t ) {
  return has_str( t.get_type() );
}

inline bool lang_parser::token::has_str() const {
  return ::has_str( type_ );
}

inline bool is_punct( lang_parser::token::type tt ) {
  return has_char( tt );
}

inline bool is_punct( lang_parser::token const &t ) {
  return is_punct( t.get_type() );
}

inline bool lang_parser::token::is_punct() const {
  return ::is_punct( type_ );
}

DEFINE_STD_SWAP_FUNCTION( lang_parser::token );

///////////////////////////////////////////////////////////////////////////////

/**
 * Compares two tokens for equality.
 *
 * @param ti The first token.
 * @param tj The second token.
 * @return Returns `true` only if the two tokens are equal.
 */
bool operator==( lang_parser::token const &ti, lang_parser::token const &tj );

/**
 * Compares two tokens for inequality.
 *
 * @param ti The first token.
 * @param tj The second token.
 * @return Returns `true` only if the two tokens are not equal.
 */
inline bool operator!=( lang_parser::token const &ti,
                        lang_parser::token const &tj ) {
  return !(ti == tj);
}

/**
 * Checks a token for a particular type.
 *
 * @param t The token to check.
 * @param tt The token type to check for.
 * @return Returns `true` only if \a t has type \a tt.
 */
inline bool operator==( lang_parser::token const &t,
                        lang_parser::token::type tt ) {
  return t.get_type() == tt;
}

/**
 * Checks a token not for a particular type.
 *
 * @param t The token to check.
 * @param tt The token type to check for.
 * @return Returns `true` only if \a t does not have type \a tt.
 */
inline bool operator!=( lang_parser::token const &t,
                        lang_parser::token::type tt ) {
  return !(t == tt);
}

/**
 * Commutative token type equality.
 *
 * @param tt The token type to check for.
 * @param t The token to check.
 * @return Returns `true` only if \a t has type \a tt.
 */
inline bool operator==( lang_parser::token::type tt,
                        lang_parser::token const &t ) {
  return t == tt;
}

/**
 * Commutative token type inequality.
 *
 * @param tt The token type to check for.
 * @param t The token to check.
 * @return Returns `true` only if \a t does not have type \a tt.
 */
inline bool operator!=( lang_parser::token::type tt,
                        lang_parser::token const &t ) {
  return t != tt;
}

///////////////////////////////////////////////////////////////////////////////

/**
 * Emits a token::type to an ostream.
 *
 * @param os The ostream to emit to.
 * @param tt The token type to emit.
 * @return Returns \a os.
 */
std::ostream& operator<<( std::ostream &os, lang_parser::token::type tt );

/**
 * Emits a token to an ostream.
 *
 * @param os The ostream to emit to.
 * @param tt The token to emit.
 * @return Returns \a os.
 */
std::ostream& operator<<( std::ostream &os, lang_parser::token const &t );

///////////////////////////////////////////////////////////////////////////////
#endif /* chsmc_lang_parser_H */
/* vim:set et sw=2 ts=2: */
