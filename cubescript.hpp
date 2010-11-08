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

class command_stack
{
public:
    typedef void (command_stack::* string_member_function)(const char *,
                                                           std::size_t);
    
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

bool eval_word(const char **, const char*, command_stack &);
bool eval_string(const char **, const char*, command_stack &);
bool eval_multiline_string(const char **, const char *, command_stack &);
bool eval_symbol(const char **, const char *, command_stack &);
bool eval_comment(const char **, const char *, command_stack &);
bool eval_expression(const char **, const char *, command_stack &);
bool eval(const char **, const char *, command_stack &);

} //namespace cubescript

#endif

