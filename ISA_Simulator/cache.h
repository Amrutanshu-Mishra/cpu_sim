#ifndef CACHE_H
#define CACHE_H

#include <cstdint>
#include <iostream>

// Cache Configuration
// Direct-mapped cache with 8 lines
// Each line holds 1 byte (block size = 1 byte)
// Address = [TAG (5 bits) | INDEX (3 bits)]

#define CACHE_LINES 8           // Number of cache lines
#define INDEX_BITS 3            // log2(CACHE_LINES)
#define TAG_BITS 5              // 8 - INDEX_BITS (for 8-bit address)
#define CACHE_HIT_CYCLES 1      // Cycles for cache hit
#define CACHE_MISS_PENALTY 5    // Cycles for cache miss (memory access)

// Cache Line Structure
struct CacheLine
{
    bool valid;         // Valid bit
    uint8_t tag;        // Tag bits
    uint8_t data;       // Data (1 byte)
};

// Cache array
extern CacheLine cache[CACHE_LINES];

// Cache performance counters
extern uint64_t cache_hits;
extern uint64_t cache_misses;

// Cache-related stall counter
extern uint64_t cache_stall_cycles;

// Initialize cache (all lines invalid)
void initialize_cache();

// Cache access function
// Returns: data at address
// Sets: hit_flag to true if hit, false if miss
// Sets: stall_cycles to number of stall cycles needed (0 for hit, MISS_PENALTY-1 for miss)
uint8_t cache_read(uint8_t address, bool &hit_flag, int &stall_cycles);

// Cache write function (write-through policy)
// Returns: stall cycles needed
int cache_write(uint8_t address, uint8_t data);

// Display cache contents
void display_cache();

// Display cache statistics
void display_cache_stats();

// Helper functions
uint8_t get_cache_index(uint8_t address);
uint8_t get_cache_tag(uint8_t address);

#endif // CACHE_H
