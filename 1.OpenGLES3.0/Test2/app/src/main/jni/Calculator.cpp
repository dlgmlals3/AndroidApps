#include <jni.h>

extern "C" jint
Java_com_example_test2_MainActivity_getSum(JNIEnv *env, jobject thiz, jint a, jint b) {
    return a + b;
}