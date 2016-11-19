#if !defined ARRAY_DICTIONARY_H
#define ARRAY_DICTIONARY_H

// Implementation of a dictionary using an array and sequential search
// The array will be dynamically resized if necessary
#include <QString>
#include <functional>
#include <stdlib.h>
#include "dictionary.h"

struct ArrayDictionaryNode {
  QString key;
  int data;

  bool operator== (ArrayDictionaryNode * a1);
  bool operator== (QString str);
  void operator= (ArrayDictionaryNode a1);
};

class ArrayDictionary : public Dictionary {

 protected:
  ArrayDictionaryNode * array;
  int maxsize = 100;
  int currentNumber;
 public:
  // Constructor
  ArrayDictionary();
  
  // Add a record to the dictionary. Returns false if key already exists
  bool addRecord(QString key, int record);
  
  // Find a key in the dictionary and return corresponding record or NULL
  // Use sequential search
  int findRecord(QString key);

  // Removes one element from the table
  bool removeElement(QString key);

  // Returns all the elements in the table as an array of strings.
  // *n is the size of the table and it is returned by reference
  QString* keys(int * n);

  // Add other methods you may need
};

#endif

