#include "Message.hpp"
#include "StringManip.hpp"

std::vector<std::string> Message::check() {
    std::vector<std::string> ret;
    if(!name.hasData()){
        ret.emplace_back("Required field not set: name");
    }
    if(!description.hasData()){
        ret.emplace_back("Required field not set: description");
    }
    if(!id.hasData()){
        ret.emplace_back("Required field not set: id");
    }
    if(!length.hasData()){
        ret.emplace_back("Required field not set: length");
    }
    for(Signal signal : signals){
        std::vector<std::string> signalCheck = signal.check();

        if(!signalCheck.empty()){
            ret.emplace_back("Signal could not be constructed: ");
            StringManip::indent(signalCheck);
            ret.insert(ret.end(), signalCheck.begin(), signalCheck.end());
        }
    }
    return ret;
}

std::vector<std::string> Message::toPy() {
    std::vector<std::string> ret;
    ret.push_back("\"" + name.getData() + "\": {");
    ret.push_back("\t\"description\": \"" + description.getData() + "\",");
    ret.push_back("\t\"id\": " + StringManip::toHexString(id.getData()) + ",");
    ret.push_back("\t\"length\": " + std::to_string(length.getData()) + ",");
    ret.emplace_back("\t\"signals\": {");
    for(auto& signal : signals){
        auto signalPy = signal.toPy();
        StringManip::indent(signalPy, 2);
        ret.insert(ret.end(), signalPy.begin(), signalPy.end());
    }
    ret.emplace_back("\t}");
    ret.emplace_back("},");
    return ret;
}

void Message::setName(std::string newName) {
    name.setData(std::move(newName));
}

void Message::setDescription(std::string newDescription) {
    description.setData(std::move(newDescription));
}

void Message::setId(int newId) {
    id.setData(newId);
}

void Message::setLength(int newLength) {
    length.setData(newLength);
}

void Message::addSignal(Signal newSignal) {
    signals.push_back(newSignal);
}
