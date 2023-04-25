# Fast Simple Data Structures
A set of data structures prioritising speed of memory efficiancy.
## How to compile
Add the top level directory of this repository as a subdirectory in the applications cmakelists then link it publically against the executible target. Do not forget to `#include "fsds.hpp"`.
```cmake
add_subdirectory(lib/fsds)
target_link_libraries(${primary_target_name} PUBLIC fsds)
```

## Getting started
There are a number of data structures included in fsds, located [here](src/). 

Each data structure provides some benefits and has targeted use cases:

### Containers
1. `List<T>`. A simple contiguous list. `O(1)` insert at end, `O(n)` random insert, `O(1)` remove from end, `O(n)` random remove, `O(1)` random access. Garenteed to be contiguous and in order.
2. `LinkedList<T>`. A simple doudbly linked list. `O(1)` insert at end, `O(1)` random insert, `O(1)` remove from end, `O(1)` random remove, `O(n)` random access.
3. `SPSCQueue<T>`. A simple queue implemented with a ring buffer, designed for single producer single consumer.  `O(1)` enqueue, `O(1)` dequeue.  Garenteed to be contiguous.
4. `SPMCQueue<T>`. A queue with mass enqueue designed for single producer multi consumer.  `O(1)` enqueue, `O(1)` dequeue.
5. `MPSCQueue<T>`. A queue with mass dequeue designed for multi producer single consumer.  `O(1)` enqueue, `O(1)` dequeue.
6. `FinitePQueue<T,N>`. A priority queue with finite number of priorities known at compile time.  `O(1)` enqueue, `O(1)` dequeue.
7. `FastInsertList<T>`.  A list with fast interts. `O(1)` insert at end, `O(1)` random insert, `O(n)` remove from end, `O(n)` random remove, `O(n)` random access.
### Strings
1. `StaticString`. An imutable string that does not own memory. Garanteed to be utf-8 compliant.
2. `DynamicString`. A mutable string that owns memory. Garanteed to be utf-8 compliant.
