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

#ifndef QSTRING_HASH_HPP_
#define QSTRING_HASH_HPP_
#include <QString>

namespace std
{
template<>
struct hash<QString>
{
  std::size_t operator()(const QString & s) const noexcept
  {
    return qHash(s);
  }
};
}  // namespace std
#endif  // QSTRING_HASH_HPP_
