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

lua_command_stack::lua_command_stack(lua_State * state, int table_index)
 :m_state(state), m_table_index(table_index)
{
    
}

std::size_t lua_command_stack::push_command()
{
    return lua_gettop(m_state) + 1;
}

void lua_command_stack::push_argument_symbol(const char * value,
                                             std::size_t length)
{
    const char * start = value;
    const char * end = start;
    const char * end_of_string = start + length;
    
    lua_pushvalue(m_state, m_table_index);
    
    while(end < end_of_string)
    {
        for(; end != end_of_string && *end !='.'; end++);
        
        if(start == end)
        {
            lua_pop(m_state, 1);
            lua_pushnil(m_state);
            //TODO error
            return;
        }
        
        lua_pushlstring(m_state, start, end - start);
        lua_gettable(m_state, -2);
        lua_replace(m_state, -2);
        
        start = end + 1;
        end = start;
    }
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

bool lua_command_stack::pop_string(std::string & output)
{
    std::size_t length;
    const char * string = lua_tolstring(m_state, -1, &length);
    if(string) output = std::string(string, length);
    else
    {
        output.assign("cannot represent ");
        output.append(lua_typename(m_state, lua_type(m_state, -1)));
        output.append(" as string");
    }
    lua_pop(m_state, 1);
    return string != NULL;
}

bool lua_command_stack::call(std::size_t index)
{
    std::size_t top = lua_gettop(m_state);
    if(index > top)
    {
        lua_pushnil(m_state);
        return true;
    }
    int status = lua_pcall(m_state, top - index, 1, 0);
    return status == 0;
}

int eval(lua_State * L)
{
    std::size_t source_length;
    const char * source = luaL_checklstring(L, 1, &source_length);
    luaL_checktype(L, 2, LUA_TTABLE);
    lua_command_stack command(L, 2);
    eval_error error = eval(&source, source + source_length, command);
    lua_pop(L, 2);
    lua_pushboolean(L, error.get_error_type() == EVAL_OK);
    lua_pushvalue(L, -2);
    return 2;
}

} //namespace cubescript

