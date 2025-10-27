/*
 * Greg Rosen
 * Project 4: HashTable
 * Implementation of HashTable and HashTableBucket classes
 */

#include "HashTable.h"
#include <algorithm>
#include <ostream>
#include <random>

/**
* Only a single constructor that takes an initial capacity for the table is
* necessary. If no capacity is given, it defaults to 8 initially
*/
HashTable::HashTable(size_t initCapacity) : tableData(initCapacity), offsets(initCapacity-1), numFilled(0)  {
    for (size_t i = 0; i < initCapacity-1; ++i) {
        offsets[i] = i + 1;
    }
    std::random_device rd;
    std::mt19937 rng(rd());
    std::ranges::shuffle(offsets, rng);
}

/**
* The bracket operator lets us access values in the map using a familiar syntax,
* similar to C++ std::map or Python dictionaries. It behaves like get, returnin
* the value associated with a given key:
int idNum = hashTable[“James”];
* Unlike get, however, the bracker operator returns a reference to the value,
* which allows assignment:
hashTable[“James”] = 1234;
If the key is not
* in the table, returning a valid reference is impossible. You may choose to
* throw an exception in this case, but for our implementation, the situation
* results in undefined behavior. Simply put, you do not need to address attempts
* to access keys not in the table inside the bracket operator method.
*/
size_t& HashTable::operator[](const std::string& key) {

}

/**
* alpha returns the current load factor of the table, or size/capacity. Since
* alpha returns a double,make sure to properly cast the size and capacity, which
* are size_t, to avoid size_teger division. You can cast a size_t num to a double
* in C++ like:
static_cast<double>(num)
The time complexity
* for this method must be O(1).
*/
double HashTable::alpha() const {
    return static_cast<double>(size())/capacity();
}

/**
* capacity returns how many buckets in total are in the hash table. The time
* complexity for this algorithm must be O(1).
*/
size_t HashTable::capacity() const {
    return tableData.size();
}

/**
* The size method returns how many key-value pairs are in the hash table. The
* time complexity for this method must be O(1)
*/
size_t HashTable::size() const {
    return numFilled;
}

/**
* keys returns a std::vector (C++ version of ArrayList, or simply list/array)
* with all of the keys currently in the table. The length of the vector should be
* the same as the size of the hash table.
*/
std::vector<std::string> HashTable::keys() const {
    std::vector<std::string> keyList(numFilled);
    size_t keyListIndex = 0;
    for (size_t i = 0; i < capacity(); ++i) {

    }
}

/**
* If the key is found in the table, find will return the value associated with
* that key. If the key is not in the table, find will return something called
* nullopt, which is a special value in C++. The find method returns an
* optional<int>, which is a way to denote a method might not have a valid value
* to return. This approach is nicer than designating a special value, like -1, to
* signify the return value is invalid. It's also much better than throwing an
* exception if the key is not found.
*/
std::optional<size_t> HashTable::get(const std::string& key) const {}

/**
* contains returns true if the key is in the table and false if the key is not in
* the table.
*/
bool HashTable::contains(const std::string& key) const {}

/**
* Insert a new key-value pair into the table. Duplicate keys are NOT allowed. The
* method should return true if the insertion was successful. If the insertion was
* unsucessful, such as when a duplicate is attempted to be inserted, the method
* should return false
*/
bool HashTable::insert(const std::string& key, const size_t& value) {}
/**
* If the key is in the table, remove will “erase” the key-value pair from the
* table. This might just be marking a bucket as empty-after-remove
*/
bool HashTable::remove(const std::string& key) {}

/**
* The default constructor can simply set the bucket type to ESS.
*
*/
HashTable::HashTableBucket::HashTableBucket() :
    type(BucketType::ESS) {}

/**
* A parameterized constructor could initialize the key and value, as
* well as set the bucket type to NORMAL.
*/
HashTable::HashTableBucket::HashTableBucket(const std::string& key, const size_t& value) :
    key(key), value(value), type(BucketType::NORMAL) {}


std::string HashTable::HashTableBucket::getKey() const{
    return key;
}

size_t HashTable::HashTableBucket::getValue() const {
    return value;
}

HashTable::HashTableBucket::BucketType HashTable::HashTableBucket::getType() const {
    return type;
}

/**
* This method would return whether the bucket is empty, regardless of
* if it has had data placed in it or not.
*/
bool HashTable::HashTableBucket::isEmpty() const {
    return type != BucketType::NORMAL;
}

/**
* A load method could load the key-value pair into the bucket, which
* should then also mark the bucket as NORMAL.
*/
void HashTable::HashTableBucket::load(const std::string& key, const size_t& value) {
    this->key = key;
    this->value = value;
    this->type = BucketType::NORMAL;
}

/**
* The stream insertion operator could be overloaded to print the
* bucket's contents. Or if preferred, you could write a print method
* instead.
*/
std::ostream& operator<<(std::ostream& os, const HashTable::HashTableBucket& bucket) {
    if (bucket.getType() == HashTable::HashTableBucket::BucketType::NORMAL) {
        os << bucket.getValue();
    }
    return os;
}

/**
* operator<< is another example of operator overloading in C++, similar to
* operator[]. The friend keyword only needs to appear in the class declaration,
* but not the definition. In addition, operator<< is not a method of HashTable,
* so do not put HashTable:: before it when defining it. operator<< will allow us
* to print the contents of our hash table using the normal syntax:
cout <<
* myHashTable << endl;
You should only print the buckets which are occupied,
* and along with each item you will print which bucket (the index of the bucket)
* the item is in. To make it easy, I suggest creating a helper method called
* something like printMe() that returns a string of everything in the table. An
* example which uses open addressing for collision resolution could print
* something like:
Bucket 5: <James, 4815>
Bucket 2: <Juliet, 1623>
Bucket
* 11: <Hugo, 42108>
*/
std::ostream& operator<<(std::ostream& os, const HashTable& hashTable) {

}