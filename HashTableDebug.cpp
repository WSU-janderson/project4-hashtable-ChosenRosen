/**
 * HashTableDebug.cpp
 *
 * Write your tests in this file
 */
#include "HashTable.h"
#include <iostream>

int main() {
    HashTable myTable;
    std::cout << "Empty Table:" << myTable << "__end__" << std::endl;

    std::cout << "Successful insertions...";
    std::cout << myTable.insert("one",1);
    std::cout << myTable.insert("two",2);
    std::cout << myTable.insert("three",3);
    std::cout << std::endl;
    std::cout << "Table just before resize from 8 to 16:" << std::endl << myTable << "__end__" << std::endl;

    std::cout << "Successful insertions...";
    std::cout << myTable.insert("four",4);
    std::cout << myTable.insert("five",5);
    std::cout << myTable.insert("six",6);
    std::cout << myTable.insert("seven",7);
    std::cout << std::endl;
    std::cout << "Table just before resize from 16 to 32:" << std::endl << myTable << "__end__" << std::endl;

    std::cout << "List of keys:" << std::endl;
    std::vector<std::string> keyList = myTable.keys();
    for (size_t i = 0; i < keyList.size(); ++i) {
        std::cout << keyList.at(i) << std::endl;
    }
    std::cout << "__end__" << std::endl;

    std::cout << "Does contains return true for key in table..." << (myTable.contains("seven") ? "yes" : "NO!!ERROR") << std::endl;
    std::cout << "Does contains return false for key NOT in table..." << (myTable.contains("blarg") ? "NO!!ERROR" : "yes") << std::endl;
    std::cout << "Does insert return false for key NOT in table..." << (myTable.insert("seven", 16) ? "NO!!ERROR" : "yes") << std::endl;

    myTable["four"] = 27;
    std::cout << "Table after using [] to assign a new value to key \"four\":" << std::endl << myTable << "__end__" << std::endl;
    myTable["Bork"] = 52;
    std::cout << "Table after using [] to assign a new value to key not in table:" << std::endl << myTable << "__end__" << std::endl;

    std::cout << "Successful removals...";
    std::cout << myTable.remove("five");
    std::cout << myTable.remove("six");
    std::cout << myTable.remove("seven");
    std::cout << std::endl;
    std::cout << "Table after removing keys \"five\", \"six\", and \"seven\":" << std::endl << myTable << "__end__" << std::endl;
    std::cout << "List of keys after removal:" << std::endl;
    keyList = myTable.keys();
    for (size_t i = 0; i < keyList.size(); ++i) {
        std::cout << keyList.at(i) << std::endl;
    }
    std::cout << "__end__" << std::endl;

    std::cout << "Does remove return false for key NOT in table..." << (myTable.remove("seven") ? "NO!!ERROR" : "yes") << std::endl;

    std::string stringsToTest[3] = {"four", "blarg","five"};
    for (size_t i = 0; i < 3; ++i) {
        std::cout << "get function returns for key \"" + stringsToTest[i] + "\"...";
        if (std::optional<size_t> optReturn = myTable.get(stringsToTest[i]);
        optReturn.has_value()) {
            std::cout << optReturn.value();
        }
        else {
            std::cout << "nullopt";
        }
        std::cout << std::endl;
    }

    std::cout << "Removing the remaining values from the table...";
    std::cout << "Successful removals...";
    std::cout << myTable.remove("one");
    std::cout << myTable.remove("two");
    std::cout << myTable.remove("three");
    std::cout << myTable.remove("four");
    std::cout << std::endl;
    std::cout << "Successful reinsertions with different keys...";
    std::cout << myTable.insert("eight", 8);
    std::cout << myTable.insert("nine" , 9);
    std::cout << myTable.insert("ten", 10);
    std::cout << myTable.insert("eleven" , 11);
    std::cout << myTable.insert("twelve", 12);
    std::cout << myTable.insert("thirteen" , 13);
    std::cout << myTable.insert("fourteen", 14);
    std::cout << std::endl;
    std::cout << "New Table" << std::endl << myTable << "__end__" << std::endl;
    std::cout << "List of keys for new table:" << std::endl;
    keyList = myTable.keys();
    for (size_t i = 0; i < keyList.size(); ++i) {
        std::cout << keyList.at(i) << std::endl;
    }
    std::cout << "__end__" << std::endl;

    std::cout << "Successful insertions of one more value, leading to another table resizing (16 to 32)...";
    std::cout << myTable.insert("fifteen", 15);
    std::cout << std::endl;
    std::cout << "Doubly Expanded Table:" << std::endl << myTable << "__end__" << std::endl;
    std::cout << "List of keys for new table:" << std::endl;
    keyList = myTable.keys();
    for (size_t i = 0; i < keyList.size(); ++i) {
        std::cout << keyList.at(i) << std::endl;
    }
    std::cout << "__end__" << std::endl;

    return 0;
}