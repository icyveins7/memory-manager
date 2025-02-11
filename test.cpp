// g++ test.cpp -o test
// cl /EHsc test.cpp
//
// valgrind --leak-check=yes ./test

#include "memmanager.h"

// Example custom class
class MyClass {
public:
    int m_x;
    double m_y;
    MyClass(int x, double y) : m_x(x), m_y(y) {
        std::cout << "MyClass constructed with (" << x << ", " << y << ")\n"; 
    }
    ~MyClass() { 
        std::cout << "MyClass destroyed\n"; 
    }
    void print() { std::cout << m_x << ", " << m_y << "\n"; }
};

int main() {
    MemoryManager manager;

    for (int i = 0; i < 2; i++){
        int* iarr = manager.allocateArray<int>(100);      // Allocate int[100]
        double* darr = manager.allocateArray<double>(200);   // Allocate double[200]
        MyClass* clsptr = manager.allocate<MyClass>(42, 3.14);  // Allocate a MyClass instance
        // pretend to fill some values
        for (int j = 0; j < 100; ++j)
            iarr[j] = j;
        for (int j = 0; j < 200; ++j)
            darr[j] = j * 0.1;
        // pretend to use the values
        printf("int[0] = %d\n", iarr[0]);
        printf("double[0] = %f\n", darr[0]);
        clsptr->print();

        // some long amount of code...
        manager.dealloc(); // Deallocates all memory safely
    }

    return 0;
}
