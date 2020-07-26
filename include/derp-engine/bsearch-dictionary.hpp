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
#ifndef DERP_ENGINE__BSEARCH_DICTIONARY_HPP_
#define DERP_ENGINE__BSEARCH_DICTIONARY_HPP_

#include <derp-engine/dictionary.hpp>

// Implementation of a dictionary using an array and binary search
// The class inherits from ArrayDictionary
#include <string>
#include <iostream>
#include <memory>
#include <utility>

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
  explicit heap(int max);
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
public:
  // Constructor
  BinarySearchDictionary();
  ~BinarySearchDictionary();

  bool add_record(const QString & key, int record) override;

  // Sort array using heap sort.
  void sort();

  int find(QString key);

  // Removes one element from the table
  bool remove_element(const QString & key) override;

  // Returns all the elements in the table as an array of strings.
  // *n is the size of the table and it is returned by reference
  std::unique_ptr<QString[]> keys(size_t * n) override;

  // Add other methods you may need
  size_t size()
  {
    return h->getSize();
  }
  void clear();

private:
  // Add any member variables you need
  bool sorted;
  std::unique_ptr<heap> h;
};
#endif  // DERP_ENGINE__BSEARCH_DICTIONARY_HPP_
