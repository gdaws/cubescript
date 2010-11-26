#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <iostream>
#include <string>
#include "cubescript.hpp"
using namespace cubescript;
#include "lua_command_stack.hpp"

static int env_table_ref = LUA_NOREF;

static int set_env_table(lua_State * L)
{
    luaL_checktype(L, 1, LUA_TTABLE);
    luaL_unref(L, LUA_REGISTRYINDEX, env_table_ref);
    lua_pushvalue(L, 1);
    env_table_ref = luaL_ref(L, LUA_REGISTRYINDEX);
    return 0;
}

int main(int, char**)
{
    lua_State * L = luaL_newstate();
    luaL_openlibs(L);
    
    luaL_Reg cubescript_functions[] = {
        {"eval", eval},
        {NULL, NULL}
    };
    luaL_register(L, "cubescript", cubescript_functions);
    
    lua_pushcfunction(L, set_env_table);
    lua_setglobal(L, "set_env_table");
    
    if(luaL_dofile(L, "./init.lua") != 0)
        std::cerr<<lua_tostring(L, -1)<<std::endl;
    
    std::string code;
    const char * line;
    while((line = readline(code.length() ? ">> " : "> ")))
    {
        if(!line[0]) continue;
        
        code += line;
        code += "\n";
        
        const char * code_c_str = code.c_str();
        const char * code_c_str_end = code_c_str + code.length();
        
        if(!is_complete_code(code_c_str, code_c_str_end)) continue;
        
        if(env_table_ref != LUA_NOREF)
            lua_rawgeti(L, LUA_REGISTRYINDEX, env_table_ref);
        else lua_pushvalue(L, LUA_GLOBALSINDEX);
        
        lua_command_stack lua_command(L, lua_gettop(L));
        eval_error error = eval(&code_c_str, code_c_str_end, lua_command);
        
        if(error && error.get_error_type() == EVAL_PARSE_ERROR)
            std::cout<<"Parse error: "<<error.get_description()<<std::endl;
        else if(error.get_error_type() == EVAL_RUNTIME_ERROR)
             std::cout<<"Runtime error: "<<error.get_description()<<std::endl;
        else
        {
            std::string output;
            if(lua_command.pop_string(output))
                std::cout<<output<<std::endl;
        }
        
        code.clear();
        
        add_history(line);
    }
       
    std::cout<<std::endl;
    return 0;
}

