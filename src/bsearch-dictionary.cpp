// Implementation of a dictionary using an array and binary search
// It will inherit from the ArrayDictionary
#include "bsearch-dictionary.h"

/*----------------------- heap ----------------------*/
heap::heap(int max) {
	n=0;
	this->max=max;
	array = new ArrayDictionaryNode[max];
}

heap::~heap() {n=0; max=0; delete [] array;}

int heap::getSize() {return n;}

bool heap::insert(QString key, int data) {
	if(key.isEmpty() || data<0) return false;
 
	for(int i=0; i < n;i++) {
		if(array[i] == key) {
			array[i].data=data; return false;
		}
	}
  
	if(n==max) {
		max=2*max;
		ArrayDictionaryNode * tmp = new ArrayDictionaryNode[max];
		for(int i=0; i<n; i++) {
			tmp[i].key=array[i].key;
			tmp[i].data=array[i].data;
		} for(int i=n; i<max; i++) {
			tmp[i].key="";
			tmp[i].data=-1;
		}
		delete [] array;
		array=tmp;
	}
  
	array[n].key = key;
	array[n].data = data;
	n++;
	int c = n-1;
	int p = parent(c);
	while(c > 0) {
		if(array[c].data > array[p].data) break;
		ArrayDictionaryNode tmp;
		tmp.key=array[c].key;
		tmp.data=array[c].data;
    
		array[c].key=array[p].key;
		array[c].data=array[p].data;

		array[p].key = tmp.key;
		array[p].data = tmp.data;
		c=p;
		p=parent(c);
	} return true;
}

ArrayDictionaryNode heap::removeMin() {
	if(!n) return (*(new ArrayDictionaryNode));
	ArrayDictionaryNode min=array[0];
	n--;
	if(!n) return min;

	array[0].key=array[n].key;
	array[0].data=array[n].data;

	int p = 0;
	int l = leftC(p);
	int r = rightC(p);

	while(l<n) {
		int m = l;
		if(r<n && array[r].data < array[l].data) m=r;
		if(array[p].data < array[m].data) break;
		ArrayDictionaryNode tmp;
		tmp.key=array[m].key;
		tmp.data=array[m].data;

		array[m].key=array[p].key;
		array[m].data=array[p].data;

		array[p].key=tmp.key;
		array[p].data=tmp.data;
		p=m;
		l=leftC(p);
		r=rightC(p);
	}
	return min;
}

bool heap::remove(QString key) {
	if(key.isEmpty()) return false;

	int mid=-1;
	for(int i=0; i<n; i++) {
		if(array[i] == key) {
			mid = i;
		}
	} if(mid == -1) return false;

	if(array[mid] == key) {
		for(int i=mid; i<n; i++) {
			if(i!=n-1) {
				array[i].key=array[i+1].key;
				array[i].data=array[i+1].data;
			}
		}
		array[--n].key="";
		array[n].data=-1;
		return true;
	} return false;
}

ArrayDictionaryNode* heap::getArray() {return array;}

int heap::find(QString key) {
	if(key.isEmpty()) return -1;
	
	for(int i=0; i<n; i++) {
		if(array[i] == key) {
			return array[i].data;
		}
	} return -1;
}

/*------------------- binary search -----------------*/

// Constructor
BinarySearchDictionary::BinarySearchDictionary()
{
	sorted=false;
	h=new heap(20);
}

bool BinarySearchDictionary::addRecord(QString key, int record) {
	sorted=false;
	return h->insert(key, record);
}

bool BinarySearchDictionary::removeElement(QString key) {
	sorted=false;
	return h->remove(key);
}

int BinarySearchDictionary::find(QString key) {
	return h->find(key);
}

QString* BinarySearchDictionary::keys(int * n) {
	*n=h->getSize();
	QString * things = new QString[*n];
	for(int i=(*n-1); i>-1; i--) {
		things[i]=h->removeMin().key;
	} return things;
}

// Sort array using heap sort
void BinarySearchDictionary::sort()
{
	// Add your implementation here
	int s=h->getSize();
	for(int i=0; i<s; i++) {
		array[i]=h->removeMin();
		//std::cerr<<array[i].data<<std::endl;
		currentNumber++;
	} for(int i=0; i<currentNumber; i++) {
		h->insert(array[i].key, array[i].data);
	}
}

void BinarySearchDictionary::clear() {delete h; h=new heap(20);}
