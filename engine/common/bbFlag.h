/// Most functions return a flag to indicate success/failure etc

#ifndef BB_FLAG_H
#define BB_FLAG_H

// the vast majority of functions return bbFlag, and other values are
// returned by reference.
typedef enum {
    bbSuccess, //Generic pass
    bbFail, //Generic fail
    bbFull, //Pool full
    bbWrongSystem, // Handle belongs to another system
    bbWrongGeneration, // Handle generation doesn't match the header
    bbOutOfBounds, // Segment index exceeds the allocated segments
    bbNull, // Handle has the reserved null generation
    bbDoubleFree, // Freeing object not in use
    bbNotAllocated,

} bbFlag;

#endif // BB_FLAG_H

