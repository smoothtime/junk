#if !defined(JVECTOR_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */

/* This is based exactly on an example from the upcoder blog entry
   'Roll Your Own Vector' locatable at upcoder.com/3/roll-your-own-vector/ */

#include <string.h>
#include <stdlib.h>
#include <new>

#define JVECTOR_INITIAL_CAPACITY 8

namespace junk
{

template <class T>
class JVector
{
public:
    typedef typename std::size_t size_type;

private:
    size_type _size;
    size_type _capacity;
    T *_data;
    Allocator *_allocator;

    T*
    allocate(size_type size)
    {
        T *result;
        _allocator->allocateArray(size, result);
        return result;
    }

    static void
    copyRange(T *begin, T *end, T *dest)
    {
        while(begin != end)
        {
            // placement new and copy constructor
            new( (void *) dest) T(*begin);
            begin++;
            dest++;
        }
    }

    static void
    deleteRange(T *begin, T *end)
    {
        while(begin != end)
        {
            begin->~T();
            begin++;
        }
    }

    void
    reallocate(size_type capacity)
    {
        T* newData;
        _allocator->allocateArray(capacity, newData);
        copyRange(_data, _data + _size, newData);
        deleteRange(_data, _data + _size);
        _allocator->deallocate(_data);
        _data = newData;
        _capacity = capacity;
    }

    static void
    constructRange(T *begin, T *end)
    {
        while(begin != end)
        {
            new ((void *) begin) T();
            ++begin;
        }
    }

    static void
    constructRange(T *begin, T *end, const T& fillWith)
    {
        while(begin != end)
        {
            new((void *) begin) T(fillWith);
            ++begin;
        }
    }

public:
    typedef T *iterator;
    typedef T value_type;

    JVector() {}

    JVector(Allocator *allocator)
    {
        _size = 0;
        _capacity = 0;
        _data = 0;
        _allocator = allocator;
    }
    
    JVector(size_type cap, Allocator *allocator)
    {
        _size = 0;
        _capacity = cap;
        _allocator = allocator;
        _data = allocate(sizeof(value_type) * _capacity);
    }

    JVector(const JVector &copy)
    {
        _allocator = copy._allocator;
        allocate(copy.size());
        copyRange(copy.begin(), copy.end(), _data);
    }
    
    ~JVector()
    {
        deleteRange(_data, _data + _size);
        free(_data);
    }

    size_type
    size()
    {
        return _size;
    }

    bool
    empty()
    {
        return _size == 0;
    }

    void
    push_back(const T &value)
    {
        if(_size != _capacity)
        {
            // placement new and copy constructor
            new ((void *) (_data + _size)) T(value);
            ++_size;
            return;
        }
        // allocate anew, move, and delete old
        size_type newCapacity = _capacity ? _capacity * 2 : JVECTOR_INITIAL_CAPACITY;
        T *newData = allocate(newCapacity);
        copyRange(_data, _data + _size, newData);
        // pn+cc for newly added value
        new ((void *) (newData + _size)) T(value);
        deleteRange(_data, _data + _size);
        free(_data);
        _data = newData;
        _capacity = newCapacity;
        ++_size;
        
    }

    // NOTE(james): these don't lower the capacity if resized smaller
    void
    resize(size_type newSize)
    {
        if(newSize <= _size)
        {
            deleteRange(_data + newSize, _data + _size);
            _size = newSize;
            return;
        }
        if(newSize <= _capacity)
        {
            constructRange(_data + _size, _data + newSize);
            _size = newSize;
            return;
        }
        size_type newCapacity = newSize;
        if(newCapacity < _size * 2)
        {
            newCapacity = _size * 2;
        }
        reallocate(newCapacity);
        constructRange(_data + _size, _data + newSize);
        _size = newSize;
    }

    // NOTE(james) if newSize is smaller than current size, fillWidth is not used
    void
    resize(size_type newSize, const T &fillWith)
    {
        if(newSize < _size)
        {
            deleteRange(_data + newSize, _data + _size);
            _size = newSize;
            return;
        }
        if(newSize < _capacity)
        {
            constructRange(_data + _size, _data + newSize, fillWith);
            _size = newSize;
            return;
        }
        size_type newCapacity = newSize;
        if(newCapacity < _size * 2)
        {
            newCapacity = _size * 2;
        }
        reallocate(newCapacity);
        constructRange(_data + _size, _data + newSize, fillWith);
        for(size_type i = _size; i < newSize; ++i)
        {
            ::new((void *) (_data + i)) T(fillWith);
        }
        _size = newSize;
    }

    T&
    operator[](size_type index)
    {
        return _data[index];
    }

    const T&
    operator[](size_type index) const
    {
        return _data[index];
    }

    T*
    begin() const
    {
        return _data;
    }

    T*
    end() const
    {
        return _data + _size - 1;
    }
    
};


}
#define JVECTOR_H
#endif
