// Extension lib defines
#define EXTENSION_NAME Vibrate
#define LIB_NAME "Vibrate"
#define MODULE_NAME "vibrate"

// Defold SDK
#define DLIB_LOG_DOMAIN LIB_NAME
#include <dmsdk/sdk.h>

#if defined(DM_PLATFORM_ANDROID)

static JNIEnv* Attach()
{
    JNIEnv* env;
    JavaVM* vm = dmGraphics::GetNativeAndroidJavaVM();
    vm->AttachCurrentThread(&env, NULL);
    return env;
}

static bool Detach(JNIEnv* env)
{
    bool exception = (bool) env->ExceptionCheck();
    env->ExceptionClear();
    JavaVM* vm = dmGraphics::GetNativeAndroidJavaVM();
    vm->DetachCurrentThread();
    return !exception;
}

namespace {
struct AttachScope
{
    JNIEnv* m_Env;
    AttachScope() : m_Env(Attach())
    {
    }
    ~AttachScope()
    {
        Detach(m_Env);
    }
};
}

static jclass GetClass(JNIEnv* env, const char* classname)
{
    jclass activity_class = env->FindClass("android/app/NativeActivity");
    jmethodID get_class_loader = env->GetMethodID(activity_class,"getClassLoader", "()Ljava/lang/ClassLoader;");
    jobject cls = env->CallObjectMethod(dmGraphics::GetNativeAndroidActivity(), get_class_loader);
    jclass class_loader = env->FindClass("java/lang/ClassLoader");
    jmethodID find_class = env->GetMethodID(class_loader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");

    jstring str_class_name = env->NewStringUTF(classname);
    jclass outcls = (jclass)env->CallObjectMethod(cls, find_class, str_class_name);
    env->DeleteLocalRef(str_class_name);
    return outcls;
}

static int Vibrate(lua_State* L)
{
    AttachScope attachscope;
    JNIEnv* env = attachscope.m_Env;

    if (lua_isnil(L, 1))
    {
        int duration = 100;

        jclass cls = GetClass(env, "com.defold.android.vibrate.Vibrate");

        jmethodID vibrate_method = env->GetStaticMethodID(cls, "vibratePhone", "(Landroid/content/Context;I)V");
        env->CallStaticObjectMethod(cls, vibrate_method, dmGraphics::GetNativeAndroidActivity(), duration);
        return 0;        
    }

    if (lua_isnumber(L, 1))
    {
        int duration = luaL_checkint(L, 1);

        jclass cls = GetClass(env, "com.defold.android.vibrate.Vibrate");

        jmethodID vibrate_method = env->GetStaticMethodID(cls, "vibratePhone", "(Landroid/content/Context;I)V");
        env->CallStaticObjectMethod(cls, vibrate_method, dmGraphics::GetNativeAndroidActivity(), duration);
        return 0;
    }

    if (lua_istable(L, 1)) 
    {
    
        luaL_checktype(L, 1, LUA_TTABLE);
        
        int size;
        size = lua_objlen(L, 1);
        
        jlongArray pattern = env->NewLongArray(size);

        jlong temp_pattern[size];

        int i;
        for (i = 0; i <= size ; i++ )
        {
            lua_rawgeti(L, 1, i + 1);
            int rtn = lua_tointeger(L,-1);
            lua_pop(L,1);
            temp_pattern[i] = (long)rtn;
        }
        
        env->SetLongArrayRegion(pattern,0,size,temp_pattern);
        
        jclass cls = GetClass(env, "com.defold.android.vibrate.Vibrate");

        jmethodID vibrate_method = env->GetStaticMethodID(cls, "vibratePhonePattern", "(Landroid/content/Context;[J)V");

        
        env->CallStaticObjectMethod(cls, vibrate_method, dmGraphics::GetNativeAndroidActivity(), pattern);
        
        return 0;
    }
    
    
}

static int Cancel(lua_State* L)
{
    AttachScope attachscope;
    JNIEnv* env = attachscope.m_Env;

    jclass cls = GetClass(env, "com.defold.android.vibrate.Vibrate");

    jmethodID vibrate_method = env->GetStaticMethodID(cls, "cancelVibratePhone", "(Landroid/content/Context;)V");
    env->CallStaticObjectMethod(cls, vibrate_method, dmGraphics::GetNativeAndroidActivity());
    return 0;
}

// Functions exposed to Lua
static const luaL_reg Module_methods[] =
{
    {"vibrate", Vibrate},
    {"cancel", Cancel},
    {0, 0}
};

static void LuaInit(lua_State* L)
{
    int top = lua_gettop(L);

    // Register lua names
    luaL_register(L, MODULE_NAME, Module_methods);

    lua_pop(L, 1);
    assert(top == lua_gettop(L));
}

static dmExtension::Result AppInitializeExtension(dmExtension::AppParams* params)
{
    return dmExtension::RESULT_OK;
}

static dmExtension::Result InitializeExtension(dmExtension::Params* params)
{
    // Init Lua
    LuaInit(params->m_L);
    printf("Registered %s Extension\n", MODULE_NAME);
    return dmExtension::RESULT_OK;
}

static dmExtension::Result AppFinalizeExtension(dmExtension::AppParams* params)
{
    return dmExtension::RESULT_OK;
}

static dmExtension::Result FinalizeExtension(dmExtension::Params* params)
{
    return dmExtension::RESULT_OK;
}

#else

static dmExtension::Result AppInitializeExtension(dmExtension::AppParams* params)
{
    dmLogWarning("Registered %s (null) Extension\n", MODULE_NAME);
    return dmExtension::RESULT_OK;
}

static dmExtension::Result InitializeExtension(dmExtension::Params* params)
{
    return dmExtension::RESULT_OK;
}

static dmExtension::Result AppFinalizeExtension(dmExtension::AppParams* params)
{
    return dmExtension::RESULT_OK;
}

static dmExtension::Result FinalizeExtension(dmExtension::Params* params)
{
    return dmExtension::RESULT_OK;
}

#endif

DM_DECLARE_EXTENSION(EXTENSION_NAME, LIB_NAME, AppInitializeExtension, AppFinalizeExtension, InitializeExtension, 0, 0, FinalizeExtension)
