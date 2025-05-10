#pragma once

#include "PlayerStrategy.hpp"
#include <memory>
#include <string>
#include <dlfcn.h>
#include <stdexcept>
#include <iostream>

namespace sevens {

class StrategyLoader {
public:
    static std::shared_ptr<PlayerStrategy> loadFromLibrary(const std::string& libraryPath) {
        void* handle = dlopen(libraryPath.c_str(), RTLD_LAZY);
        if (!handle) {
            throw std::runtime_error("Failed to open library: " + libraryPath);
        }

        auto createFn = (CreateStrategyFn)dlsym(handle, "createStrategy");
        if (!createFn) {
            dlclose(handle);
            throw std::runtime_error("Missing createStrategy() in library: " + libraryPath);
        }

        PlayerStrategy* rawPtr = createFn();

        return std::shared_ptr<PlayerStrategy>(rawPtr, [handle](PlayerStrategy* ptr) {
            delete ptr;
            dlclose(handle);
        });
    }
};

} // namespace sevens
