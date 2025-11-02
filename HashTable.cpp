/*
 * Greg Rosen
 * Project 4: HashTable
 * Implementation of HashTable and HashTableBucket classes
 */

#include "HashTable.h"
#include <algorithm>
#include <iostream>

/**
 * @brief Default and parameterized constructor for hash table.
 *
 * Default and parameterized constructor for hash table.
 * Creates a hash table with given number of initial empty buckets.
 * If no value is given, the initial number defaults to 8.
 * The offsets vector is created in such a manner that the first element is always 0,
 * but the remaining elements (1 to capacity-1) are randomized.
 * This guarantees that iterating over the offsets always probes the home location first.
 *
 * @param initCapacity Initial number of empty buckets in hash table.
 * @param inThreshold The load factor threshold for rehashing (default 0.5).
 * @param inResizeFactor The factor by which the capacity of the hash table will be increased upon rehashing (default 2.0).
 */
HashTable::HashTable(const size_t initCapacity, const double inThreshold, const double inResizeFactor) :
    threshold(inThreshold), resizeFactor(inResizeFactor), tableData(initCapacity), offsets(initCapacity), numFilled(0), badKeyDrain(0) {
    for (size_t i = 0; i < initCapacity; ++i) {
        offsets[i] = i;
    }
    std::ranges::shuffle(offsets.begin() + 1, offsets.end(), std::mt19937(std::random_device{}()));
}

/**
 * @brief Subscript operator overload for hash table.
 *
 * Allows retrieval and assignment of value associated with given key in a manner like using an array index.
 * E.G:
 * hashTable["name"] will return a reference to the value associated with the key "name" if it is in the table.
 * hashTable["name"] = 5 will change that value.
 * If the key is not in the table, the returned reference will point to a dummy numerical field of the HashTable.
 *
 * @warning There is no explicit indication that the key was not present and the value is a dummy.
 * It is recommended that brackets only be used if the presence of the key in the table is a certainty.
 *
 * @param key Key to be searched.
 * @return Reference to value associated with key.
 */
size_t& HashTable::operator[](const std::string& key) {
    if (HashTableBucket* foundBucket = find(key); foundBucket != nullptr) {
        return foundBucket->getValueRef();
    }
    return badKeyDrain;
}

/**
 * @brief Getter for capacity of hash table.
 *
 * The capacity is the total number of buckets (empty or filled) in the hash table.
 *
 * @return capacity of hash table
 */
size_t HashTable::capacity() const {
    return tableData.size();
}

/**
 * @brief Getter for size of hash table.
 *
 * The size is the total number of filled buckets in the hash table.
 * This value is tracked internally and stored in the field numFilled for O(1) access.
 *
 * @return size of hash table
 */
size_t HashTable::size() const {
    return numFilled;
}

/**
 * @brief Getter for load factor (alpha) of hash table.
 *
 * Calculated as the ratio between the number of filled buckets (size)
 * and the total number of buckets in the table (capacity).
 *
 * @return load factor (alpha) of hash table
 */
double HashTable::alpha() const {
    return static_cast<double>(size())/static_cast<double>(capacity());
}

/**
 * @brief Getter for a list of keys currently used in the hash table.
 *
 * The list is returned as a vector of strings.
 * The method may in may iterate over every bucket in the hash table,
 * so its time complexity is O(capacity).
 *
 * @return vector of keys present in the hash table.
 */
std::vector<std::string> HashTable::keys() const {
    std::vector<std::string> keyList(numFilled); // Size of keyList is known in advance.
    for (size_t keyListIndex = 0, bucketNum = 0; bucketNum < capacity(); ++bucketNum) {
        if (const HashTableBucket *currBucket = &tableData.at(bucketNum);
        !currBucket->isEmpty()) {
            keyList.at(keyListIndex) = currBucket->getKey(); // Add every key found to keyList.
            ++keyListIndex;
        }
        if (keyListIndex == numFilled) {break;} // If numFilled keys found, all remaining buckets must be empty.
    }
    return keyList;
}

/**
 * @brief Getter for value stored using a given key.
 *
 * Searches for key using the helper method find.
 * If key is not found, returns nullopt.
 *
 * @param key Key to be searched.
 * @return value associated with key or nullopt.
 */
std::optional<size_t> HashTable::get(const std::string& key) {
    if (const HashTableBucket* foundBucket = find(key); foundBucket != nullptr) {
        return foundBucket->getValue();
    }
    return std::nullopt;
}

/**
 * @brief Predicate for if a given key is stored in table.
 *
 * Searches for key using the helper method find.
 * Returns true if key is found, false otherwise.
 *
 * @param key Key to be searched.
 * @return true if key found, false otherwise.
 */
bool HashTable::contains(const std::string& key) {
    if (const HashTableBucket* foundBucket = find(key); foundBucket != nullptr) {
        return true;
    }
    return false;
}

/**
 * @brief Insert key-value pair into table.
 *
 * The bucket to be filled is found using pseudo-random probing.
 * Returns true if insertion is successful.
 * Returns false if the key is already present in the hash table or hash table is full.
 * If the insertion raises the load factor of the hash table to or above the threshold (default 0.5), the table is rehashed.
 *
 * @param key of key-value pair to be inserted.
 * @param value Value of key-value pair to be inserted.
 *
 * @return true if insertion successful, false otherwise.
 */
bool HashTable::insert(const std::string& key, const size_t& value) {
    const size_t home = hash(key) % capacity();
    HashTableBucket* firstEARFound = nullptr;
    for (const size_t offset : offsets) {
        if (HashTableBucket* currBucket = &tableData.at((home + offset) % capacity());
        currBucket->isEmpty()) {
            if (currBucket->isESS()) { // If ESS bucket is encountered, insert into it or first EAR bucket found earlier during search.
                if (firstEARFound != nullptr) {currBucket = firstEARFound;}
                currBucket->load(key,value);
                ++numFilled;
                if (alpha() >= threshold) { // Rehash if necessary.
                    rehash();
                }
                return true;
            }
            if (firstEARFound == nullptr) { // Mark first EAR bucket found.
                firstEARFound = currBucket;
            }
        }
        else if (key == currBucket->getKey()) { // Return false if duplicate key found.
            return false;
        }
    }
    if (firstEARFound != nullptr) { // Insert at first EAR bucket encountered if all empty buckets are tombstones.
        firstEARFound->load(key,value);
        ++numFilled;
        if (alpha() >= threshold) { // Rehash if necessary.
            rehash();
        }
        return true;
    }
    return false; // Return false if table is full.
}

/**
 * @brief Remove key-value pair from table.
 *
 * Searches for key using the helper method find.
 * The bucket is marked EAR (tombstone), making its contents inaccessible.
 *
 * @param key Key to be searched.
 * @return true if removal successful, false otherwise.
 */
bool HashTable::remove(const std::string& key) {
    if (HashTableBucket* foundBucket = find(key); foundBucket != nullptr) {
        foundBucket->unload();
        --numFilled;
        return true;
    }
    return false; // key is not present in table
}

/**
 * @brief Time-complexity testing version of insert.
 *
 * Like insert, but returns number of probes required to either insert key-value pair
 * or determine key is a duplicate or table is full.
 * Also omits check for rehashing
 *
 * @param key of key-value pair to be inserted.
 * @param value Value of key-value pair to be inserted.
 *
 * @return number of probes required for insertion.
 */
size_t HashTable::insertTCT(const std::string& key, const size_t& value) {
    const size_t home = hash(key) % capacity();
    HashTableBucket* firstEARFound = nullptr;
    for (size_t probeNum = 0; probeNum < offsets.size(); ++probeNum) {
        if (HashTableBucket* currBucket = &tableData.at((home + offsets.at(probeNum)) % capacity());
        currBucket->isEmpty()) {
            if (currBucket->isESS()) { // If ESS bucket is encountered, insert into it or first EAR bucket found earlier during search.
                if (firstEARFound != nullptr) {currBucket = firstEARFound;}
                currBucket->load(key,value);
                ++numFilled;
                return probeNum + 1;
            }
            if (firstEARFound == nullptr) { // Mark first EAR bucket found.
                firstEARFound = currBucket;
            }
        }
        else if (key == currBucket->getKey()) { // Stop searching if duplicate key found.
            return probeNum + 1;
        }
    }
    if (firstEARFound != nullptr) { // Insert at first EAR bucket encountered if all empty buckets are tombstones.
        firstEARFound->load(key,value);
        ++numFilled;
    }
    return capacity(); // Return table capacity if table is full.
}

/**
 * @brief Time-complexity testing version of remove.
 *
 * Like remove, but returns number of probes required to
 * either insert key-value pair or determine key is not in the table.
 *
 * @param key Key to be searched.
 * @return number of probes required for removal.
 */
size_t HashTable::removeTCT(const std::string& key) {
    const size_t home = hash(key) % capacity();
    for (size_t probeNum = 0; probeNum < offsets.size(); ++probeNum) {
        HashTableBucket *currBucket = &tableData.at((home + offsets.at(probeNum)) % capacity());
        if (currBucket->isESS()) { // If ESS bucket is reached, key cannot be present in table.
            return probeNum + 1;
        }
        if (currBucket->isEAR()) { // Continue probing if bucket holds tombstone.
            continue;
        }
        if (currBucket->getKey() == key) { // Remove key-value pair if found.
            currBucket->unload();
            --numFilled;
            return probeNum + 1;
        }
    }
    return capacity(); //Will only be reached if the key is not present and the table is full or all empty buckets are tombstones.
}

/**
 * @brief Rehashes the table, increasing its size.
 *
 * Increases the size of hash table by resizeFactor and reinserts all key-value pairs
 * from the older version of the table.
 *
 */
void HashTable::rehash() {
    HashTable newTable(capacity() * resizeFactor); // A new random offsets vector is created during construction.
    for (size_t bucketNum = 0; bucketNum < capacity(); ++bucketNum) {
        if (const HashTableBucket *currBucket = &tableData.at(bucketNum);
        !currBucket->isEmpty()) {
            newTable.insertIntoNewTable(currBucket->getKey(),currBucket->getValue()); // Insert key-value pair into new table.
        }
        // Stop searching for filled buckets if all filled buckets from old table version have been copied.
        if (this->numFilled == newTable.numFilled) {
            break;
        }
    }
    this->tableData = newTable.tableData;
    this->offsets = newTable.offsets;
}

/**
 * @brief Insert key-value pair into a new table during rehashing.
 *
 * Simplified private helper version of the insert method for the case where key-value pairs are
 * being inserted into a new table during rehashing.
 * The check for duplicates and resizing the table are unnecessary, and elements may be inserted
 * at the first empty bucket found.
 *
 * @param key of key-value pair to be inserted.
 * @param value Value of key-value pair to be inserted.
 *
 * @return true if insertion successful, false otherwise.
 */
bool HashTable::insertIntoNewTable(const std::string& key, const size_t& value) {
    const size_t home = hash(key) % capacity();
    for (const size_t offset : offsets) {
        if (HashTableBucket* currBucket = &tableData.at((home + offset) % capacity());
        currBucket->isESS()) {
            currBucket->load(key,value);
            ++numFilled;
            return true;
        }
    }
    return false; // Should not be possible if resizeFactor is greater than 1.
}


/**
 * @brief Find bucket containing key.
 *
 * Returns pointer to a bucket containing the given key, or returns nullptr.
 * Private helper method for pseudo-random probing.
 * Returns a pointer to the bucket with the key if the search is successful.
 * Returns nullptr if the key is not present in the hash table.
 *
 * @param key Key to be searched.
 * @return Pointer to found bucket, or nullptr.
 */
HashTable::HashTableBucket* HashTable::find(const std::string& key) {
    const size_t home = hash(key) % capacity();
    for (const size_t offset : offsets) {
        HashTableBucket* currBucket = &tableData.at((home + offset) % capacity());
        if (currBucket->isESS()) { // If ESS bucket is reached, key cannot be present in table.
            return nullptr;
        }
        if (currBucket->isEAR()) { // Continue probing if bucket holds tombstone.
            continue;
        }
        if (currBucket->getKey() == key) { // Return bucket pointer if key found.
            return currBucket;
        }
    }
    return nullptr; //Will only be reached if the key is not present and the table is full or all empty buckets are tombstones.
}

/**
 * @brief Default constructor for HashTableBucket.
 *
 * Constructs bucket with type ESS.
 * While unnecessary, also sets key as empty string and value as 0 for easy analysis.
 */
HashTable::HashTableBucket::HashTableBucket() :
    key(""), value(0), type(BucketType::ESS) {}

/**
 * @brief Parameterized constructor for HashTableBucket.
 *
 * Constructs filled bucket with given key-value pair and type NORMAL
 *
 * @param key Key for hash table entry
 * @param value Value for hash table entry
 */
HashTable::HashTableBucket::HashTableBucket(const std::string& key, const size_t& value) :
    key(key), value(value), type(BucketType::NORMAL) {}

/**
 * @brief Getter for key stored in hash table bucket.
 *
 * @warning A bucket of type EAR may hold a previously removed key.
 * @return Key stored in hash table bucket.
 */
std::string HashTable::HashTableBucket::getKey() const{
    return key;
}

/**
 * @brief Getter for value stored in hash table bucket.
 *
 * @warning A bucket of type EAR may hold a previously removed key.
 * @return Value stored in hash table bucket.
 */
size_t HashTable::HashTableBucket::getValue() const {
    return value;
}

/**
 * @brief Getter for reference to value stored in hash table bucket.
 *
 * For use with subscript operator overload of HashTable.
 * Allows for mutation of stored value.
 *
 * @warning A bucket of type EAR may hold a previously removed key.
 * @return Reference to value stored in hash table bucket.
 */
size_t& HashTable::HashTableBucket::getValueRef() {
    return value;
}

/**
 * @brief Predicate for determining if bucket is empty.
 *
 * If empty, bucket has either never been filled or emptied after removal.
 *
 * @return true if empty, false if filled.
 */
bool HashTable::HashTableBucket::isEmpty() const {
    return type != BucketType::NORMAL;
}

/**
 * @brief Predicate for determining if bucket is tombstone.
 *
 * A bucket is a tombstone if its type is EAR,
 * meaning that it has been emptied after being filled since the table was created or last rehashed.
 *
 * @return true if tombstone, false if not.
 */
bool HashTable::HashTableBucket::isEAR() const {
    return type == BucketType::EAR;
}

/**
 * @brief Predicate for determining if bucket has never been filled.
 *
 * A bucket has type ESS if it has never been filled since the table was created or last rehashed.
 *
 * @return true if never filled, false if not.
 */
bool HashTable::HashTableBucket::isESS() const {
    return type == BucketType::ESS;
}

/**
 * @brief Fills bucket with key-value pair.
 *
 * Marks bucket as filled (type NORMAL)
 *
 * @param inKey key to be stored
 * @param inValue value to be stored
 */
void HashTable::HashTableBucket::load(const std::string& inKey, const size_t& inValue) {
    this->key = inKey;
    this->value = inValue;
    this->type = BucketType::NORMAL;
}

/**
 * @brief Empties bucket
 *
 * Marks bucket as EAR, effectively rendering its contents inaccessible.
 *
 * @warning The key-value pair remain in memory and may be accessed using getKey(), getValue(), or [].
 */
void HashTable::HashTableBucket::unload() {
    this->type = BucketType::EAR;
}

/**
 * @brief Stream insertion operator overload for HashTableBucket.
 *
 * Outputs bucket content in the form "<key, value>"
 *
 * @warning Does not check if bucket is empty.
 *
 * @param os output stream
 * @param bucket hash table bucket to be output
 * @return output stream with bucket output added
 */
std::ostream& operator<<(std::ostream& os, const HashTable::HashTableBucket& bucket) {
    os << "<" << bucket.getKey() << ", " << bucket.getValue() << ">";
    return os;
}

/**
 * @brief Stream insertion operator overload for HashTable.
 *
 * Outputs the filled buckets in the table bucket-by-bucket on separate lines.
 * The bucket number prepended is prepended to the contents.
 *
 * @param os output stream
 * @param hashTable hash table to be output
 * @return output stream with hash table output added
 */
std::ostream& operator<<(std::ostream& os, const HashTable& hashTable) {
    for (size_t bucketNum = 0; bucketNum < hashTable.capacity(); ++bucketNum) {
        if (HashTable::HashTableBucket currBucket = hashTable.tableData.at(bucketNum);
        !currBucket.isEmpty()) {
            os << "Bucket " << bucketNum << ": " << currBucket << std::endl;
        }
    }
    return os;
}