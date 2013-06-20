#include <v8.h>
#include <node.h>
#include <security/pam_appl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace node;
using namespace v8;

#define REQ_FUN_ARG(I, VAR)                                       \
    if (args.Length() <= (I) || !args[I]->IsFunction())           \
        return ThrowException(Exception::TypeError(               \
            String::New("Argument " #I " must be a function")));  \
    Local<Function> VAR = Local<Function>::Cast(args[I]);

extern "C" void init(Handle<Object>);

extern "C" {

    struct pam_response *reply;

    int null_conv(int num_msg, const struct pam_message **msg, struct pam_response **resp, void *appdata_ptr) {
        *resp = reply;
        return PAM_SUCCESS;
    }                       

    static struct pam_conv conv = { null_conv, NULL };

    const char* ToCString(const v8::String::Utf8Value& value) {
        return *value ? *value : "<string conversion failed>";
    }               

    int _pam_authenticate(const char *service, const char *username, const char *password) {
        pam_handle_t *pamh = NULL;
        int retval = pam_start(service, username, &conv, &pamh);
                                                                                    
        if (retval == PAM_SUCCESS) {
            reply = (struct pam_response *) malloc(sizeof(struct pam_response));
            reply[0].resp = (char *) password;
            reply[0].resp_retcode = 0;

            retval = pam_authenticate(pamh, 0);
            pam_end(pamh, PAM_SUCCESS);
        }

        return retval;
    }
}

static Handle<Value> PAMAuthSync(const Arguments& args) {
    HandleScope scope;
    const char *usage = "usage: pamauth(service, username, password, callback)";

    if (args.Length() != 4)
        return ThrowException(Exception::Error(String::New(usage)));

    REQ_FUN_ARG(3, cb);

    String::Utf8Value servicein(args[0]);
    String::Utf8Value usernamein(args[1]);
    String::Utf8Value passwordin(args[2]);

    char *service = strdup(ToCString(servicein));
    char *username = strdup(ToCString(usernamein));
    char *password = strdup(ToCString(passwordin));

    int retval = _pam_authenticate(service, username, password);

    const unsigned argc = 1;
    Local<Value> argv[argc];

    if (retval == PAM_SUCCESS)
        argv[0] = Local<Boolean>::New(True());
    else
        argv[0] = Local<Boolean>::New(False());

    TryCatch try_catch;

    cb->Call(Context::GetCurrent()->Global(), 1, argv);

    if (try_catch.HasCaught())
        FatalException(try_catch);

    return scope.Close(Undefined());
}

extern "C" void init (Handle<Object> target) {
    HandleScope scope;
    NODE_SET_METHOD(target, "auth", PAMAuthSync);
}

NODE_MODULE(pam, init);
