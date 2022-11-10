//
// Copyright © 2017 zhangyao. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "glue/base/atomic.h"
#include "glue/base/common.h"

namespace glue {

//==============================================================================
/**
    Used by container classes as an indirect way to delete an object of a
    particular type.

    The generic implementation of this class simply calls 'delete', but you can
    create a specialised version of it for a particular class if you need to
    delete that type of object in a more appropriate way.

    @see OwnedArray

    @tags{Core}
*/
template <typename ObjectType>
struct ContainerDeletePolicy
{
    static void destroy(ObjectType* object)
    {
        // If the line below triggers a compiler error, it means that you are using
        // an incomplete type for ObjectType (for example, a type that is declared
        // but not defined). This is a problem because then the following delete is
        // undefined behaviour. The purpose of the sizeof is to capture this situation.
        // If this was caused by a OwnedArray of a forward-declared type, move the
        // implementation of all methods trying to use the OwnedArray (e.g. the destructor
        // of the class owning it) into cpp files where they can see to the definition
        // of ObjectType. This should fix the error.
        ignoreUnused(sizeof(ObjectType));

        delete object;
    }
};

//==============================================================================
/**
    A base class which provides methods for reference-counting.

    To add reference-counting to a class, derive it from this class, and
    use the RefCountedObjectPtr class to point to it.

    e.g. @code
    class MyClass : public RefCountedObject
    {
        void foo();

        // This is a neat way of declaring a typedef for a pointer class,
        // rather than typing out the full templated name each time..
        using Ptr = RefCountedObjectPtr<MyClass>;
    };

    MyClass::Ptr p = new MyClass();
    MyClass::Ptr p2 = p;
    p = nullptr;
    p2->foo();
    @endcode

    Once a new RefCountedObject has been assigned to a pointer, be
    careful not to delete the object manually.

    This class uses an Atomic<int> value to hold the reference count, so
    the reference count can be updated on multiple threads. Note that
    whilst it's thread-safe to create and delete a RefCountedObjectPtr
    to a RefCountedObject shared between threads, it's not thread-safe
    to modify or swap the RefCountedObject.

    For a faster but non-thread-safe version, use SingleThreadedReferenceCountedObject
    instead.

    @see RefCountedObjectPtr, ReferenceCountedArray, SingleThreadedReferenceCountedObject

    @tags{Core}
*/
class GLUE_API RefCountedObject
{
public:
    //==============================================================================
    /** Increments the object's reference count.

        This is done automatically by the smart pointer, but is public just
        in case it's needed for nefarious purposes.
    */
    void incReferenceCount() noexcept
    {
        ++refCount;
    }

    /** Decreases the object's reference count.
        If the count gets to zero, the object will be deleted.
    */
    void decReferenceCount() noexcept
    {
        GLUE_ASSERT(getReferenceCount() > 0);

        if (--refCount == 0) delete this;
    }

    /** Decreases the object's reference count.
        If the count gets to zero, the object will not be deleted, but this method
        will return true, allowing the caller to take care of deletion.
    */
    bool decReferenceCountWithoutDeleting() noexcept
    {
        GLUE_ASSERT(getReferenceCount() > 0);
        return --refCount == 0;
    }

    /** Returns the object's current reference count. */
    int getReferenceCount() const noexcept
    {
        return refCount.get();
    }

protected:
    //==============================================================================
    /** Creates the reference-counted object (with an initial ref count of zero). */
    RefCountedObject() = default;

    /** Copying from another object does not affect this one's reference-count. */
    RefCountedObject(const RefCountedObject&) noexcept
    {}
    /** Copying from another object does not affect this one's reference-count. */
    RefCountedObject(RefCountedObject&&) noexcept
    {}
    /** Copying from another object does not affect this one's reference-count. */
    RefCountedObject& operator=(const RefCountedObject&) noexcept
    {
        return *this;
    }
    /** Copying from another object does not affect this one's reference-count. */
    RefCountedObject& operator=(RefCountedObject&&) noexcept
    {
        return *this;
    }

    /** Destructor. */
    virtual ~RefCountedObject()
    {
        // it's dangerous to delete an object that's still referenced by something else!
        GLUE_ASSERT(getReferenceCount() == 0);
    }

    /** Resets the reference count to zero without deleting the object.
        You should probably never need to use this!
    */
    void resetReferenceCount() noexcept
    {
        refCount = 0;
    }

private:
    //==============================================================================
    Atomic<int> refCount{0};
    friend struct ContainerDeletePolicy<RefCountedObject>;
};

//==============================================================================
/**
    A smart-pointer class which points to a reference-counted object.

    The template parameter specifies the class of the object you want to point to - the easiest
    way to make a class reference-countable is to simply make it inherit from RefCountedObject
    or SingleThreadedReferenceCountedObject, but if you need to, you can roll your own
   reference-countable class by implementing a set of methods called incReferenceCount(),
   decReferenceCount(), and decReferenceCountWithoutDeleting(). See RefCountedObject for
   examples of how these methods should behave.

    When using this class, you'll probably want to create a typedef to abbreviate the full
    templated name - e.g.
    @code
    struct MyClass  : public RefCountedObject
    {
        using Ptr = RefCountedObjectPtr<MyClass>;
        ...
    }
    @endcode

    @see RefCountedObject

    @tags{Core}
*/
template <class ObjectType>
class RefCountedObjectPtr
{
public:
    /** The class being referenced by this pointer. */
    using ReferencedType = ObjectType;

    //==============================================================================
    /** Creates a pointer to a null object. */
    RefCountedObjectPtr() = default;

    /** Creates a pointer to a null object. */
    RefCountedObjectPtr(decltype(nullptr)) noexcept
    {}

    /** Creates a pointer to an object.
        This will increment the object's reference-count.
    */
    RefCountedObjectPtr(ReferencedType* refCountedObject) noexcept
        : referencedObject(refCountedObject)
    {
        incIfNotNull(refCountedObject);
    }

    /** Creates a pointer to an object.
        This will increment the object's reference-count.
    */
    RefCountedObjectPtr(ReferencedType& refCountedObject) noexcept
        : referencedObject(&refCountedObject)
    {
        refCountedObject.incReferenceCount();
    }

    /** Copies another pointer.
        This will increment the object's reference-count.
    */
    RefCountedObjectPtr(const RefCountedObjectPtr& other) noexcept
        : referencedObject(other.referencedObject)
    {
        incIfNotNull(referencedObject);
    }

    /** Takes-over the object from another pointer. */
    RefCountedObjectPtr(RefCountedObjectPtr&& other) noexcept
        : referencedObject(other.referencedObject)
    {
        other.referencedObject = nullptr;
    }

    /** Copies another pointer.
        This will increment the object's reference-count (if it is non-null).
    */
    template <typename Convertible>
    RefCountedObjectPtr(const RefCountedObjectPtr<Convertible>& other) noexcept
        : referencedObject(other.get())
    {
        incIfNotNull(referencedObject);
    }

    /** Changes this pointer to point at a different object.
        The reference count of the old object is decremented, and it might be
        deleted if it hits zero. The new object's count is incremented.
    */
    RefCountedObjectPtr& operator=(const RefCountedObjectPtr& other)
    {
        return operator=(other.referencedObject);
    }

    /** Changes this pointer to point at a different object.
        The reference count of the old object is decremented, and it might be
        deleted if it hits zero. The new object's count is incremented.
    */
    template <typename Convertible>
    RefCountedObjectPtr& operator=(const RefCountedObjectPtr<Convertible>& other)
    {
        return operator=(other.get());
    }

    /** Changes this pointer to point at a different object.

        The reference count of the old object is decremented, and it might be
        deleted if it hits zero. The new object's count is incremented.
    */
    RefCountedObjectPtr& operator=(ReferencedType* newObject)
    {
        if (newObject != nullptr) return operator=(*newObject);

        reset();
        return *this;
    }

    /** Changes this pointer to point at a different object.

        The reference count of the old object is decremented, and it might be
        deleted if it hits zero. The new object's count is incremented.
    */
    RefCountedObjectPtr& operator=(ReferencedType& newObject)
    {
        if (referencedObject != &newObject)
        {
            newObject.incReferenceCount();
            auto* oldObject = referencedObject;
            referencedObject = &newObject;
            decIfNotNull(oldObject);
        }

        return *this;
    }

    /** Resets this pointer to a null pointer. */
    RefCountedObjectPtr& operator=(decltype(nullptr))
    {
        reset();
        return *this;
    }

    /** Takes-over the object from another pointer. */
    RefCountedObjectPtr& operator=(RefCountedObjectPtr&& other) noexcept
    {
        std::swap(referencedObject, other.referencedObject);
        return *this;
    }

    /** Destructor.
        This will decrement the object's reference-count, which will cause the
        object to be deleted when the ref-count hits zero.
    */
    ~RefCountedObjectPtr()
    {
        decIfNotNull(referencedObject);
    }

    //==============================================================================
    /** Returns the object that this pointer references.
        The pointer returned may be null, of course.
    */
    ReferencedType* get() const noexcept
    {
        return referencedObject;
    }

    /** Resets this object to a null pointer. */
    void reset() noexcept
    {
        auto oldObject = referencedObject; // need to null the pointer before deleting the object
        referencedObject = nullptr;        // in case this ptr is itself deleted as a side-effect
        decIfNotNull(oldObject);           // of the destructor
    }

    // the -> operator is called on the referenced object
    ReferencedType* operator->() const noexcept
    {
        GLUE_ASSERT(referencedObject != nullptr); // null pointer method call!
        return referencedObject;
    }

    /** Dereferences the object that this pointer references.
        The pointer returned may be null, of course.
    */
    ReferencedType& operator*() const noexcept
    {
        GLUE_ASSERT(referencedObject != nullptr);
        return *referencedObject;
    }

    /** Checks whether this pointer is null */
    bool operator==(decltype(nullptr)) const noexcept
    {
        return referencedObject == nullptr;
    }
    /** Checks whether this pointer is null */
    bool operator!=(decltype(nullptr)) const noexcept
    {
        return referencedObject != nullptr;
    }

    /** Compares two RefCountedObjectPtrs. */
    bool operator==(const ObjectType* other) const noexcept
    {
        return referencedObject == other;
    }
    /** Compares two RefCountedObjectPtrs. */
    bool operator==(const RefCountedObjectPtr& other) const noexcept
    {
        return referencedObject == other.get();
    }
    /** Compares two RefCountedObjectPtrs. */
    bool operator!=(const ObjectType* other) const noexcept
    {
        return referencedObject != other;
    }
    /** Compares two RefCountedObjectPtrs. */
    bool operator!=(const RefCountedObjectPtr& other) const noexcept
    {
        return referencedObject != other.get();
    }

#if GLUE_STRICT_REFCOUNTEDPOINTER
    /** Checks whether this pointer is null */
    explicit operator bool() const noexcept
    {
        return referencedObject != nullptr;
    }

#else
    /** Returns the object that this pointer references.
        The pointer returned may be null, of course.
        Note that this methods allows the compiler to be very lenient with what it allows you to do
        with the pointer, it's safer to disable this by setting GLUE_STRICT_REFCOUNTEDPOINTER=1,
       which increased type safety and can prevent some common slip-ups.
    */
    operator ReferencedType*() const noexcept
    {
        return referencedObject;
    }
#endif

private:
    //==============================================================================
    ReferencedType* referencedObject = nullptr;

    static void incIfNotNull(ReferencedType* o) noexcept
    {
        if (o != nullptr) o->incReferenceCount();
    }

    static void decIfNotNull(ReferencedType* o) noexcept
    {
        if (o != nullptr && o->decReferenceCountWithoutDeleting())
            ContainerDeletePolicy<ReferencedType>::destroy(o);
    }
};

//==============================================================================
/** Compares two RefCountedObjectPtrs. */
template <typename Type>
bool operator==(const Type* object1, const RefCountedObjectPtr<Type>& object2) noexcept
{
    return object1 == object2.get();
}

/** Compares two RefCountedObjectPtrs. */
template <typename Type>
bool operator!=(const Type* object1, const RefCountedObjectPtr<Type>& object2) noexcept
{
    return object1 != object2.get();
}

} // namespace glue
