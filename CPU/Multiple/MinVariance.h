#ifndef MINVARIANCE_H
#define MINVARIANCE_H

#include <cstring>
#include <random>
#include <unordered_map>
#include <vector>

#include "DoubleEndArrayHeap.h"
#include "bucket.h"
#include "Util.h"

struct MinVarianceBucket {
  vector<TUPLES_ID> IDs;
  TUPLES_VALUE Value;

  MinVarianceBucket() {}

  MinVarianceBucket(vector<TUPLES_ID> _IDs, TUPLES_VALUE _Value) {
    IDs = _IDs;
    Value = _Value;
  }

  MinVarianceBucket(TUPLES_ID _ID, TUPLES_VALUE _Value) {
    IDs.push_back(_ID);
    Value = _Value;
  }

  MinVarianceBucket(TUPLES tuple) {
    IDs.push_back(tuple.id);
    Value = tuple.value;
  }

  MinVarianceBucket update(TUPLES_VALUE newValue) {
    Value = newValue;
    return *this;
  }

  void add(TUPLES_ID _id, TUPLES_VALUE _value) {
    IDs.push_back(_id);
    Value += _value;
  }

  void add(TUPLES_VALUE _value) {
    Value += _value;
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
typedef std::unordered_map<TUPLES_ID, bucket<MinVarianceBucket>*> ValuesMap;

class OurMinVarianceUSS {
 public:
  OurMinVarianceUSS() {}
  OurMinVarianceUSS(uint32_t _MEMORY, uint32_t _MAX_KEY_NUM_PER_BUCKET, std::string _name = "OurMinVarianceUSS") {
    this->name = _name;
    MAX_KEY_NUM_PER_BUCKET = _MAX_KEY_NUM_PER_BUCKET;
    BUCKET_NUM = _MEMORY / (2 * MAX_KEY_NUM_PER_BUCKET * sizeof(TUPLES_ID) + sizeof(TUPLES_VALUE) + sizeof(uint32_t));
    counterHeap = DoubleEndArrayHeap<MinVarianceBucket>(BUCKET_NUM);
    std::cout<<"Bucket number: "<<BUCKET_NUM<<std::endl;
  }

  ~OurMinVarianceUSS() {}

  void Insert(TUPLES &item) {

#ifdef FAIRNESS
    S += item.value;
#endif
    if (mp.find(item.id) != mp.end()) {
      bucket<MinVarianceBucket>* toUpdate = mp[item.id];
      toUpdate->object.add(item.value * item.id.hashSign());
      counterHeap.set(toUpdate, toUpdate->object, false);
    } else if(mp.size() < BUCKET_NUM) {
      mp[item.id] = new bucket(MinVarianceBucket(item.id, item.value * item.id.hashSign()), counterHeap.Size());
      counterHeap.add(mp[item.id]);
    } else if (mp.size() < MAX_KEY_NUM_PER_BUCKET * BUCKET_NUM) {
      uint32_t idx = (mp.size() / BUCKET_NUM) % 2 == 0 ? BUCKET_NUM : 0;
      bucket<MinVarianceBucket>* toSwitch = counterHeap.bucketAt(idx);
      mp[item.id] = toSwitch;
      toSwitch->object.add(item.id, item.value * item.id.hashSign());
      counterHeap.set(toSwitch, toSwitch->object, true);
    } else {
      uint32_t idx = MAX_KEY_NUM_PER_BUCKET % 2 == 0 ? BUCKET_NUM : 0;
      bucket<MinVarianceBucket>* toUpdate = counterHeap.bucketAt(idx);
      double norm1 = sqrt(item.value.sum_squares()),
             norm2 = sqrt(toUpdate->object.Value.sum_squares());
      if(norm1 + norm2 != 0) {
        double prob = norm1 / (norm1 + norm2);
        static std::mt19937 e2(rd());
        std::uniform_real_distribution<> dist(0, 1);
        if(dist(e2) < prob) {
          std::uniform_int_distribution<> randomIdx(0, MAX_KEY_NUM_PER_BUCKET - 1);
          uint32_t toReplace = randomIdx(e2);
          mp.erase(toUpdate->object.IDs[toReplace]);
          toUpdate->object.IDs[toReplace] = item.id;
          mp[item.id] = toUpdate;
          counterHeap.set(toUpdate, toUpdate->object.update(item.value * item.id.hashSign() / prob), false);
        } else {
          counterHeap.set(toUpdate, toUpdate->object.update(toUpdate->object.Value / (1 - prob)), false);
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
    // for (uint32_t i = 0; i <= BUCKET_NUM; ++i) {
    //   for (uint32_t j = 0; j < MAX_KEY_NUM_PER_BUCKET; ++j) {
    //     ret[counterHeap.objectAt(i).IDs[j]] = counterHeap.objectAt(i).Value * counterHeap.objectAt(i).IDs[j].hashSign();
    //   }
    // }
    for (auto pair = mp.begin(); pair != mp.end(); ++pair) {
      ret[pair->first] = pair->second->object.Value * (pair->first.hashSign());
    }
    return ret;
  }

 private:
  std::string name;
  ValuesMap mp;
  DoubleEndArrayHeap<MinVarianceBucket> counterHeap;
  uint32_t BUCKET_NUM;
  uint32_t MAX_KEY_NUM_PER_BUCKET;

  // uint32_t mdpt;
  bool b=0;

#ifdef FAIRNESS
  TUPLES_VALUE S = TUPLES_VALUE();
#endif
};

#endif
