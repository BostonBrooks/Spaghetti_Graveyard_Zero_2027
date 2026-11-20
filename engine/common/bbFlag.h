/// Most functions return a flag to indicate success/failure etc

#ifndef BB_FLAG_H
#define BB_FLAG_H

// the vast majority of functions return bbFlag, and other values are
// returned by reference.
typedef enum {
    bbSuccess, //Generic pass
    bbFail, //Generic fail

} bbFlag;

#endif // BB_FLAG_H

