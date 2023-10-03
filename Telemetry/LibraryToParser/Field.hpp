#ifndef LIBRARY_TO_PARSER_FIELD_HPP
#define LIBRARY_TO_PARSER_FIELD_HPP

#include <cassert>

/**
 * @class Field
 * Generic class that holds a required or optional data field
 * @tparam T type of data
 */
template<typename T>
class Field {
public:
    /**
     * Constructs a required data field
     */
    Field() : required(true), set(false){}

    /**
     * Constructs an optional data field with a default value
     * @param defaultData default value
     */
    explicit Field(T defaultData) : required(false), set(false),
            defaultData(defaultData) {}

    /**
     * Checks if field had been field
     * @return true if value is optional or value has been set, false otherwise
     */
    bool hasData() {
        return !required || set;
    }

    /**
     * Gets data in field. Should only be called after checking if the field
     * has data.
     * @return data in field
     */
    T getData() {
        assert(hasData());
        return set ? data : defaultData;
    }

    /**
     * Sets the data in the field
     * @param newData data to put in field
     */
    void setData(T newData){
        set = true;
        this->data = newData;
    }

private:
    bool required;
    bool set;
    T data;
    T defaultData;
};

#endif //LIBRARY_TO_PARSER_FIELD_HPP
