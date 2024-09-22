#ifndef DOUBLEENDARRAYHEAP_H
#define DOUBLEENDARRAYHEAP_H
#include<iostream>
using namespace std;
#include<vector>
template <typename ComparableObject>
struct bucket {
    ComparableObject object;
    uint32_t index;

    bucket(ComparableObject obj = nullptr, uint32_t idx = 0) {
        object = obj;
        index = idx;
    }

    bool operator<(const bucket &a) const {
        return this->object < a.object;
    }

};
template <typename ComparableObject>
class DoubleEndArrayHeap// : public std::priority_queue
{
private:

    vector<bucket> buckets; //这里应该是一列指向bucket的指针，我不知道应该写vector<bucket>还是vector<bucket*>
    uint32_t maxSize;
    uint32_t size;
    uint32_t midpoint;

public:
    
    DoubleEndArrayHeap(uint32_t _maxSize) {
        midpoint = 0;
        size = 0;
        maxSize = _maxSize;
        buckets.resize(maxSize + 1);
        memset(buckets[0], 0, sizeof(bucket) * (maxSize + 1));
    };

    ~DoubleEndArrayHeap() {
    };

    void swap(uint32_t k1, uint32_t k2) {
        buckets[k1].index = k2;
        buckets[k2].index = k1;
        buckets[midpoint] = buckets[k1];
        buckets[k1] = buckets[k2];
        buckets[k2] = buckets[midpoint];
    }

    uint32_t minBucketIdx(uint32_t k1, uint32_t k2) {
        return buckets[k1] < buckets[k2] ? k1 : k2 ;
    }

    uint32_t minBucketIdx(uint32_t k1, uint32_t k2, uint32_t k3) {
        return buckets[k1] < buckets[k2] ? minBucketIdx(k1, k3) : minBucketIdx(k2, k3) ;
    }

    uint32_t parent(uint32_t idx) {
        return idx < midpoint ? (idx - 1) / 2 : maxSize - (maxSize - idx) / 2;
    }

    uint32_t leftChild(uint32_t idx) {
        return idx < midpoint ? 2 * idx + 1 : 2 * idx - maxSize;
    }

    uint32_t rightChild(uint32_t idx) {
        return idx < midpoint ? 2 * idx + 2 : 2 * idx - maxSize - 1;
    }

    void bubbleUp(uint32_t idx) {
        uint32_t currParent = parent(idx);
        while(!(currParent < 0) && buckets[idx] < buckets[currParent]) {
            swap(idx, currParent);
            idx = currParent;
            currParent = parent(idx);
        }
    }

    void bubbleDown(uint32_t idx) {
        while(true) {
            uint32_t currLeft = leftChild(idx), currRight = rightChild(idx);
            bool leftValid = (midpoint - idx) * (midpoint - currLeft) > 0,
                 rightValid = (midpoint - idx) * (midpoint - currRight) > 0;
            if(leftValid) {
                if(rightValid) {
                    uint32_t toGo = minBucketIdx(idx, currLeft, currRight);
                    if(toGo == idx) {return;}
                    swap(idx, toGo);
                    idx = toGO;
                } else if(buckets[currLeft] < buckets[idx]) {
                    swap(idx, currLeft);
                    idx = currLeft;
                }
                return;
            } else if(rightValid && [currRight] < buckets[idx]) {
                swap(idx, currRight);
                    idx = currRight;
            } else {
                return;
            }
        }
    }

    ComparableObject getMin(bool fromEnd = 0) { return fromEnd ? buckets[maxSize].object : buckets[0].object;}

    void add(ComparableObject obj/*, bool toEndHeap = 0*/) {
        midpoint++;
        buckets[size] = new bucket(obj, size);
        bubbleUp(size);
        size++;
    }

    void set(bucket bct, ComparableObject toObj, bool switchEnd = 0) {
        if(!switchEnd) {
            if(bct.object < toObj) {
                bct.object = toObj;
                bubbleDown(bct.index);
            } else {
                bct.object = toObj;
                bubbleUp(bct.index);
            }
        } else if(bct.index < midpoint) {
            buckets[midpoint] = bct;
            buckets[bct.index] = buckets[midpoint - 1];
            buckets[bct.index].index = bct.index;
            midpoint--;
            bubbleDown(bct.index);
            bct.index = midpoint + 1;
            bubbleUp(bct.index);
        } else if(bct.index > midpoint) {
            buckets[midpoint] = bct;
            buckets[bct.index] = buckets[midpoint + 1];
            buckets[bct.index].index = bct.index;
            midpoint++;
            bubbleDown(bct.index);
            bct.index = midpoint - 1;
            bubbleUp(bct.index);
        }
    }

    bucket bucketAt(uint32_t idx) { return buckets[idx];}

    ComparableObject objectAt(uint32_t idx) { return buckets[idx].object;}

    uint32_t size() {
        return size;
    }

    uint32_t midpoint() {
        return midpoint;
    }

};

// DoubleEndArrayHeap::DoubleEndArrayHeap(/* args */)
// {
// }

// DoubleEndArrayHeap::~DoubleEndArrayHeap()
// {
// }
