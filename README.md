# CS2303-Project1-Android_Process_Tree

This is the first project for Operating System in SJTU, semester 2022 Spring.

The project is composed of 4 sub-problems.

## Problem 1: Implement ptree syscall, printing the Android Process Tree in AVD

The source file is `ptree_module/ptree_mod.c` with the corresponding makefile `ptree_module/Makefile`.

Execute `cd ptree_module&&make` and then get the target module file `ptree_module/ptree_mod.ko`.

## Problem 2: Test the newly added ptree syscall

The source file is `ptree/jni/ptree.c` with the corresponding makefile `ptree/jni/Android.mk`.

Execute `cd ptree/jni&&ndk-build` and then get the target file `ptree/libs/armeabi/ptree`.

## Problem 3: Generate a new process, and execute ptree in the child process

The source file is `ptree_gen/jni/ptree_gen.c` with the corresponding makefile `ptree_gen/jni/Android.mk`.

Execute `cd ptree_gen/jni&&ndk-build` and then get the target file `ptree_gen/libs/armeabi/ptree_gen`.

## Problem 4: Implement a Caesar Encryption Server using Concurrent Programming

The source file is `encryption/jni/server.c` and `encryption/jni/client.c` with the corresponding makefile `encryption/jni/Android.mk`.

Execute `cd encryption/jni&&ndk-build` and then get the target file `encryption/libs/armeabi/server` and `encryption/libs/armeabi/client`.