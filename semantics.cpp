#include <bits/stdc++.h>
#include "parser.h"
#include "semantics.h"
using namespace std;

int deleteold = 0;
int deleteOnce = 0;
int deleteCnt = 0;
int forCount   = 0;
int swCase     = 0;
int funArgc = 0;
int tenary = 0;
string Fname = "";
string track_fname="";
int LvIndex = 0;
int lIndex = 0;
SymbolTable st;
stack<string> scopeCount;
stack<tokenClass*> char_stack;
int scopeDepth;
int tempCount;

string saveG[12]       = {"g8", "g9", "g18", "g19", "g20", "g21", "g22", "g23", "g24", "g25", "g26", "g27"};
string argV[8]         = {"x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17"};
string saveV[12]       = { "x9", "x18", "x19", "x20", "x21", "x22", "x23", "x24", "x25", "x26", "x27"};
string tempV[7]        = {"x5", "x6", "x7", "x28", "x29", "x30", "x31"};
string instruction[19] = { "add","sub", "div","mod", "mul", "or", "xor", "and", "or",
                         "xor", "and", "sll", "srl", "not", "add", "sub", "div", "mod", "mul"};
string branch[9]       = { "ld", "0", "0", "bne","ble", "blt", "bge", "bgt", "beq"};
TokenType changeSign[9]={TOKEN_EQUAL,TOKEN_LOGICAL_AND,TOKEN_LOGICAL_OR, TOKEN_BANG_EQUAL,TOKEN_LESS_EQUAL, 
                            TOKEN_LESS, TOKEN_GREATER_EQUAL, TOKEN_GREATER, TOKEN_EQUAL_EQUAL};

bool expect(TokenType typeA, TokenType typeB, TokenType typeC, TokenType typeD, int count){
    if((typeA == st.tokenTable[count]->type)&&(typeB ==st.tokenTable[count+1]->type )
    &&(typeC ==st.tokenTable[count+2]->type )&&(typeD ==st.tokenTable[count+3]->type )){
        return true;
    }
    return false;}
bool expect(TokenType typeA, TokenType typeB, TokenType typeC, int count){
    if((typeA == st.tokenTable[count]->type)&&(typeB ==st.tokenTable[count+1]->type )
    &&(typeC ==st.tokenTable[count+2]->type )){
        return true;
    }
    return false;}
bool expect(TokenType typeA, TokenType typeB, int count){
    if((typeA == st.tokenTable[count]->type)&&(typeB ==st.tokenTable[count+1]->type )){
        return true;
    }
    return false;}
bool expect(TokenType typeA, int count){
    if(typeA == st.tokenTable[count]->type){
        return true;
    }
    return false;}
    
void error(string error, int lineNo){
    cout<<error<<" "<<lineNo<<endl;
    exit(1);}

static void codeGenerator(string write){
    int n = write.length();
    char char_array[n + 1];
    strcpy(char_array, write.c_str());

    FILE *Files ;
    if(deleteold == 0){
        deleteold++;
        if (Files = fopen("compile.txt", "r")) {
            fclose(Files);
            if (remove("compile.txt") == 0){
                
            }
        }
    }
    
    Files = fopen("compile.txt", "a");
	fputs(char_array, Files);
    fputs("\n", Files);
	fclose(Files);

}

static void assignReg(int pointer){
    
    if(st.indexE > 1){
        int startV = st.funcTable[st.indexE-1]->Vs;        
        int endV   = st.indexV; 
        int startA = st.funcTable[st.indexE-1]->As;
        int endA   = st.indexA;
        while(startV < endV){
            if(st.localVariable[startV]->Vname == st.tokenTable[pointer]->identifier)
            {   st.tokenTable[pointer]->regName = saveV[st.localVariable[startV]->vNo];
                st.tokenTable[pointer]->Vtype   = st.localVariable[startV]->vType;
                return;
            }startV++;
        }
        while(startA < endA){
            if(st.ArgumentTable[startA]->argName == st.tokenTable[pointer]->identifier){
                st.tokenTable[pointer]->regName  =  argV[5+st.ArgumentTable[startA]->argNo];
                st.tokenTable[pointer]->Vtype    =  st.ArgumentTable[startA]->argType;
                return;
            }startA++;
        }        
    }
    int gvCount= st.indexG-1;
    while(gvCount >= 0){
        if(st.globalVariable[gvCount]->Gname == st.tokenTable[pointer]->identifier){
           st.tokenTable[pointer]->regName   =  saveG[gvCount];
           st.tokenTable[pointer]->Vtype     =  st.globalVariable[gvCount]->vType;
           return;}
        gvCount-- ;
    }     
    error("Cannot recognize variable on line", st.tokenTable[pointer]->lineNo);
        
}

static void alu(int pointer){
    tokenClass* token1 = st.expressList[pointer];    
    tokenClass* token2 = st.expressList[pointer+1]; 
    
    if((token1->type == TOKEN_STRING)||(token1->type == TOKEN_STRINGVAR)){
        error("A string variable is not expected in the expression on line", st.expressList[pointer]->lineNo);
    }
    switch(token2->type){
        case TOKEN_ADD_SELF:
            if(token1->Vtype == TOKEN_VAR){
                codeGenerator("addi  "+token1->regName+"  "+token1->regName+"  1");
            }else{error("Invalid expression on line", st.expressList[pointer]->lineNo);}
        break;
        case TOKEN_SUB_SELF:
            if(token1->Vtype == TOKEN_VAR){
                codeGenerator("addi  "+token1->regName+"  "+token1->regName+"  -1");
            }else{error("Invalid expression on line", st.expressList[pointer]->lineNo);}
        break;
        default:
        error("invalid unary expression on line",  st.expressList[pointer]->lineNo);

    }    
}

static void aluB(int pointer){
    tokenClass* token1 = st.expressList[pointer];    
    tokenClass* token2 = st.expressList[pointer+1];    
    tokenClass* token3 = st.expressList[pointer+2];
    string printfunc;
    string command;
    string storeReg;
    string RS1;
    string RS2;
    
    if((token1->type == TOKEN_IDENTIFIER)||(token1->type == TOKEN_OUTPUT)){
        RS2 = token1->regName;
    }else
    if(token1->type == TOKEN_NUMBER){
        RS2 = token1->identifier;
    }    
    if(token1->type == TOKEN_STRING){
        RS2 = "STR"+to_string(scopeDepth);
        scopeDepth++;
    }
    if((token2->type == TOKEN_IDENTIFIER)||(token2->type == TOKEN_OUTPUT)){
        RS1 = token2->regName;
    }else
    if(token2->type == TOKEN_NUMBER){
        codeGenerator("ldi  "+tempV[tempCount] + " "+token2->identifier);
        RS1 = tempV[tempCount];
        tempCount++;
    }else
    if(token2->type == TOKEN_STRING){
        codeGenerator("ldi  "+tempV[tempCount] + "   STR"+to_string(scopeDepth));
        RS1 = tempV[tempCount];
        tempCount++;
    }
    if(token2->type != TOKEN_IDENTIFIER)
    {   storeReg = RS1;
    }else
    if(token1->type == TOKEN_OUTPUT){
        storeReg = RS2;
    }else
    {   storeReg = tempV[tempCount];
        tempCount++;
    }  

    if(token3->type == TOKEN_EQUAL)
    {   command = branch[token3->type];
        if((token1->type == TOKEN_STRING)||(token1->type == TOKEN_NUMBER))
        { command = command + "i";   
        }
        codeGenerator(command+"  "+ token2->regName +"  "+RS2);
       
    }else 
    if(token3->type <= TOKEN_BANG_EQUAL)
    {   command = branch[token3->type];

    if(scopeCount.top().find("*") != string::npos){  
        printfunc = scopeCount.top();
        printfunc.erase(0, 5); 
        }else{
            printfunc = scopeCount.top();
        }
        codeGenerator(command+"   "+RS1+"   "+RS2+" "+printfunc+"_exit");
    }else
    if(token3->type <= TOKEN_ASS_AND)
    {   command = instruction[token3->type - TOKEN_TENARY];
        if(token1->type == TOKEN_NUMBER)
            { command = command + "i"; }    
        codeGenerator(command+"   "+ token2->regName +"   "+RS1+"   "+RS2);          
    }else
    if(token3->type <= TOKEN_STAR)
    {   command = instruction[token3->type - TOKEN_TENARY];
        if(token1->type == TOKEN_NUMBER)
            { command = command + "i";   }
        codeGenerator(command+"   "+storeReg+"   "+RS1+"   "+RS2);    
        st.expressList[pointer]->type    = TOKEN_OUTPUT;
        st.expressList[pointer]->regName = storeReg;            
    }   

    if(token1->type == TOKEN_STRING){     
        codeGenerator(".data"); 
        codeGenerator("STR"+to_string(scopeDepth)+":");
        codeGenerator("asciiz'"+token1->identifier+" ' "); 
        codeGenerator(".text");
        scopeDepth++;        
    } 
}

static int checkBang(int start, int end){
    int operand = 0;
    for(int x = end; x < start; x++){ 
        if(st.expressList[x]->type == TOKEN_BANG)
        {  
            for(int i = x-1; i>= 0; i--)
            {
                if((st.expressList[i]->type == TOKEN_NUMBER)||(st.expressList[i]->type == TOKEN_IDENTIFIER)
                ||(st.expressList[i]->type == TOKEN_FUNCALL))
                {   operand++;
                    if(st.expressList[i]->Vtype == TOKEN_BOOL)
                    {   codeGenerator("xor  "+tempV[tempCount]+"  "+st.expressList[i]->regName+"   xFFFF");
                        st.expressList[i]->regName = tempV[tempCount];
                        tempCount++;
                        st.expressList[i]->type = TOKEN_OUTPUT;                               
                    }
                }else 
                if(st.expressList[i]->type <= TOKEN_STAR)
                {   operand--;
                    if(st.expressList[i]->type <= TOKEN_BANG_EQUAL){
                        st.expressList[i]->type = changeSign[st.expressList[i]->type];
                    }   
                }  
                if(operand == 1){
                    break;
                }
            }                 
            for(int n = x; n < st.indexL;  n++){
                st.expressList[n] = st.expressList[n+1];}          
            start--; 
            st.indexL--;                                   
        }
    }
    return(start);     
}

static void readExpressList(int start, int end);

static int checkLand(int start, int end){
    int operand = 0;
    int secondC = 0; 
    int cnt     = 0;  
    for(int x = start-1; x>= end; x--)
    {
        if(st.expressList[x]->type == TOKEN_LOGICAL_AND)
        {
            for(int n = x; n < st.indexL;  n++){
                st.expressList[n] = st.expressList[n+1];}          
            start--; 
            st.indexL--;  
            for(int i = x-1; i>= end; i--)
            {
                if(st.expressList[i]->type > TOKEN_STAR){
                    operand++;
                }else if(st.expressList[i]->type <= TOKEN_STAR){
                    operand--;
                }
                if(operand == 1){
                    secondC++;
                    operand = 0;
                    if(deleteOnce == 0){
                        deleteOnce = 1;
                        readExpressList(x, i);
                        deleteOnce = 0;
                        cnt = x - i;                         
                        for(int o = i; o < st.indexL - cnt;  o++){
                            st.expressList[o] = st.expressList[o + cnt];}                      
                        start -=     cnt;
                        st.indexL -= cnt;  
                    }else
                    {
                    readExpressList(x, i);                       
                    }                      
          
                    x = i;                              
                } 
                if(secondC == 2){
                    break;
                }                                 
            }
        }
    } return(start);               
}

int  checkLor(int start, int end){

    int operand = 0;
    int secondC = 0; 
    int cnt     = 0; 
    string scope = "";
    string printfunc = "";
    for(int x = start-1; x>= end; x--)
    {
        if(st.expressList[x]->type == TOKEN_LOGICAL_OR)
        {
            for(int n = x; n < st.indexL;  n++){
                st.expressList[n] = st.expressList[n+1];}          
            start--; 
            st.indexL--;  
            for(int i = x-1; i>= end; i--)
            {
                if(st.expressList[i]->type > TOKEN_STAR){
                    operand++;
                }else if(st.expressList[i]->type <= TOKEN_STAR){
                    operand--;
                }
                if(operand == 1){
                    secondC++;
                    operand = 0;
                    if(secondC == 1){
                        scopeCount.push(scopeCount.top()+"_"+to_string(scopeDepth)); 
                        scopeDepth++;                       
                    }
                    if(deleteCnt == 0){
                        deleteCnt = 1;
                        readExpressList(x, i);
                        deleteCnt = 0;
                        cnt = x - i;                         
                        for(int o = i; o < st.indexL - cnt;  o++){
                            st.expressList[o] = st.expressList[o + cnt];}                      
                        start -=     cnt;
                        st.indexL -= cnt;  
                    }else
                    {
                    readExpressList(x, i);                       
                    } 
                    x = i;                                            
                    if(secondC == 1){
                    printfunc = scopeCount.top();     
                    printfunc.erase(0, 5);  
                    codeGenerator("beq   x0    0  "+ printfunc); 
                    codeGenerator( printfunc+"_exit:"); 
                    scopeCount.pop(); 
                    }else
                    if(secondC == 2){
                        codeGenerator(printfunc+":");
                        break;
                    }                     
                }                
            }
        }
    }return(start);   
            
}

static void readExpressList(int start, int end){

    start = checkBang(start, end); 
    start = checkLand(start, end);
    start = checkLor(start, end);

    tempCount = 0;
    for(int i = end; i< start-1; i++){
       
       if((st.expressList[i]->type > TOKEN_SEMICOLON)
       &&((st.expressList[i+1]->type == TOKEN_ADD_SELF)||(st.expressList[i+1]->type == TOKEN_SUB_SELF))){        
            alu(i);             
            for(int n = i+1; n < start-1;  n++){
                st.expressList[n] = st.expressList[n+1];
            }          
            start--;
            st.indexL--;
            i = end;
        }else 
        if(start > 2){
            if((st.expressList[i]->type > TOKEN_SEMICOLON)
            &&(st.expressList[i+1]->type > TOKEN_SEMICOLON)&&(st.expressList[i+2]->type < TOKEN_BANG)){ 

            aluB(i);
            if(st.expressList[i+2]->type <= TOKEN_ASS_AND){
                for(int n = i; n < start-3;  n++){
                     st.expressList[n] = st.expressList[n+3];
                } 
                start -=3;
                st.indexL -=3;
            }else{
                for(int n = i+1; n < start-2;  n++){
                    st.expressList[n] = st.expressList[n+2];} 
                start -=2;
                st.indexL -=2;
                } i = end; 
            } 
        }                  
    }   
    if((start - end == 1)
    &&((st.expressList[end]->Vtype == TOKEN_BOOL)
    ||(st.expressList[end]->identifier == "0" && st.expressList[end]->identifier == "1"))
    ){  string printfunc = scopeCount.top();
        printfunc.erase(0, 5);
        codeGenerator("ble   "+st.expressList[end]->regName+"   x0   "+printfunc+"_exit");
        start--;
        st.indexL--;
    }
    
    int check = start - end;

    if(check > 1){
        readExpressList(start , end);
    }  
}

int expression(int pointer, TokenType express){
    st.indexL = 0;
    int braces = 0;
    int pointerStart = pointer;  
    while(!(expect(express, pointer))||(braces != 0)){

      if((st.tokenTable[pointer]->type  > TOKEN_OUTPUT)
      ||(st.tokenTable[pointer]->type  == TOKEN_SEMICOLON)){
            error("invalid syntax in line", st.tokenTable[pointer]->lineNo);
        }else    
        if(st.tokenTable[pointer]->type  == TOKEN_EOF){
            error("Unexpected end of file on line", st.tokenTable[pointer-2]->lineNo);
        }else 
        if((expect(TOKEN_LEFT_PAREN, pointer))){
            braces++;
        }else
        if((expect(TOKEN_RIGHT_PAREN, pointer))){
            braces--;
        }else
        if((expect(TOKEN_TENARY, pointer))){
            scopeCount.push("*ten_"+to_string(scopeDepth));
            scopeDepth++; 
            tenary = pointerStart;
            pointerStart = pointerStart + 2;                  
            break;
        }else
        if((expect(TOKEN_FUNCALL, pointer))){         
            c_tokenfuncall(pointer); 

        }else
        if(
        ((st.tokenTable[pointer+1]->type > TOKEN_END)&&(st.tokenTable[pointer]->type > TOKEN_END))
        ||(st.tokenTable[pointer]->type > TOKEN_INFINITE)
        ){
            error("Invalid token expression in line", st.tokenTable[pointer]->lineNo);
              
        }        
        if((expect(express, pointer))&&(braces == 0)){
            break;
        }
      
        pointer++;
    }
    int pointerEnd = pointer; 
     pointerEnd--;  

    while(pointerEnd >= pointerStart){

        if((st.tokenTable[pointerEnd]->type == TOKEN_STRING) &&(!expect(TOKEN_SEMICOLON, pointerEnd+1))
        &&(!expect(TOKEN_EQUAL, pointerEnd-1)))
        {
            error("Invalid string expression on line", st.expressList[pointerEnd]->lineNo);
        }

        if(expect(TOKEN_START, pointerEnd)){ 
            while(!expect(TOKEN_END, pointerEnd)){
                pointerEnd--;
            }pointerEnd--;
            st.expressList[st.indexL] = st.tokenTable[pointerEnd];
            st.indexL++;
        }else     
        if((expect(TOKEN_STRING, pointerEnd))
        ||(expect(TOKEN_IDENTIFIER, pointerEnd))||(expect(TOKEN_NUMBER, pointerEnd))){
            if((expect(TOKEN_RIGHT_PAREN, pointerEnd-1))||(expect(TOKEN_LEFT_PAREN, pointerEnd+1))){ 
                if(!(expect(TOKEN_RIGHT_PAREN, pointerStart-1))&& !(expect(TOKEN_LEFT_PAREN, pointer+1))){                 
                    char_stack.push(new tokenClass(TOKEN_STAR, 0, 0));
                }
            }
            if(st.tokenTable[pointerEnd]->type == TOKEN_IDENTIFIER){
            assignReg(pointerEnd); 
            }
            st.expressList[st.indexL] = st.tokenTable[pointerEnd];
            st.indexL++;
            
        }else  
        if((expect(TOKEN_FALSE, pointerEnd))||(expect(TOKEN_TRUE, pointerEnd))){
            if(expect(TOKEN_FALSE, pointerEnd)){
                st.expressList[st.indexL] = new tokenClass(TOKEN_NUMBER, 0, "0");
                st.indexL++;
            }else{
                st.expressList[st.indexL] = new tokenClass(TOKEN_NUMBER, 0, "1");
                st.indexL++;
            }
        }else 
        if(expect(TOKEN_RIGHT_PAREN, pointerEnd)){
            char_stack.push(st.tokenTable[pointerEnd]);

        }else 
        if (expect(TOKEN_LEFT_PAREN, pointerEnd)) {
			while (!(char_stack.top()->type == TOKEN_RIGHT_PAREN)) {
                st.expressList[st.indexL] = char_stack.top();      
                st.indexL++;
				char_stack.pop();
                if(char_stack.empty())                        
                    error("Missing ')' in expression on line", st.tokenTable[pointerEnd]->lineNo);                
			}
			char_stack.pop();
		}else                 
        if((st.tokenTable[pointerEnd]->type == TOKEN_PLUS)
        &&(st.tokenTable[pointerEnd-1]->type < TOKEN_BANG)){

        }else
        if((st.tokenTable[pointerEnd]->type == TOKEN_SUB)
        &&(st.tokenTable[pointerEnd-1]->type < TOKEN_BANG)){
                st.expressList[st.indexL] = new tokenClass(TOKEN_NUMBER, 0, "0");
                st.indexL++;
                st.expressList[st.indexL] = new tokenClass(TOKEN_SUB, 0, 0);
                st.indexL++;                
        }else  
        if (st.tokenTable[pointerEnd]->type < TOKEN_LEFT_PAREN){ 
            if(!char_stack.empty()){
                while ((st.tokenTable[pointerEnd]->type <= char_stack.top()->type)
                &&(char_stack.top()->type != TOKEN_RIGHT_PAREN))
                {
                    st.expressList[st.indexL] = char_stack.top();
                    st.indexL++;
                    char_stack.pop();
                    if(char_stack.empty())                        
                        break;
                }
                char_stack.push(st.tokenTable[pointerEnd]);
                
            }else{
                char_stack.push(st.tokenTable[pointerEnd]);
                }
		}              
        pointerEnd--;
    }
   
    while(!char_stack.empty()){
        if(char_stack.top()->type != TOKEN_RIGHT_PAREN){ 
        st.expressList[st.indexL] = char_stack.top();
        st.indexL++;
        }
		char_stack.pop();
	} 
   
    if(funArgc > 0){ 
        funArgc--;
        st.expressList[st.indexL]  = new tokenClass(TOKEN_IDENTIFIER,  0, "0");
        st.expressList[st.indexL]->regName = argV[funArgc];
        st.indexL++;
        st.expressList[st.indexL]  = new tokenClass(TOKEN_EQUAL, 0);
        st.indexL++;
         
    }          
    readExpressList(st.indexL, 0);
    pointer++;  
    return(pointer);
}

int c_rightBrace(int pointer){
    pointer++; 
    if(scopeCount.top().find("*fun_") != string::npos){ 
        string printfunc = scopeCount.top();
        printfunc.erase(0, 5);
        codeGenerator(printfunc+"_exit:");       
        scopeCount.pop();   
        int NoLocalVariables = st.funcTable[st.indexE -1]->Vn;
        int vCnt = ( NoLocalVariables * 16 ) +  32;
        int cnt =0;
        while(vCnt > 32)
        {
            vCnt -= 16;
            codeGenerator("ld    "+saveV[cnt]+",  "+to_string(vCnt)+"(sp)");  
            cnt++; 
        }
        codeGenerator("ld    x8,  16(sp)"); 
        codeGenerator("ld    x1,  0(sp)"); 
        codeGenerator("addi  sp   sp   "+to_string(NoLocalVariables)); 
        codeGenerator("jalr  x0,   0(x1)");     
    }else
    if(scopeCount.top().find("*whi") != string::npos){  
        string printfunc = scopeCount.top();
        printfunc.erase(0, 5);                 
        codeGenerator("beq  x0   x0,  "+printfunc);  
        codeGenerator(printfunc+"_exit");
        scopeCount.pop();
    }else          
    if(scopeCount.top().find("*for") != string::npos){
        st.tokenTable[forCount-1] = new tokenClass(TOKEN_LEFT_PAREN, 0, 0);
        expression(forCount-1, TOKEN_LEFT_BRACE);
        string printfunc = scopeCount.top();
        printfunc.erase(0, 5); 
        codeGenerator("beq x0   x0,  "+printfunc);                                  
        codeGenerator(printfunc+"_exit:");
        scopeCount.pop();
    }else
    if((scopeCount.top().find("*ifs") != string::npos)&& (expect(TOKEN_ELSE, pointer))){
        string printfunc = scopeCount.top();
        printfunc.erase(0, 5);     
        scopeCount.pop();

        scopeCount.push("*ifs_"+to_string(scopeDepth));
        scopeDepth++;
        string printEL = scopeCount.top();
        printEL.erase(0, 5); 
        codeGenerator("beq  x0  x0,  "+printEL+"_exit"); 
        codeGenerator(printfunc+"_exit:");            
        return(pointer);         
    }else    
    if((scopeCount.top().find("*ifs") != string::npos)&& !(expect(TOKEN_ELSE, pointer))){
        string printfunc = scopeCount.top();
        printfunc.erase(0, 5);          
        codeGenerator(printfunc+"_exit:"); 
        scopeCount.pop(); 
    }else
    if(scopeCount.top().find("*elf") != string::npos){
        string printfunc = scopeCount.top();
        printfunc.erase(0, 5);     
        scopeCount.pop();

        string printEL = scopeCount.top();
        printEL.erase(0, 5); 
        codeGenerator("beq  x0  x0,  "+printEL+"_exit"); 
        codeGenerator(printfunc+"_exit:"); 
        if(!(expect(TOKEN_ELSE, pointer))){
            printfunc = scopeCount.top();
            printfunc.erase(0, 5);   
            codeGenerator(printfunc+"_exit:");               
            scopeCount.pop();   
        }return(pointer);       
    }else      
    if(scopeCount.top().find("*swi") != string::npos){ 
        string printfunc = scopeCount.top();
        printfunc.erase(0, 5);          
        codeGenerator(printfunc+"_exit:"); 
        scopeCount.pop();                        
    }   
    return pointer;
}

int c_tokenfun(int pointer){
    int funcIndex = st.indexE -1; 
    pointer++;
    while(funcIndex > 0){
        if((st.tokenTable[pointer]->identifier == st.funcTable[funcIndex]->Fname)
        &&(st.funcTable[funcIndex]->call == 1)){
            error("function already exists", st.tokenTable[pointer]->lineNo);          
        }
        funcIndex--;   
    }
    Fname = st.tokenTable[pointer]->identifier;
    st.funcTable[st.indexE]  = new funcList(0, Fname, 0, 0, 0, 0);
    st.indexE++; 
    st.funcTable[st.indexE-1]->As = st.indexA;
    pointer +=2;
    int count = pointer;
    int argNo = 0;
    if(st.tokenTable[pointer]->type != TOKEN_RIGHT_PAREN){
        argNo = 1;
        while(!expect(TOKEN_RIGHT_PAREN, count)){
            if(st.tokenTable[count]->type == TOKEN_COMMA){
                argNo++;
            }
            count++;
        }
    }  
    st.funcTable[st.indexE-1]->An = argNo; 
    if(argNo == 0){
        pointer++;
    } 
    while(argNo != 0){
        st.ArgumentTable[st.indexA]  = new ArgumentList(st.tokenTable[pointer]->type,
         st.tokenTable[pointer+1]->identifier, argNo-1);  
        pointer +=3;
        st.indexA++;
        argNo--;
    } 
    if(expect(TOKEN_LEFT_BRACE, pointer)){
        st.funcTable[st.indexE-1]->call = 1;
        scopeCount.push("*fun_"+Fname);

        codeGenerator(Fname+":"); 
        int braces = 1;
        int variable = 0;
        pointer++;
        count =  pointer;
        while(braces != 0){
            if(st.tokenTable[count]->type == TOKEN_LEFT_BRACE){
                braces++;
            }else 
            if(st.tokenTable[count]->type == TOKEN_RIGHT_BRACE){
                braces--;
            }else 
            if(expect(TOKEN_VAR, TOKEN_IDENTIFIER, count) 
            || expect(TOKEN_STRINGVAR, TOKEN_IDENTIFIER, count) 
            || expect(TOKEN_BOOL, TOKEN_IDENTIFIER, count)){
                variable++;   
            }else
            if(st.tokenTable[count]->type == TOKEN_EOF){
                error("Function  "+Fname+"  have incompatible number of braces", 0);
            }
            count++;
        }
        braces = variable;
        variable = (variable * 16) + 32; 

        count = 0;
        codeGenerator("addi  sp,  sp,  -"+to_string(variable));

        while(variable > 32){
            variable -= 16;
            codeGenerator("sd    "+saveV[count]+",  "+to_string(variable)+"(sp)"); 
            count++; 
        }
        codeGenerator("sd    x8,  16(sp)");
        codeGenerator("sd    x1,  0(sp)");
        codeGenerator("addi  fp   sp   "+to_string(braces));

        st.funcTable[st.indexE-1]->Vn = braces;                
    }else
    if(expect(TOKEN_SEMICOLON, pointer)){
        st.funcTable[st.indexE-1]->call = 0;
        pointer++;
    }else{
        error("MISSING ; or { in function "+Fname+ " on line", st.tokenTable[pointer]->lineNo);
        }
    return pointer;            
}

int c_varDeclaration(int pointer){
    if(track_fname != Fname){
        track_fname = Fname;
        LvIndex = 0; 
        lIndex = st.indexV;
        st.funcTable[st.indexE -1]->Vs = lIndex;
    }
    int Lsearch = st.indexV;
    if(!scopeCount.empty()){
    while(Lsearch > lIndex){
        if(st.tokenTable[pointer + 1]->identifier == st.localVariable[Lsearch-1]->Vname ){
            error("variable already declared on line", st.tokenTable[pointer]->lineNo);}
            Lsearch--;
    }
        st.localVariable[st.indexV]  = new lVList(st.tokenTable[pointer]->type,
         st.tokenTable[pointer+1]->identifier, LvIndex);
        LvIndex++;
        st.indexV++;   
        
    }else
    if(scopeCount.empty()){
        int Lsearch = st.indexG;        
        while(Lsearch > 0){
            if(st.tokenTable[pointer+1]->identifier == st.globalVariable[Lsearch-1]->Gname ){
                error("variable already declared on line", st.tokenTable[pointer]->lineNo);}
            Lsearch--;
        }      
        st.globalVariable[st.indexG]  = new gVList(st.tokenTable[pointer]->type,
         st.tokenTable[pointer+1]->identifier);
        st.indexG++;   
    }pointer++;  
      
    if(expect(TOKEN_SEMICOLON, pointer+1)){ 
    pointer = pointer + 2;
    }else
    if(expect(TOKEN_COMMA, pointer+1)){ 
        st.tokenTable[pointer+1] = st.tokenTable[pointer-1];
        pointer = c_varDeclaration(pointer+1);
    }else
    {
        pointer = expression(pointer, TOKEN_SEMICOLON);
    } return pointer;
    
}

int c_while(int pointer){
    int paren = 0;
    pointer++; 
    int count = pointer;
    while(!expect(TOKEN_LEFT_BRACE, count)){
        if(expect(TOKEN_LEFT_PAREN, count)){
            paren++;
        }else
        if(expect(TOKEN_RIGHT_PAREN, count)){
            paren--;
        }
        if(expect(TOKEN_EOF, count)){
            error("Unexpected end of file on line ", st.tokenTable[count]->lineNo );   
        }
        if((paren == 0)&&(!expect(TOKEN_LEFT_BRACE, count+1))){
            error("Missing opening '{' in while loop on line", st.tokenTable[count]->lineNo );       
        } 
        if((paren == 0)&&(expect(TOKEN_LEFT_BRACE, count+1))){
            break;         
        }                 
    count++;
    }    
    codeGenerator("while_"+to_string(scopeDepth)+":");
    scopeCount.push("*whi_"+to_string(scopeDepth));
    scopeDepth++; 
    pointer = expression(pointer, TOKEN_LEFT_BRACE);
    return pointer;
}

int c_forloop(int pointer){
    forCount = 0;
    int paren = 1;
    pointer = pointer + 2;

    if(expect(TOKEN_VAR, TOKEN_IDENTIFIER, pointer)){
        pointer = c_varDeclaration(pointer);
    }else
    if(expect(TOKEN_IDENTIFIER, pointer)){                    
        pointer = expression(pointer, TOKEN_SEMICOLON);
    }else{
        error("Syntax error in for loop in line", st.tokenTable[pointer]->lineNo);
        }
    scopeCount.push("*for_"+to_string(scopeDepth));
        
    codeGenerator("for_"+to_string(scopeDepth)+":");            
    scopeDepth++;
    pointer = expression(pointer, TOKEN_SEMICOLON); 
    forCount = pointer;     
    while(!expect(TOKEN_LEFT_BRACE, pointer)){
        if(expect(TOKEN_LEFT_PAREN, pointer)){
            paren++;
        }else
        if(expect(TOKEN_RIGHT_PAREN, pointer)){
            paren--;
        }
        if((paren == 0) && !(expect(TOKEN_LEFT_BRACE, pointer+1))){
            error("Missing opening '{' in for loop on line", st.tokenTable[pointer]->lineNo );
        }
        if((paren == 0)&&(expect(TOKEN_LEFT_BRACE, pointer+1))){
            break;         
        }         
        pointer++;
    }pointer += 2;     
    return pointer;
}

int c_if(int pointer){
    int paren = 0;
    pointer++;     
    int count = pointer;
    while(!expect(TOKEN_LEFT_BRACE, count)){
        if(expect(TOKEN_LEFT_PAREN, count)){
            paren++;
        }else
        if(expect(TOKEN_RIGHT_PAREN, count)){
            paren--;          
        }
        if((paren == 0)&&(!expect(TOKEN_LEFT_BRACE, count+1))){        
            error("Missing opening '{' in if statement on line", st.tokenTable[count]->lineNo );          
        } 
        if((paren == 0)&&(expect(TOKEN_LEFT_BRACE, count+1))){
            break;         
        }count++;
    }
    scopeCount.push("*ifs_"+to_string(scopeDepth));
    scopeDepth++;                    
    pointer = expression(pointer, TOKEN_LEFT_BRACE);
    return(pointer); 
}

int c_else(int pointer){
    scopeCount.push("*elf_"+to_string(scopeDepth));
    scopeDepth++;
    int paren = 0;
    pointer+=2;     
    int count = pointer;
    while(!expect(TOKEN_LEFT_BRACE, count)){
        if(expect(TOKEN_LEFT_PAREN, count)){
            paren++;
        }else
        if(expect(TOKEN_RIGHT_PAREN, count)){
            paren--;          
        }
        if((paren == 0)&&(!expect(TOKEN_LEFT_BRACE, count+1))){        
            error("Missing opening '{' in if statement on line", st.tokenTable[count]->lineNo );          
        } 
        if((paren == 0)&&(expect(TOKEN_LEFT_BRACE, count+1))){
            break;         
        }count++;
    }                 
    pointer = expression(pointer, TOKEN_LEFT_BRACE);
    return(pointer); 
}

int c_switch(int pointer){
    pointer = pointer + 2;
    swCase = pointer;
    pointer++;
    scopeCount.push("*swi_"+to_string(scopeDepth));
    scopeDepth++;
    if(! expect(TOKEN_RIGHT_PAREN, TOKEN_LEFT_BRACE, TOKEN_CASE, pointer)){ 
        error("Switch expects '){' on line ", st.tokenTable[pointer]->lineNo);}
    pointer = pointer + 2;
    return(pointer);    
}

int c_case(int pointer){ 
    scopeCount.push("*cas_"+to_string(scopeDepth));
    scopeDepth++;
    st.tokenTable[pointer-1] = st.tokenTable[swCase]; 
    st.tokenTable[pointer]  = new tokenClass(TOKEN_EQUAL_EQUAL, 0);     
    pointer = expression(pointer-1, TOKEN_COLON);
    if(expect(TOKEN_LEFT_BRACE, pointer)){
        error("Case statement does not need a '{' on line ", st.tokenTable[pointer]->lineNo);
    } 
    return(pointer);
}

int c_swbreak(int pointer){
    string printfunc = scopeCount.top();
    printfunc.erase(0, 5);     
    if(scopeCount.top().find("*swi") != string::npos){ 
    error("Unexpected break statement, this maybe due to adding"
     "a break statement after the default case", st.tokenTable[pointer]->lineNo);
        }   
    scopeCount.pop();

    string printSw = scopeCount.top();
    printSw.erase(0, 5); 

    codeGenerator("beq  x0  x0,  "+printSw+"_exit"); 
    codeGenerator(printfunc+"_exit"); 

    pointer = pointer + 2;
    return(pointer);
}

int c_default(int pointer){
    pointer = pointer +2 ;
    if(expect(TOKEN_LEFT_BRACE, pointer)){
        error("default statement does not need a '{' on line ", st.tokenTable[pointer]->lineNo);
    }
    return(pointer);
}

int c_tokenfuncall( int pointer){
    int cnt   = 0;
    int brace = 0;
    int funcIndex = st.indexE -1;
    int argument = 0;
    int storeC = 0;
    while(funcIndex > 0){
        if((st.tokenTable[pointer]->identifier == st.funcTable[funcIndex]->Fname)){
            cnt++;
            argument = st.funcTable[funcIndex]->An;       
        }
        funcIndex--;   
    }
    if(cnt == 0){
        error("The function "+st.tokenTable[pointer]->identifier+
        " has not been declared. Error on line", st.tokenTable[pointer]->lineNo);}
    
    if((tempCount) > 0){
        brace = tempCount *  16;
        cnt   = 0;
        codeGenerator("addi  sp, sp,  -"+brace);
        brace -= 16;
        while(brace >=  0){
            codeGenerator("sd  "+tempV[cnt]+"  "+to_string(brace)+"(sp)");
            cnt++;          
            brace -= 16;
        }
    } 
    pointer++;    
    storeC = pointer;
    pointer++;
    cnt   = 0;
    funArgc = argument;
    while(argument > cnt){
       
        if(argument - cnt  == 1){
            pointer = expression(pointer, TOKEN_RIGHT_PAREN);
            pointer--;
        }else 
        if(argument - cnt  > 1){          
            pointer = expression(pointer, TOKEN_COMMA);
        }
        cnt++;
    } 
    codeGenerator("jal  XI "+st.tokenTable[storeC-1]->identifier);   
    st.tokenTable[storeC-1]   = new tokenClass(TOKEN_IDENTIFIER,  0, "");  
    st.tokenTable[storeC-1]->regName = argV[0];              
    st.tokenTable[storeC]     = new tokenClass(TOKEN_END,  0);
    st.tokenTable[pointer]  = new tokenClass(TOKEN_START,  0);      
    return(pointer);    
}

int c_tenary(int pointer){

    string printfunc = scopeCount.top();
    printfunc.erase(0, 5);  

    st.tokenTable[pointer-2]     = st.tokenTable[tenary];
    st.tokenTable[pointer-1]     = st.tokenTable[tenary+1];         
    pointer = expression(pointer-2, TOKEN_COLON);
    codeGenerator("beq x0    x0  "+printfunc+"f_exit");
    codeGenerator(printfunc+"_exit");

    st.tokenTable[pointer-2]     = st.tokenTable[tenary];
    st.tokenTable[pointer-1]     = st.tokenTable[tenary+1];    
    pointer = expression(pointer-2, TOKEN_SEMICOLON);           
    codeGenerator(printfunc+"f_exit:");

    scopeCount.pop();
    return(pointer); 
}

int c_cout(int pointer){
    int count  = pointer;
    int argCnt = 0;
    while(!expect(TOKEN_SEMICOLON, count)){
        if(st.tokenTable[count]->type == TOKEN_SHIFT_LEFT){
            argCnt++;
        }
        if(st.tokenTable[count]->type == TOKEN_EOF){
            error("Unexpected end of file due to print error on line", st.tokenTable[pointer-2]->lineNo);
        }        
        count++;       
    }
    if(argCnt == 0){
        error("Invalid print statement on line", st.tokenTable[pointer-2]->lineNo);
    }     
    pointer+=2;
    funArgc = argCnt;        
    while(argCnt != 0){
        if(argCnt == 1){
            pointer = expression(pointer, TOKEN_SEMICOLON);
        }else
        if(argCnt > 1){
            pointer = expression(pointer, TOKEN_SHIFT_LEFT);     
        }
        argCnt--;
    }
    codeGenerator("jal cout");
    return pointer;
}

int c_return(int pointer){
    pointer++;   
    if(!(expect(TOKEN_SEMICOLON, pointer))){
        funArgc = 1;
        pointer = expression(pointer, TOKEN_SEMICOLON);  
        pointer--;
    }
    pointer++;          
    codeGenerator("j  "+Fname+"_exit"); 
    return(pointer);  
}

int c_break(int pointer){
    while((scopeCount.top().find("*for_") == string::npos)
    &&(scopeCount.top().find("*whi_") == string::npos)
    &&(!scopeCount.empty())){
        scopeCount.pop();
    }
    if(((scopeCount.top().find("*for_") != string::npos)
    ||(scopeCount.top().find("*whi_") != string::npos))
    &&(!scopeCount.empty())){
        string printfunc = scopeCount.top();
        printfunc.erase(0, 5); 
        codeGenerator("j  "+printfunc+"_exit:");        
        scopeCount.pop();
    }else
    {
        error("breaks are only used within a loop ", st.tokenTable[pointer]->lineNo);
    }
    pointer += 2;  
    return pointer;    
}

int c_continue(int pointer){
    while((scopeCount.top().find("*for_") == string::npos)
    &&(scopeCount.top().find("*whi_") == string::npos)
    &&(!scopeCount.empty())){
        scopeCount.pop();
    }
    if(((scopeCount.top().find("*for_") != string::npos)
    ||(scopeCount.top().find("*whi_") != string::npos))
    &&(!scopeCount.empty())){
        string printfunc = scopeCount.top();
        printfunc.erase(0, 5); 
        codeGenerator("j  "+printfunc+":");        
        scopeCount.pop();
    }else
    {
        error("breaks are only used within a loop ", st.tokenTable[pointer]->lineNo);
    }
    pointer += 2;  
    return pointer;    
}

int c_goto(int pointer){
    pointer += 2;     
    codeGenerator("j  "+st.tokenTable[pointer]->identifier); 
    if(expect(TOKEN_SEMICOLON, pointer+1)){
        pointer += 2; 
    }else{
        error("invalid goto keyword call on line ", st.tokenTable[pointer]->lineNo);
    } 

    return pointer; 
}

int c_label(int pointer){
    codeGenerator(st.tokenTable[pointer]->identifier+":");  
    pointer += 2;  
    return pointer; 
}
void sendToparser(TokenType type, int line, std::string numToken){
    st.tokenTable[st.index]  = new tokenClass(type,  line, numToken);
    st.index++;
}
void sendToparser(TokenType type, int line){
    st.tokenTable[st.index]  = new tokenClass(type,  line);
    st.index++;
    if(type == TOKEN_EOF){
        codeGenerator(".text");
        codeGenerator(".extern printf");
        codeGenerator(".globl main");
        parse(1);
        
    }
}