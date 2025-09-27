//
// Created by julian on 9/27/25.
//

#ifndef X11TEST_ALLOCTRACKER_H
#define X11TEST_ALLOCTRACKER_H
#include <iostream>

// todo: thread safety
static int allocCount = 0;
static int freeCount = 0;
static size_t allocSum = 0;
static size_t freeSum = 0;

#if TRACK_ALLOCATIONS
void *operator new(const size_t size) {
    std::cout << "Allocating " << size << " bytes" << std::endl;
    allocCount++;
    allocSum += size;
    const auto ptr = malloc(size);

    if (!ptr) throw std::bad_alloc();
    return ptr;
}

void operator delete(void *ptr) noexcept {
    if (ptr) {
        std::cout << "Freeing " << ptr << std::endl;
        freeCount++;
        free(ptr);
    } else std::cout << "Warning! Attempt to free null pointer!" << std::endl;
}

void operator delete(void *ptr, const size_t size) noexcept {
    std::cout << "Freeing " << ptr << " with size: " << size << std::endl;
    freeCount++;
    freeSum += size;
    free(ptr);
}
#endif

inline void printAllocStats() {
    std::cout << "=== Allocation statistics ===" << std::endl;
    std::cout << "Allocations: " << allocCount
            << ", total amount in Bytes: " << allocSum
            << ", average: " << static_cast<float>(allocSum) / static_cast<float>(allocCount) << std::endl;
    std::cout << "Frees: " << freeCount
            << ", total amount in Bytes: " << freeSum
            << ", average: " << static_cast<float>(freeSum) / static_cast<float>(freeCount) << std::endl;

    if (allocSum != freeSum) {
        std::cout << "Memory leak detected! Difference: " << (allocSum - freeSum) << " Bytes" << std::endl;
    } else {
        std::cout << "No memory leaks detected." << std::endl;
    }
    std::cout << "=============================" << std::endl;
}

#endif //X11TEST_ALLOCTRACKER_H
