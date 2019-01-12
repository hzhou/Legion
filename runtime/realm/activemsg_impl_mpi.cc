#include "am_mpi.h"
#include <mpi.h>
#include <limits>
#include "realm/activemsg.h"
#include "realm/cmdline.h"
#include "realm/threads.h"
#include "realm/timers.h"
#include "realm/logging.h"

const char* f_get_msgid(int id);

int next_outgoing_message_id = 0;
extern void **g_am_bases;


class AM_Manager {
public:
    AM_Manager(){
        pthread_mutex_init(&mutex, 0);
        pthread_cond_init(&condvar, 0);
        shutdown_flag = false;
    }
    ~AM_Manager(void){}
    void init_corereservation(Realm::CoreReservationSet& crs){
        core_rsrv = new Realm::CoreReservation("AM workers", crs, Realm::CoreReservationParameters());
    }
    void start_threads(){
        p_thread = Realm::Thread::create_kernel_thread<AM_Manager, &AM_Manager::thread_loop>(this, Realm::ThreadLaunchParameters(), *core_rsrv);
    }
    void thread_loop(void){
        while (true) {
            if (shutdown_flag) {
                break;
            }
            AMPoll();
        }
        AM_Finalize();
    }
    void stop_threads(){
        shutdown_flag = true;
        p_thread->join();
        delete p_thread;
    }
protected:
    pthread_mutex_t mutex;
    pthread_cond_t condvar;
    Realm::CoreReservation *core_rsrv;
    Realm::Thread *p_thread;

    bool shutdown_flag;
};

AM_Manager g_am_manager;

/*---------------------------------------------*/
void enqueue_message(NodeID target, int msgid,
                    const void *args, size_t arg_size,
                    const void *payload, size_t payload_size,
                    int payload_mode, void *dstptr)
{
    int n_args = arg_size/4;
    if (payload_mode == PAYLOAD_NONE) {
        AM_short_n(n_args, target, msgid, (const int *) args);
    } else {
        int *tp_args = (int*)(args);
        if (tp_args[0] == BaseMedium::MESSAGE_ID_MAGIC) {
        } else {
            assert(tp_args[2] == BaseMedium::MESSAGE_ID_MAGIC);
            tp_args += 2;
        }
        tp_args[0] = next_outgoing_message_id++;
        tp_args[1] = 1;
        tp_args[2] = 0;
        tp_args[3] = 0;
        if (!dstptr) {
            AM_medium_n(n_args, target, msgid, payload, payload_size, (const int *) args);
        } else {
            dstptr -= (size_t) g_am_bases[target];
            AM_long_n(n_args, target, msgid, payload, payload_size, dstptr, (const int *) args);
        }
    }
}

void enqueue_message(NodeID target, int msgid,
                    const void *args, size_t arg_size,
                    const void *payload, size_t line_size,
                    off_t line_stride, size_t line_count,
                    int payload_mode, void *dstptr)
{
    int n_args = arg_size/4;
    size_t payload_size = line_size*line_count;
    if (payload_mode == PAYLOAD_NONE) {
        AM_short_n(n_args, target, msgid, (const int *) args);
    } else {
        int *tp_args = (int*)(args);
        if (tp_args[0] == BaseMedium::MESSAGE_ID_MAGIC) {
        } else {
            assert(tp_args[2] == BaseMedium::MESSAGE_ID_MAGIC);
            tp_args += 2;
        }
        tp_args[0] = next_outgoing_message_id++;
        tp_args[1] = 1;
        tp_args[2] = 0;
        tp_args[3] = 0;
        if (!dstptr) {
            AM_medium_n(n_args, target, msgid, payload, payload_size, (const int *) args);
        } else {
            dstptr -= (size_t) g_am_bases[target];
            AM_long_n(n_args, target, msgid, payload, payload_size, dstptr, (const int *) args);
        }
    }
}

void enqueue_message(NodeID target, int msgid,
                    const void *args, size_t arg_size,
                    const SpanList& spans, size_t payload_size,
                    int payload_mode, void *dstptr)
{
    int n_args = arg_size/4;
    const void *payload = NULL;
    assert(0 && "mpi_enqueue_message - SpanPayload - active messages not available!");
    if (payload_mode == PAYLOAD_NONE) {
        AM_short_n(n_args, target, msgid, (const int *) args);
    } else {
        int *tp_args = (int*)(args);
        if (tp_args[0] == BaseMedium::MESSAGE_ID_MAGIC) {
        } else {
            assert(tp_args[2] == BaseMedium::MESSAGE_ID_MAGIC);
            tp_args += 2;
        }
        tp_args[0] = next_outgoing_message_id++;
        tp_args[1] = 1;
        tp_args[2] = 0;
        tp_args[3] = 0;
        if (!dstptr) {
            AM_medium_n(n_args, target, msgid, payload, payload_size, (const int *) args);
        } else {
            dstptr -= (size_t) g_am_bases[target];
            AM_long_n(n_args, target, msgid, payload, payload_size, dstptr, (const int *) args);
        }
    }
}

void do_some_polling(void)
{
}

size_t get_lmb_size(NodeID target_node)
{
    // lmb: Long Message Buffer - threshold for breaking up long messages
    // MPI is handling long messages, so we are not breaking it up.
    return std::numeric_limits<size_t>::max();
}

void record_message(NodeID source, bool sent_reply)
{
}

void send_srcptr_release(token_t token, uint64_t srcptr)
{
    assert(0);
}

NodeID get_message_source(token_t token)
{
    return (int) (long)token;
    return 0;
}

void enqueue_incoming(NodeID sender, IncomingMessage *msg)
{
    msg->run_handler();
}

bool adjust_long_msgsize(NodeID source, void *&ptr, size_t &buffer_size,
                        int message_id, int chunks)
{
    return true;
}

void handle_long_msgptr(NodeID source, const void *ptr)
{
}

void add_handler_entry(int msgid, void (*fnptr)())
{
    AM_add_handler(msgid, fnptr);
}

void init_endpoints(int gasnet_mem_size_in_mb,
                    int registered_mem_size_in_mb,
                    int registered_ib_mem_size_in_mb,
                    Realm::CoreReservationSet& crs,
                    std::vector<std::string>& cmdline)
{
    size_t attach_size = (((size_t)gasnet_mem_size_in_mb)<<20)+(((size_t)registered_mem_size_in_mb)<<20)+(((size_t)registered_ib_mem_size_in_mb)<<20);
    AM_Init_segments(attach_size);
    g_am_manager.init_corereservation(crs);
}

void start_polling_threads(int count)
{
    g_am_manager.start_threads();
}

void start_handler_threads(int count, Realm::CoreReservationSet&, size_t)
{
}

void stop_activemsg_threads(void)
{
    g_am_manager.stop_threads();
}

const char* f_get_msgid(int id)
{
    switch (id) {
        case NODE_ANNOUNCE_MSGID: return "NODE_ANNOUNCE_MSGID";
        case SPAWN_TASK_MSGID: return "SPAWN_TASK_MSGID";
        case LOCK_REQUEST_MSGID: return "LOCK_REQUEST_MSGID";
        case LOCK_RELEASE_MSGID: return "LOCK_RELEASE_MSGID";
        case LOCK_GRANT_MSGID: return "LOCK_GRANT_MSGID";
        case EVENT_SUBSCRIBE_MSGID: return "EVENT_SUBSCRIBE_MSGID";
        case EVENT_TRIGGER_MSGID: return "EVENT_TRIGGER_MSGID";
        case EVENT_UPDATE_MSGID: return "EVENT_UPDATE_MSGID";
        case REMOTE_MALLOC_MSGID: return "REMOTE_MALLOC_MSGID";
        case REMOTE_MALLOC_RPLID: return "REMOTE_MALLOC_RPLID";
        case CREATE_ALLOC_MSGID: return "CREATE_ALLOC_MSGID";
        case CREATE_ALLOC_RPLID: return "CREATE_ALLOC_RPLID";
        case CREATE_INST_MSGID: return "CREATE_INST_MSGID";
        case CREATE_INST_RPLID: return "CREATE_INST_RPLID";
        case VALID_MASK_REQ_MSGID: return "VALID_MASK_REQ_MSGID";
        case VALID_MASK_DATA_MSGID: return "VALID_MASK_DATA_MSGID";
        case VALID_MASK_FTH_MSGID: return "VALID_MASK_FTH_MSGID";
        case ROLL_UP_TIMER_MSGID: return "ROLL_UP_TIMER_MSGID";
        case ROLL_UP_DATA_MSGID: return "ROLL_UP_DATA_MSGID";
        case CLEAR_TIMER_MSGID: return "CLEAR_TIMER_MSGID";
        case DESTROY_INST_MSGID: return "DESTROY_INST_MSGID";
        case REMOTE_WRITE_MSGID: return "REMOTE_WRITE_MSGID";
        case REMOTE_REDUCE_MSGID: return "REMOTE_REDUCE_MSGID";
        case REMOTE_SERDEZ_MSGID: return "REMOTE_SERDEZ_MSGID";
        case REMOTE_WRITE_FENCE_MSGID: return "REMOTE_WRITE_FENCE_MSGID";
        case REMOTE_WRITE_FENCE_ACK_MSGID: return "REMOTE_WRITE_FENCE_ACK_MSGID";
        case DESTROY_LOCK_MSGID: return "DESTROY_LOCK_MSGID";
        case REMOTE_REDLIST_MSGID: return "REMOTE_REDLIST_MSGID";
        case MACHINE_SHUTDOWN_MSGID: return "MACHINE_SHUTDOWN_MSGID";
        case BARRIER_ADJUST_MSGID: return "BARRIER_ADJUST_MSGID";
        case BARRIER_SUBSCRIBE_MSGID: return "BARRIER_SUBSCRIBE_MSGID";
        case BARRIER_TRIGGER_MSGID: return "BARRIER_TRIGGER_MSGID";
        case BARRIER_MIGRATE_MSGID: return "BARRIER_MIGRATE_MSGID";
        case METADATA_REQUEST_MSGID: return "METADATA_REQUEST_MSGID";
        case METADATA_RESPONSE_MSGID: return "METADATA_RESPONSE_MSGID";
        case METADATA_INVALIDATE_MSGID: return "METADATA_INVALIDATE_MSGID";
        case METADATA_INVALIDATE_ACK_MSGID: return "METADATA_INVALIDATE_ACK_MSGID";
        case XFERDES_REMOTEWRITE_MSGID: return "XFERDES_REMOTEWRITE_MSGID";
        case XFERDES_REMOTEWRITE_ACK_MSGID: return "XFERDES_REMOTEWRITE_ACK_MSGID";
        case XFERDES_CREATE_MSGID: return "XFERDES_CREATE_MSGID";
        case XFERDES_DESTROY_MSGID: return "XFERDES_DESTROY_MSGID";
        case XFERDES_NOTIFY_COMPLETION_MSGID: return "XFERDES_NOTIFY_COMPLETION_MSGID";
        case XFERDES_UPDATE_BYTES_WRITE_MSGID: return "XFERDES_UPDATE_BYTES_WRITE_MSGID";
        case XFERDES_UPDATE_BYTES_READ_MSGID: return "XFERDES_UPDATE_BYTES_READ_MSGID";
        case REGISTER_TASK_MSGID: return "REGISTER_TASK_MSGID";
        case REGISTER_TASK_COMPLETE_MSGID: return "REGISTER_TASK_COMPLETE_MSGID";
        case REMOTE_MICROOP_MSGID: return "REMOTE_MICROOP_MSGID";
        case REMOTE_MICROOP_COMPLETE_MSGID: return "REMOTE_MICROOP_COMPLETE_MSGID";
        case REMOTE_SPARSITY_CONTRIB_MSGID: return "REMOTE_SPARSITY_CONTRIB_MSGID";
        case REMOTE_SPARSITY_REQUEST_MSGID: return "REMOTE_SPARSITY_REQUEST_MSGID";
        case APPROX_IMAGE_RESPONSE_MSGID: return "APPROX_IMAGE_RESPONSE_MSGID";
        case SET_CONTRIB_COUNT_MSGID: return "SET_CONTRIB_COUNT_MSGID";
        case REMOTE_ID_REQUEST_MSGID: return "REMOTE_ID_REQUEST_MSGID";
        case REMOTE_ID_RESPONSE_MSGID: return "REMOTE_ID_RESPONSE_MSGID";
        case REMOTE_IB_ALLOC_REQUEST_MSGID: return "REMOTE_IB_ALLOC_REQUEST_MSGID";
        case REMOTE_IB_ALLOC_RESPONSE_MSGID: return "REMOTE_IB_ALLOC_RESPONSE_MSGID";
        case REMOTE_IB_FREE_REQUEST_MSGID: return "REMOTE_IB_FREE_REQUEST_MSGID";
        case REMOTE_COPY_MSGID: return "REMOTE_COPY_MSGID";
        case REMOTE_FILL_MSGID: return "REMOTE_FILL_MSGID";
        case MEM_STORAGE_ALLOC_REQ_MSGID: return "MEM_STORAGE_ALLOC_REQ_MSGID";
        case MEM_STORAGE_ALLOC_RESP_MSGID: return "MEM_STORAGE_ALLOC_RESP_MSGID";
        case MEM_STORAGE_RELEASE_REQ_MSGID: return "MEM_STORAGE_RELEASE_REQ_MSGID";
        case MEM_STORAGE_RELEASE_RESP_MSGID: return "MEM_STORAGE_RELEASE_RESP_MSGID";
    }
    return NULL;
}
