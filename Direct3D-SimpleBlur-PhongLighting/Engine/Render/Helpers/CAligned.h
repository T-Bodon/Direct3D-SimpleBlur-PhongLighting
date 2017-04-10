#pragma once

#include <malloc.h>
#include <new>
#pragma warning( push )
// - multiple copy constructors specified
// - disabled because CAligned have templated constructor
// - CAligned(const CAligned &other) is used when copy-constructing 
// object inside const method or from const variable
// - CAligned(CAligned &other) is used in other copy-constructing cases
#pragma warning( disable : 4521 )

// allocates instance of type T in aligned memory
// better use it for simple structures
template<class T, size_t Alignment>
class CAligned 
{
public:
  template<class... Types>
  CAligned(Types&&... _args) {
    Alloc();
    new (mData) T(std::forward<Types>(_args)...);
  }

  CAligned(CAligned& _other) {
    Copy(_other);
  }

  CAligned(const CAligned& _other) {
    Copy(_other);
  }

  CAligned(CAligned&& _other) {
    Move(std::move(_other));
  }

  ~CAligned() {
    Free();
  }

  CAligned &operator=(const CAligned& _other) {
    if (this != &_other) 
      Copy(_other);

    return *this;
  }

  CAligned &operator=(CAligned&& _other) {
    if (this != &_other) {
      Free();
      Move(std::move(_other));
    }

    return *this;
  }

  T *operator->() {
    return mData;
  }

  const T *operator->() const {
    return mData;
  }

  T &operator*() {
    return *mData;
  }

  const T &operator*() const {
    return *mData;
  }

private:
  T * mData;

  void Alloc() {
    mData = static_cast<T *>(_mm_malloc(sizeof(T), Alignment));
  }

  void Copy(const CAligned& _other ) {
    if (other.mData) {
      // cases:
      // - other.data && this->data
      // - other.data && !this->data
      if (!mData) 
        Alloc();      

      std::memcpy(mData, _other.mData, sizeof(T));
    }
    else 
    if (!_other.mData && mData)
      Free();
    // else if (!other.data && !this->data) do nothing
  }

  void Move(CAligned&& _other)  {
    // just copy pointer...
    mData = _other.mData;
    // ...and...
    _other.mData = nullptr;
    // ...to move data
  }

  void Free() {
    if (mData) {
      mData->~T();
      _mm_free(mData);

      // set nullptr in order to detect when re-Alloc is needed
      mData = nullptr;
    }
  }
};

#pragma warning( pop )