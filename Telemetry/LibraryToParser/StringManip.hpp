#ifndef HYTECH_C_TO_PY_STRINGMANIP_HPP
#define HYTECH_C_TO_PY_STRINGMANIP_HPP

#include <string>
#include <vector>
#include <fstream>

/**
 * @Namespace StringManip
 * Contains all string manipulation methods
 */
namespace StringManip{
    constexpr std::string JAVADOC_START = "/**";
    constexpr std::string JAVADOC_END = "*/";

    /**
     * Removes leading and trailing whitespace. Performs the operation in-place
     * @param s the string to remove whitespace from
     */
    void removeLeadingAndTrailingWhitespace(std::string &s);

    /**
     * Indents all of the lines. Performs the operation in-place
     * @param lines list of strings representing lines
     * @param amount number of indents. default: 1
     */
    void indent(std::vector<std::string> &lines, int amount = 1);

    /**
     * Gets the number in hex
     * @param x number to get hex value of
     * @return string equivalent to the numbers hex value
     */
    std::string toHexString(int x);

    /**
     * Extracts all annotations from a file
     * @param fin std::ifstream for the file
     * @return list of strings that contains all annotation lines
     */
    std::vector<std::string> getAnnotations(std::ifstream &fin);

    /**
     * Extracts annotations from a line starting with the signal annotation
     * @param s line to get annotation from
     * @return annotations in pairs
     */
    std::vector<std::pair<std::string, std::string>> parseSignalAnnotations(std::string &s);

    /**
     * Extracts a pair of ints from a string
     * @param s string containing comma-divided ints
     * @return pair of ints
     */
    std::pair<int,int> parseIntPair(std::string &s);
}

#endif //HYTECH_C_TO_PY_STRINGMANIP_HPP
