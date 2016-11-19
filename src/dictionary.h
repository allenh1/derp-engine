
// Abstract class for a string dictionary

#if !defined DICTIONARY_H
#define DICTIONARY_H

#include <QString>

class Dictionary {
public:
    // Add a record to the dictionary. Returns false if key already exists
    virtual bool addRecord(QString key, int record) = 0;

    // Removes one element from the table
    virtual bool removeElement(QString key) = 0;

    // Returns all the elements in the table as an array of strings.
    // *n is the size of the table and it is returned by reference
    virtual QString* keys(int * n)=0;
};

#endif

