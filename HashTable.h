#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <optional>
#include <random>
#include <string>
#include <vector>

/*
 * Greg Rosen
 * Project 4: HashTable
 * Declaration for HashTable class
 */

/*
 * HashTable: A map with string keys and non-negative integer values
 *
 */
class HashTable {
private:
    class HashTableBucket {
    public:
        /*
         * BucketType: History type of HashTableBucket
         * NORMAL - Bucket contains key-value pair
         * ESS - "Empty Since Start" - Bucket has never been filled since HashTable was last created/resized
         * EAR - "Empty After Removal" - Tombstone - Bucket is empty, but has been filled since HashTable was last created/resized
         */
        enum class BucketType {NORMAL, ESS, EAR};

    private:
        std::string key; // Key for mapping
        size_t value; // Value mapped
        BucketType type; // See enum BucketType

    public:
        HashTableBucket(); // Default constructor
        HashTableBucket(const std::string& key, const size_t& value); // Parameterized constructor

        [[nodiscard]] std::string getKey() const;
        [[nodiscard]] size_t getValue() const;
        size_t& getValueRef();
        [[nodiscard]] BucketType getType() const;

        [[nodiscard]] bool isEmpty() const; // Predicate for determining if bucket is empty (type == ESS or EAR)
        [[nodiscard]] bool isEAR() const; // Predicate for determining if bucket type is EAR (tombstone)
        [[nodiscard]] bool isESS() const; // Predicate for determining if bucket type is ESS

        void load(const std::string& inKey, const size_t& inValue); // Fills bucket with key-value pair
        void unload();
    };

    std::vector<HashTableBucket> tableData; // The Hash Table itself, implemented as a vector of HashTableBucket elements
    std::vector<size_t> offsets; // Offsets for pseudo-random probing
    size_t numFilled; // The number of filled buckets in the Hash Table (NORMAL status);
    std::mt19937 rng; // Random number generator for pseudo-random probing
    std::hash<std::string> hash;
    size_t badKeyDrain;

    void resize(); // Double the size of the hashTable, rehashing in the process.
    HashTableBucket* find(const std::string& key); // Find a bucket containing key-value pair with input key, or return nullptr

public:
    explicit HashTable(size_t initCapacity = 8); // Default and parameterized constructor

    size_t& operator[](const std::string& key); // Subscript operator overload

    [[nodiscard]] double alpha() const; // Getter for alpha (load factor)
    [[nodiscard]] size_t capacity() const; // Getter for capacity (# of buckets in table)
    [[nodiscard]] size_t size() const; // Getter for size (# of filled buckets in table)
    [[nodiscard]] std::vector<std::string> keys() const; // Getter for list of keys currently used in table
    [[nodiscard]] std::optional<size_t> get(const std::string& key); // Getter for value stored using given key

    [[nodiscard]] bool contains(const std::string& key); // Predicate for determining whether given key is used in table

    bool insert(const std::string& key, const size_t& value, bool skipResizeCheckFlag = false); // Insert key-value pair into table, resizing if required
    bool remove(const std::string& key); // Remove key-value pair from table

    friend std::ostream& operator<<(std::ostream& os, const HashTableBucket& bucket); // Stream insertion operator overload HashTableBucket
    friend std::ostream& operator<<(std::ostream& os, const HashTable& hashTable); // Stream insertion operator overload for HashTable
};

#endif // HASHTABLE_H
