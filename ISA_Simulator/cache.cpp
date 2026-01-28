#include "cache.h"
#include "data_memory.h"
#include "log_handler.h"
#include <iostream>
#include <iomanip>

using namespace std;

// Cache array
CacheLine cache[CACHE_LINES];

// Cache performance counters
uint64_t cache_hits = 0;
uint64_t cache_misses = 0;
uint64_t cache_stall_cycles = 0;

// Initialize cache - all lines invalid
void initialize_cache()
{
    for (int i = 0; i < CACHE_LINES; i++)
    {
        cache[i].valid = false;
        cache[i].tag = 0;
        cache[i].data = 0;
    }
    
    cache_hits = 0;
    cache_misses = 0;
    cache_stall_cycles = 0;
    
    cout << "Cache initialized: " << CACHE_LINES << " lines, direct-mapped" << endl;
}

// Get index bits from address
uint8_t get_cache_index(uint8_t address)
{
    // Index = lower INDEX_BITS of address
    return address & ((1 << INDEX_BITS) - 1);
}

// Get tag bits from address
uint8_t get_cache_tag(uint8_t address)
{
    // Tag = upper TAG_BITS of address
    return address >> INDEX_BITS;
}

// Cache read function
uint8_t cache_read(uint8_t address, bool &hit_flag, int &stall_cycles)
{
    uint8_t index = get_cache_index(address);
    uint8_t tag = get_cache_tag(address);
    
    // Check for cache hit
    if (cache[index].valid && cache[index].tag == tag)
    {
        // Cache HIT
        hit_flag = true;
        stall_cycles = 0;  // Hit takes 1 cycle (no additional stall)
        cache_hits++;
        
        cout << "    [CACHE] HIT at address 0x" << hex << (int)address << dec
             << " (index=" << (int)index << ", tag=" << (int)tag << ")"
             << " -> data=0x" << hex << (int)cache[index].data << dec << endl;
        
        logger1("CACHE HIT: address=0x" + to_string(address) + 
               " index=" + to_string(index) + " tag=" + to_string(tag) +
               " data=0x" + to_string(cache[index].data));
        
        return cache[index].data;
    }
    else
    {
        // Cache MISS - need to fetch from main memory
        hit_flag = false;
        stall_cycles = CACHE_MISS_PENALTY - 1;  // Miss penalty cycles (minus current cycle)
        cache_misses++;
        cache_stall_cycles += stall_cycles;
        
        // Fetch from main memory
        uint8_t data = read_data_memory(address);
        
        // Update cache line
        cache[index].valid = true;
        cache[index].tag = tag;
        cache[index].data = data;
        
        cout << "    [CACHE] MISS at address 0x" << hex << (int)address << dec
             << " (index=" << (int)index << ", tag=" << (int)tag << ")"
             << " -> fetching from memory, stall " << stall_cycles << " cycles" << endl;
        
        logger1("CACHE MISS: address=0x" + to_string(address) + 
               " index=" + to_string(index) + " tag=" + to_string(tag) +
               " fetched data=0x" + to_string(data) + " stall_cycles=" + to_string(stall_cycles));
        
        return data;
    }
}

// Cache write function (write-through policy)
int cache_write(uint8_t address, uint8_t data)
{
    uint8_t index = get_cache_index(address);
    uint8_t tag = get_cache_tag(address);
    
    // Write-through: always write to memory
    write_data_memory(address, data);
    
    // Update cache if line is valid and matches
    if (cache[index].valid && cache[index].tag == tag)
    {
        // Cache line exists - update it
        cache[index].data = data;
        cache_hits++;
        
        cout << "    [CACHE] WRITE HIT at address 0x" << hex << (int)address << dec
             << " -> updated cache and memory" << endl;
        
        logger1("CACHE WRITE HIT: address=0x" + to_string(address) + 
               " data=0x" + to_string(data));
        
        return 0;  // No additional stall for write hit
    }
    else
    {
        // Cache miss on write - allocate new line (write-allocate)
        cache[index].valid = true;
        cache[index].tag = tag;
        cache[index].data = data;
        cache_misses++;
        
        cout << "    [CACHE] WRITE MISS at address 0x" << hex << (int)address << dec
             << " -> allocating cache line, writing to memory" << endl;
        
        logger1("CACHE WRITE MISS: address=0x" + to_string(address) + 
               " data=0x" + to_string(data) + " (write-allocate)");
        
        // For simplicity, treat write misses with same penalty
        int stall_cycles = CACHE_MISS_PENALTY - 1;
        cache_stall_cycles += stall_cycles;
        return stall_cycles;
    }
}

// Display cache contents
void display_cache()
{
    cout << "\n=== CACHE CONTENTS ===" << endl;
    cout << "Line | Valid | Tag  | Data" << endl;
    cout << "-----|-------|------|--------" << endl;
    
    for (int i = 0; i < CACHE_LINES; i++)
    {
        cout << "  " << i << "  |   " << (cache[i].valid ? "1" : "0") 
             << "   | 0x" << hex << setw(2) << setfill('0') << (int)cache[i].tag
             << " | 0x" << setw(2) << setfill('0') << (int)cache[i].data 
             << dec << setfill(' ') << endl;
    }
    cout << endl;
}

// Display cache statistics
void display_cache_stats()
{
    double hit_rate = 0.0;
    uint64_t total_accesses = cache_hits + cache_misses;
    
    if (total_accesses > 0)
    {
        hit_rate = (double)cache_hits / (double)total_accesses * 100.0;
    }
    
    cout << "\n=====================================" << endl;
    cout << "      CACHE STATISTICS" << endl;
    cout << "=====================================" << endl;
    cout << "Cache Hits:          " << cache_hits << endl;
    cout << "Cache Misses:        " << cache_misses << endl;
    cout << "Total Accesses:      " << total_accesses << endl;
    cout << "Hit Rate:            " << fixed << setprecision(2) << hit_rate << "%" << endl;
    cout << "Cache Stall Cycles:  " << cache_stall_cycles << endl;
    cout << "=====================================" << endl;
    cout << endl;
}
