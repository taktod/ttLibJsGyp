#include "binary.hpp"

#include <ttLibC/allocator.h>
#include <pthread.h>
#include <list>

using namespace v8;

typedef struct BinaryHolder_t {
    void *data;
    size_t data_size;
} BinaryHolder_t;

static std::list<BinaryHolder_t *> *binary_list = NULL;
static pthread_mutex_t binary_mutex;

void BinaryPassingWorker::Init() {
    pthread_mutex_init(&binary_mutex, NULL);
    binary_list = new std::list<BinaryHolder_t *>();
}

void BinaryPassingWorker::Close() {
    pthread_mutex_destroy(&binary_mutex);
    // こっちもlistを消す前にallocしてものをすべて撤去しなければならない。
    delete binary_list;
}

BinaryPassingWorker::BinaryPassingWorker(
            uint8_t *data,
            size_t data_size,
            Nan::Callback *callback) :
                Nan::AsyncWorker(callback) {
    int r = pthread_mutex_lock(&binary_mutex);
    if(r != 0) {
        puts("failed  lock.");
        return;
    }
    BinaryHolder_t *holder = (BinaryHolder_t *)ttLibC_malloc(sizeof(BinaryHolder_t));
/*    uint8_t *data_ = (uint8_t *)ttLibC_malloc(data_size);
    memcpy(data_, data, data_size);
    data_size_ = data_size;*/
    holder->data = ttLibC_malloc(data_size);
    memcpy(holder->data, data, data_size);
    holder->data_size = data_size;
    binary_list->push_back(holder);
    r = pthread_mutex_unlock(&binary_mutex);
    if(r != 0) {
        puts("failed  unlock.");
    }
}

void BinaryPassingWorker::Execute() {
}

void BinaryPassingWorker::HandleOKCallback() {
    int r = pthread_mutex_lock(&binary_mutex);
    if(r != 0) {
        puts("failed  lock..");
        return;
    }
    BinaryHolder_t *holder = binary_list->front();
    if(holder != NULL) {
        binary_list->remove(holder);
        Local<Object> binary = Nan::CopyBuffer((char *)holder->data, holder->data_size).ToLocalChecked();
        Local<Value> args[] = {
            Nan::Null(),
            binary};
        callback->Call(2, args);
        ttLibC_free(holder->data);
        ttLibC_free(holder);
    }
    r = pthread_mutex_unlock(&binary_mutex);
    if(r != 0) {
        puts("failed  unlock..");
    }
}
