#ifndef LIBRARY_TO_PARSER_MESSAGE_HPP
#define LIBRARY_TO_PARSER_MESSAGE_HPP


#include <string>
#include <vector>
#include "Field.hpp"
#include "Signal.hpp"

/**
 * @class Message
 * Class that represents one message
 */
class Message {
public:

    /**
     * Checks if data is all filled
     * @return a string containing information on data that is not filled
     */
    std::vector<std::string> check();

    /**
     * Returns the python representation of the message
     * @return list of strings representing python code
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
     * Sets id field
     */
    void setId(int newId);

    /**
     * Sets length field
     */
    void setLength(int newLength);

    /**
     * Adds a new signal to the list of signals
     */
    void addSignal(Signal newSignal);

private:
    Field<std::string> name;
    Field<std::string> description{""};
    Field<int> id;
    Field<int> length;
    std::vector<Signal> signals;
};


#endif //LIBRARY_TO_PARSER_MESSAGE_HPP
