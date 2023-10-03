#ifndef LIBRARY_TO_PARSER_SIGNAL_HPP
#define LIBRARY_TO_PARSER_SIGNAL_HPP


#include <string>
#include <vector>
#include "Field.hpp"

class Signal {
public:

    /**
     * Builds a new signal from a string of annotations
     * @param s annotation string. must start with Signal tag
     * @return new signal. Not guaranteed to have complete data
     */
    static Signal buildSignal(std::string &s);

    /**
     * Checks to see if there is enough information to create the signal
     * @return a list of strings that describe what data is missing
     */
    std::vector<std::string> check();

    /**
     * Converts the signal into the python data structure
     * @return a list of strings that represent the lines of python
     */
    std::vector<std::string> toPy();

    /**
     * Sets name field
     */
    void setName(std::string newName);

    /**
     * Sets description field
     */
    void setDescription(std::string newDescription);

    /**
     * Sets units field
     */
    void setUnits(std::string newUnits);

    /**
     * Sets path field
     */
    void setPath(std::string newPath);

    /**
     * Sets bits field
     */
    void setBits(std::pair<int,int> newBits);

    /**
     * Sets signed field
     */
    void setIsSigned(bool newIsSigned);

    /**
     * Sets endianness field
     */
    void setEndianness(std::string newEndianness);

    /**
     * Sets transformation field
     */
    void setTransformation(std::string newTransformation);

private:
    Field<std::string> name;
    Field<std::string> description{""};
    Field<std::string> units{""};
    Field<std::string> path;
    Field<std::pair<int,int>> bits;
    Field<bool> isSigned;
    Field<std::string> endianness{"little"};
    Field<std::string> transformation{"x"};
};


#endif //LIBRARY_TO_PARSER_SIGNAL_HPP
