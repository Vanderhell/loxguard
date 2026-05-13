# Consumer Smoke Tests

These projects are intentionally tiny and exist to verify that loxguard can be consumed as a library:

- `add_subdirectory` consumption (tests should default OFF in a superproject)
- `install()` + `find_package(loxguard CONFIG REQUIRED)` consumption

They are exercised by CTest from the top-level build when `LOXGUARD_BUILD_TESTS=ON`.
