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

        [[nodiscard]] uint64_t CurrentUsage() const noexcept {
            return TotalAllocated - TotalFreed;
        }
    };

    static AllocationMetrics s_AllocationMetrics;
}

#if PR_DEBUG == 1

void *operator new(const size_t size) {
    g4m::diagnostics::s_AllocationMetrics.TotalAllocated += size;
    // LOG_DEBUG("Allocating " << size << " bytes");
    return malloc(size);
}

void operator delete(void *memory, const size_t size) noexcept {
    g4m::diagnostics::s_AllocationMetrics.TotalFreed += size;
    // LOG_DEBUG("Freeing " << size << " bytes");
    free(memory);
}

static void printMemoryUsage() {
    cout << "Memory Usage: " << g4m::diagnostics::s_AllocationMetrics.CurrentUsage() << " bytes\n"
         << "Total Allocated: " << g4m::diagnostics::s_AllocationMetrics.TotalAllocated << " bytes\n"
         << "Total Freed: " << g4m::diagnostics::s_AllocationMetrics.TotalFreed << " bytes\n";
}

void operator delete(void *memory) noexcept {
    free(memory);
}

#endif

#endif
