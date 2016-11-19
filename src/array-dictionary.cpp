// Implementation of a dictionary using an array and sequential search
// The array will be dynamically resized if necessary
#include "array-dictionary.h"

bool ArrayDictionaryNode::operator== (ArrayDictionaryNode * a1) {
	return (!this->key.compare(a1->key));
}

bool ArrayDictionaryNode::operator== (QString str) {
	return (!this->key.compare(str));
}

void ArrayDictionaryNode::operator= (ArrayDictionaryNode a1) {
	this->key=a1.key;
	this->data=a1.data;
}

// Constructor
ArrayDictionary::ArrayDictionary() {
	array = new ArrayDictionaryNode[maxsize];
	for(int i=0; i < maxsize; i++) {
		array[i].key="";
		array[i].data=-1;
	}
	currentNumber=0;
}

// Add a record to the dictionary. Returns false if key already exists
bool ArrayDictionary::addRecord(QString key, int record)
{
	// Add your code here
	if(key.isEmpty() && record<0) return false;
    // check if the key already exists
    for(int i=0;i < currentNumber;i++) {
		if(array[i] == key) {
			array[i].data=record; return false;
		}
    }
    // resize if needed
    if(currentNumber==maxsize) {
		maxsize = 2*maxsize;
		ArrayDictionaryNode * tmp = new ArrayDictionaryNode[maxsize];
		for(int i=0; i < currentNumber; i++) {
			tmp[i].key=array[i].key;
			tmp[i].data=array[i].data;
		}
		for(int i=currentNumber; i < maxsize; i++) {
			tmp[i].key="";
			tmp[i].data=-1;
		}
		delete [] array;
		array=tmp;
    }
    // add new entry to array
    array[currentNumber].key=key;
    array[currentNumber].data=record;
    currentNumber++; return true;
	return false;
}

// Find a key in the dictionary and return corresponding record or NULL
int ArrayDictionary::findRecord(QString key)
{
	// add your code here
	if(key.isEmpty()) return -1;
    for(int i=0;i < currentNumber;i++) {
		if(array[i] == key) return array[i].data;
    } return -1;
}

// Removes one element from the table
bool ArrayDictionary::removeElement(QString key)
{
	// Add your code here
	if(key.isEmpty()) return false;
    for(int i=0; i < currentNumber; i++) {
		if(array[i] == key) {
			for(int n=i; n < currentNumber; n++) {
				if(n!=currentNumber-1) array[n]=array[n+1];
			}
			array[--currentNumber].key="";
			array[currentNumber].data=-1;
			return true;
		}
    } return false;
}

// Returns all the elements in the table as an array of strings.
// *n is the size of the table and it is returned by reference
QString* ArrayDictionary::keys(int * n)
{
	// Add your code here
	*n=currentNumber;
	QString * things = new QString[currentNumber];
	for(int i=0; i < currentNumber; i++) {things[i]=array[i].key;}
	return things;
}
