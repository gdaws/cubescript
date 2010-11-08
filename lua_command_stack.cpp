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
#include "lua_command_stack.hpp"
#include <iostream>

namespace cubescript{

lua_command_stack::lua_command_stack(lua_State * state)
 :m_state(state)
{
    
}

std::size_t lua_command_stack::push_command()
{
    return lua_gettop(m_state) + 1;
}

void lua_command_stack::push_argument_symbol(const char * value,
                                             std::size_t length)
{
    lua_pushlstring(m_state, value, length);
    lua_gettable(m_state, LUA_GLOBALSINDEX);
}

void lua_command_stack::push_argument()
{
    lua_pushnil(m_state);
}

void lua_command_stack::push_argument(bool value)
{
    lua_pushboolean(m_state, value);
}

void lua_command_stack::push_argument(int value)
{
    lua_pushinteger(m_state, value);
}

void lua_command_stack::push_argument(float value)
{
    lua_pushnumber(m_state, value);
}

void lua_command_stack::push_argument(const char * value, std::size_t length)
{
    lua_pushlstring(m_state, value, length);
}

std::string lua_command_stack::pop_string()
{
    std::size_t length;
    const char * string = lua_tolstring(m_state, -1, &length);
    std::string result(string, length);
    lua_pop(m_state, 1);
    return result;
}

void lua_command_stack::call(std::size_t index)
{
    if(lua_pcall(m_state, lua_gettop(m_state) - index, 1, 0) !=0)
    {
        //FIXME
        std::cerr<<lua_tostring(m_state, -1)<<std::endl;
    }
}

} //namespace cubescript

