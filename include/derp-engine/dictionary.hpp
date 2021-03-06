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

// Abstract class for a string dictionary

#ifndef DERP_ENGINE__DICTIONARY_HPP_
#define DERP_ENGINE__DICTIONARY_HPP_

#include <QString>

#include <memory>

class Dictionary
{
public:
  // Add a record to the dictionary. Returns false if key already exists
  virtual bool add_record(const QString & key, int record) = 0;

  // Removes one element from the table
  virtual bool remove_element(const QString & key) = 0;

  // Returns all the elements in the table as an array of strings.
  // *n is the size of the table and it is returned by reference
  virtual std::unique_ptr<QString[]> keys(size_t * n) = 0;
};

#endif  // DERP_ENGINE__DICTIONARY_HPP_
