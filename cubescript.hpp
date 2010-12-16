/*
  Copyright (c) 2010 Graham Daws <graham.daws@gmail.com>

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/
#ifndef CUBESCRIPT_HPP
#define CUBESCRIPT_HPP

#include <cstddef>
#include <string>

namespace cubescript{

enum eval_error_type
{
    EVAL_OK = 0,
    EVAL_PARSE_ERROR,
    EVAL_RUNTIME_ERROR
};

enum parse_errors
{
    PARSE_OK = 0,            // No errors
    PARSE_UNTERMINATED,      // Parse was incomplete
    PARSE_INVALID_CHARACTER, // Unknown character
    PARSE_EXPECTED,          // Expected a character sequence
    PARSE_UNEXPECTED         // Unexpected character (used in the wrong place) 
};

class eval_error
{
public:
    eval_error();

    eval_error(eval_error_type, int, std::string);
    operator bool()const;
    operator eval_error_type()const;
    eval_error_type get_error_type()const;
    int get_error_code()const;
    const std::string & get_description()const;
private:
    eval_error_type m_type;
    int m_code;
    std::string m_description;
};

class command_stack
{
public:
    virtual std::size_t push_command()=0;
    virtual void push_argument_symbol(const char *, std::size_t)=0;
    virtual void push_argument()=0;
    virtual void push_argument(bool)=0;
    virtual void push_argument(int)=0;
    virtual void push_argument(float)=0;
    virtual void push_argument(const char *, std::size_t)=0;
    virtual std::string pop_string()=0;
    virtual void call(std::size_t)=0;
};

void eval_word(const char **, const char*, command_stack &);
void eval_string(const char **, const char*, command_stack &);
void eval_multiline_string(const char **, const char *, command_stack &);
void eval_symbol(const char **, const char *, command_stack &);
void eval_comment(const char **, const char *, command_stack &);
void eval_expression(const char **, const char *, command_stack &, 
                           bool is_sub_expression = false);
void eval(const char **, const char *, command_stack &);

bool is_complete_code(const char *, const char *);

} //namespace cubescript

#endif

