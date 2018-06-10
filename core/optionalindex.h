#ifndef EUPHORIA_OPTIONALINDEX_H
#define EUPHORIA_OPTIONALINDEX_H

template <typename T>
class OptionalIndex
{
 public:
  static OptionalIndex<T>
  Null()
  {
    return {};
  }

  static OptionalIndex<T>
  FromIndex(T index)
  {
    return {index};
  }

  T
  GetIndex() const
  {
    ASSERT(is_valid);
    return index;
  }

  operator bool() const
  {
    return is_valid;
  }

 private:
  OptionalIndex()
      : is_valid(false)
      , index(0)
  {
  }

  OptionalIndex(T index)
      : is_valid(true)
      , index(index)
  {
  }

  bool is_valid;
  T    index;
};

#endif  // EUPHORIA_OPTIONALINDEX_H