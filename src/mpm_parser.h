#ifndef HTCW_MPM_PARSER_H
#define HTCW_MPM_PARSER_H
#include <stddef.h>

/* CONFIGURATION
    #define HTCW_MPM_NO_STDIO to disable the file support and remove the stdio dependency
*/

/// @brief The type of parsing element returned
typedef enum {
    /// @brief An error occurred in the stream
    MPM_ERROR = -1,
    /// @brief End of input
    MPM_END = 0,
    /// @brief A part of a header name
    MPM_HEADER_NAME_PART,
    /// @brief The end of a header name
    MPM_HEADER_NAME_END,
    /// @brief A part of a header value
    MPM_HEADER_VALUE_PART,
    /// @brief The end of a header value
    MPM_HEADER_VALUE_END,
    /// @brief A part of the content
    MPM_CONTENT_PART,
    /// @brief The end of the content
    MPM_CONTENT_END    
} mpm_node_t;

/// @brief The callback used to read a value from the input (similar to fgetc() but the arg is user defined)
typedef int(*mpm_on_read_callback)(void* state);
/// @brief The context struct for parsing. The fields are for internal use
typedef struct {
    int state;
    const char* boundary;
    size_t boundary_size;
    int boundary_repl;
    int boundary_pos;
    int i;
    char skip_next_read;
    mpm_on_read_callback on_read;
    void* read_state;
} mpm_context_t;

#ifdef __cplusplus
extern "C" {
#endif
/// @brief Initializes the multipart MIME parser context
/// @param boundary The MIME content boundary
/// @param boundary_size The length of the MIME content boundary
/// @param on_read The callback to read the next character
/// @param read_state The state to pass to the on_read callback
/// @param out_context The resulting context
void mpm_init(const char* boundary, size_t boundary_size, mpm_on_read_callback on_read, void* read_state,mpm_context_t* out_context);
#ifndef HTCW_MPM_NO_STDIO
/// @brief Initializes the multipart MIME parser from a FILE pointer
/// @param boundary The MIME content boundary
/// @param boundary_size The length of the MIME content boundary
/// @param path The file path
/// @param out_context The resulting context
/// @return 0 if sucessful, otherwise -1 if invalid argument or path not found
#endif // HTCW_MPM_NO_STDIO
int mpm_init_file(const char* boundary, size_t boundary_size, const char* path, mpm_context_t* out_context);
/// @brief Parses the next portion of the multipart MIME content
/// @param ctx The previously initialized context
/// @param buffer A buffer to hold the result
/// @param in_out_size (on input) the size of the buffer - (on output) the amount of bytes filled. Will be less than or equal to the size passed in
/// @return The node value of the parsed element
mpm_node_t mpm_parse(mpm_context_t* ctx, void* buffer, size_t* in_out_size);
#ifdef __cplusplus
}
#endif

#if 0 // DEMO
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "mpm_parser.h"

const char* boundary =
    "---------------------------9051914041544843365972754266";
const char* content =
    "-----------------------------9051914041544843365972754266\r\n"
    "Content-Disposition: form-data; name=\"text\"\r\n"
    "\r\n"
    "text default\r\n"
    "-----------------------------9051914041544843365972754266\r\n"
    "Content-Disposition: form-data; name=\"file1\"; filename=\"a.txt\"\r\n"
    "Content-Type: text/plain\r\n"
    "\r\n"
    "Data of a.txt.\r\n"
    "\r\n"
    "-----------------------------9051914041544843365972754266\r\n"
    "Content-Disposition: form-data; name=\"file2\"; filename=\"a.html\"\r\n"
    "Content-Type: text/html\r\n"
    "\r\n"
    "<!DOCTYPE html><title>Data of a.html.</title>\r\n"
    "\r\n"
    "-----------------------------9051914041544843365972754266--";
typedef struct {
    const char* input;
} string_buffer_t;
static int string_read(void* state) {
    string_buffer_t* sb = (string_buffer_t*)state;
    if (sb->input == NULL) {
        return -1;
    }
    if (!*sb->input) {
        return -1;
    }
    return *(sb->input++);
}
int main() {
    mpm_context_t ctx;
    string_buffer_t buf = {0};
    buf.input = content;
    mpm_init(boundary, 0, string_read, &buf, &ctx);
    char buffer[1025];
    size_t size = sizeof(buffer) - 1;
    mpm_node_t node;
    char disposition = 0;
    while ((node = mpm_parse(&ctx, buffer, &size)) > 0) {
        switch (node) {
            case MPM_HEADER_NAME_PART:
            case MPM_HEADER_VALUE_PART:
            case MPM_CONTENT_PART:
                buffer[size] = '\0';
                fputs(buffer, stdout);
                break;
            case MPM_HEADER_NAME_END:
                fputs(": ", stdout);
                break;
            case MPM_HEADER_VALUE_END:
                fputs(" (HEADER)\r\n", stdout);
                break;
            case MPM_CONTENT_END:
                fputs("<END CONTENT>\r\n", stdout);
                break;
        }
        size = sizeof(buffer) - 1;
    }
    return 0;
}
#endif // #if 0
#endif // HTCW_MPM_PARSER_H
