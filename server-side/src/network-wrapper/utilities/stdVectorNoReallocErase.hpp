
#include <functional>
#include <vector>

template <typename T>
bool
stdVectorNoReallocEraseByValue(std::vector<T>& container, const T& value) {
  auto it = std::find(container.begin(), container.end(), value);
  if (it == container.end()) {
    return false;
  }

  return stdVectorNoReallocEraseByIterator(container, it);
}

template <typename T>
bool
stdVectorNoReallocEraseByCallback(std::vector<T>& container, const std::function<bool(const T&)>& findCallback) {
  auto it = std::find_if(container.begin(), container.end(), findCallback);
  if (it == container.end()) {
    return false;
  }

  return stdVectorNoReallocEraseByIterator(container, it);
}

template <typename T, typename _Alloc = std::allocator<T>>
bool
stdVectorNoReallocEraseByIterator(std::vector<T, _Alloc>& container, typename std::vector<T, _Alloc>::iterator& it) {
  if (it == container.end()) {
    return false;
  }

  // the value to remove from the container was found

  // is the value the last element of the container?
  const bool isLastElement = (it == (container.end() - 1));
  if (!isLastElement) {
    // not the last element -> swap with the last element of the container
    std::swap(*it, container.back());
  }

  // remove the (potentially swapped) value from the container
  // this will not reallocate
  // -> unlike std::vector<T>::erase()
  container.pop_back();
  return true;
}
