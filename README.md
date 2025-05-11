# mpm parser

A pull parser for reading multipart MIME data

This is quite possibly the most frugal open source multipart MIME parser on the Internet, in terms of memory usage.

- It requires a minumum total of 37 bytes of space for both the context, and the necessary output buffer of (at least) 1 char, on typical 32-bit systems.

- As far as the output buffer, it uses any size you give it

- The parser is streaming, so it doesn't have to preload anything. It parses entirely as it goes, a chunk at a time, as determined by the size of the output buffer.

- Pull parsing, so no complicated callbacks to implement. It works like Microsoft .NET's XmlReader class that way. You use a while loop to call the parse method repeatedly and act on each result.

- It flexibly loads from any source. You implement a simple callback (much like `fgetc()`) to read content. Unbuffered file reads are already supported, and the example code below includes a buffered file read. You can also create a callback for strings, network sockets, or any other transport you need.

- Straight C, cross platform, and no dependencies other than the C runtimes

- Open source, free to use, permissive MIT license

Note: The demos are all text, but this has been tested with binary data.

### Example:
```c
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "mpm_parser.h"

typedef struct {
    FILE* file;
    char buffer[1024];
    size_t buffer_size;
    size_t buffer_pos;
} read_file_buffer_t;
static int buffered_file_read(void* state) {
    read_file_buffer_t* rfb = (read_file_buffer_t*)state;
    if(rfb->file==NULL) { return -1; }
    if(rfb->buffer_pos>=rfb->buffer_size) {
        rfb->buffer_size=fread(rfb->buffer,sizeof(char),sizeof(rfb->buffer),rfb->file);
        rfb->buffer_pos = 0;
        if(rfb->buffer_size==0) { 
            return -1;
        }
    }
    return rfb->buffer[rfb->buffer_pos++];
}
int main() {
    // fetched from the Content-Type HTTP header:
    const char* boundary = "---------------------------90519141544843365972754266";
    mpm_context_t ctx;
    read_file_buffer_t buf = {0};
    // we froze the HTTP request body in mpart.txt
    buf.file = fopen("..\\..\\mpart.txt", "rb");
    mpm_init(boundary, 0, buffered_file_read, &buf ,&ctx);
    char buffer[1025];
    size_t size = sizeof(buffer) - 1;
    mpm_node_t node;
    while((node = mpm_parse(&ctx, buffer, &size)) > 0) {
        switch(node) {
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
    fclose(buf.file);
    return 0;
}
```
#### mpart.txt contents:
```
-----------------------------9051914041544843365972754266
Content-Disposition: form-data; name="text"

text default
-----------------------------9051914041544843365972754266
Content-Disposition: form-data; name="file1"; filename="a.txt"
Content-Type: text/plain

Data of a.txt.

-----------------------------9051914041544843365972754266
Content-Disposition: form-data; name="file2"; filename="a.html"
Content-Type: text/html

<!DOCTYPE html><title>Data of a.html.</title>

-----------------------------9051914041544843365972754266--
```
#### Example output:
```
Content-Disposition: form-data; name="text" (HEADER)
text default<END CONTENT>
Content-Disposition: form-data; name="file1"; filename="a.txt" (HEADER)
Content-Type: text/plain (HEADER)
Data of a.txt.
<END CONTENT>
Content-Disposition: form-data; name="file2"; filename="a.html" (HEADER)
Content-Type: text/html (HEADER)
<!DOCTYPE html><title>Data of a.html.</title>
<END CONTENT>
````


- Arduino repo: htcw_mpm_parser

- PlatformIO platformio.ini:
```
[env:node32s]
platform = espressif32
board = node32s
framework = arduino
lib_deps = 
	codewitch-honey-crisis/htcw_mpm_parser
```