#ifndef MemoryDiagnostics_Included
#define MemoryDiagnostics_Included

/* Set of utility functions that can be used to track allocations and deallocations of
 * various node types. In actual C++ code, you wouldn't do something like this and would
 * instead rely on external tools like the wonderful Valgrind, which you'll see if you
 * continue on to CS107!
 */

#include <string>

/* Macro: TRACK_ALLOCATIONS_OF(type)
 * ------------------------------------------------------------------------------
 * Hooks the specified node type into the memory diagnostics system. To use this,
 * inside the struct or class that you're defining, insert a line like this one:
 *
 *     struct MyType {
 *         ...
 *         TRACK_ALLOCATIONS_OF(MyType);
 *     };
 */
#define TRACK_ALLOCATIONS_OF(type) /* Something you shouldn't worry about */

/* Function: instancesOf(const std::string& type);
 * ----------------------------------------------------------------------
 * Returns how many instances of a particular type have been allocated
 * but not yet deallocated. You can use this to detect whether a certain
 * operation has leaked memory by calling instancesOf at the beginning of
 * a block of code and at the end, then seeing whether the values are the
 * same. For example:
 *
 *    int count = instancesOf("History::Cell");
 *    // do something with a history object
 *    if (instancesOf("History::Cell") != count) {
 *        ... oops! memory leak! ...
 *    }
 *
 * In order for this operation to work, you need to have added the
 * TRACK_ALLOCATIONS_OF macro to the type in question. You also need to
 * give the full name of the type in question, so for a type called Inner
 * nested in a class called Outer, you'd use instancesOf("Outer::Inner")
 * rather than instancesOf("Inner") or instancesOf("Outer"). If the type
 * name can't be found, this will report an error.
 */
int instancesOf(const std::string& typeName);











/* * * * * Implementation Below This Point * * * * */










#include <typeinfo>
#include <cstddef>
#include <typeindex>
#include <unordered_map>

/* Helper functions for memory diagnostics. You are not expected to use any of these
 * functions in your code.
 */
namespace MemoryDiagnostics {
    void recordNew(const std::type_info& type);
    void recordDelete(const std::type_info& type);

    template <typename T> struct MemorySentinel {
        static int initializer;
    };

    /* Installs the specified type into the main type tables. Returns an integer value
     * that is completely of no consequence.
     */
    int registerSentinel(const std::type_info& type);

    /* Hook to allocate and deallocate memory. Use this to customize how the memory system
     * allocates and deallocates objects of your type.
     */
    template <typename T> struct Allocator {
        static void* scalarAlloc(std::size_t bytes) {
            return ::operator new(bytes);
        }

        static void* vectorAlloc(std::size_t bytes) {
            return ::operator new[](bytes);
        }

        static void scalarFree(void* memory) {
            ::operator delete(memory);
        }

        static void vectorFree(void* memory) {
            ::operator delete[](memory);
        }
    };
}

template <typename T>
int MemoryDiagnostics::MemorySentinel<T>::initializer = registerSentinel(typeid(T));

/* Implementation of TRACK_ALLOCATIONS introduces operator new/delete hooks that call
 * into the memory diagnostics system.
 *
 * If you were redirected here by hitting F2 or fn+F2, you've been sent to the
 * implementation of TRACK_ALLOCATIONS_OF rather than the definition. Scroll higher
 * up in this file for more information.
 */
#undef TRACK_ALLOCATIONS_OF
#define TRACK_ALLOCATIONS_OF(Type)                                           \
    void* operator new(std::size_t bytes) {                                  \
        (void) ::MemoryDiagnostics::MemorySentinel<Type>::initializer;       \
        ::MemoryDiagnostics::recordNew(typeid(Type));                        \
        return MemoryDiagnostics::Allocator<Type>::scalarAlloc(bytes);       \
    }                                                                        \
    void* operator new[](std::size_t bytes) {                                \
        ::MemoryDiagnostics::recordNew(typeid(Type));                        \
        return MemoryDiagnostics::Allocator<Type>::vectorAlloc(bytes);       \
    }                                                                        \
    void operator delete(void* ptr) {                                        \
        ::MemoryDiagnostics::recordDelete(typeid(Type));                     \
        return MemoryDiagnostics::Allocator<Type>::scalarFree(ptr);          \
    }                                                                        \
    void operator delete(void* ptr, std::size_t) {                           \
        ::MemoryDiagnostics::recordDelete(typeid(Type));                     \
        return MemoryDiagnostics::Allocator<Type>::scalarFree(ptr);          \
    }                                                                        \
    void operator delete[](void* ptr) {                                      \
        ::MemoryDiagnostics::recordDelete(typeid(Type));                     \
        return MemoryDiagnostics::Allocator<Type>::vectorFree(ptr);          \
    }                                                                        \
    void operator delete[](void* ptr, std::size_t) {                         \
        ::MemoryDiagnostics::recordDelete(typeid(Type));                     \
        return MemoryDiagnostics::Allocator<Type>::vectorFree(ptr);          \
    }                                                                        \
    static_assert(true, "Just so we need a semicolon.")

#endif
