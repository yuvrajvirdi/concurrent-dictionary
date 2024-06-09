#include <iostream>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <functional>
#include "file_io.h" 

using namespace std;

template <typename Key, typename Value, size_t NumBuckets = 16>
class ConcurrentDictionary {
public:
    ConcurrentDictionary(const string& filename) : buckets(NumBuckets), filename(filename) {}

    void insert(const Key& key, const Value& value) {
        size_t bucket = get_bucket(key);
        unique_lock<mutex> lock(buckets[bucket].mutex);
        buckets[bucket].map[key] = value;
        write_to_disk(filename.c_str(), key.c_str(), value.c_str());
    }

    bool remove(const Key& key) {
        size_t bucket = get_bucket(key);
        unique_lock<mutex> lock(buckets[bucket].mutex);
        auto& bucket_map = buckets[bucket].map;
        auto it = bucket_map.find(key);
        if (it != bucket_map.end()) {
            bucket_map.erase(it);
            // TODO: disk removal
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
        } else {
            char c_value[256];
            read_from_disk(filename.c_str(), key.c_str(), c_value, sizeof(c_value));
            if (strlen(c_value) > 0) {
                value = string(c_value);
                return true;
            }
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
    string filename;

    size_t get_bucket(const Key& key) const {
        static hash<Key> hasher;
        return hasher(key) % buckets.size();
    }
};

int main() {
    ConcurrentDictionary<string, string> dict("data.txt");

    dict.insert("key1", "value1");
    dict.insert("key2", "value2");

    string value;
    if (dict.get("key1", value)) {
        cout << "key1: " << value << endl;
    } else {
        cout << "key1 not found" << endl;
    }

    return 0;
}
