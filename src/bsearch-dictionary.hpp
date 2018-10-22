// Copyright 2018 Hunter L. Allen
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Implementation of a dictionary using an array and binary search
// The class inherits from ArrayDictionary
#ifndef BSEARCH_DICTIONARY_HPP_
#define BSEARCH_DICITONARY_HPP_
#include <stdlib.h>
#include <string>
#include <iostream>
#include <stdio.h>

#include "dictionary.hpp"

#define leftC(i) (((i) << 1) + 1)
#define rightC(i) (((i) << 1) + 2)
#define parent(i) (((i) - 1) >> 1)

struct ArrayDictionaryNode
{
  ArrayDictionaryNode() {}

  ArrayDictionaryNode(const ArrayDictionaryNode & other)
  {
    key = other.key;
    data = other.data;
  }

  QString key;
  int data;

  bool operator==(ArrayDictionaryNode * a1);
  bool operator==(QString str);
  void operator=(ArrayDictionaryNode a1);
};

struct heap
{
private:
  int n;
  int max;
  ArrayDictionaryNode * array;

public:
  heap(int max);
  ~heap();
  bool insert(QString key, int data);
  ArrayDictionaryNode removeMin();
  bool remove(QString key);
  ArrayDictionaryNode * getArray();
  int getSize();
  int find(QString key);
};

class BinarySearchDictionary : public Dictionary
{
  // Add any member variables you need
  bool sorted;
  heap * h;

public:
  // Constructor
  BinarySearchDictionary();
  ~BinarySearchDictionary();

  bool addRecord(QString key, int record);

  // Sort array using heap sort.
  void sort();

  int find(QString key);

  // Removes one element from the table
  bool removeElement(QString key);

  // Returns all the elements in the table as an array of strings.
  // *n is the size of the table and it is returned by reference
  QString * keys(int * n);

  // Add other methods you may need
  int size() {return h->getSize();}
  void clear();
};
#endif  // BSEARCH_DICTIONARY_HPP_
