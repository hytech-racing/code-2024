#include <sstream>
#include "StringManip.hpp"

void StringManip::removeLeadingAndTrailingWhitespace(std::string &s) {
    int leading = 0;
    while(leading < int(s.size()) && (s[leading] == ' ' || s[leading] == '\t')){
        leading++;
    }
    int trailing = 0;
    while(trailing < int(s.size()) && (s[int(s.size())-1-trailing] == ' ' ||
                                       s[int(s.size()) - 1 - trailing] == '\t')) {
        trailing++;
    }
    if(leading + trailing >= int(s.size())){
        s = "";
        return;
    }
    s = s.substr(leading, int(s.size()) - leading - trailing);
}

void StringManip::indent(std::vector<std::string> &lines, int amount) {
    for(auto &line : lines){
        std::string newLine;
        for(int j = 0; j < amount; j++) newLine.push_back('\t');
        newLine += line;
        line = newLine;
    }
}

std::string StringManip::toHexString(int x) {
    std::stringstream stream;
    stream << "0x" << std::uppercase << std::hex << x;
    return stream.str();
}

std::vector<std::string> StringManip::getAnnotations(std::ifstream &fin) {
    std::vector<std::string> ret;
    std::string temp;
    bool inDoc = false;

    std::stringstream iss;
    iss << fin.rdbuf();
    std::string entireFile = iss.str();
    for(char c : entireFile){
        if(c == '\n'){
            if(temp.empty()) continue;
            StringManip::removeLeadingAndTrailingWhitespace(temp);
            if(temp == JAVADOC_START) inDoc = true;
            else if(inDoc && temp == JAVADOC_END) inDoc = false;
            else if(inDoc) ret.push_back(temp.substr(2, int(temp.size())-2));

            temp = "";
        }else temp.push_back(c);
    }
    return ret;
}

std::vector<std::pair<std::string, std::string>> StringManip::parseSignalAnnotations(std::string &s) {
    s = s.substr(8, int(s.size())-8);
    s.push_back(' ');
    std::vector<std::pair<std::string, std::string>> ret;

    std::vector<std::string> annotations;

    std::string temp;
    for(char c : s){
        if(c == ' ' || c == '@' || c == '\t'){
            if(!temp.empty()) annotations.push_back(temp);
            temp = "";
        }else temp.push_back(c);
    }

    for(auto& annotation : annotations){
        if(annotation[int(annotation.size())-1] == ')'){
            std::string first, second;
            bool isFirst = true;
            for(char c : annotation){
                if(c=='(' || c==')') isFirst ^= true;
                else if(isFirst) first.push_back(c);
                else second.push_back(c);
            }
            ret.emplace_back(first, second);
        }else{
            ret.emplace_back(annotation, "");
        }
    }

    return ret;
}

std::pair<int, int> StringManip::parseIntPair(std::string &s) {
    std::string first, second;
    bool isFirst = true;
    for(char c : s){
        if(c == ',') isFirst = false;
        else if(isFirst) first.push_back(c);
        else second.push_back(c);
    }
    return std::make_pair(stoi(first), stoi(second));
}
