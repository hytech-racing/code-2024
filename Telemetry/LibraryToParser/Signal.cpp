#include "Signal.hpp"

#include <utility>
#include <iostream>
#include "StringManip.hpp"

std::vector<std::string> Signal::check() {
    std::vector<std::string> ret;
    if(!name.hasData()) {
        ret.emplace_back("Required field not set: name");
    }
    if(!description.hasData()) {
        ret.emplace_back("Required field not set: description");
    }
    if(!units.hasData()) {
        ret.emplace_back("Required field not set: units");
    }
    if(!path.hasData()) {
        ret.emplace_back("Required field not set: path");
    }
    if(!bits.hasData()) {
        ret.emplace_back("Required field not set: bits");
    }
    if(!isSigned.hasData()) {
        ret.emplace_back("Required field not set: signed");
    }
    if(!endianness.hasData()) {
        ret.emplace_back("Required field not set: endianness");
    }
    if(!transformation.hasData()) {
        ret.emplace_back("Required field not set: transformation");
    }
    return ret;
}

std::vector<std::string> Signal::toPy() {
    std::vector<std::string> ret;
    ret.push_back("\"" + name.getData() + "\": {");
    ret.push_back("\t\"description\": \"" + description.getData() + "\",");
    ret.push_back("\t\"units\": \"" + units.getData() + "\",");
    ret.push_back("\t\"path\": \"" + path.getData() + "\",");
    ret.push_back("\t\"bits\": (" + std::to_string(bits.getData().first) + "," +
                                    std::to_string(bits.getData().second) + "),");
    ret.push_back("\t\"signed\": " + std::string(isSigned.getData() ? "True" : "False") + ",");
    ret.push_back("\t\"endianness\": \"" + endianness.getData() + "\",");
    ret.push_back("\t\"transformation\": lambda x: " + transformation.getData());
    ret.emplace_back("},");
    return ret;
}

void Signal::setName(std::string newName) {
    name.setData(std::move(newName));
}

void Signal::setDescription(std::string newDescription) {
    description.setData(std::move(newDescription));
}

void Signal::setUnits(std::string newUnits) {
    units.setData(std::move(newUnits));
}

void Signal::setPath(std::string newPath) {
    path.setData(std::move(newPath));
}

void Signal::setBits(std::pair<int, int> newBits) {
    bits.setData(newBits);
}

void Signal::setIsSigned(bool newIsSigned) {
    isSigned.setData(newIsSigned);
}

void Signal::setEndianness(std::string newEndianness) {
    endianness.setData(std::move(newEndianness));
}

void Signal::setTransformation(std::string newTransformation) {
    transformation.setData(std::move(newTransformation));
}

Signal Signal::buildSignal(std::string &s) {
    Signal ret;
    auto parsedAnnotations = StringManip::parseSignalAnnotations(s);
    for(auto& [key,val] : parsedAnnotations){
        if(key == "Name") ret.setName(val);
        else if(key == "Description") ret.setDescription(val);
        else if(key == "Units") ret.setUnits(val);
        else if(key == "Path") ret.setPath(val);
        else if(key == "Bits") ret.setBits(StringManip::parseIntPair(val));
        else if(key == "Signed") ret.setIsSigned(true);
        else if(key == "Unsigned") ret.setIsSigned(false);
        else if(key == "LittleEndian") ret.setEndianness("little");
        else if(key == "BigEndian") ret.setEndianness("big");
        else if(key == "Transformation") ret.setTransformation(val);
    }
    return ret;
}
