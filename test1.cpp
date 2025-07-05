

#ifndef __PARSER_H__
#define __PARSER_H__
using namespace std;
#include <string>
#include "string.h"
const int MAX = 100000;

typedef enum {
TOKEN_EQUAL, TOKEN_LOGICAL_OR,  TOKEN_LOGICAL_AND, TOKEN_EQUAL_EQUAL, TOKEN_GREATER,
TOKEN_GREATER_EQUAL,  TOKEN_LESS, TOKEN_LESS_EQUAL, TOKEN_BANG_EQUAL,

 TOKEN_COLON, TOKEN_TENARY,

 TOKEN_ADD_ASS, TOKEN_SUB_ASS, TOKEN_SLASH_ASS, 
TOKEN_STAR_ASS, TOKEN_ASS_OR, TOKEN_ASS_XOR, TOKEN_ASS_AND,

TOKEN_BIT_OR,  TOKEN_BIT_XOR, TOKEN_BIT_AND, 
TOKEN_SHIFT_LEFT, TOKEN_SHIFT_RIGHT, TOKEN_INVERSE, TOKEN_PLUS,  TOKEN_SUB,
TOKEN_SLASH, TOKEN_MODULUS, TOKEN_STAR, 


TOKEN_BANG, TOKEN_SUB_SELF, TOKEN_ADD_SELF,

 // Single-character tokens.
 TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
 TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
 TOKEN_COMMA, TOKEN_DOT, TOKEN_SEMICOLON,  
 
 TOKEN_START, TOKEN_END,
 // One or two character tokens.

 // Literals.
 TOKEN_BOOL, TOKEN_VAR, TOKEN_STRINGVAR, TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER, 
  TOKEN_PIN,TOKEN_FALSE, TOKEN_TRUE, TOKEN_FUNCALL, TOKEN_OUTPUT, TOKEN_ENDL,

  TOKEN_INFINITE, TOKEN_FUN,TOKEN_VOID,  TOKEN_LEFT_BLOCK, TOKEN_RIGHT_BLOCK,

 TOKEN_BREAK,TOKEN_CONTINUE, TOKEN_SWITCH,TOKEN_CASE, TOKEN_DEFAULT, TOKEN_ELSE,
 TOKEN_FOR, TOKEN_IF, TOKEN_NIL, TOKEN_PRINT, TOKEN_RETURN,
 TOKEN_WHILE, TOKEN_HYPHEN, TOKEN_ERROR, TOKEN_COUT,
 TOKEN_EOF

}TokenType;

class tokenClass{

public:
    TokenType type;
    TokenType  Vtype;
    std::string identifier;
    int lineNo, index;
     std::string regName;
    tokenClass(TokenType type, int lineNo, int index)
    {
        this->type      = type;
        this->lineNo    = lineNo;
        this->index     = index;              
   }

    tokenClass(TokenType type, int lineNo, int index, string identifier)
    {
        this->type = type;
        this->lineNo    = lineNo;
        this->index     = index;
        this->identifier= identifier;        
 }
    friend class SymbolTable;

};
class funcList { 
 public:   
    string Fname;
    int An, Vn, Vs,  As;
    int call = 0;
    funcList(int An, string Fname, int Vn, int Vs, int As, int call)
    {
        this->An   = An;
        this->Fname        = Fname; 
        this->Vn   = Vn;  
        this->Vs   = Vs;               
        this->As   = As; 
        this->call = call;                 
    }
    friend class SymbolTable;   
};
class lVList {   
public:  
    string  Vname;
    TokenType vType;
    int vNo;
    lVList(TokenType vType, string Vname, int vNo)
    {
        this->vType  = vType;
        this->Vname  = Vname;
        this->vNo    = vNo;
    }
    friend class SymbolTable;      
};
class gVList { 
public:  
    string  Gname;
    TokenType vType;
    gVList(TokenType vType, string Gname)
    {
        this->vType  = vType;
        this->Gname  = Gname;
    }
    friend class SymbolTable;
};
class ArgumentList { 
public:  
    TokenType argType;
    string  argName;
    int  argNo;

    ArgumentList(TokenType argType, string argName, int argNo)
    {
        this->argType        = argType;
        this->argName        = argName;
        this->argNo          = argNo;
   }
    friend class SymbolTable;    
};
class SymbolTable {

public:
     
    funcList*        funcTable[MAX];
    tokenClass*      tokenTable[MAX];
    tokenClass*      expressList[MAX];   
    lVList*          localVariable[MAX]; 
    gVList*          globalVariable[MAX];
    ArgumentList*    ArgumentTable[MAX]; 
    
    int index, indexA, indexV, indexE, indexG, indexL;
    SymbolTable()
    {   index  = 1;
        indexA = 1;
        indexV = 1;
        indexG = 1;
        indexE = 1;
        indexL = 1;

        for (int i = 0; i < MAX; i++){
            tokenTable[i]      = NULL;            
            expressList[i]      = NULL;
            funcTable[i]       = NULL;    
            ArgumentTable[i]   = NULL;  
            localVariable[i]   = NULL;
            ArgumentTable[i]   = NULL;   
        }                                                     
    }

};

void sendToparser(TokenType type, int line, std::string numToken);
void sendToparser(TokenType type, int line);
void error(string error, int lineNo);

#endif








