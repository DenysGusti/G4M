#ifndef G4M_EUROPE_DG_DEBUGGING_HPP
#define G4M_EUROPE_DG_DEBUGGING_HPP

#include <iostream>

#if PR_DEBUG == 1
#define LOG_DEBUG(...) do { cout << format(__VA_ARGS__) << '\n'; } while(false)
#elif defined(PR_RELEASE)
#define LOG_DEBUG(...)
#endif


using namespace std;

namespace g4m::diagnostics {

    struct AllocationMetrics {
        uint64_t TotalAllocated = 0;
        uint64_t TotalFreed = 0;

        [[nodiscard]] uint64_t currentUsage() const noexcept {
            return TotalAllocated - TotalFreed;
        }

        void printMemoryUsage() const noexcept {
            cout << "Memory Usage: " << currentUsage() << " bytes\n"
                 << "Total Allocated: " << TotalAllocated << " bytes\n"
                 << "Total Freed: " << TotalFreed << " bytes\n";
        }
    };

    static AllocationMetrics s_AllocationMetrics;
}

#if PR_DEBUG == 1

void *operator new(const size_t size) {
    g4m::diagnostics::s_AllocationMetrics.TotalAllocated += size;
    return malloc(size);
}

void operator delete(void *memory, const size_t size) noexcept {
    g4m::diagnostics::s_AllocationMetrics.TotalFreed += size;
    free(memory);
}

void operator delete(void *memory) noexcept {
    free(memory);
}

#endif

#endif
