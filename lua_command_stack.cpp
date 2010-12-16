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
#include <sstream>

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
        if(lua_type(m_state, -1) == LUA_TNIL)
        {
            lua_pop(m_state, 1);
            std::stringstream format;
            format<<"attempt to index '"<<std::string(value, start - 1)
                  <<"' (a nil value)";
            throw eval_error(EVAL_RUNTIME_ERROR, 0, format.str());
        }
        
        for(; end != end_of_string && *end !='.'; end++);
        
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

std::string lua_command_stack::pop_string()
{
    std::string output;
    std::size_t length;
    const char * string = lua_tolstring(m_state, -1, &length);
    if(string) output = std::string(string, length);
    else
    {
        switch(lua_type(m_state, -1))
        {
            case LUA_TNIL:
                output = "nil";
                break;
            case LUA_TBOOLEAN:
                output = (lua_toboolean(m_state, -1) ? "true" : "false");
                break;
            default:
            {
                std::stringstream format;
                format<<"<"<<lua_typename(m_state, lua_type(m_state, -1))
                      <<": "<<std::hex<<lua_topointer(m_state, -1)
                      <<">"<<std::endl;
                
                output = format.str();
            }
        }
    }
    lua_pop(m_state, 1);
    return output;
}

void lua_command_stack::call(std::size_t index)
{
    std::size_t top = lua_gettop(m_state);
    if(index > top)
    {
        lua_pushnil(m_state);
        return;
    }
    int status = lua_pcall(m_state, top - index, LUA_MULTRET, 0);
    if(status != 0) 
        throw eval_error(EVAL_RUNTIME_ERROR, 0, lua_tostring(m_state, -1));
}

namespace lua{

int eval(lua_State * L)
{
    std::size_t source_length;
    const char * source = luaL_checklstring(L, 1, &source_length);
    luaL_checktype(L, 2, LUA_TTABLE);
    
    lua_command_stack command(L, 2);
    
    int bottom = lua_gettop(L);
    lua_pushnil(L);
    
    try
    {
        eval(&source, source + source_length, command);
    }
    catch(const eval_error & error)
    {
        lua_pushstring(L, error.get_description().c_str());
        lua_replace(L, bottom + 1);
    }

    return lua_gettop(L) - bottom;
}

} //namespace lua
} //namespace cubescript

