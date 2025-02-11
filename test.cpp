#include "memmanager.h"

int main() {
    MemoryManager manager;
    manager.allocateArray<int>(100);      // Allocate int[100]
    manager.allocateArray<double>(200);   // Allocate double[200]
    manager.allocate<MyClass>(42, 3.14);  // Allocate a MyClass instance
    manager.stop(); // Deallocates all memory safely

    return 0;
}
