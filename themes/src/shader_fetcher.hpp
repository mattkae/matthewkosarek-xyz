#ifndef SHADER_FETCHER_HPP
#define SHADER_FETCHER_HPP

#include <string>

struct ShaderFetchPaths {
    const char* vertex;
    const char* fragment;
};

struct ShaderFetchResult {
    std::string vertex;
    std::string fragment;
    void* user_data;
};

void fetch_shader(ShaderFetchPaths, void (*cb)(ShaderFetchResult*), void* user_data = nullptr);

#endif