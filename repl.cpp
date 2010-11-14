#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <iostream>
#include <string>
#include "cubescript.hpp"
using namespace cubescript;
#include "lua_command_stack.hpp"

int main(int, char**)
{
    lua_State * L = luaL_newstate();
    luaL_openlibs(L);
    
    lua_command_stack lua_command(L, LUA_GLOBALSINDEX);
    
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
        
        eval_error error = eval(&code_c_str, code_c_str_end, lua_command);
        
        if(error && error.get_error_type() == EVAL_PARSE_ERROR)
            std::cout<<"Parse error: "<<error.get_description()<<std::endl;
        
        const char * output_prefix = "";
        if(error.get_error_type() == EVAL_RUNTIME_ERROR)
            output_prefix = "Runtime error: ";
        
        std::cout<<output_prefix<<lua_command.pop_string()<<std::endl;
        
        code.clear();
    }
       
    std::cout<<std::endl;
    return 0;
}

