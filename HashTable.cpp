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
 */
HashTable::HashTable(const size_t initCapacity) :
    tableData(initCapacity), offsets(initCapacity), numFilled(0), badKeyDrain(0) {
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
 * hashTable["name"] = 5 will change that value
 * If the key is not in the table, the returned reference will point to a dummy numerical field of the HashTable.
 * NOTE: There is no explicit indication that the key was not present and the value is a dummy.
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
 * This value is tracked internally and stored in the field numFilled.
 *
 * @return size of hash table
 */
size_t HashTable::size() const {
    return numFilled;
}

/**
 * @brief Getter for load factor of hash table.
 *
 * Calculated as the ratio between the number of filled buckets (size)
 * and the total number of buckets in the table (capacity).
 *
 * @return load factor of hash table
 */
double HashTable::alpha() const {
    return static_cast<double>(size())/static_cast<double>(capacity());
}

/**
 * @brief Getter for a list of keys currently used in the hash table.
 *
 * The list is returned as a vector of strings.
 * The method may in the worst case (last bucket of table filled) iterate over every bucket in the hash table,
 * so its time complexity is O(capacity).
 *
 * @return vector of keys present in the hash table.
 */
std::vector<std::string> HashTable::keys() const {
    std::vector<std::string> keyList(numFilled); // Size of keyList is known in advance.
    for (size_t keyListIndex = 0, bucketNum = 0; bucketNum < capacity(); ++bucketNum) {
        if (const HashTableBucket *currBucket = &tableData.at(bucketNum);
        !currBucket->isEmpty()) {
            keyList.at(keyListIndex) = currBucket->getKey();
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
 * The bucket to be filled is found using pseudo-random probing
 * Returns true if insertion is successful.
 * Returns false if the key is already present in the hash table.
 * Also returns false if the table is full, which should not be possible if the table
 * begins empty and is rehashed correctly after insertions.
 * If the insertion raises the load factor of the hash table to the threshold of 50%, the table is rehashed.
 * The optional parameter skipRehashCheckFlag (default false) will skip the load factor test, which is useful
 * during rehashing or any other time it is certain that the load factor will not reach the rehash threshold.
 * @param key of key-value pair to be inserted.
 * @param value Value of key-value pair to be inserted.
 * @param skipRehashCheckFlag Flag for skipping load factor check.
 * @return true if insertion successful, false otherwise.
 */
bool HashTable::insert(const std::string& key, const size_t& value, const bool skipRehashCheckFlag) {
    const size_t home = hash(key) % capacity();
    for (size_t probeNum = 0; probeNum < offsets.size(); ++probeNum) {
        HashTableBucket* currBucket = &tableData.at((home + offsets.at(probeNum)) % capacity());
        if (currBucket->isEmpty()) {
            currBucket->load(key,value);
            ++numFilled;
            if (!skipRehashCheckFlag && alpha() >= 0.5) {
                rehash();
            }
            return true;
        }
        if (key == currBucket->getKey()) {
            return false;
        }
    }
    return false; // This line can only be reached if the table is full.
}

/**
 * @brief Remove key-value pair from table.
 *
 * Searches for key using the helper method find.
 * The bucket is marked EAR, making its contents inaccessible.
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
    return false;
}

/**
 * @brief Rehashes the table, doubling its size.
 *
 * Doubles size of hash table and reinserts all key-value pairs
 * from the older version of the table.
 * As the new table version is guaranteed to have a load factor less than 0.5,
 * load factor checking may be bypassed.
 */
void HashTable::rehash() {
    HashTable newTable(capacity() * 2); // A new random offsets table is created during construction.
    for (size_t bucketNum = 0; bucketNum < capacity(); ++bucketNum) {
        if (const HashTableBucket *currBucket = &tableData.at(bucketNum);
        !currBucket->isEmpty()) {
            newTable.insert(currBucket->getKey(),currBucket->getValue(),true); //
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
 * @brief Returns pointer to a bucket containing the given key, or returns nullptr.
 *
 * The key is searched using pseudo-random probing.
 * Returns a pointer to the bucket with the key if the search is successful.
 * Returns nullptr if the key is not present in the hash table.
 *
 * @param key Key to be searched.
 * @return Pointer to found bucket, or nullptr.
 */
HashTable::HashTableBucket* HashTable::find(const std::string& key) {
    const size_t home = hash(key) % capacity();
    for (size_t probeNum = 0; probeNum < offsets.size(); ++probeNum) {
        HashTableBucket* currBucket = &tableData.at((home + offsets.at(probeNum)) % capacity());
        if (currBucket->isESS()) { // If ESS bucket is reached, key cannot be present in table.
            return nullptr;
        }
        if (currBucket->isEAR()) { // Continue probing if bucket holds tombstone.
            continue;
        }
        if (currBucket->getKey() == key) {
            return currBucket;
        }
    }
    /*
     * Will only be reached if the key is not present and the table is full or all empty buckets are tombstones.
     * Represents the worst-case scenario for time complexity of key searching.
    */
    return nullptr;
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
 * @warning If bucket is empty, returned key will be empty string.
 * @return Key stored in hash table bucket.
 */
std::string HashTable::HashTableBucket::getKey() const{
    return key;
}

/**
 * @brief Getter for value stored in hash table bucket.
 *
 * @warning If bucket is empty, returned value will be 0. A non-empty bucket may ALSO store the value 0.
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
 * @warning If bucket is empty, returned value will be 0. A non-empty bucket may ALSO store the value 0.
 * @return Reference to value stored in hash table bucket.
 */
size_t& HashTable::HashTableBucket::getValueRef() {
    return value;
}

/**
 * @brief Getter for type of hash table bucket.
 *
 * Type may be:
 * NORMAL - Bucket contains key-value pair.
 * ESS - "Empty Since Start" - Bucket has never been filled since the hash table was last created/rehashed.
 * EAR - "Empty After Removal" - Tombstone - Bucket is empty, but has been filled since HashTable was last created/rehashed.
 *
 * @return type of hash table bucket.
 */
HashTable::HashTableBucket::BucketType HashTable::HashTableBucket::getType() const {
    return type;
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
 */
void HashTable::HashTableBucket::unload() {
    this->type = BucketType::EAR;
}

/**
 * @brief Stream insertion operator overload for HashTableBucket.
 *
 * Outputs bucket content in the form "<key, value>"
 *
 * @warning If empty bucket is read, resulting output will be "<,0>".
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