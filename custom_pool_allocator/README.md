# Custom Pool Allocator

A fixed-size object pool allocator using an intrusive free list, eliminating
per-allocation calls into the OS/heap allocator.

## Design
- Pre-allocated buffer of N fixed-size slots, sized/aligned for type T
- Intrusive free list: free slots store a `next` pointer in their own unused bytes
  (zero extra memory overhead for the free list itself)
- `allocate`/`deallocate`: raw memory operations, O(1), just free-list push/pop
- `construct`/`destroy`: layer placement-new/explicit destructor calls on top,
  supporting non-trivial types with real constructors/destructors

## Benchmark Results
Measured on 4-core CPU (2712 MHz), Release build, construct+destroy per iteration.

| Allocator            | Time per op |
|-----------------------|-------------|
| Pool allocator         | 3.37 ns    |
| new/delete (baseline)  | 64.5 ns    |

~19x faster than new/delete, consistent with avoiding allocator metadata
walks and heap-lock overhead discussed in the design notes.

## Known limitations
- Not thread-safe as implemented (single-threaded free list) — a lock-free
  version using atomics (similar to the SPSC ring buffer project) would be
  a natural extension
- Fixed pool size at compile time