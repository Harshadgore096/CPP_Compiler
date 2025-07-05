#include <bits/stdc++.h>
#include "parser.h"
using namespace std;
char* token;
int line = 1;
int lineNo = 1;
int parenCount = 0;
int braceCount = 0;
int deleteOld = 0;
TokenType previousToken;
string tokenNames[] = {
"TOKEN_EQUAL",  "TOKEN_LOGICAL_OR", "TOKEN_LOGICAL_AND", "TOKEN_EQUAL_EQUAL", 
"TOKEN_GREATER","TOKEN_GREATER_EQUAL",  "TOKEN_LESS", 
"TOKEN_LESS_EQUAL", "TOKEN_BANG_EQUAL", 

 "TOKEN_COLON", "TOKEN_TENARY",

 "TOKEN_ADD_ASS", "TOKEN_SUB_ASS", "TOKEN_SLASH_ASS", 
"TOKEN_STAR_ASS", "TOKEN_ASS_OR", "TOKEN_ASS_XOR", "TOKEN_ASS_AND",

"TOKEN_BIT_OR",  "TOKEN_BIT_XOR", "TOKEN_BIT_AND", 
"TOKEN_SHIFT_LEFT", "TOKEN_SHIFT_RIGHT", "TOKEN_INVERSE",
"TOKEN_PLUS", "TOKEN_SUB",   "TOKEN_SLASH", "TOKEN_MODULUS",  "TOKEN_STAR", 
"TOKEN_BANG", "TOKEN_SUB_SELF", "TOKEN_ADD_SELF",

 // Single-character "TOKENs.
 "TOKEN_LEFT_PAREN", "TOKEN_RIGHT_PAREN",
 "TOKEN_LEFT_BRACE", "TOKEN_RIGHT_BRACE",

 "TOKEN_COMMA", "TOKEN_DOT", "TOKEN_SEMICOLON",
 "TOKEN_START", "TOKEN_END",
 // One or two character "TOKENs.
 // Literals.
  "TOKEN_BOOL", "TOKEN_VAR", "TOKEN_STRINGVAR", 
  "TOKEN_IDENTIFIER", "TOKEN_STRING", "TOKEN_NUMBER",
  "TOKEN_PIN","TOKEN_FALSE", "TOKEN_TRUE", "TOKEN_FUNCALL",
  "TOKEN_OUTPUT","TOKEN_ENDL",
 // Keywords.
"TOKEN_INFINITE",   "TOKEN_FUN", 
 "TOKEN_VOID", "TOKEN_LEFT_BLOCK", "TOKEN_RIGHT_BLOCK",

"TOKEN_BREAK", "TOKEN_CONTINUE",  "TOKEN_SWITCH", 
 "TOKEN_CASE", "TOKEN_DEFAULT", "TOKEN_ELSE",
 "TOKEN_FOR", "TOKEN_IF", "TOKEN_NIL", 
 "TOKEN_PRINT", "TOKEN_RETURN", "TOKEN_WHILE", "TOKEN_HYPHEN",
 "TOKEN_ERROR", "TOKEN_COUT","TOKEN_GOTO", "TOKEN_EOF"
};

void writeFile(string Data){
    int n = Data.length();
    char char_array[n + 1];
    strcpy(char_array, Data.c_str());

    FILE *Files ;
    if(deleteOld == 0){
        deleteOld++;
        if (Files = fopen("tokens.txt", "r")) {
            fclose(Files);
            if (remove("tokens.txt") == 0){            
            }
        }
    }
    Files = fopen("tokens.txt", "a");
	fputs(char_array, Files);
    fputs("\n", Files);
	fclose(Files);
}
void makeToken(TokenType type){
    string code =   "("+to_string(lineNo)+").  " + tokenNames[type] +"--"+ to_string(line); 
    lineNo++;   
    writeFile(code); 
    sendToparser(type, line); 
    previousToken = type; 
}
void makeToken(string tokenData, TokenType type){
    if(tokenData==""){tokenData = " ";}
    string code = "("+to_string(lineNo)+").  " +tokenNames[type] +"--"+ to_string(line) +"--"+ tokenData;
    lineNo++;
    writeFile(code); 
    sendToparser(type, line,  tokenData);
    previousToken = type;        
}
static bool end_of_file(){
    if(token[0] == '\0'){
        return true;
    };    
    return false;
}
static void skipToken(char A){

    while((!end_of_file())&& !(token[0] == A)){
        token++;
    }
    if(end_of_file()){
        error("Unexpected end of file on line", line);
    }
}
static void skipToken(char A, char B){
    while((!end_of_file())&& !((token[0]== A)&&(token[1] == B))){
        token++;
    }token++;  
        if(end_of_file()){            
        error("Unexpected end of file on line", line);
    }else{token = token + 2;
        }
    
}
static void skip_whitespace(){

    switch(*token){
        case ' ':
            token++;
            skip_whitespace();
            break;
        case '\r':
            token++;
            skip_whitespace();
            break;
        case '\t':
            token++;
            skip_whitespace();
            break;            
        case '\n':
            token++;
            line++;
            skip_whitespace();
            break;
        case '/':
            if(token[1] == '/'){
                token++;
                skipToken('\n');
                line++;
            }else if(token[1] == '*'){
                token++;
                skipToken('*', '/');
                line++;
            }else{makeToken(TOKEN_SLASH);
            token++;};
            skip_whitespace();
            break;
        default:
            break;
    }
}
static bool isNum(char B){
    int A = B - '0';
    if(A >= 0 && A <= 9 ){
        return true;
    }else{ return false;}
}
static bool isAlphabet(char A){
    if((A >= 'a' && A <= 'z') || (A >= 'A' && A <= 'Z') || A == '_'){
        return true;
    }else{ return false;}
};
static void generateToken(TokenType type){
    string tokenData = "";
    if(type == TOKEN_NUMBER){
        while((!end_of_file())&&(isNum(*token))){
            tokenData = tokenData + token[0]; 
            token++;
        }
        token--;
        if(end_of_file()){
            error("Unexpected end of file on line ", line);
        } else {
            makeToken(tokenData, TOKEN_NUMBER);
        }           
    }else
    if(type == TOKEN_INFINITE){
        tokenData = "1";
        makeToken(tokenData, TOKEN_NUMBER);    
    }else 
    if(type == TOKEN_STRING){      
        while((!end_of_file())&&(*token != '"')){
            tokenData = tokenData + token[0]; 
            token++;
        }
        if(token[0] == '"'){
            makeToken(tokenData, TOKEN_STRING);
        }
        else{
            error("expecting double quotes on line", line);
        }
    }
    else 
    if(type == TOKEN_IDENTIFIER){      
      while((!end_of_file())&&(isNum(*token)||isAlphabet(*token)||*token == '_'))
      {     tokenData = tokenData + token[0]; 
            token++; 
        }token--;          
        if(end_of_file()){
            error("Unterminated identifier on line", line);
        }
        else 
        if((token[1] == '(')
        &&((previousToken == TOKEN_VAR)||(previousToken == TOKEN_BOOL)
        ||( previousToken == TOKEN_VOID)||(previousToken == TOKEN_PIN)
        ||(previousToken == TOKEN_STRING))){
            makeToken(tokenData, TOKEN_FUN);
        }
        else
        if(token[1] == '('){
            makeToken(tokenData, TOKEN_FUNCALL);
        }else {
            makeToken(tokenData, TOKEN_IDENTIFIER); 
        }                                           
    }  
}
static bool checkKeyword(string charValue, int numChar, TokenType type){
    if(!(isAlphabet(token[numChar+1])) && !(isNum(token[numChar+1]))){

        for(int i = numChar; i > 0; i--){ 
                    
            if(charValue[i-1] != token[i]){

                return false;
            }
        }
        if(type ==TOKEN_ENDL){
            makeToken("'/n'",TOKEN_STRING);
        }else        
        {
        makeToken(type); 
        }return true;
    }return false;
}
static void keywordToken(){
    switch(*token){
        case 'b':
            if(checkKeyword( "reak", 4, TOKEN_BREAK)){token = token+4;}
            else if(checkKeyword("ool", 3, TOKEN_BOOL)){token = token+3;}              
            else{generateToken(TOKEN_IDENTIFIER);}
            break;
        case 'c':
            if(checkKeyword("ase", 3, TOKEN_CASE)){token = token+3;}
            else if(checkKeyword("out", 3, TOKEN_COUT)){token = token+3;}
            else if(checkKeyword("ontinue", 7, TOKEN_CONTINUE)){token = token+7;}                           
            else{generateToken(TOKEN_IDENTIFIER);}
            break;
        case 'd':
            if(checkKeyword("efault", 6, TOKEN_DEFAULT)){token = token+6;}
            else{generateToken(TOKEN_IDENTIFIER);}
            break;            
        case 'e':
            if(checkKeyword("lse", 3, TOKEN_ELSE)){token = token+3;}
            else if(checkKeyword("ndl", 3, TOKEN_ENDL)){token = token+3;}
            else{generateToken(TOKEN_IDENTIFIER);}            
            break;
        case 'f':
            if(checkKeyword("or", 2, TOKEN_FOR)){
                token = token+2;}              
            else if(checkKeyword("alse", 4, TOKEN_FALSE)){
                token = token+4;}
            else{generateToken(TOKEN_IDENTIFIER);}          
            break;
        case 'g':
            if(checkKeyword("oto", 3, TOKEN_GOTO)){
                token = token+3;}              
            else{generateToken(TOKEN_IDENTIFIER);}          
            break;
        case 'i':
            if(checkKeyword("f", 1, TOKEN_IF)){token++;}
            else
            if(checkKeyword("nt", 2, TOKEN_VAR)){
                token = token+ 2;}
            else{generateToken(TOKEN_IDENTIFIER);}          
            break;            
        case 'n':
            if(checkKeyword("il", 2, TOKEN_NIL)){token = token+2;}
            else{generateToken(TOKEN_IDENTIFIER);}
            break;
        case 'p':
            if(checkKeyword("rint", 4, TOKEN_PRINT)){
                token = token+4;}
            else if(!checkKeyword("in", 2, TOKEN_PIN)){
                    generateToken(TOKEN_IDENTIFIER);}               
            break;
        case 'r':
            if(checkKeyword("eturn", 5, TOKEN_RETURN)){token = token+5;}
            else{generateToken(TOKEN_IDENTIFIER);}
            break;
        case 's':
            if(checkKeyword("witch", 5, TOKEN_SWITCH)){
                token = token+5;
            }else            
            if(checkKeyword("tring", 5, TOKEN_STRINGVAR)){  
                token = token+5;}           
            else{generateToken(TOKEN_IDENTIFIER);}
            break;            
        case 't':
            if(checkKeyword("rue", 3, TOKEN_TRUE)){token = token+3;}
            else{generateToken(TOKEN_IDENTIFIER);}
            break;
        case 'w':
            if(checkKeyword("hile", 4, TOKEN_WHILE)){token = token+4;}
            else{generateToken(TOKEN_IDENTIFIER);}
            break;
        case 'v':
            if(checkKeyword("oid", 3, TOKEN_VOID)){token = token+3;}
            else{generateToken(TOKEN_IDENTIFIER);}
            break;            
        default:
            generateToken(TOKEN_IDENTIFIER);
            break;
    }
}
static void checkField(int type){
    switch(type){
        case 1:
        if(parenCount == 0){error("Unexpected ')' on line ", line);}
        break;
        case 2:
        if(parenCount != 0){error("Unexpected '{' on line ", line);}
        break;        
        case 3:
        if(braceCount == 0){error("Unexpected '}' on line ", line);}
        if(parenCount != 0){error("Unexpected '}' on line ", line);} 
        break; 
    }
}
static void charToken(){
    if(end_of_file()){
        return;}
    switch(*token){
        case '(':
        checkField(0);
        parenCount++;
        makeToken(TOKEN_LEFT_PAREN);
        break;
        case ')':
        checkField(1);
        parenCount--;
        makeToken(TOKEN_RIGHT_PAREN);
        break;
        case '{':
        checkField(2);
        braceCount++;
        makeToken(TOKEN_LEFT_BRACE);
        break;
        case '}':
        checkField(3);
        makeToken(TOKEN_RIGHT_BRACE);
        braceCount--;
        break;
        case ';':
        if(token[1] == ';'){
        generateToken(TOKEN_INFINITE);
        token++;
        }else{      
        makeToken(TOKEN_SEMICOLON);}
        break;
        case ',':
        makeToken(TOKEN_COMMA);
        break;
        case '.':
        makeToken(TOKEN_DOT);
        break;
        case '-':
        if(token[1] == '+'){
        makeToken(TOKEN_SUB);
        token++;
        }else
        if(token[1] == '-'){
        makeToken(TOKEN_SUB_SELF);
        token++; 
        }else
        if(token[1] == '='){
        makeToken(TOKEN_SUB_ASS);
        token++;                         
        }else{
        makeToken(TOKEN_SUB);
        }          
        break;
        case '+':
        if(token[1] == '+'){
        makeToken(TOKEN_ADD_SELF);
        token++;
        }else
        if(token[1] == '-'){
        makeToken(TOKEN_SUB);
        token++; 
        }else
        if(token[1] == '='){
        makeToken(TOKEN_ADD_ASS);
        token++;                          
        }else{
        makeToken(TOKEN_PLUS);
        }          
        break;
        case '/':
        if(token[1] == '='){
        makeToken(TOKEN_SLASH_ASS);
        token++;                          
        }else{
        makeToken(TOKEN_SLASH);
        }          
        break;        
        case '*':
        if(token[1] == '='){
        makeToken(TOKEN_STAR_ASS);
        token++;                          
        }else{
        makeToken(TOKEN_STAR);
        }          
        break;  
        case ':':
        makeToken(TOKEN_COLON);
        break;
        case '?':
        makeToken(TOKEN_TENARY);
        break; 
        case '^':
        if(token[1] == '='){
            makeToken(TOKEN_ASS_XOR);
            token++;
        }else{
            makeToken(TOKEN_BIT_XOR);
        }  
        case '~':
        makeToken(TOKEN_INVERSE);
        break;
        case '%':
        makeToken(TOKEN_MODULUS);
        break;
        case '[':
        makeToken(TOKEN_LEFT_BLOCK);
        break;
        case ']':
        makeToken(TOKEN_RIGHT_BLOCK);
        break;
        case '"':
        token++;
        generateToken(TOKEN_STRING);
        break;
        case '!':
        if(token[1] == '='){
            makeToken(TOKEN_BANG_EQUAL);
            token++;
        }else{
            makeToken(TOKEN_BANG);
        }          
        break;
        case '&':
        if(token[1] == '&'){
            makeToken(TOKEN_LOGICAL_AND);
            token++;
        }else if(token[1] == '='){
            makeToken(TOKEN_ASS_AND);
            token++;                  
        }else{
            makeToken(TOKEN_BIT_AND);
        }          
        break;  
        case '|':
        if(token[1] == '|'){
            makeToken(TOKEN_LOGICAL_OR);
            token++;
        }else if(token[1] == '='){
            makeToken(TOKEN_ASS_OR);
            token++;                  
        }else{
            makeToken(TOKEN_BIT_OR);
        }          
        break;                        
        case '=':
        if(token[1] == '='){
            makeToken(TOKEN_EQUAL_EQUAL);
            token++;
        }else{
            makeToken(TOKEN_EQUAL);
        }          
        break;
        case '<':
        if(token[1] == '='){
            makeToken(TOKEN_LESS_EQUAL);
            token++;
        }else if(token[1] == '<'){
            makeToken(TOKEN_SHIFT_LEFT);
            token++;                
        }else{
            makeToken(TOKEN_LESS);
        }          
        break;
        case '>':
        if(token[1] == '='){
            makeToken(TOKEN_GREATER_EQUAL);
            token++;
        }else if(token[1] == '>'){
            makeToken(TOKEN_SHIFT_RIGHT);
            token++;                  
        }else{
            makeToken(TOKEN_GREATER);
        }          
        break;  
        default:
        skip_whitespace();                   
    }
}
void scanner(char* buffer){

    token = buffer;

    while(!end_of_file()){
        
        skip_whitespace();    
        if(isNum(*token)){         
            generateToken(TOKEN_NUMBER);
        }else 
        if(isAlphabet(*token)){
            keywordToken();
        }else{
            charToken();
        }
        token++;
    }
    if(end_of_file()){
        makeToken(TOKEN_EOF);
        }

}



