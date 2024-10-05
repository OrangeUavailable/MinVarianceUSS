#ifndef BUCKET_H
#define BUCKET_H

template <typename ComparableObject>
class bucket {
    public:
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

#endif
