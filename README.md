[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/7RAFczHS)
# Project 4 - HashTable

Project description can be found [in the repo](Project4_HashTable.pdf)

Place your complexity analysis below.

---



The five methods for which we are supposed to perform time complexity analysis are the following:
* insert
* remove
* contains
* get
* operator[]

In my hash table implementation, the latter four methods all call the private helper method find, which  
searches through the table using pseudo-random probing until it either:
1. Finds a bucket containing the key being searched.
2. Finds a bucket marked ESS.
3. Searches every bucket in the table. 

The last case only occurs if the key is not in the table and every bucket is full or every empty bucket is a tombstone.  
All four methods then perform a different O(1) series of operations and return a different data type, but their time  
complexity is tied to that of find. The insert method does not call find, but it does perform the same number of  
searches for a specific key. It uses the same pseudo-random probing algorithm with the same three termination condtions  
are the same, and the operations performed by insert in each case are also O(1). The time complexity of all five methods   
should therefore be the same; namely, the time complexity of searching for an ESS bucket or bucket with duplicate key  
using pseudo-random probing.

Theoretically, the time complexity of pseudo-random probing should be of constant order (O(1)) and scale with only the  
load factor of the table $\alpha$. Specifically, the expectation value for the number of probes necessary to find an  
empty bucket should scale as $\frac{1}{1-\alpha}$, assuming the hash function is well randomized and the effect of  
tombstones is neglected.  

My HashTableDebug class contains a method which tests the time complexity of insert and remove by determining the  
average number of probes needed to insert and remove multiple randomized key-value pairs for a series of chosen load  
factors and table capacities. The results are in keeping with the theoretical prediction: For tests involving 100  
randomized pairs over an exponentially increasing capacity range of 1e4, 1e5, and 1e6 and load factors ranging from  
0.1 to 0.9, the number of probes required for insertion and removal of a given key tracked very closely. Moreover, the  
averages did not scale with the capacity of the table, but instead with the load factor as expected, and the results  
were consistently close to the predicted value of $\frac{1}{1-\alpha}$. I am confident to conclude that all five methods  
have constant order time complexity. The testing further verifies that if an average probe number less than 2 per  
insert/remove is desired, then $\alpha$ = 0.5 is a good choice for the rehashing threshold for the hash table.

While not requested by the project prompt and not explicitly tested, I expect the keys and rehash methods  
all have O(N) time complexity given that each method must iterate over at least as many buckets as there are  
key-value pairs in the table and cannot utilize the benefits of pseudo-random probing. All remaining public methods   
should be O(1) time complexity.