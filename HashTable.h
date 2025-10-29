#ifndef HASHTABLE_H
#define HASHTABLE_H

/*
 * Greg Rosen
 * Project 4: HashTable
 * Declaration for HashTable class
 */

#include <optional>
#include <random>
#include <string>
#include <vector>

/**
 * @class HashTable
 * @brief HashTable for <string, unsigned long> key-value pairs
 *
 * Hash Table implementation for string keys and unsigned long (size_t) values.
 * Hash Table is stored internally as a std::vector.
 * Uses the hash template of the std library as applied to strings (std::hash<std::string>) for the hash function.
 * Uses pseudo-random probing for collision resolution.
 * Rehashes whenever load factor reaches or exceeds 0.5, at which point the table doubles in size.
 *
 * @author Greg Rosen
 * @date October 28, 2025
 */
class HashTable {
private:
    /**
     * @class HashTableBucket
     * @brief Bucket for HashTable
     *
     * Bucket for Hash Table
     * Stores key, value, and type
     */
    class HashTableBucket {
    public:
        /**
         * @enum BucketType
         * @brief History type of HashTableBucket.
         *
         * History type of HashTableBucket.
         * NORMAL - Bucket contains key-value pair.
         * ESS - "Empty Since Start" - Bucket has never been filled since the hash table was last created/resized.
         * EAR - "Empty After Removal" - Tombstone - Bucket is empty, but has been filled since HashTable was last created/resized.
         */
        enum class BucketType {NORMAL, ESS, EAR};

    private:
        std::string key; // Key for hash table entry.
        size_t value; // Value for hash table entry.
        BucketType type; // History type for bucket.

    public:
        HashTableBucket(); // Default constructor for HashTableBucket.
        HashTableBucket(const std::string& key, const size_t& value); // Parameterized constructor for HashTableBucket.

        [[nodiscard]] std::string getKey() const; // Getter for key stored in hash table bucket.
        [[nodiscard]] size_t getValue() const; // Getter for value stored in hash table bucket.
        [[nodiscard]] size_t& getValueRef(); // Getter for reference to value stored in hash table bucket.
        [[nodiscard]] BucketType getType() const; // Getter for type of hash table bucket.

        [[nodiscard]] bool isEmpty() const; // Predicate for determining if bucket is empty.
        [[nodiscard]] bool isEAR() const; // Predicate for determining if bucket is tombstone.
        [[nodiscard]] bool isESS() const; // Predicate for determining if bucket has never been filled.

        void load(const std::string& inKey, const size_t& inValue); // Fills bucket with key-value pair.
        void unload(); // Empties bucket.
    };

    std::vector<HashTableBucket> tableData; // The Hash Table itself, implemented as a vector of HashTableBucket elements.
    std::vector<size_t> offsets; // Offsets for pseudo-random probing.
    size_t numFilled; // The number of filled buckets in the hash table.
    std::hash<std::string> hash; // Using () overload, effectively provides hash function size_t hash(std::string)
    size_t badKeyDrain; // Dummy variable for capturing invalid uses of subscript operator.

    void rehash(); // Rehashes the table, doubling its size.
    HashTableBucket* find(const std::string& key); // Returns pointer to a bucket containing the given key, or returns nullptr.

public:
    explicit HashTable(size_t initCapacity = 8); // Default and parameterized constructor for hash table.

    size_t& operator[](const std::string& key); // Subscript operator overload for hash table.

    [[nodiscard]] size_t capacity() const; // Getter for capacity of the hash table.
    [[nodiscard]] size_t size() const; // Getter for size of the hash table.
    [[nodiscard]] double alpha() const; // Getter for the load factor of the hash table.
    [[nodiscard]] std::vector<std::string> keys() const; // Getter for a list of keys currently used in the hash table.
    [[nodiscard]] std::optional<size_t> get(const std::string& key); // Getter for value stored using a given key.

    [[nodiscard]] bool contains(const std::string& key); // Predicate for if a given key is stored in table.

    bool insert(const std::string& key, const size_t& value, bool skipResizeCheckFlag = false); // Insert key-value pair into table.
    bool remove(const std::string& key); // Remove key-value pair from table.

    friend std::ostream& operator<<(std::ostream& os, const HashTableBucket& bucket); // Stream insertion operator overload for HashTableBucket.
    friend std::ostream& operator<<(std::ostream& os, const HashTable& hashTable); // Stream insertion operator overload for HashTable.
};

#endif // HASHTABLE_H
