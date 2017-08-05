#ifndef EUPHORIA_CACHE_H
#define EUPHORIA_CACHE_H

#include <map>
#include <memory>

// todo: support hotloading

template<typename Key, typename Data, typename Loader>
class Cache {
 public:
  std::shared_ptr<Data> Get(const Key& key) {
    auto found = cache.find(key);
    if (found != cache.end()) {
      auto cached = found->second.lock();
      if (cached) {
        return cached;
      } else {
        cache.erase(found);
      }
    }

    std::shared_ptr<Data> data = static_cast<Loader*>(this)->Create(key);
    cache.insert(std::pair<Key, std::weak_ptr<Data>>(key, data));
    return data;
  }

 private:
  std::map<Key, std::weak_ptr<Data>> cache;
};

#endif //EUPHORIA_CACHE_H
