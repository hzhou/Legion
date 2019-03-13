#ifndef AM_MPI_H_INCLUDED
#define AM_MPI_H_INCLUDED

#include <thread>
#include <atomic>
#include <mpi.h>
#include <cstddef>
#include <cstdlib>
#include <cstdio>
#include <assert.h>

typedef void (*AM_HANDLER_T)();
typedef void (*AM_SHORT_1)(int src, int arg_1);
typedef void (*AM_MEDIUM_1)(int src, void* msg, int len, int arg_1);
typedef void (*AM_SHORT_2)(int src, int arg_1, int arg_2);
typedef void (*AM_MEDIUM_2)(int src, void* msg, int len, int arg_1, int arg_2);
typedef void (*AM_SHORT_3)(int src, int arg_1, int arg_2, int arg_3);
typedef void (*AM_MEDIUM_3)(int src, void* msg, int len, int arg_1, int arg_2, int arg_3);
typedef void (*AM_SHORT_4)(int src, int arg_1, int arg_2, int arg_3, int arg_4);
typedef void (*AM_MEDIUM_4)(int src, void* msg, int len, int arg_1, int arg_2, int arg_3, int arg_4);
typedef void (*AM_SHORT_5)(int src, int arg_1, int arg_2, int arg_3, int arg_4, int arg_5);
typedef void (*AM_MEDIUM_5)(int src, void* msg, int len, int arg_1, int arg_2, int arg_3, int arg_4, int arg_5);
typedef void (*AM_SHORT_6)(int src, int arg_1, int arg_2, int arg_3, int arg_4, int arg_5, int arg_6);
typedef void (*AM_MEDIUM_6)(int src, void* msg, int len, int arg_1, int arg_2, int arg_3, int arg_4, int arg_5, int arg_6);
typedef void (*AM_SHORT_7)(int src, int arg_1, int arg_2, int arg_3, int arg_4, int arg_5, int arg_6, int arg_7);
typedef void (*AM_MEDIUM_7)(int src, void* msg, int len, int arg_1, int arg_2, int arg_3, int arg_4, int arg_5, int arg_6, int arg_7);
typedef void (*AM_SHORT_8)(int src, int arg_1, int arg_2, int arg_3, int arg_4, int arg_5, int arg_6, int arg_7, int arg_8);
typedef void (*AM_MEDIUM_8)(int src, void* msg, int len, int arg_1, int arg_2, int arg_3, int arg_4, int arg_5, int arg_6, int arg_7, int arg_8);
typedef void (*AM_SHORT_9)(int src, int arg_1, int arg_2, int arg_3, int arg_4, int arg_5, int arg_6, int arg_7, int arg_8, int arg_9);
typedef void (*AM_MEDIUM_9)(int src, void* msg, int len, int arg_1, int arg_2, int arg_3, int arg_4, int arg_5, int arg_6, int arg_7, int arg_8, int arg_9);
typedef void (*AM_SHORT_10)(int src, int arg_1, int arg_2, int arg_3, int arg_4, int arg_5, int arg_6, int arg_7, int arg_8, int arg_9, int arg_10);
typedef void (*AM_MEDIUM_10)(int src, void* msg, int len, int arg_1, int arg_2, int arg_3, int arg_4, int arg_5, int arg_6, int arg_7, int arg_8, int arg_9, int arg_10);
typedef void (*AM_SHORT_11)(int src, int arg_1, int arg_2, int arg_3, int arg_4, int arg_5, int arg_6, int arg_7, int arg_8, int arg_9, int arg_10, int arg_11);
typedef void (*AM_MEDIUM_11)(int src, void* msg, int len, int arg_1, int arg_2, int arg_3, int arg_4, int arg_5, int arg_6, int arg_7, int arg_8, int arg_9, int arg_10, int arg_11);
typedef void (*AM_SHORT_12)(int src, int arg_1, int arg_2, int arg_3, int arg_4, int arg_5, int arg_6, int arg_7, int arg_8, int arg_9, int arg_10, int arg_11, int arg_12);
typedef void (*AM_MEDIUM_12)(int src, void* msg, int len, int arg_1, int arg_2, int arg_3, int arg_4, int arg_5, int arg_6, int arg_7, int arg_8, int arg_9, int arg_10, int arg_11, int arg_12);
typedef void (*AM_SHORT_13)(int src, int arg_1, int arg_2, int arg_3, int arg_4, int arg_5, int arg_6, int arg_7, int arg_8, int arg_9, int arg_10, int arg_11, int arg_12, int arg_13);
typedef void (*AM_MEDIUM_13)(int src, void* msg, int len, int arg_1, int arg_2, int arg_3, int arg_4, int arg_5, int arg_6, int arg_7, int arg_8, int arg_9, int arg_10, int arg_11, int arg_12, int arg_13);
typedef void (*AM_SHORT_14)(int src, int arg_1, int arg_2, int arg_3, int arg_4, int arg_5, int arg_6, int arg_7, int arg_8, int arg_9, int arg_10, int arg_11, int arg_12, int arg_13, int arg_14);
typedef void (*AM_MEDIUM_14)(int src, void* msg, int len, int arg_1, int arg_2, int arg_3, int arg_4, int arg_5, int arg_6, int arg_7, int arg_8, int arg_9, int arg_10, int arg_11, int arg_12, int arg_13, int arg_14);
typedef void (*AM_SHORT_15)(int src, int arg_1, int arg_2, int arg_3, int arg_4, int arg_5, int arg_6, int arg_7, int arg_8, int arg_9, int arg_10, int arg_11, int arg_12, int arg_13, int arg_14, int arg_15);
typedef void (*AM_MEDIUM_15)(int src, void* msg, int len, int arg_1, int arg_2, int arg_3, int arg_4, int arg_5, int arg_6, int arg_7, int arg_8, int arg_9, int arg_10, int arg_11, int arg_12, int arg_13, int arg_14, int arg_15);
typedef void (*AM_SHORT_16)(int src, int arg_1, int arg_2, int arg_3, int arg_4, int arg_5, int arg_6, int arg_7, int arg_8, int arg_9, int arg_10, int arg_11, int arg_12, int arg_13, int arg_14, int arg_15, int arg_16);
typedef void (*AM_MEDIUM_16)(int src, void* msg, int len, int arg_1, int arg_2, int arg_3, int arg_4, int arg_5, int arg_6, int arg_7, int arg_8, int arg_9, int arg_10, int arg_11, int arg_12, int arg_13, int arg_14, int arg_15, int arg_16);

void AM_Init(int *p_node_this, int *p_node_size);
void AM_Init_segments(size_t size);
void AM_Finalize();
void AM_add_handler(int i, AM_HANDLER_T handler);
void AMPoll();
void AMPoll_cancel();
void AM_short_n(int n, int tgt, int handler, const int *args);
void AM_medium_n(int n, int tgt, int handler, const void *msg, int len, const int *args);
void AM_long_n(int n, int tgt, int handler, const void *msg, int len, void *dst, const int *args);

#endif /* AM_MPI_H_INCLUDED */
