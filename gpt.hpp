#ifndef GPT_H
#define GPT_H

#include <emscripten/fetch.h>
#include <emscripten/emscripten.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>

inline std::string gpt_str;

void downloadSucceeded(emscripten_fetch_t *fetch) {
    std::cout << "Finished downloading " << fetch->numBytes << " bytes\n";
    std::cout << std::string(fetch->data, fetch->numBytes) << "\n";

    std::string responseData(fetch->data, fetch->numBytes);
    auto json_response = nlohmann::json::parse(responseData);
    std::string content = json_response["choices"][0]["message"]["content"];
    gpt_str = content;

    emscripten_fetch_close(fetch);
}

void downloadFailed(emscripten_fetch_t *fetch) {
    std::cerr << "Downloading failed!\n";
    emscripten_fetch_close(fetch);
}

EM_JS(char*, get_my_sentence, (), {
  var jsString = 'whats 8+8';
  return stringToNewUTF8(jsString);
});

inline std::string gpt_request_body;
extern "C"
{
    EMSCRIPTEN_KEEPALIVE
    void ask_gpt(const char* text)
    {
        emscripten_fetch_attr_t attr;
        emscripten_fetch_attr_init(&attr);
        strcpy(attr.requestMethod, "POST"); // or GET
        attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
        attr.onsuccess = downloadSucceeded;
        attr.onerror = downloadFailed;

        // Set headers (e.g., for OpenAI API key)
        const char *headers[] = {
            "Content-Type", "application/json",
            "Authorization", "Bearer OPENAI_API_KEY",
            0
        };
        attr.requestHeaders = headers;

        // JSON body
        nlohmann::json json_body = {
            {"model", "gpt-4o-mini"},
            {"messages", {{{"role", "user"}, {"content", std::string(text)}}}}
        };
        gpt_request_body = json_body.dump();
        std::cout << "BODY: " << gpt_request_body << '\n';

        attr.requestData = gpt_request_body.c_str();
        attr.requestDataSize = strlen(gpt_request_body.c_str());

        emscripten_fetch(&attr, "https://api.openai.com/v1/chat/completions");
    }
}

#endif