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
    // Allocate arrays (T[])
    template <typename T>
    void allocateArray(size_t size) {
        static_assert(!std::is_class_v<T>, "Use allocate<T>() for custom classes, not allocateArray.");
        void* rawPtr = static_cast<void*>(new T[size]); // Allocate and cast to void*
        memoryBlocks.emplace_back(std::make_unique<MemoryBlock>(
            rawPtr, [](void* p) { delete[] static_cast<T*>(p); } // Correct array deletion
        ));
        std::cout << "Allocated array of " << typeid(T).name() << "[" << size << "]\n";
    }

    // Allocate single objects (custom classes)
    template <typename T, typename... Args>
    void allocate(Args&&... args) {
        static_assert(std::is_class_v<T>, "Use allocateArray<T>() for primitive types, not allocate<T>().");
        void* rawPtr = static_cast<void*>(new T(std::forward<Args>(args)...)); // Allocate object
        memoryBlocks.emplace_back(std::make_unique<MemoryBlock>(
            rawPtr, [](void* p) { delete static_cast<T*>(p); } // Correct object deletion
        ));
        std::cout << "Allocated object of " << typeid(T).name() << "\n";
    }

    void stop() {
        memoryBlocks.clear(); // Deallocates all memory
        std::cout << "All memory deallocated.\n";
    }
};

// Example custom class
class MyClass {
public:
    MyClass(int x, double y) { 
        std::cout << "MyClass constructed with (" << x << ", " << y << ")\n"; 
    }
    ~MyClass() { 
        std::cout << "MyClass destroyed\n"; 
    }
};
