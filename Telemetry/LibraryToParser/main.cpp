#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include "StringManip.hpp"
#include "Message.hpp"

constexpr std::string LIB_DIR = "../libDir";
constexpr std::string PARSER_PATH = "../parserDir";
constexpr std::string PARSER_OUT_FILE = "/newParser.py";

/**
 * Reads in C++ library and generates python parser
 * Compiles with: g++-13
 *
 * Message annotations:
 *  - @Parse: Sets name of message
 *  - @Id: Sets message id
 *  - @Length: Sets length of message
 *
 * Signal annotations:
 *  - @Signal: Defines a new signal
 *  - @Name: Sets name of signal
 *  - @Description: Sets description of signal (default:"")
 *  - @Units: Sets units of signal (default:"")
 *  - @Path: Sets path of signal
 *  - @Bits: Sets bits of signal
 *  - @Signed: Sets signal to be signed
 *  - @Unsigned: Sets signal to be unsigned
 *  - @LittleEndian: Sets endianness to little (endianness is already little by default)
 *  - @BigEndian: Sets endianness to big
 *  - @Transformation: Sets transformation (default:"x")
 */
int main() {
    std::vector<std::vector<std::string>> annotations;
    std::vector<std::string> pyOutput;

    for(auto& dirEntry : std::filesystem::recursive_directory_iterator(LIB_DIR)){
        if(!dirEntry.is_directory()){
            std::string path = dirEntry.path().string();
            std::ifstream fin(path);
            annotations.push_back(StringManip::getAnnotations(fin));
        }
    }

    std::vector<Message> messages;

    for(auto& annotation : annotations){
        Message message;
        bool parse = false;
        for(auto& line : annotation){
            int len = int(line.size());
            if(len >= 6 && line.substr(0,6) == "@Parse"){
                parse = true;
                if(len > 7) message.setName(line.substr(7, len-7));
            } else if(len >= 3 && line.substr(0,3) == "@Id"){
                message.setId(std::stoi(line.substr(4,len-4),nullptr,0));
            } else if(len >= 7 && line.substr(0,7) == "@Length"){
                message.setLength(std::stoi(line.substr(7, len-7),nullptr,0));
            } else if(len >= 7 && line.substr(0,7) == "@Signal"){
                message.addSignal(Signal::buildSignal(line));
            }
        }
        if(!parse) continue;
        auto check = message.check();
        if(!check.empty()){
            for(auto& error : check){
                std::cerr << error << std::endl;
            }
        } else messages.push_back(message);
    }

    pyOutput.emplace_back("#Do not edit. Auto-generated file.");
    pyOutput.emplace_back("import pandas as pd");
    pyOutput.emplace_back("import numpy as np");
    pyOutput.emplace_back("from parser_helpers import get_offsets_masks, parse_to_np");
    pyOutput.emplace_back("");
    pyOutput.emplace_back("NOMINAL_TORQUE = 9.8");
    pyOutput.emplace_back("GRAVITY = 9.801665");
    pyOutput.emplace_back("");
    pyOutput.emplace_back("PARSERS_DICT = {");
    pyOutput.emplace_back("\t\"messages\": {");

    for(auto message : messages){
        auto toPy = message.toPy();
        StringManip::indent(toPy, 2);
        pyOutput.insert(pyOutput.end(), toPy.begin(), toPy.end());
    }

    pyOutput.emplace_back("\t}");
    pyOutput.emplace_back("}");

    std::ofstream fout(PARSER_PATH + PARSER_OUT_FILE);
    for(auto &s : pyOutput){
        fout << s << std::endl;
    }
    fout.close();
}
