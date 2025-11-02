/*
 * Greg Rosen
 * Project 4: HashTable
 * Debug Tests for hash table
 */

#include "HashTable.h"
#include <iostream>
#include <iomanip>

void functionalityTest();
void memLeakTest();
void timeComplexityTest();
std::string makeRandomString(unsigned char length, std::uniform_int_distribution<char>& charDist, std::mt19937& rngEngine);

int main() {
    functionalityTest();
    timeComplexityTest();
    // memLeakTest();
}

/**
 * @brief Tests the functionality of HashTable.
 */
void functionalityTest() {
    HashTable myTable;
    std::cout << "Empty Table:" << std::endl << myTable << "__end__" << std::endl;

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
    for (const auto & key : keyList) {
        std::cout << key << std::endl;
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
    std::cout << R"(Table after removing keys "five", "six", and "seven":)" << std::endl << myTable << "__end__" << std::endl;
    std::cout << "List of keys after removal:" << std::endl;
    keyList = myTable.keys();
    for (const auto & key : keyList) {
        std::cout << key << std::endl;
    }
    std::cout << "__end__" << std::endl;

    std::cout << "Does remove return false for key NOT in table..." << (myTable.remove("seven") ? "NO!!ERROR" : "yes") << std::endl;

    std::string stringsToTest[3] = {"four", "blarg","five"};
    for (const auto & str : stringsToTest) {
        std::cout << R"(get function returns for key ")" << str << R"("...)";
        if (std::optional<size_t> optReturn = myTable.get(str);
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
    for (const auto & key : keyList) {
        std::cout << key << std::endl;
    }
    std::cout << "__end__" << std::endl;

    std::cout << "Successful insertions of one more value, leading to another table resizing (16 to 32)...";
    std::cout << myTable.insert("fifteen", 15);
    std::cout << std::endl;
    std::cout << "Doubly Expanded Table:" << std::endl << myTable << "__end__" << std::endl;
    std::cout << "List of keys for new table:" << std::endl;
    keyList = myTable.keys();
    for (const auto & key : keyList) {
        std::cout << key << std::endl;
    }
    std::cout << "__end__" << std::endl;
}

/**
 * Tests HashTable for memory leaks.
 */
void memLeakTest() {
    constexpr size_t numTables = 1e3;
    constexpr size_t numEntries = 4e3;
    std::cout << "Starting Memory Leak Test. Make breakpoint here and check memory used by HashTableDebug...";
    std::cout << std::endl;
    for (size_t i = 0; i < numTables; ++i) {
        HashTable table;
        for (size_t j = 0; j < numEntries; ++j) {
            std::string key = std::to_string(j);
            std::string badKey = std::to_string(j+1);
            bool goodInsert = table.insert(key, j);
            bool badInsert = table.insert(key, j);
            bool badRemove = table.remove(badKey);
            bool goodContains = table.contains(key);
            bool badContains = table.contains(badKey);
            std::optional<size_t> getVal = table.get(key);
            std::optional<size_t> getBadVal = table.get(badKey);
            size_t subscriptGetVal = table[key];
            size_t subscriptGetBadVal = table[badKey];
        }
        for (size_t j = 0; j < numEntries; ++j) {
            std::string key = std::to_string(j);
            bool removeFlag = table.remove(key);
        }
    }
    std::cout << "Memory Leak test complete. Make breakpoint here and check memory used by HashTableDebug..." << std::endl;
}

/**
 * @brief Time complexity testing for HashTable.
 *
 * For the given set of table capacities and load factors:
 * 1. A hash table is created and populated with enough random key-value pairs to raise its load factor to the given value.
 * 2. A number of additional random strings are created. Each is inserted and then immediately removed from the table.
 * 3. The number of probes necessary to both insert and remove each of the additional strings is tracked.
 * 4. An average is calculated for all insertions/removals for a given capacity/load factor combination.
 * 5. All such averages are reported.
 */
void timeComplexityTest() {
    constexpr unsigned char minLength = 5; // minimum random string length.
    constexpr unsigned char maxLength = 15; // maximum random string length.
    constexpr unsigned char numCapTested = 3; // Number of capacities tested. Must be modified if later array is modified.
    constexpr unsigned char numAlphaTested = 9; // Number of load factors tested. Must be modified if later array is modified.
    constexpr size_t numTests = 100; // Number of random strings to be tested for each capacity/load factor combination.
    constexpr size_t capacitiesTested[numCapTested] = {10000, 100000, 1000000};
    constexpr double loadFactorsTested[numAlphaTested] = {0.1, 0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9};
    double results[numCapTested][numAlphaTested][2]; // Array to capture results.
    std::mt19937 rngEngine(std::random_device{}());
    std::uniform_int_distribution<char> characterDist('!','~'); // All ASCII letters and punctuation
    std::uniform_int_distribution<size_t> valueDist(0, std::numeric_limits<size_t>::max()); // All possible values for size_t
    std::uniform_int_distribution<unsigned char> lengthDist(minLength,maxLength);
    std::cout << "Starting time complexity test..." << std::endl;
    for (unsigned char capInd = 0; capInd < numCapTested; ++capInd) {
        const size_t capacity = capacitiesTested[capInd];
        for (unsigned char alphaInd = 0; alphaInd < numAlphaTested; ++alphaInd) {
            const double alpha = loadFactorsTested[alphaInd];
            size_t insertNumAccesses[numTests];
            size_t removeNumAccesses[numTests];
            HashTable table(capacity,1.0);
            for (size_t fillCt = 0; fillCt < (alpha * capacity); ++fillCt) { // Fill hash table up to load factor
                const unsigned char randLength = lengthDist(rngEngine);
                std::string randKey = makeRandomString(randLength, characterDist, rngEngine);
                size_t randVal = valueDist(rngEngine);
                table.insert(randKey, randVal);
            }
            for (size_t testNum = 0; testNum < numTests; ++testNum) { // Insert/Remove numTests random strings
                const unsigned char randLength = lengthDist(rngEngine);
                std::string randKey = makeRandomString(randLength, characterDist, rngEngine);
                size_t randVal = valueDist(rngEngine);
                insertNumAccesses[testNum] = table.insertTCT(randKey, randVal);
                removeNumAccesses[testNum] = table.removeTCT(randKey);
            }
            // Calculate and record averages.
            size_t sumIns = 0;
            size_t sumRem = 0;
            for (size_t testNum = 0; testNum < numTests; ++testNum) {
                sumIns += insertNumAccesses[testNum];
                sumRem += removeNumAccesses[testNum];
            }
            results[capInd][alphaInd][0] = static_cast<double>(sumIns)/static_cast<double>(numTests);
            results[capInd][alphaInd][1] = static_cast<double>(sumRem)/static_cast<double>(numTests);
        }
    }
    // Display results.
    std::cout << "____RESULTS____" << std::endl;
    for (unsigned char capInd = 0; capInd < numCapTested; ++capInd) {
        std::cout << "Capacity = " << capacitiesTested[capInd] << ": " << std::endl;
        std::cout << "   Alpha   Avg # Probes___" << std::endl;
        std::cout << "           Insert   Remove" << std::endl;
        for (unsigned char alphaInd = 0; alphaInd < numAlphaTested; ++alphaInd) {
            std::cout << std::fixed << std::setprecision(3) << "   " << loadFactorsTested[alphaInd] <<
            "   " << results[capInd][alphaInd][0] << "    " << results[capInd][alphaInd][1] << std::endl;
        }
    }
}

/**
 * @brief Creates a string of random characters
 *
 * Returns a string of the given length composed of characters randomly chosen from the given character distribution.
 *
 * @param length length of random string
 * @param charDist uniform distribution of characters from which to build string
 * @param rngEngine mt19937 random engine
 * @return random string
 */
std::string makeRandomString(const unsigned char length, std::uniform_int_distribution<char>& charDist, std::mt19937& rngEngine) {
    std::string randomString;
    randomString.reserve(length);
    for (unsigned char i = 0; i < length; ++i) {
        const char nextCharacter = charDist(rngEngine);
        randomString += nextCharacter;
    }
    return randomString;
}