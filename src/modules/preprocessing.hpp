#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>   
#include <algorithm>

using std::cout, std::cerr, std::endl;
using std::string, std::stringstream, std::ifstream;
using std::remove_if, std::size_t;

class Preprocessor {
private:  
  string m_src;

  void checkSourcePath(int argc){
    if (argc != 2){
      cerr << "You must insert the source code path\n";
      cerr << "Correct usage is: comp <file.shq>\n";
      exit(EXIT_FAILURE);
    }
  }

  string getSourceContents(char* path){
    ifstream src(path);

    if (!src.is_open()){
      cerr << "ERROR! Souce file couldn't be opened!\n";
      exit(EXIT_FAILURE);
    }

    string contents;
    stringstream buffer;

    buffer << src.rdbuf();
    contents = buffer.str();
    src.close();
    return contents;
  }

  void removeComments(string& src){
    for(size_t start = src.find("/*"), end; start != string::npos; start = src.find("/*")){
    end = src.find("*/", start) + 2;
    if (end == string::npos) end = src.size();

    src.erase(start, end - start);
    }

    for(size_t start = src.find('#'), end; start != string::npos; start = src.find('#')){
      
      end = src.find('\n', start);
      if (end == string::npos) end = src.size();

      src.erase(start, end - start);
    }
  }

  void removeSpecialCharacters(string& src){
    auto predicate = [](unsigned char c){return c == '\n';};
    src.erase(remove_if(src.begin(), src.end(), predicate), src.end());
  }

public:
  Preprocessor(int argc, char* argv[]) {
    checkSourcePath(argc);

    m_src = getSourceContents(argv[1]);
    removeComments(m_src);
    removeSpecialCharacters(m_src);
  }

  string getSrc() const { return m_src; }
};