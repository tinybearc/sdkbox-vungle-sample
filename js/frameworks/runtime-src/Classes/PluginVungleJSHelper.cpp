
#include "PluginVungleJsHelper.h"
#include "cocos2d_specifics.hpp"
#include "PluginVungle/PluginVungle.h"
#include "SDKBoxJSHelper.h"


static JSContext* s_cx = nullptr;

class VungleListenerJsHelper : public sdkbox::VungleListener {

public:
    void setJSDelegate(JSObject* delegate)
    {
        mJsDelegate = delegate;
    }

    JSObject* getJSDelegate()
    {
        return mJsDelegate;
    }

    void onVungleCacheAvailable() {
        std::string name = "onVungleCacheAvailable";
        invokeDelegate(name, 0);
    }

    void onVungleStarted() {
        std::string name = "onVungleStarted";
        invokeDelegate(name, 0);
    }

    void onVungleFinished() {
        std::string name = "onVungleFinished";
        invokeDelegate(name, 0);
    }

    void onVungleAdViewed(bool isComplete) {
        std::string name = "onVungleAdViewed";
        if (isComplete) {
            invokeDelegate(name, 1);
        } else {
            invokeDelegate(name, 2);
        }
    }

    void onVungleAdReward(const std::string& name) {
        std::string fname = "onVungleAdReward";
        invokeDelegate(fname, 3, name);
    }

private:
    void invokeDelegate(std::string& fName, int tag, const std::string paramStr = "") {
        if (!s_cx) {
            return;
        }
        // cocos2d::Director::getInstance()->getScheduler()->performFunctionInCocosThread([=](){
        JSContext* cx = s_cx;
        const char* func_name = fName.c_str();

        JS::RootedObject obj(cx, mJsDelegate);
        JSAutoCompartment ac(cx, obj);

#if defined(MOZJS_MAJOR_VERSION)
#if MOZJS_MAJOR_VERSION >= 33
        bool hasAction;
        JS::RootedValue retval(cx);
        JS::RootedValue func_handle(cx);
#else
        bool hasAction;
        jsval retval;
        JS::RootedValue func_handle(cx);
#endif
#elif defined(JS_VERSION)
        JSBool hasAction;
        jsval retval;
        jsval func_handle;
#endif
        jsval dataVal[1];

        if (1 == tag) {
            dataVal[0] = BOOLEAN_TO_JSVAL(true);
        } else if (0 == tag || 2 == tag) {
            dataVal[0] = BOOLEAN_TO_JSVAL(false);
        } else if (3 == tag) {
            dataVal[0] = c_string_to_jsval(cx, paramStr.c_str());
        }

        if (JS_HasProperty(cx, obj, func_name, &hasAction) && hasAction) {
            if(!JS_GetProperty(cx, obj, func_name, &func_handle)) {
                return;
            }
            if(func_handle == JSVAL_VOID) {
                return;
            }

#if MOZJS_MAJOR_VERSION >= 31
            if (0 == tag) {
                JS_CallFunctionName(cx, obj, func_name, JS::HandleValueArray::empty(), &retval);
            } else {
                JS_CallFunctionName(cx, obj, func_name, JS::HandleValueArray::fromMarkedLocation(sizeof(dataVal)/sizeof(*dataVal), dataVal), &retval);
            }
#else
            if (0 == tag) {
                JS_CallFunctionName(cx, obj, func_name, 0, nullptr, &retval);
            } else {
                JS_CallFunctionName(cx, obj, func_name, sizeof(dataVal)/sizeof(*dataVal), dataVal, &retval);
            }
#endif
        }
        // });
    }

private:
    JSObject* mJsDelegate;

};


#if defined(MOZJS_MAJOR_VERSION)
bool js_PluginVungleJS_PluginVungle_setListener(JSContext *cx, uint32_t argc, jsval *vp)
#elif defined(JS_VERSION)
JSBool js_PluginVungleJS_PluginVungle_setListener(JSContext *cx, unsigned argc, JS::Value *vp)
#endif
{
    s_cx = cx;
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;

    if (argc == 1) {

        if (!args.get(0).isObject())
        {
            ok = false;
        }
        JSObject *tmpObj = args.get(0).toObjectOrNull();

        JSB_PRECONDITION2(ok, cx, false, "js_PluginVungleJS_PluginVungle_setListener : Error processing arguments");
        VungleListenerJsHelper* lis = new VungleListenerJsHelper();
        lis->setJSDelegate(tmpObj);
        sdkbox::PluginVungle::setListener(lis);

        args.rval().setUndefined();
        return true;
    }
    JS_ReportError(cx, "js_PluginVungleJS_PluginVungle_setListener : wrong number of arguments");
    return false;
}

#if defined(MOZJS_MAJOR_VERSION)
#if MOZJS_MAJOR_VERSION >= 33
void register_all_PluginVungleJS_helper(JSContext* cx, JS::HandleObject global) {
    JS::RootedObject pluginObj(cx);
    sdkbox::getJsObjOrCreat(cx, global, "sdkbox.PluginVungle", &pluginObj);

    JS_DefineFunction(cx, pluginObj, "setListener", js_PluginVungleJS_PluginVungle_setListener, 1, JSPROP_READONLY | JSPROP_PERMANENT);
}
#else
void register_all_PluginVungleJS_helper(JSContext* cx, JSObject* global) {
    JS::RootedObject pluginObj(cx);
    sdkbox::getJsObjOrCreat(cx, JS::RootedObject(cx, global), "sdkbox.PluginVungle", &pluginObj);

    JS_DefineFunction(cx, pluginObj, "setListener", js_PluginVungleJS_PluginVungle_setListener, 1, JSPROP_READONLY | JSPROP_PERMANENT);
}
#endif
#elif defined(JS_VERSION)
void register_all_PluginVungleJS_helper(JSContext* cx, JSObject* global) {
    jsval pluginVal;
    JSObject* pluginObj;
    pluginVal = sdkbox::getJsObjOrCreat(cx, global, "sdkbox.PluginVungle", &pluginObj);

    JS_DefineFunction(cx, pluginObj, "setListener", js_PluginVungleJS_PluginVungle_setListener, 1, JSPROP_READONLY | JSPROP_PERMANENT);
}
#endif

