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
#include <cstdio>
#include <cassert>
#include <vector>
#include <string>
#include "cubescript.hpp"

namespace cubescript{

namespace expression{
enum token_id
{
    ERROR = 0,
    WHITESPACE,
    CHAR,
    END_ROOT_EXPRESSION,
    START_EXPRESSION,
    END_EXPRESSION,
    START_SYMBOL,
    START_END_STRING,
    START_MULTILINE_STRING,
    END_MULTILINE_STRING,
    START_COMMENT
};
static const token_id symbols[] = 
    {ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, 
    ERROR, WHITESPACE, END_ROOT_EXPRESSION, ERROR, ERROR, END_ROOT_EXPRESSION, ERROR, ERROR, 
    ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, 
    ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, 
    WHITESPACE, CHAR, START_END_STRING, CHAR, START_SYMBOL, CHAR, CHAR, CHAR, 
    START_EXPRESSION, END_EXPRESSION, CHAR, CHAR, CHAR, CHAR, CHAR, START_COMMENT, 
    CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, 
    CHAR, CHAR, CHAR, END_ROOT_EXPRESSION, CHAR, CHAR, CHAR, CHAR, 
    CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, 
    CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, 
    CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, 
    CHAR, CHAR, CHAR, START_MULTILINE_STRING, CHAR, END_MULTILINE_STRING, CHAR, CHAR, 
    CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, 
    CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, 
    CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, 
    CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, CHAR, ERROR, 
    ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, 
    ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, 
    ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, 
    ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, 
    ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, 
    ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, 
    ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, 
    ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, 
    ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, 
    ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, 
    ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, 
    ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, 
    ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, 
    ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, 
    ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, 
    ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR, ERROR};
} //namespace expression

enum word_type{
    WORD_INTEGER = 0,
    WORD_REAL    = 1,
    WORD_STRING  = 2
};

bool eval_word(const char ** source_begin, 
                const char * source_end, command_stack & command)
{
    const char * start = *source_begin;
    word_type type = WORD_INTEGER;
    
    for(const char * cursor = start; cursor != source_end; cursor++)
    {
        char c = *cursor;
        expression::token_id token_id = 
            expression::symbols[static_cast<std::size_t>(c)];
        
        if(token_id != expression::CHAR)
        {
            *source_begin = cursor - 1;
            std::size_t length = cursor - start;
            
            if(token_id != expression::ERROR && cursor != start) 
            {
                switch(type)
                {
                    case WORD_INTEGER:
                    {
                        float n;
                        std::sscanf(start, "%f", &n);
                        command.push_argument(static_cast<int>(n));
                        break;
                    }
                    case WORD_REAL:
                    {
                        float n;
                        std::sscanf(start, "%f", &n);
                        command.push_argument(n);
                        break;
                    }
                    case WORD_STRING:
                        command.push_argument(start, length);
                        break;
                }
                
                return true;
            }
            else
            {
                *source_begin = cursor;
                return false;
            }
        }
        
        if(type != WORD_STRING && !(c >= '0' && c <= '9') && c != '-' &&
          (cursor == *source_begin || c != 'e'))
        {
            if(c == '.') type = WORD_REAL;
            else type = WORD_STRING;
        }
    }
    
    *source_begin = source_end;
    return false;
}

static std::string decode_string(const char ** begin, const char * end, 
                              const std::vector<const char *> & escape_sequence)
{
    const char * start = *begin;
    
    std::string result;
    result.reserve(end - start);
    
    result.append(start, escape_sequence[0]);
    
    typedef std::vector<const char *>::const_iterator iterator;
    for(iterator iter = escape_sequence.begin(); iter != escape_sequence.end();
        iter++)
    {
        assert(*iter != end);
        
        const char * escape = *iter;
        
        switch(*(escape + 1))
        {
            case '\"': result.append(1, '\"'); break; 
            case '\\': result.append(1, '\\'); break; 
            case 'n':  result.append(1, '\n'); break; 
            case 'r':  result.append(1, '\r'); break; 
            case 't':  result.append(1, '\t'); break; 
            case 'f':  result.append(1, '\f'); break; 
            case 'b':  result.append(1, '\b'); break;
        }
        
        const char * sub_end = end;
        if(iter + 1 != escape_sequence.end())
            sub_end = *(iter + 1);
        
        result.append(escape + 2, sub_end);
    }
    
    return result;
}

bool eval_string(const char ** source_begin, 
                  const char * source_end, command_stack & command)
{
    assert(**source_begin == '"');
    const char * start = (*source_begin) + 1;
    *source_begin = start;
    
    std::vector<const char *> escape_sequence;
    
    for(const char * cursor = start; cursor != source_end; cursor++)
    {
        char c = *cursor;
        switch(c)
        {
            case '"':
                *source_begin = cursor;
                
                if(escape_sequence.size())
                {
                    std::string string = decode_string(source_begin, cursor,
                                                       escape_sequence);
                    command.push_argument(string.c_str(), string.length());
                }
                else
                {
                    std::size_t length = cursor - start;
                    command.push_argument(start, length);
                }
                return true;
            case '\\':
            case '^':
                escape_sequence.push_back(cursor);
                cursor++;
                break;
            case '\r':
            case '\n':
                *source_begin = cursor;
                return false;
            default:break;
        }
    }
    
    *source_begin = source_end;
    return false;
}

static std::string decode_multiline_string(const char ** begin, const char * end
    ,const std::vector<const char *> & interpolations, command_stack & command)
{
    const char * start = *begin;
    
    std::string result;
    result.reserve(end - start);
    
    result.append(start, interpolations[0]);
    
    typedef std::vector<const char *>::const_iterator iterator;
    for(iterator iter = interpolations.begin(); iter != interpolations.end();
        iter++)
    {
        const char * cursor = *iter;
        
        for(; *cursor == '@' && cursor != end; cursor++);
        
        if(*cursor == '(') eval_expression(&cursor, end, command);
        else eval_symbol(&cursor, end, command);//FIXME requires whitespace
        
        result.append(command.pop_string());
        
        if(cursor + 1 < end)
        {
            const char * sub_end = end;
            if(iter + 1 != interpolations.end())
                sub_end = *(iter + 1);
            
            result.append(cursor + 1, sub_end);
        }
    }
    
    return result;
}

bool eval_multiline_string(const char ** source_begin, const char * source_end,
                            command_stack & command)
{
    assert(**source_begin == '[');
    const char * start = (*source_begin) + 1;
    *source_begin = start;
    
    std::vector<const char *> interpolations;
    int nested = 1;
    
    for(const char * cursor = start; cursor != source_end; cursor++)
    {
        char c = *cursor;
        
        switch(c)
        {
            case '[':
                nested++;
                break;
            case ']':
                if(--nested == 0)
                {
                    if(interpolations.size())
                    {
                        std::string string = decode_multiline_string(
                            source_begin, cursor, interpolations, command);
                        command.push_argument(string.c_str(), string.length());
                    }
                    else 
                    {
                        std::size_t length = cursor - start;
                        command.push_argument(start, length);
                    }
                    *source_begin = cursor;
                    return true;
                }
                break;
            case '@':
            {
                const char * first_at = cursor;
                for(; *cursor == '@' && cursor != source_end; cursor++);
                if(cursor - first_at >= nested)
                    interpolations.push_back(first_at);
                cursor--;
                break;
            }
            default: break;
        }
    }
    
    *source_begin = source_end;
    return false;
}

bool eval_symbol(const char ** source_begin, const char * source_end,
                  command_stack & command)
{
    const char * start = *source_begin;
    if(*start == '$') *source_begin = ++start;
    
    for(const char * cursor = start; cursor != source_end; cursor++)
    {
        expression::token_id token_id = 
            expression::symbols[static_cast<std::size_t>(*cursor)];
        
        if(token_id != expression::CHAR)
        {
            *source_begin = cursor - 1;
            if(token_id != expression::ERROR)
            {
                std::size_t length = cursor - start;
                command.push_argument_symbol(start, length);
                return true;
            }
            else
            {
                *source_begin = cursor;
                return false;
            }
        }
    }
    
    *source_begin = source_end;
    return false;
}

bool eval_comment(const char ** source_begin, const char * source_end,
                    command_stack & command)
{
    assert(**source_begin == '/');
    const char * start = (*source_begin) + 1;
    *source_begin = start;
    
    if(*start != '/') return false;
    start++;
    
    for(const char * cursor = start; cursor != source_end; cursor++)
    {
        expression::token_id token_id = 
            expression::symbols[static_cast<std::size_t>(*cursor)];
        
        if(token_id == expression::END_ROOT_EXPRESSION)
        {
            *source_begin = cursor - 1;
            return true;
        }
    }
    
    *source_begin = source_end;
    return false;
}


bool eval_expression(const char ** source_begin, const char * source_end,
                      command_stack & command)
{
    const char * start = *source_begin;
    if(*start == '(') start++;
    
    std::size_t call_index = command.push_command();
    bool first_argument = true;
    
    for(const char * cursor = start; cursor != source_end; cursor++)
    {
        char c = *cursor;
        expression::token_id token_id = 
            expression::symbols[static_cast<std::size_t>(c)];
        
        switch(token_id)
        {
            case expression::WHITESPACE:
                // Do nothing
                break;
            case expression::END_EXPRESSION:
            {
                const char * start = *source_begin;
                *source_begin = cursor;
                if(*start != '(') return false;
                command.call(call_index);
                return true;
            }
            case expression::END_ROOT_EXPRESSION:
                *source_begin = cursor;
                command.call(call_index);
                return true;
            case expression::START_EXPRESSION:
                eval_expression(&cursor, source_end, command);
                first_argument = false;
                break;
            case expression::START_SYMBOL:
                eval_symbol(&cursor, source_end, command);
                first_argument = false;
                break;
            case expression::START_END_STRING:
                eval_string(&cursor, source_end, command);
                break;
            case expression::START_MULTILINE_STRING:
                eval_multiline_string(&cursor, source_end, command);
                first_argument = false;
                break;
            case expression::CHAR:
                if(!first_argument) eval_word(&cursor, source_end, command);
                else eval_symbol(&cursor, source_end, command);
                first_argument = false;
                break;
            case expression::START_COMMENT:
                eval_comment(&cursor, source_end, command);
                break;
            case expression::ERROR:
            default:
                *source_begin = cursor;
                return false;
        }
    }

    *source_begin = source_end;
    return false;
}

} //namespace cubescript

