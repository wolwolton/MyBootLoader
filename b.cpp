extern "C" int MyMain(unsigned long (*edk_print)(const char16_t* format, ...) __attribute__((ms_abi))){
    edk_print(u"Hallo!! EnoshimaOS");
    return 0;   
}
