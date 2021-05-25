## Queue
These operators manage a first in, first out, queue of values. The queue can 
hold up to 16 values. The length of the queue can be dynamically changed and 
the contents will be preserved. There is also an averaging operator which is 
useful for smoothing input values.

The queue has a maximum size of 64 elements, with a dynamic length marker,
set by `Q.N`.

Most operators manipulates the elements up to (and including) length. Exceptions are `Q.I i x` and `Q.P2`.

Examples, only first 8 elements shown for clarity:
By default all elements of the queue have a value of `0` and the length is set to 1.
```
Q.N "length" ->| 
element nb: 1  |  2 3 4 5 6 7 8 
value       0  |  0 0 0 0 0 0 0
```

Using the Q OP will add values to the beginning of the queue and push the other elements to the right.
`Q 1`
```
1  |  0 0 0 0 0 0 0

Q 2 // add 2 to queue
Q 3 // add 3 to queue

3  |  2 1 0 0 0 0 0
```

Using the `Q` getter OP will return the last element in the queue, but not modify content or the state of the queue.

```
Q // will return 3

3  |  2 1 0 0 0 0 0
```

Using the `Q.N` OP will either return the position of the end marker (1-indexed) or move it:

```
Q.N 2 // increace the length to two by moving the end marker:

3 2  |  1 0 0 0 0 0

Q // get the value at the end, now `2`
```

By default grow is disabled, but it can be turned on with `Q.GRW 1`. With grow enabled, the queue will automatically expand when new elements are added with `Q x` and likewise shrink when reading with `Q`.

```
Q.GRW // enable grow
3 2  |  1 0 0 0 0 0
Q 4 // add to to queue
4 3 2  |  1 0 0 0 0
Q // read element from queue, will return 2
4 3  |  2 1 0 0 0 0
```

