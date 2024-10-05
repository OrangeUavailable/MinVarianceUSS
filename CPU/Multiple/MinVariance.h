#ifndef MINVARIANCE_H
#define MINVARIANCE_H

#include <cstring>
#include <random>
#include <unordered_map>
#include<vector>

#include "DoubleEndArrayHeap.h"
#include"bucket.h"
#include "Util.h"

struct MinVarianceBucket {
  vector<TUPLES_ID> IDs;
  TUPLES_VALUE Value;

  MinVarianceBucket(vector<TUPLES_ID> _IDs, TUPLES_VALUE _Value) {
    IDs = _IDs;
    Value = _Value;
  }

  MinVarianceBucket(TUPLES_ID _ID, TUPLES_VALUE _Value) {
    IDs = {_ID};
    Value = _Value;
  }

  MinVarianceBucket(TUPLES tuple) {
    MinVarianceBucket(tuple.id, tuple.value);
  }

  MinVarianceBucket update(TUPLES_VALUE newValue) {
    Value = newValue;
    return *this;
  }

  bool operator<(const MinVarianceBucket &a) const {
    return Value < a.Value;
  }

  // MinVarianceBucket operator+(const MinVarianceBucket &a) {
  //   Value += a.Value;
  //   return *this;
  // }

};


typedef std::unordered_map<TUPLES_ID, TUPLES_VALUE> HashMap;
typedef std::unordered_map<TUPLES_ID, bucket<MinVarianceBucket>*> ValuesMap; //这里每个键对应的应该是指向bucket的指针，我不知道应该这样写还是得在某个地方加个*

class OurMinVarianceUSS {
 public:
  OurMinVarianceUSS();
  OurMinVarianceUSS(uint32_t _MEMORY, uint32_t _MAX_KEY_NUM_PER_BUCKET, std::string _name = "OurMinVarianceUSS") {
    this->name = _name;
    MAX_KEY_NUM_PER_BUCKET = _MAX_KEY_NUM_PER_BUCKET;
    BUCKET_NUM = _MEMORY / (2 * MAX_KEY_NUM_PER_BUCKET * sizeof(TUPLES_ID) + sizeof(TUPLES_VALUE) + sizeof(uint32_t));
    counterHeap = DoubleEndArrayHeap<MinVarianceBucket>(BUCKET_NUM);
  }

  ~OurMinVarianceUSS() {
    //不会写:(
  }

  void Insert(TUPLES &item) {

#ifdef FAIRNESS
    S += item.value;
#endif

    if (mp.find(item.id) != mp.end()) {
      bucket<MinVarianceBucket>* counterBucket = mp[item.id];
      counterHeap.set(*counterBucket, counterBucket->object.update(counterBucket->object.Value + (item.value * item.id.HASH())), false);
        return;
    } else if(mp.size() < BUCKET_NUM) {
      mp[item.id] = counterHeap.bucketAt(mp.size()); //我不知道这个bucketAt()返回的是一个指针还是一个bucket
      counterHeap.add(MinVarianceBucket(item));
    }
    else if (mp.size() < MAX_KEY_NUM_PER_BUCKET * BUCKET_NUM) {
      uint32_t idx = ((mp.size() + 1) / BUCKET_NUM) % 2 == 0 ? 0 : BUCKET_NUM;
      bucket toUpdate = counterHeap.bucketAt(idx); //问题同上
      mp[item.id] = toUpdate;
      counterHeap.set(*toUpdate, toUpdate->object.update(toUpdate.object.Value + (item.value * item.id.HASH())), true);
    } else {
      uint32_t idx = MAX_KEY_NUM_PER_BUCKET % 2 == 0 ? BUCKET_NUM : 0;
      bucket toUpdate = counterHeap.bucketAt(idx); //问题同上
      double norm1 = sqrt(item.value.sum_squares()),
             norm2 = sqrt(toUpdate->object.Value.sum_squares());
      if(norm1 + norm2 != 0) {
        double prob = norm1 / (norm1 + norm2);
        static std::mt19937 e2(rd());
        std::uniform_real_distribution<> dist(0, 1);
        if(dist(e2) < prob) {
          std::uniform_int_distribution<> randomIdx(0, MAX_KEY_NUM_PER_BUCKET - 1);
          uint32_t toReplace = randomIdx(e2);
          mp.erase(toUpdate.object.IDs[toReplace]);
          toUpdate.object.IDs[toReplace] = item.id;
          mp[item.id] = toUpdate; //问题同上
          counterHeap.set(*toUpdate, toUpdate->object.update(item.value * item.id.HASH()) / prob, false);
        } else {
          counterHeap.set(*toUpdate, toUpdate->object.update(toUpdate.object.Value / (1 - prob)), false);
        }
      }
    }
    
#ifdef FAIRNESS
    double sqrt1 = sqrt(item.value.normalize(S).sum_squares());
    double sqrt2 =
        sqrt(counter[minHash][minPos].value.normalize(S).sum_squares());
#else
#endif
    
  }

  HashMap AllQuery() {
    HashMap ret;
    for (auto pair = mp.begin(); pair != mp.end(); ++pair) {
      ret[pair->first] = pair->second->object.Value;
    }
    return ret;
  }

 private:
  std::string name;
  ValuesMap mp;
  DoubleEndArrayHeap<MinVarianceBucket> counterHeap;
  uint32_t BUCKET_NUM;
  uint32_t MAX_KEY_NUM_PER_BUCKET;

#ifdef FAIRNESS
  TUPLES_VALUE S = TUPLES_VALUE();
#endif
};

#endif
