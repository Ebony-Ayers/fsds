# Fast Simple Data Structures
A set of data structures prioritising speed over memory efficiancy.
## How to compile
Add the top level directory of this repository as a subdirectory in the application's cmakelists then link it publicly against the executable target. Do not forget to `#include "fsds.hpp"`.
```cmake
add_subdirectory(lib/fsds)
target_link_libraries(${primary_target_name} PUBLIC fsds)
```

## Getting started
There are a number of data structures included in fsds, located [here](src/). 

Each data structure provides some benefits and has targeted use cases:

### Containers
1. `List<T>`. A simple contiguous list. `O(1)` insert at end, `O(n)` random insert, `O(1)` remove from end, `O(n)` random remove, `O(1)` random access. Guarenteed to be contiguous and in order. Functionally equivalent to `std::vector<T>`.

2. `LinkedList<T>`. A simple doubly linked list. `O(1)` insert at end, `O(1)` random insert, `O(1)` remove from end, `O(1)` random remove, `O(n)` random access. Functionally equivalent to `std::list<T>`.

3. `SPSCQueue<T>`. A simple queue implemented with a ring buffer, designed for single producer single consumer.  `O(1)` enqueue, `O(1)` dequeue. Guarenteed to be contiguous.

4. `SPMCQueue<T>`. A queue with mass enqueue designed for single producer multi consumer.  `O(1)` enqueue, `O(1)` dequeue.

5. `MPSCQueue<T>`. A queue with mass dequeue designed for multi producer single consumer.  `O(1)` enqueue, `O(1)` dequeue.

6. `FinitePQueue<T,N>`. A priority queue with finite number of priorities known at compile time.  `O(1)` enqueue, `O(1)` dequeue.

7. `ChunkList<T,chunkSize>`. A list designed solely for storing objects. `O(1)` add, `O(1)` remove. Pointers into the container are guaranteed to be stable and any active objects will be destroyed upon container destruction. Elements are not contiguous. The container is not indexable or iterable. Functions like an allocator except faster but with a higher memory overhead. There is some protection against dangling pointers but not guaranteed. The intended use case is when you have a type of object you know you will be making and/or destroying a non trivial number of, but order and iterability are not required, rather than dynamically allocating each one seperately, a chunk list would be faster.
### Strings
1. `StaticString`. An immutable string that does not own memory. Guaranteed to be utf-8 compliant.

2. `DynamicString`. A mutable string that owns memory. Guaranteed to be utf-8 compliant.
