/*
 * The use-case for this memory manager is to tackle one specific scenario:
 *
 * Allocations and de-allocations are happening _outside_ the constructor and destructor of
 * objects. This means that a typical scenario where the stack would unwind due to objects
 * going out of scope no longer happens.
 *
 * Consider a class with the following methods:
 *   MyClass();
 *   ~MyClass();
 *   setup();
 *   run();
 *   teardown();
 *
 * Now consider that all memory allocations occur only in setup(), and all memory deallocations
 * occur only in teardown(), and that a loop of (setup->run->teardown) happens repeatedly.
 * This would mean that using smart pointers would not suffice, since the destructor doesn't get
 * called within the loop.
 *
 * If many heap allocated objects and pointers are created manually in setup(), this may be difficult
 * to track and free in teardown(). This is where this memory manager comes in.
 *
 * 1) Use this manager to create _all_ objects and arrays on the heap required.
 * 2) Just call MemoryManager.dealloc() inside teardown().
 *    This will automatically free everything allocated.
 *    No need to track down every last pointer.
 */


#include <iostream>
#include <vector>
#include <memory>

class MemoryManager {
private:
    struct MemoryBlock {
        void* ptr;
        void (*deleter)(void*); // Function pointer for custom deletion

        MemoryBlock(void* p, void (*d)(void*)) : ptr(p), deleter(d) {}

        ~MemoryBlock() { 
            if (ptr) deleter(ptr); // Ensures proper deletion
        }
    };

    std::vector<std::unique_ptr<MemoryBlock>> memoryBlocks;

public:
    /**
     * @brief Allocate an array of POD Types.
     *        Example: int, float, double etc.
     *
     * @tparam T Type of data
     * @param size Number of elements to allocate
     * @return Pointer to the newly allocated data on the heap
     */
    template <typename T>
    T* allocateArray(size_t size) {
        static_assert(!std::is_class_v<T>, "Use allocate<T>() for custom classes, not allocateArray.");
        void* rawPtr = static_cast<void*>(new T[size]); // Allocate and cast to void*
        memoryBlocks.emplace_back(std::make_unique<MemoryBlock>(
            rawPtr, [](void* p) { delete[] static_cast<T*>(p); } // Correct array deletion
        ));
        return static_cast<T*>(rawPtr);

#ifndef DNDEBUG
        std::cout << "Allocated array of " << typeid(T).name() << "[" << size << "]\n";
#endif
    }

    /**
     * @brief Allocate a class object, with its constructor arguments.
     *
     * @tparam T Type of class/struct
     * @param args Input arguments to the constructor
     * @return Pointer to the newly allocated class on the heap
     */
    template <typename T, typename... Args>
    T* allocate(Args&&... args) {
        static_assert(std::is_class_v<T>, "Use allocateArray<T>() for primitive types, not allocate<T>().");
        void* rawPtr = static_cast<void*>(new T(std::forward<Args>(args)...)); // Allocate object
        memoryBlocks.emplace_back(std::make_unique<MemoryBlock>(
            rawPtr, [](void* p) { delete static_cast<T*>(p); } // Correct object deletion
        ));
#ifndef DNDEBUG
        std::cout << "Allocated object of " << typeid(T).name() << "\n";
#endif
        return static_cast<T*>(rawPtr);
    }

    /**
     * @brief Deallocates all internally held memory.
     */
    void dealloc() {
        memoryBlocks.clear(); // Frees all memory
#ifndef DNDEBUG
        std::cout << "All memory deallocated.\n";
#endif
    }
};

