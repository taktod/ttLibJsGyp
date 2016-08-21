// テストでつくるc言語プログラム
#include <nan.h>
// まず、nanの世界でデータの取り扱い方をまとめておかないと作業しにくくてやってられない。
/*
#include "frame/frame.hpp"

NAN_METHOD(hogeFunc)
{
    // 非常にややこしいな・・・まぁ、できないでもないけど。
    puts("hoge is called.");
    printf("thisってとれるのか？%d\n", info.Length());
    v8::Local<v8::Value> val = Nan::Get(info.This(), Nan::New("key").ToLocalChecked()).ToLocalChecked();
    if(val->IsString()) {
        puts("文字列");
        v8::String::Utf8Value str(val->ToString());
        printf("%s\n", (const char *)(*str));
    }
    info.GetReturnValue().SetUndefined();
}

NAN_METHOD(testFunc)
{
    // これで任意の関数をobjectのmethodとしてくっつけて、応答することが可能になった模様。
    // でもclassをwrapして動作できるとその方が都合がいい
    puts("test is called.");
    v8::Local<v8::Object> val = Nan::New<v8::Object>();
    // なるほど・・・これで準備できるわけか・・・
    Nan::Set(val, Nan::New("key").ToLocalChecked(), Nan::New("value").ToLocalChecked());
    Nan::Set(val, Nan::New("func").ToLocalChecked(), Nan::GetFunction(Nan::New<v8::FunctionTemplate>(hogeFunc)).ToLocalChecked());
    updateObject(val);
    // functionつけれるかな？
//    val.ToLocalChecked();
//    val->Set(Nan::New("test"), Nan::New("hoge"));
//    v8::MaybeLocal<v8::String> val = Nan::New("test");
//    info.GetReturnValue().Set(val);
    info.GetReturnValue().Set(Nan::GetFunction(Nan::New<v8::FunctionTemplate>(hogeFunc)).ToLocalChecked());
}

NAN_MODULE_INIT(init)
{
    NAN_EXPORT(target, testFunc);
}

NODE_MODULE(test, init);
*/

class MyObject : public Nan::ObjectWrap {
public:
    static NAN_MODULE_INIT(Init) {
        v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
        tpl->SetClassName(Nan::New("MyObject").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        SetPrototypeMethod(tpl, "getHandle", GetHandle);
        SetPrototypeMethod(tpl, "getValue", GetValue);
        SetPrototypeMethod(tpl, "doSomething", DoSomething);

        constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
        Nan::Set(target, Nan::New("MyObject").ToLocalChecked(),
            Nan::GetFunction(tpl).ToLocalChecked());
    }
private:
  explicit MyObject(double value = 0) : value_(value) {}
  ~MyObject() {
      puts("デストラクタ呼ばれたか？");
  }

  static NAN_METHOD(New) {
      puts("newか？");
    if (info.IsConstructCall()) {
      double value = info[0]->IsUndefined() ? 0 : Nan::To<double>(info[0]).FromJust();
      MyObject *obj = new MyObject(value);
      obj->Wrap(info.This());
      info.GetReturnValue().Set(info.This());
    } else {
      const int argc = 1;
      v8::Local<v8::Value> argv[argc] = {info[0]};
      v8::Local<v8::Function> cons = Nan::New(constructor());
      info.GetReturnValue().Set(cons->NewInstance(argc, argv));
    }
  }

  static NAN_METHOD(GetHandle) {
    MyObject* obj = Nan::ObjectWrap::Unwrap<MyObject>(info.Holder());
    info.GetReturnValue().Set(obj->handle());
  }

  static NAN_METHOD(GetValue) {
    MyObject* obj = Nan::ObjectWrap::Unwrap<MyObject>(info.Holder());
    info.GetReturnValue().Set(obj->value_);
  }

  // ここにmethodをいろいろと並べておこう。
  static NAN_METHOD(DoSomething) {
    if(info.Length() > 0) {
      v8::Local<v8::Value> val = info[0];
      if(val->IsUndefined()) {
          // 値がundefinedの場合
      }
      if(val->IsNull()) {
          // 値がnullの場合
      }
      if(val->IsBoolean()) {
          // booleanの場合
          if(val->IsTrue()) {
              // trueの場合
          }
          if(val->IsFalse()) {
              // falseの場合
          }
      }
      if(val->IsString()) {
          // 文字列の場合(strというクラスをToStringで初期化している感じか？)
          v8::String::Utf8Value str(val->ToString());
          printf("%s \n", (const char *)(*str));
      }
      if(val->IsFunction()) {
          // callbackとかの関数の場合
          v8::Local<v8::Value> args[] = {
              // 適当なargをつくっておく。
          };
          auto callback = new Nan::Callback(val.As<v8::Function>());
          callback->Call(0, args); // これでcallbackを呼び出すことが可能になる。
      }
      if(val->IsArray()) {
          // 配列の場合(まだ未調査)
      }
      if(val->IsObject()) {
          // uint8Arrayとかもここ Functionもここくるので、注意が必要
          // uint8Arrayとかの場合は
          char *buf = node::Buffer::Data(val->ToObject());
          size_t length = node::Buffer::Length(val->ToObject());
          // これがbufとlengthになる。
      }
      if(val->IsNumber()) {
          // 数値の場合
//          val->Uint32Value(); // これかな
          (float)val->NumberValue(); // こんな感じでcastできる。
      }
      if(val->IsExternal()) {
          // これなにかわからない。
      }
      if(val->IsInt32()) {

      }
      if(val->IsUint32()) {
          // objectと同じ扱いでOK
      }
      // データを作る方法
      // 文字列
      Nan::New("test");
      // 数値
      Nan::New(13); // だいたいこれでいける。
      {
        // objectを応答したい場合
        v8::Local<v8::Object> val = Nan::New<v8::Object>();
        // なるほど・・・これで準備できるわけか・・・
        // 任意の文字列をつける。
        Nan::Set(val, Nan::New("key").ToLocalChecked(), Nan::New("value").ToLocalChecked());
        // 任意の関数をつける
//        Nan::Set(val, Nan::New("func").ToLocalChecked(), Nan::GetFunction(Nan::New<v8::FunctionTemplate>(hogeFunc)).ToLocalChecked());
      }
    }
  }

  static inline Nan::Persistent<v8::Function> & constructor() {
    static Nan::Persistent<v8::Function> my_constructor;
    return my_constructor;
  }

  double value_;
};

NODE_MODULE(test, MyObject::Init);