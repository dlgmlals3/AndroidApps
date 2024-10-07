#include <jni.h>
#include <string>

extern "C" JNIEXPORT jstring

JNICALL
Java_com_example_nativetest_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {

    return env->NewStringUTF(hello.c_str());
}