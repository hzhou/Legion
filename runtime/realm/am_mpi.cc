#include "am_mpi.h"


MPI_Win g_am_win;
void *g_am_base;
size_t g_am_win_size;
void **g_am_bases;
int node_size;
int node_this;
AM_HANDLER_T AM_table[256];
unsigned int buf_recv[20];
MPI_Request req_recv = MPI_REQUEST_NULL;
thread_local int thread_id = 0;
std::atomic_uint num_threads;
extern int my_node_id;

void AM_Init(int *p_node_this, int *p_node_size)
{
    int mpi_thread_model;
    MPI_Init_thread(NULL, NULL, MPI_THREAD_MULTIPLE, &mpi_thread_model);
    assert(mpi_thread_model == MPI_THREAD_MULTIPLE);
    MPI_Comm_size(MPI_COMM_WORLD, &node_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &node_this);
    *p_node_size = node_size;
    *p_node_this = node_this;
}

void AM_Init_segments(size_t size)
{
    int ret;

    g_am_win_size = size;
    assert(g_am_win_size != 0);
    g_am_base = malloc(g_am_win_size);
    assert(g_am_base != NULL);
    ret = MPI_Win_create(g_am_base, g_am_win_size, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &g_am_win);
    if (ret != MPI_SUCCESS) {
        fprintf(stderr, "MPI error in [$(@args)]\n");
        exit(-1);
    }
    g_am_bases = (void **) malloc(node_size * sizeof(void*));
    ret = MPI_Allgather(&g_am_base, 8, MPI_BYTE, g_am_bases, 8, MPI_BYTE, MPI_COMM_WORLD);
    if (ret != MPI_SUCCESS) {
        fprintf(stderr, "MPI error in [$(@args)]\n");
        exit(-1);
    }
}

void AM_Finalize()
{
    MPI_Request req_final;
    int ret;

    MPI_Ibarrier(MPI_COMM_WORLD, &req_final);
    while (1) {
        int is_done;
        MPI_Status status;
        MPI_Test(&req_final, &is_done, &status);
        if (req_final == MPI_REQUEST_NULL) {
            break;
        }
        AMPoll();
    }
    AMPoll_cancel();
    if (g_am_base) {
        ret = MPI_Win_free(&g_am_win);
        if (ret != MPI_SUCCESS) {
            fprintf(stderr, "MPI error in [$(@args)]\n");
            exit(-1);
        }
        free(g_am_base);
    }
    MPI_Finalize();
}

void AM_add_handler(int i, AM_HANDLER_T handler)
{
    assert(i>=0 && i<256);
    AM_table[i] = handler;
}

void AMPoll()
{
    int ret;
    char *s;
    int len;

    if (req_recv == MPI_REQUEST_NULL) {
        ret = MPI_Irecv(buf_recv, 20, MPI_INT, MPI_ANY_SOURCE, 0x1, MPI_COMM_WORLD, &req_recv);
        if (ret != MPI_SUCCESS) {
            fprintf(stderr, "MPI error in [$(@args)]\n");
            exit(-1);
        }
    }
    while (1) {
        int got_am;
        MPI_Status status;
        MPI_Test(&req_recv, &got_am, &status);
        if (!got_am) {
            break;
        } else {
            if (buf_recv[0] < 0x100) {
                if (buf_recv[0] == 1) {
                    ((AM_SHORT_1) AM_table[buf_recv[1]]) (status.MPI_SOURCE, buf_recv[2]);
                } else if (buf_recv[0] == 2) {
                    ((AM_SHORT_2) AM_table[buf_recv[1]]) (status.MPI_SOURCE, buf_recv[2], buf_recv[3]);
                } else if (buf_recv[0] == 3) {
                    ((AM_SHORT_3) AM_table[buf_recv[1]]) (status.MPI_SOURCE, buf_recv[2], buf_recv[3], buf_recv[4]);
                } else if (buf_recv[0] == 4) {
                    ((AM_SHORT_4) AM_table[buf_recv[1]]) (status.MPI_SOURCE, buf_recv[2], buf_recv[3], buf_recv[4], buf_recv[5]);
                } else if (buf_recv[0] == 5) {
                    ((AM_SHORT_5) AM_table[buf_recv[1]]) (status.MPI_SOURCE, buf_recv[2], buf_recv[3], buf_recv[4], buf_recv[5], buf_recv[6]);
                } else if (buf_recv[0] == 6) {
                    ((AM_SHORT_6) AM_table[buf_recv[1]]) (status.MPI_SOURCE, buf_recv[2], buf_recv[3], buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7]);
                } else if (buf_recv[0] == 7) {
                    ((AM_SHORT_7) AM_table[buf_recv[1]]) (status.MPI_SOURCE, buf_recv[2], buf_recv[3], buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7], buf_recv[8]);
                } else if (buf_recv[0] == 8) {
                    ((AM_SHORT_8) AM_table[buf_recv[1]]) (status.MPI_SOURCE, buf_recv[2], buf_recv[3], buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7], buf_recv[8], buf_recv[9]);
                } else if (buf_recv[0] == 9) {
                    ((AM_SHORT_9) AM_table[buf_recv[1]]) (status.MPI_SOURCE, buf_recv[2], buf_recv[3], buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7], buf_recv[8], buf_recv[9], buf_recv[10]);
                } else if (buf_recv[0] == 10) {
                    ((AM_SHORT_10) AM_table[buf_recv[1]]) (status.MPI_SOURCE, buf_recv[2], buf_recv[3], buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7], buf_recv[8], buf_recv[9], buf_recv[10], buf_recv[11]);
                } else if (buf_recv[0] == 11) {
                    ((AM_SHORT_11) AM_table[buf_recv[1]]) (status.MPI_SOURCE, buf_recv[2], buf_recv[3], buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7], buf_recv[8], buf_recv[9], buf_recv[10], buf_recv[11], buf_recv[12]);
                } else if (buf_recv[0] == 12) {
                    ((AM_SHORT_12) AM_table[buf_recv[1]]) (status.MPI_SOURCE, buf_recv[2], buf_recv[3], buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7], buf_recv[8], buf_recv[9], buf_recv[10], buf_recv[11], buf_recv[12], buf_recv[13]);
                } else if (buf_recv[0] == 13) {
                    ((AM_SHORT_13) AM_table[buf_recv[1]]) (status.MPI_SOURCE, buf_recv[2], buf_recv[3], buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7], buf_recv[8], buf_recv[9], buf_recv[10], buf_recv[11], buf_recv[12], buf_recv[13], buf_recv[14]);
                } else if (buf_recv[0] == 14) {
                    ((AM_SHORT_14) AM_table[buf_recv[1]]) (status.MPI_SOURCE, buf_recv[2], buf_recv[3], buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7], buf_recv[8], buf_recv[9], buf_recv[10], buf_recv[11], buf_recv[12], buf_recv[13], buf_recv[14], buf_recv[15]);
                } else if (buf_recv[0] == 15) {
                    ((AM_SHORT_15) AM_table[buf_recv[1]]) (status.MPI_SOURCE, buf_recv[2], buf_recv[3], buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7], buf_recv[8], buf_recv[9], buf_recv[10], buf_recv[11], buf_recv[12], buf_recv[13], buf_recv[14], buf_recv[15], buf_recv[16]);
                } else if (buf_recv[0] == 16) {
                    ((AM_SHORT_16) AM_table[buf_recv[1]]) (status.MPI_SOURCE, buf_recv[2], buf_recv[3], buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7], buf_recv[8], buf_recv[9], buf_recv[10], buf_recv[11], buf_recv[12], buf_recv[13], buf_recv[14], buf_recv[15], buf_recv[16], buf_recv[17]);
                } else {
                    fprintf(stderr, "AM_SHORT: too many args\n");
                    exit(-1);
                }
            } else if (buf_recv[0] < 0x200) {
                buf_recv[0] -= 0x100;
                len = buf_recv[2];
                int msg_tag = buf_recv[3];
                s = (char *) malloc(len);
                if (len > 0) {
                    ret = MPI_Recv(s, len, MPI_BYTE, status.MPI_SOURCE, msg_tag, MPI_COMM_WORLD, &status);
                    if (ret != MPI_SUCCESS) {
                        fprintf(stderr, "MPI error in [$(@args)]\n");
                        exit(-1);
                    }
                }
                if (buf_recv[0] == 1) {
                    ((AM_MEDIUM_1) AM_table[buf_recv[1]]) (status.MPI_SOURCE, s, len, buf_recv[4]);
                } else if (buf_recv[0] == 2) {
                    ((AM_MEDIUM_2) AM_table[buf_recv[1]]) (status.MPI_SOURCE, s, len, buf_recv[4], buf_recv[5]);
                } else if (buf_recv[0] == 3) {
                    ((AM_MEDIUM_3) AM_table[buf_recv[1]]) (status.MPI_SOURCE, s, len, buf_recv[4], buf_recv[5], buf_recv[6]);
                } else if (buf_recv[0] == 4) {
                    ((AM_MEDIUM_4) AM_table[buf_recv[1]]) (status.MPI_SOURCE, s, len, buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7]);
                } else if (buf_recv[0] == 5) {
                    ((AM_MEDIUM_5) AM_table[buf_recv[1]]) (status.MPI_SOURCE, s, len, buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7], buf_recv[8]);
                } else if (buf_recv[0] == 6) {
                    ((AM_MEDIUM_6) AM_table[buf_recv[1]]) (status.MPI_SOURCE, s, len, buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7], buf_recv[8], buf_recv[9]);
                } else if (buf_recv[0] == 7) {
                    ((AM_MEDIUM_7) AM_table[buf_recv[1]]) (status.MPI_SOURCE, s, len, buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7], buf_recv[8], buf_recv[9], buf_recv[10]);
                } else if (buf_recv[0] == 8) {
                    ((AM_MEDIUM_8) AM_table[buf_recv[1]]) (status.MPI_SOURCE, s, len, buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7], buf_recv[8], buf_recv[9], buf_recv[10], buf_recv[11]);
                } else if (buf_recv[0] == 9) {
                    ((AM_MEDIUM_9) AM_table[buf_recv[1]]) (status.MPI_SOURCE, s, len, buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7], buf_recv[8], buf_recv[9], buf_recv[10], buf_recv[11], buf_recv[12]);
                } else if (buf_recv[0] == 10) {
                    ((AM_MEDIUM_10) AM_table[buf_recv[1]]) (status.MPI_SOURCE, s, len, buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7], buf_recv[8], buf_recv[9], buf_recv[10], buf_recv[11], buf_recv[12], buf_recv[13]);
                } else if (buf_recv[0] == 11) {
                    ((AM_MEDIUM_11) AM_table[buf_recv[1]]) (status.MPI_SOURCE, s, len, buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7], buf_recv[8], buf_recv[9], buf_recv[10], buf_recv[11], buf_recv[12], buf_recv[13], buf_recv[14]);
                } else if (buf_recv[0] == 12) {
                    ((AM_MEDIUM_12) AM_table[buf_recv[1]]) (status.MPI_SOURCE, s, len, buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7], buf_recv[8], buf_recv[9], buf_recv[10], buf_recv[11], buf_recv[12], buf_recv[13], buf_recv[14], buf_recv[15]);
                } else if (buf_recv[0] == 13) {
                    ((AM_MEDIUM_13) AM_table[buf_recv[1]]) (status.MPI_SOURCE, s, len, buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7], buf_recv[8], buf_recv[9], buf_recv[10], buf_recv[11], buf_recv[12], buf_recv[13], buf_recv[14], buf_recv[15], buf_recv[16]);
                } else if (buf_recv[0] == 14) {
                    ((AM_MEDIUM_14) AM_table[buf_recv[1]]) (status.MPI_SOURCE, s, len, buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7], buf_recv[8], buf_recv[9], buf_recv[10], buf_recv[11], buf_recv[12], buf_recv[13], buf_recv[14], buf_recv[15], buf_recv[16], buf_recv[17]);
                } else if (buf_recv[0] == 15) {
                    ((AM_MEDIUM_15) AM_table[buf_recv[1]]) (status.MPI_SOURCE, s, len, buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7], buf_recv[8], buf_recv[9], buf_recv[10], buf_recv[11], buf_recv[12], buf_recv[13], buf_recv[14], buf_recv[15], buf_recv[16], buf_recv[17], buf_recv[18]);
                } else if (buf_recv[0] == 16) {
                    ((AM_MEDIUM_16) AM_table[buf_recv[1]]) (status.MPI_SOURCE, s, len, buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7], buf_recv[8], buf_recv[9], buf_recv[10], buf_recv[11], buf_recv[12], buf_recv[13], buf_recv[14], buf_recv[15], buf_recv[16], buf_recv[17], buf_recv[18], buf_recv[19]);
                } else {
                    fprintf(stderr, "AM_MEDIUM: too many args\n");
                    exit(-1);
                }
                free(s);
            } else {
                buf_recv[0] -= 0x200;
                len = buf_recv[2];
                s = (char *) g_am_base + buf_recv[3];
                if (buf_recv[0] == 1) {
                    ((AM_MEDIUM_1) AM_table[buf_recv[1]]) (status.MPI_SOURCE, s, len, buf_recv[4]);
                } else if (buf_recv[0] == 2) {
                    ((AM_MEDIUM_2) AM_table[buf_recv[1]]) (status.MPI_SOURCE, s, len, buf_recv[4], buf_recv[5]);
                } else if (buf_recv[0] == 3) {
                    ((AM_MEDIUM_3) AM_table[buf_recv[1]]) (status.MPI_SOURCE, s, len, buf_recv[4], buf_recv[5], buf_recv[6]);
                } else if (buf_recv[0] == 4) {
                    ((AM_MEDIUM_4) AM_table[buf_recv[1]]) (status.MPI_SOURCE, s, len, buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7]);
                } else if (buf_recv[0] == 5) {
                    ((AM_MEDIUM_5) AM_table[buf_recv[1]]) (status.MPI_SOURCE, s, len, buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7], buf_recv[8]);
                } else if (buf_recv[0] == 6) {
                    ((AM_MEDIUM_6) AM_table[buf_recv[1]]) (status.MPI_SOURCE, s, len, buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7], buf_recv[8], buf_recv[9]);
                } else if (buf_recv[0] == 7) {
                    ((AM_MEDIUM_7) AM_table[buf_recv[1]]) (status.MPI_SOURCE, s, len, buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7], buf_recv[8], buf_recv[9], buf_recv[10]);
                } else if (buf_recv[0] == 8) {
                    ((AM_MEDIUM_8) AM_table[buf_recv[1]]) (status.MPI_SOURCE, s, len, buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7], buf_recv[8], buf_recv[9], buf_recv[10], buf_recv[11]);
                } else if (buf_recv[0] == 9) {
                    ((AM_MEDIUM_9) AM_table[buf_recv[1]]) (status.MPI_SOURCE, s, len, buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7], buf_recv[8], buf_recv[9], buf_recv[10], buf_recv[11], buf_recv[12]);
                } else if (buf_recv[0] == 10) {
                    ((AM_MEDIUM_10) AM_table[buf_recv[1]]) (status.MPI_SOURCE, s, len, buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7], buf_recv[8], buf_recv[9], buf_recv[10], buf_recv[11], buf_recv[12], buf_recv[13]);
                } else if (buf_recv[0] == 11) {
                    ((AM_MEDIUM_11) AM_table[buf_recv[1]]) (status.MPI_SOURCE, s, len, buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7], buf_recv[8], buf_recv[9], buf_recv[10], buf_recv[11], buf_recv[12], buf_recv[13], buf_recv[14]);
                } else if (buf_recv[0] == 12) {
                    ((AM_MEDIUM_12) AM_table[buf_recv[1]]) (status.MPI_SOURCE, s, len, buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7], buf_recv[8], buf_recv[9], buf_recv[10], buf_recv[11], buf_recv[12], buf_recv[13], buf_recv[14], buf_recv[15]);
                } else if (buf_recv[0] == 13) {
                    ((AM_MEDIUM_13) AM_table[buf_recv[1]]) (status.MPI_SOURCE, s, len, buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7], buf_recv[8], buf_recv[9], buf_recv[10], buf_recv[11], buf_recv[12], buf_recv[13], buf_recv[14], buf_recv[15], buf_recv[16]);
                } else if (buf_recv[0] == 14) {
                    ((AM_MEDIUM_14) AM_table[buf_recv[1]]) (status.MPI_SOURCE, s, len, buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7], buf_recv[8], buf_recv[9], buf_recv[10], buf_recv[11], buf_recv[12], buf_recv[13], buf_recv[14], buf_recv[15], buf_recv[16], buf_recv[17]);
                } else if (buf_recv[0] == 15) {
                    ((AM_MEDIUM_15) AM_table[buf_recv[1]]) (status.MPI_SOURCE, s, len, buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7], buf_recv[8], buf_recv[9], buf_recv[10], buf_recv[11], buf_recv[12], buf_recv[13], buf_recv[14], buf_recv[15], buf_recv[16], buf_recv[17], buf_recv[18]);
                } else if (buf_recv[0] == 16) {
                    ((AM_MEDIUM_16) AM_table[buf_recv[1]]) (status.MPI_SOURCE, s, len, buf_recv[4], buf_recv[5], buf_recv[6], buf_recv[7], buf_recv[8], buf_recv[9], buf_recv[10], buf_recv[11], buf_recv[12], buf_recv[13], buf_recv[14], buf_recv[15], buf_recv[16], buf_recv[17], buf_recv[18], buf_recv[19]);
                } else {
                    fprintf(stderr, "AM_MEDIUM: too many args\n");
                    exit(-1);
                }
            }
            if (req_recv == MPI_REQUEST_NULL) {
                ret = MPI_Irecv(buf_recv, 20, MPI_INT, MPI_ANY_SOURCE, 0x1, MPI_COMM_WORLD, &req_recv);
                if (ret != MPI_SUCCESS) {
                    fprintf(stderr, "MPI error in [$(@args)]\n");
                    exit(-1);
                }
            }
        }
    }
}

void AMPoll_cancel()
{
    int ret;

    if (req_recv != MPI_REQUEST_NULL) {
        ret = MPI_Cancel(&req_recv);
        if (ret != MPI_SUCCESS) {
            fprintf(stderr, "MPI error in [$(@args)]\n");
            exit(-1);
        }
    }
}

void AM_short_n(int n, int tgt, int handler, const int *args)
{
    int buf_send[20];
    int ret;

    buf_send[0] = n;
    buf_send[1] = handler;
    for (int  i = 0; i<n; i++) {
        buf_send[2+i] = args[i];
    }
    assert(tgt != my_node_id);
    ret = MPI_Send(buf_send, n + 2, MPI_INT, tgt, 0x1, MPI_COMM_WORLD);
    if (ret != MPI_SUCCESS) {
        fprintf(stderr, "MPI error in [$(@args)]\n");
        exit(-1);
    }
}

void AM_medium_n(int n, int tgt, int handler, const void *msg, int len, const int *args)
{
    int buf_send[20];
    int ret;

    buf_send[0] = n + 0x100;
    buf_send[1] = handler;
    buf_send[2] = len;
    int msg_tag = 0x0;
    if (thread_id == 0) {
        num_threads++;
        thread_id = num_threads;
    }
    msg_tag = thread_id << 1;
    buf_send[3] = msg_tag;
    for (int  i = 0; i<n; i++) {
        buf_send[4+i] = args[i];
    }
    assert(tgt != my_node_id);
    ret = MPI_Send(buf_send, n + 4, MPI_INT, tgt, 0x1, MPI_COMM_WORLD);
    if (ret != MPI_SUCCESS) {
        fprintf(stderr, "MPI error in [$(@args)]\n");
        exit(-1);
    }
    if (len > 0) {
        assert(tgt != my_node_id);
        ret = MPI_Send(msg, len, MPI_BYTE, tgt, msg_tag, MPI_COMM_WORLD);
        if (ret != MPI_SUCCESS) {
            fprintf(stderr, "MPI error in [$(@args)]\n");
            exit(-1);
        }
    }
}

void AM_long_n(int n, int tgt, int handler, const void *msg, int len, void *dst, const int *args)
{
    int ret;
    int buf_send[20];

    ret = MPI_Win_lock(MPI_LOCK_SHARED, tgt, 0, g_am_win);
    if (ret != MPI_SUCCESS) {
        fprintf(stderr, "MPI error in [$(@args)]\n");
        exit(-1);
    }
    assert(g_am_win);
    ret = MPI_Put(msg, len, MPI_BYTE, tgt, (MPI_Aint) dst, len, MPI_BYTE, g_am_win);
    if (ret != MPI_SUCCESS) {
        fprintf(stderr, "MPI error in [$(@args)]\n");
        exit(-1);
    }
    ret = MPI_Win_unlock(tgt, g_am_win);
    if (ret != MPI_SUCCESS) {
        fprintf(stderr, "MPI error in [$(@args)]\n");
        exit(-1);
    }
    buf_send[0] = n + 0x200;
    buf_send[1] = handler;
    buf_send[2] = len;
    buf_send[3] = (int32_t) ((uint64_t) dst & 0xffffffff);
    for (int  i = 0; i<n; i++) {
        buf_send[4+i] = args[i];
    }
    assert(tgt != my_node_id);
    ret = MPI_Send(buf_send, n + 4, MPI_INT, tgt, 0x1, MPI_COMM_WORLD);
    if (ret != MPI_SUCCESS) {
        fprintf(stderr, "MPI error in [$(@args)]\n");
        exit(-1);
    }
}
