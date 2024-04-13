#include <mutex>
#include <shared_mutex>
#include <string>
#include <functional>
#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;

template <typename Key, typename Value, size_t NumBuckets = 16>
class ConcurrentDictionary {
public:
    ConcurrentDictionary() : buckets(NumBuckets) {}

    void insert(const Key& key, const Value& value) {
        size_t bucket = get_bucket(key);
        unique_lock<mutex> lock(buckets[bucket].mutex);
        buckets[bucket].map[key] = value;
    }

    bool remove(const Key& key) {
        size_t bucket = get_bucket(key);
        unique_lock<mutex> lock(buckets[bucket].mutex);
        auto& bucket_map = buckets[bucket].map;
        auto it = bucket_map.find(key);
        if (it != bucket_map.end()) {
            bucket_map.erase(it);
            return true;
        }
        return false;
    }

    bool get(const Key& key, Value& value) const {
        size_t bucket = get_bucket(key);
        unique_lock<mutex> lock(buckets[bucket].mutex);
        const auto& bucket_map = buckets[bucket].map;
        auto it = bucket_map.find(key);
        if (it != bucket_map.end()) {
            value = it->second;
            return true;
        }
        return false;
    }

    bool contains(const Key& key) const {
        size_t bucket = get_bucket(key);
        unique_lock<mutex> lock(buckets[bucket].mutex);
        const auto& bucket_map = buckets[bucket].map;
        return bucket_map.find(key) != bucket_map.end();
    }

private:
    struct Bucket {
        unordered_map<Key, Value> map;
        mutable mutex mutex;
    };

    vector<Bucket> buckets;

    size_t get_bucket(const Key& key) const {
        static hash<Key> hasher;
        return hasher(key) % buckets.size();
    }
};
