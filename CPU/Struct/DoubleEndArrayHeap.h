#ifndef DOUBLEENDARRAYHEAP_H
#define DOUBLEENDARRAYHEAP_H

#include <iostream>
#include <vector>
#include "bucket.h"

using namespace std;

template <typename ComparableObject> class DoubleEndArrayHeap /* : public std::priority_queue */ {
private:
    vector<bucket<ComparableObject>*> buckets;
    uint32_t maxSize;
    uint32_t size;
    uint32_t midpoint;

public:
    DoubleEndArrayHeap() { DoubleEndArrayHeap(3000);}
    DoubleEndArrayHeap(uint32_t _maxSize) {
        midpoint = 0;
        size = 0;
        maxSize = _maxSize;
        // for (uint32_t i = 0; i < maxSize; i++) {
        //     buckets[i] = nullptr;
        // }
        buckets.resize(maxSize + 1);
    }

    ~DoubleEndArrayHeap() {}

    void swap(uint32_t k1, uint32_t k2) {
        if (k1 == midpoint || k2 == midpoint)
        {   cout<<"donno mess with midpoint!"<<endl;
            return;
        }
        if (buckets[k1] && buckets[k2]) {
            buckets[k1]->index = k2;
            buckets[k2]->index = k1;
            buckets[midpoint] = buckets[k1];
            buckets[k1] = buckets[k2];
            buckets[k2] = buckets[midpoint];
        } else {
            cout<<"donno swap null buckets!"<<endl;
        }
    }

    uint32_t minBucketIdx(uint32_t k1, uint32_t k2) {
        if (buckets[k1] && buckets[k2]) {
            return *buckets[k1] < *buckets[k2] ? k1 : k2;
        } else {
            cout<<"donno compare null buckets!"<<endl;
            return -1;
        }
    }

    uint32_t minBucketIdx(uint32_t k1, uint32_t k2, uint32_t k3) {
        if (buckets[k1] && buckets[k2]) {
            return *buckets[k1] < *buckets[k2] ? minBucketIdx(k1, k3) : minBucketIdx(k2, k3);
        } else {
            cout<<"donno compare null buckets!"<<endl;
            return -1;
        }
    }

    uint32_t parent(uint32_t idx) {
        if (idx == midpoint) {
            cout<<"midpoint is an orphan!"<<endl;
            return -1;
        } else if (idx < midpoint) {
            return idx == 0 ? 0 : (idx - 1) / 2;
        } else {
            return idx == maxSize ? maxSize : maxSize - (maxSize - idx - 1) / 2;
        }
    }

    uint32_t leftChild(uint32_t idx) {
        if (idx == midpoint) {
            cout<<"midpoint is impotent!"<<endl;
            return -1;
        } else if (idx < midpoint) {
            uint32_t lc = 2 * idx + 1;
            return (lc < midpoint && lc > idx) ? lc : -1;
        } else {
            uint lc = 2 * idx - maxSize - 1;
            return (lc > midpoint && lc < idx) ? lc : -1;
        }
    }

    uint32_t rightChild(uint32_t idx) {
        if (idx == midpoint) {
            cout<<"midpoint is impotent!"<<endl;
            return -1;
        } else if (idx < midpoint) {
            uint32_t lc = 2 * idx + 2;
            return (lc < midpoint && lc > idx + 1) ? lc : -1;
        } else {
            uint lc = 2 * idx - maxSize - 2;
            return (lc > midpoint && lc < idx - 1) ? lc : -1;
        }
    }

    void bubbleUp(uint32_t idx) {
        if (idx == midpoint) {
            cout<<"midpoint canno bubbleUp; midpoint: "<<midpoint<<endl;
            return;
        }
        uint32_t currParent = parent(idx);
        while(currParent != idx && *buckets[idx] < *buckets[currParent]) {
            swap(idx, currParent);
            idx = currParent;
            currParent = parent(idx);
        }
    }

    void bubbleDown(uint32_t idx) {
        if (idx == midpoint) {
            cout<<"midpoint canno bubbleDown!"<<endl;
            return;
        }
        while(true) {
            uint32_t currLeft = leftChild(idx), currRight = rightChild(idx);
            if(currLeft != -1) {
                if(currRight != -1) {
                    uint32_t toGo = minBucketIdx(idx, currLeft, currRight);
                    if(toGo == idx) return;
                    swap(idx, toGo);
                    idx = toGo;
                } else if(*buckets[currLeft] < *buckets[idx]) {
                    swap(idx, currLeft);
                    idx = currLeft;
                } else {
                    return;
                }
            } else if(currRight != -1 && *buckets[currRight] < *buckets[idx]) {
                swap(idx, currRight);
                    idx = currRight;
            } else {
                return;
            }
        }
    }

    ComparableObject getMin(bool fromEnd = 0) {
        return buckets[fromEnd ? maxSize : 0]->object;
    }

    void add(bucket<ComparableObject>* bct/*, bool toEndHeap = 0*/) {
        if (size < maxSize) {
            midpoint++;
            buckets[size] = bct;
            bct->index = size;
            bubbleUp(size);
            size++;
        } else {
            cout<<"heap is full!"<<endl;
        }
    }

    void add(ComparableObject obj) {
        if (size < maxSize) {
            midpoint++;
            buckets[size] = new bucket(obj, size);
            bubbleUp(size);
            size++;
        } //else {
        //     cout<<"heap is full!"<<endl;
        // }
    }

    void set(bucket<ComparableObject>* bct, ComparableObject toObj, bool switchEnd = 0) {
        if(!switchEnd) {
            if(bct->object < toObj) {
                bct->object = toObj;
                bubbleDown(bct->index);
            } else {
                bct->object = toObj;
                bubbleUp(bct->index);
            }
        } else if(bct->index < midpoint) {
            if(midpoint - bct->index == 1) {
                buckets[midpoint] = bct;
                bct->index = midpoint;
                midpoint--;
                bubbleUp(bct->index);
            } else {
                buckets[midpoint] = bct;
                buckets[bct->index] = buckets[midpoint - 1];
                buckets[bct->index]->index = bct->index;
                midpoint--;
                bubbleDown(bct->index);
                bct->index = midpoint + 1;
                bubbleUp(bct->index);
            }
        } else if(bct->index > midpoint) {
            if(bct->index - midpoint == 1) {
                buckets[midpoint] = bct;
                bct->index = midpoint;
                midpoint++;
                bubbleUp(bct->index);
            } else {
                buckets[midpoint] = bct;
                buckets[bct->index] = buckets[midpoint + 1];
                buckets[bct->index]->index = bct->index;
                midpoint++;
                bubbleDown(bct->index);
                bct->index = midpoint - 1;
                bubbleUp(bct->index);
            }
        }
    }

    bucket<ComparableObject>* bucketAt(uint32_t idx) { return buckets[idx];}

    ComparableObject objectAt(uint32_t idx) { return buckets[idx]->object;}

    uint32_t Size() { return size;}

    uint32_t Midpoint() { return midpoint;}

};

#endif
