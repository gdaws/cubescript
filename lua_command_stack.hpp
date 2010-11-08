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
#ifndef CUBESCRIPT_LUA_COMMAND_STACK_HPP
#define CUBESCRIPT_LUA_COMMAND_STACK_HPP

#include <lua.hpp>
#include "cubescript.hpp"

namespace cubescript{

class lua_command_stack:public command_stack
{
public:
    lua_command_stack(lua_State *, int table_index);
    std::size_t push_command();
    void push_argument_symbol(const char *, std::size_t);
    void push_argument();
    void push_argument(bool);
    void push_argument(int);
    void push_argument(float);
    void push_argument(const char *, std::size_t);
    std::string pop_string();
    bool call(std::size_t);
private:
    lua_State * m_state;
    int m_table_index;
};

} //namespace cubescript

#endif

