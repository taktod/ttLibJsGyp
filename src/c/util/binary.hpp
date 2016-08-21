#include <nan.h>
#include <stdio.h>
#include <stdbool.h>

class BinaryPassingWorker : public Nan::AsyncWorker {
public:
    static void Init();
    static void Close();
    BinaryPassingWorker(
            uint8_t *data,
            size_t data_size,
            Nan::Callback *callback);
    void Execute();
    void HandleOKCallback();
};