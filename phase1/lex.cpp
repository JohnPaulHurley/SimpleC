#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cctype>

using namespace std;

int main(){
  string keyword[] = { "auto", "break","case","char","const","continue","default","do","double","else","enum","extern","float","for","goto", "if", "int","long","register","return","short","signed","sizeof","static", "struct","switch","typedef","union","unsigned","void","volatile","while"};

  char input;
  char prev;
  string buffer;
  while(!cin.eof()){
    input = cin.get();
    // token is a keyword or id
    if(isalpha(input) || input == '_'){
      buffer = "";
      buffer += input;
      input = cin.get();
      while(isalpha(input) || input == '_' ){
        buffer += input;
        input = cin.get();
      }
      cin.putback(input); 
      // check if token is a keyword
      int i;
      for(i = 0; i<32; ++i){
        if(buffer == keyword[i]){
          cout << "keyword:" << buffer << endl;
          break;
        }  
      }
      if(i==32)
        cout << "identifier:" << buffer << endl;
    }
    // token is a number
    else if(isdigit(input)){
      buffer = "";
      buffer += input;
      input = cin.get();
      while(isdigit(input)){
        buffer += input;
        input = cin.get();
      }
      cin.putback(input);
      if(input == 'l' || input == 'L'){
        cout << "long:" << buffer << input << endl;
      }
      else
        cout << "int:" << buffer << endl;
    }
    
    //token is a string
    else if(input == '"'){
      buffer = "";
      buffer += input;
      prev = input;
      input = cin.get();
      while(input != '"' || prev == '\\'){
        buffer += input;
        prev = input;
        input = cin.get();
      }
      buffer += input;
      cout << "string:" << buffer << endl;
    }
    
    // token is a comment
    else if((input == '/') && (cin.peek() == '*')){
      cin.get();
      input = cin.get();
      while(!((input == '*') && (cin.peek() == '/'))){
        input = cin.get();
      }
      cin.get(); 
    }

    // token could be an operator
    else{
      string op;
      char peek = cin.peek();
      switch(input){
        case '=': if(peek == '='){
                     op = "==";
                     cin.get();
                  } 
                  else
                    op = "=";
                  break; 
        case '|': if(peek == '|'){
                    op = "||";
                    cin.get();
                  }
                  break; 
        case '&': if(peek == '&'){
                    op = "&&";
                    cin.get();
                  }
                  else
                    op = "&";
                  break;
        case '!': if(peek == '='){
                    op = "!=";
                    cin.get();
                  }
                  else
                    op = "!";
                  break;
        case '<': if(peek == '='){
                    op = "<=";
                    cin.get();
                  }
                  else
                    op = "<";
                  break;
        case '>': if(peek == '='){
                    op = ">=";
                    cin.get();
                  }
                  else
                    op = ">";
                  break;
        case '+': if(peek == '+'){
                    op = "++";
                    cin.get();
                  }
                  else
                    op = "+";
                  break;
        case '-': if(peek == '-'){
                    op = "--";
                    cin.get();
                  }
                  else if(peek == '>'){
                    op = "->";
                    cin.get();
                  }
                  else
                    op = "-";
                  break;
        case '*': op = "*";
                  break;
        case '/': op = "/"; 
                  break;
        case '%': op = "%";
                  break;
        case '.': op = ".";
                  break;
        case '(': op = "(";
                  break;
        case ')': op = ")";
                  break;
        case '[': op = "[";
                  break;
        case ']': op = "]";
                  break;
        case '{': op = "{";
                  break;
        case '}': op = "}";
                  break;
        case ';': op = ";";
                  break;
        case ':': op = ":";
                  break;
        case ',': op = ",";
                  break;
        default: continue;
      }
      cout << "operator:" << op << endl;
    }
  }
  return 0;
}
