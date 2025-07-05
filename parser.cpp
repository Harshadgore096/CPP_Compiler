#include <bits/stdc++.h>

#include "semantics.h"
using namespace std;

void parse(int pointer){

    while(!expect(TOKEN_EOF, pointer))
    {
        if(((expect(TOKEN_VAR, pointer))||(expect(TOKEN_BOOL, pointer))
        ||(expect(TOKEN_VOID, pointer))||(expect(TOKEN_PIN, pointer))
        ||(expect(TOKEN_STRING, pointer)))&&(expect(TOKEN_FUN, pointer+1))){
            pointer = c_tokenfun(pointer);
        }  
        else
        if((expect(TOKEN_FUNCALL, pointer))){
            pointer = expression(pointer, TOKEN_SEMICOLON);
        } 
        else
        if(expect(TOKEN_VAR, TOKEN_IDENTIFIER, pointer) 
        || expect(TOKEN_STRINGVAR, TOKEN_IDENTIFIER, pointer) 
        || expect(TOKEN_BOOL, TOKEN_IDENTIFIER, pointer)){
            pointer = c_varDeclaration(pointer);
        }
        else 
        if(scopeCount.top().find("*tenary") != string::npos){
            pointer = c_tenary(pointer);
        }
        else          
        if(expect(TOKEN_COUT, TOKEN_SHIFT_LEFT, pointer)){
            pointer = c_cout(pointer);
        }
        else  
        if(expect(TOKEN_IDENTIFIER, TOKEN_COLON, pointer)){ 
            pointer = c_label(pointer); 
        }else           
        if(expect(TOKEN_GOTO, TOKEN_COLON, pointer)){ 
            pointer = c_goto(pointer); 
        }else      
        if(expect(TOKEN_WHILE, TOKEN_LEFT_PAREN, pointer)){               
            pointer = c_while(pointer);            
        }else    
        if(expect(TOKEN_FOR, TOKEN_LEFT_PAREN, pointer)){
            pointer = c_forloop(pointer);
        }else    
        if(expect(TOKEN_RETURN, pointer)){
            pointer = c_return(pointer); 
        }else   
        if(expect(TOKEN_IF, TOKEN_LEFT_PAREN, pointer)){  
            pointer = c_if(pointer);
        }else           
        if(expect(TOKEN_ELSE, TOKEN_IF, pointer)){ 
            pointer =  c_else(pointer);
        }else
        if(expect(TOKEN_ELSE, TOKEN_LEFT_BRACE, pointer)){
            pointer = pointer +2 ;     
        }else         
        if((expect(TOKEN_CONTINUE, TOKEN_SEMICOLON, pointer))){    
            pointer = c_continue(pointer);
        }else    
        if(expect(TOKEN_IDENTIFIER, pointer)){ 
            pointer = expression(pointer, TOKEN_SEMICOLON); 
        }else       
        if(expect(TOKEN_SWITCH, TOKEN_LEFT_PAREN, pointer)){  
            pointer = c_switch(pointer);
        }else      
        if(expect(TOKEN_CASE, pointer)){  
            pointer = c_case(pointer);
        }else       
        if(expect(TOKEN_DEFAULT,TOKEN_COLON, pointer)){
            pointer = c_default(pointer);
        }else       
        if(expect(TOKEN_BREAK,TOKEN_SEMICOLON,  pointer)
        && (scopeCount.top().find("*case") != string::npos)){
            pointer = c_swbreak(pointer);
        }
        else    
        if(expect(TOKEN_BREAK, TOKEN_SEMICOLON, pointer)){
            pointer = c_break(pointer);
        }
        else
        if(expect(TOKEN_RIGHT_BRACE, pointer)){
            pointer = c_rightBrace(pointer);
        }
        else{
                error("Syntax error on line", st.tokenTable[pointer]->lineNo);
            } 
    }
    
    if(expect(TOKEN_EOF, pointer)){
        exit(0);
    }
}
