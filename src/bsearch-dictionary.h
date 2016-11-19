// Implementation of a dictionary using an array and binary search
// The class inherits from ArrayDictionary
#include <stdlib.h>
#include <string>
#include <iostream>
#include <stdio.h>

#include "array-dictionary.h"

#define leftC(i) (2*(i)+1)
#define rightC(i) (2*(i)+2)
#define parent(i) (((i)-1)/2)

struct heap {
private:
  int n;
  int max;
  ArrayDictionaryNode * array;
public:
  heap(int max);
  bool insert(QString key, int data);
  ArrayDictionaryNode removeMin();
  bool remove(QString key);
  ArrayDictionaryNode* getArray();
  int getSize();
  int find(QString key);
  ~heap();
};

class BinarySearchDictionary : public ArrayDictionary {
  // Add any member variables you need
  bool sorted;
  heap * h;
 public:
  // Constructor
  BinarySearchDictionary();
  
  bool addRecord(QString key, int record);

  // Sort array using heap sort.
  void sort();

  int find(QString key);

  // Removes one element from the table
  bool removeElement(QString key);

  // Returns all the elements in the table as an array of strings.
  // *n is the size of the table and it is returned by reference
  QString* keys(int * n);

  // Add other methods you may need
  int size(){return h->getSize();};
  void clear();
};

