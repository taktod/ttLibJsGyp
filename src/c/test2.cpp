#include <nan.h>
#include <unistd.h>

class AddAsyncWorker : public Nan::AsyncWorker
{
public:
    AddAsyncWorker(int left, int right, Nan::Callback* callback)
        : Nan::AsyncWorker(callback), left(left), right(right)
    {}

    // 非同期処理の中身
    void Execute()
    {
        // 3秒待つ ※Windowsでは unistd.h の代わりに Windows.h の Sleep を使わないといけないかも
        sleep(3);
        result = left + right;
    }

    // 非同期処理が完了したとき呼び出される
    void HandleOKCallback()
    {
        v8::Local<v8::Value> callbackArgs[] = {
            Nan::Null(),
            Nan::New(result),
        };

        // コールバック呼び出し
        callback->Call(2, callbackArgs);
    }

private:
    int left;
    int right;
    int result;
};

NAN_METHOD(addAsync)
{
    // 非同期処理を開始
    auto left = info[0]->Int32Value();
    auto right = info[1]->Int32Value();
    auto callback = new Nan::Callback(info[2].As<v8::Function>());
    Nan::AsyncQueueWorker(new AddAsyncWorker(left, right, callback));
}

NAN_MODULE_INIT(init)
{
    // add 関数を外部に公開
    NAN_EXPORT(target, addAsync);
}

NODE_MODULE(my_extension, init);