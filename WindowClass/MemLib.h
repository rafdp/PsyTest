#ifndef MEMLIB_H_INCLUDED
#define MEMLIB_H_INCLUDED

#include <assert.h>

template <typename T>
class ObjectReference;

template <typename T>
class ReferenceCollector
{
    T* pointer;
    int refs;
    bool active;
    bool destroyed;
    friend class ObjectReference<T>;
    public:

    ReferenceCollector (T* pt) :
        pointer (pt),
        refs   (0),
        active (false),
        destroyed (false)
    {
        if (destroyed) return;
    }

    void AddReference ()
    {
        if (destroyed) return;
        if (!active) active = true;
        refs ++;
    }

    void DeleteReference ()
    {
        assert (refs != 0);
        if (destroyed) return;
        refs --;
        if (refs == 0)
        {
            delete this;
        }
    }

    ~ReferenceCollector ()
    {
        if (destroyed) return;
        if (pointer) delete pointer;
        pointer = NULL;
        destroyed = true;
    }
};

template <typename T>
class ObjectReference
{
    T* pointer;
    ReferenceCollector<T>* mm;
    bool deleted;
public:
    ObjectReference (ReferenceCollector<T>* mem) :
        pointer (mem->pointer),
        mm (mem),
        deleted (false)
    {
        assert (mm);
        mm->AddReference();
    }

    ObjectReference (const ObjectReference<T>& ref) :
        pointer (ref.pointer),
        mm (ref.mm),
        deleted (false)
    {
        assert (ref.pointer);
        assert (ref.mm);
        assert (!ref.deleted);
    }

    T& operator * ()
    {
        assert (!deleted);
        return *pointer;
    }

    T* operator -> ()
    {
        assert (!deleted);
        return &*pointer;
    }

    ~ObjectReference ()
    {
        assert (mm);
        if (!deleted) mm->DeleteReference();
        deleted = true;
    }

    void Delete ()
    {
        assert (mm);
        if (!deleted) mm->DeleteReference();
        deleted = true;
    }
};

#define CREATE_OBJECT(type, objName, initList, refName) \
type* objName = new type initList; \
ReferenceCollector<type>* AllocationPointer##objName = new ReferenceCollector<type> (objName); \
ObjectReference<type> refName (AllocationPointer##objName)

#define COPY_REF(oldRef, newRef) auto newRef (oldRef)

#define DELETE_REFER(ref) ref.Delete()

#define REF_TYPE(type) ObjectReference<type>

#endif
