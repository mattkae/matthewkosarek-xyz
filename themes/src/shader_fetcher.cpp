#include "shader_fetcher.hpp"
#include "types.h"
#include <cstdio>
#include <emscripten/fetch.h>

struct FetchtimeData {
    ShaderFetchResult result;
    ShaderFetchPaths paths_data;
    void (*cb)(ShaderFetchResult*);
};

void on_failure(emscripten_fetch_t *fetch) {
    FetchtimeData* ftd = (FetchtimeData*)fetch->userData;
	printf("Downloading %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);
	emscripten_fetch_close(fetch); // Also free data on failure.
    ftd->cb(nullptr);
}

void on_fragment_shader(emscripten_fetch_t *fetch) {
    FetchtimeData* ftd = (FetchtimeData*)fetch->userData;
	printf("Finished downloading %llu bytes from URL %s.\n", fetch->numBytes, fetch->url);
	const i32 len = fetch->numBytes;
	char* data = (char*)fetch->data;
	data[len - 1] = '\0';
    ftd->result.fragment = data;
	emscripten_fetch_close(fetch); // Free data associated with the fetch.

    ftd->cb(&ftd->result);
    delete ftd;
}

void on_vertex_shader(emscripten_fetch_t *fetch) {

    FetchtimeData* ftd = (FetchtimeData*)fetch->userData;
	printf("Finished downloading %llu bytes from URL %s.\n", fetch->numBytes, fetch->url);
	const i32 len = fetch->numBytes;
	char* data = (char*)fetch->data;
	data[len - 1] = '\0';

    ftd->result.vertex = data;
	emscripten_fetch_close(fetch); // Free data associated with the fetch.

    // Fetch fragment shader next
    emscripten_fetch_attr_t attr;
	emscripten_fetch_attr_init(&attr);
	strcpy(attr.requestMethod, "GET");
	attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
	attr.onsuccess = on_fragment_shader;
	attr.onerror = on_failure;
	auto* request = emscripten_fetch(&attr, ftd->paths_data.fragment);
	request->userData = ftd;
}

void fetch_shader(ShaderFetchPaths paths, void (*cb)(ShaderFetchResult*), void* user_data) {
    FetchtimeData* ftd = new FetchtimeData();
    ftd->cb = cb;
    ftd->paths_data = paths;
    ftd->result.user_data = user_data;

    // Fetch vertex shader
    emscripten_fetch_attr_t attr;
	emscripten_fetch_attr_init(&attr);
	strcpy(attr.requestMethod, "GET");
	attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
	attr.onsuccess = on_vertex_shader;
	attr.onerror = on_failure;
	auto* request = emscripten_fetch(&attr, paths.vertex);
	request->userData = ftd;
}