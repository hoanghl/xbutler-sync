add_executable(smoke_test smoke_test.cpp)
target_compile_definitions(smoke_test PRIVATE APP_BINARY="$<TARGET_FILE:xbutler-sync>")
add_test(NAME xbutler_sync_smoke_test COMMAND smoke_test)
