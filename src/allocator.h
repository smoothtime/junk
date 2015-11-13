#if !defined(ALLOCATOR_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */

namespace address_math
{
    void*       alignForward(void* address, uint8 alignment);
    const void* alignForward(const void* address, uint8 alignment);

    void*       alignBackward(void* address, uint8 alignment);
    const void* alignBackward(const void* address, uint8 alignment);

    uint8          alignForwardAdjustment(const void* address, uint8 alignment);
    uint8          alignForwardAdjustmentWithHeader(const void* address, uint8 alignment, uint8 headerSize);

    uint8          alignBackwardAdjustment(const void* address, uint8 alignment);

    void*       add(void* p, size_t x);
    const void* add(const void* p, size_t x);

    void*       subtract(void* p, size_t x);
    const void* subtract(const void* p, size_t x);
};

class Allocator
{
public:
    Allocator()
    {
        
    }
    
    Allocator(Allocator *inParent)
    {
        parent = inParent;        
    }
    virtual ~Allocator() {}
    virtual void *allocate(uint64 size, uint8 alignment = 4) = 0;
    virtual void deallocate(void *ptr) = 0;

    template <class T> void
    allocateArray(uint64 arraySize, T*& result)
    {
        result = static_cast<T*>(allocate(sizeof(T) * arraySize));
    }
    
private:
    Allocator *parent;
};

namespace address_math
{
    inline void* alignForward(void* address, uint8 alignment)
    {
        return (void*)( ( reinterpret_cast<uptr>(address) + static_cast<uptr>(alignment-1) ) & static_cast<uptr>(~(alignment-1)) );
    }

    inline const void* alignForward(const void* address, uint8 alignment)
    {
        return (void*)( ( reinterpret_cast<uptr>(address) + static_cast<uptr>(alignment-1) ) & static_cast<uptr>(~(alignment-1)) );
    }

    inline void* alignBackward(void* address, uint8 alignment)
    {
        return (void*)( reinterpret_cast<uptr>(address) & static_cast<uptr>(~(alignment-1)) );
    }

    inline const void* alignBackward(const void* address, uint8 alignment)
    {
        return (void*)( reinterpret_cast<uptr>(address) & static_cast<uptr>(~(alignment-1)) );
    }

    inline uint8 alignForwardAdjustment(const void* address, uint8 alignment)
    {
        uint8 adjustment =  alignment - ( reinterpret_cast<uptr>(address) & static_cast<uptr>(alignment-1) );
    
        if(adjustment == alignment)
            return 0; //already aligned
    
        return adjustment;
    }

    inline uint8 alignForwardAdjustmentWithHeader(const void* address, uint8 alignment, uint8 headerSize)
    {
        uint8 adjustment =  alignForwardAdjustment(address, alignment);

        uint8 neededSpace = headerSize;

        if(adjustment < neededSpace)
        {
            neededSpace -= adjustment;

            //Increase adjustment to fit header
            adjustment += alignment * (neededSpace / alignment);

            if(neededSpace % alignment > 0)
                adjustment += alignment;
        }
    
        return adjustment;
    }

    inline uint8 alignBackwardAdjustment(const void* address, uint8 alignment)
    {
        uint8 adjustment =  reinterpret_cast<size_t>(address) & static_cast<uptr>(alignment-1);
    
        if(adjustment == alignment)
            return 0; //already aligned
    
        return adjustment;
    }

    inline void* add(void* p, size_t x)
    {
        return (void*)( reinterpret_cast<uptr>(p) + x);
    }

    inline const void* add(const void* p, size_t x)
    {
        return (const void*)( reinterpret_cast<uptr>(p) + x);
    }

    inline void* subtract(void* p, size_t x)
    {
        return (void*)( reinterpret_cast<uptr>(p) - x);
    }

    inline const void* subtract(const void* p, size_t x)
    {
        return (const void*)( reinterpret_cast<uptr>(p) - x);
    }
};

#define ALLOCATOR_H
#endif
