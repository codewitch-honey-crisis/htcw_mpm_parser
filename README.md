# mpm parser

A pull parser for reading multipart MIME data

Requires very little memory.

- Aside from a few fields in the context structure, it only uses whatever memory you give it

- Streaming, so it doesn't have to preload anything

- Pull parsing, so no complicated callbacks to implement

- Flexibly loads from any source

- Straight C, cross platform, and no dependencies other than the C runtimes

example:
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
    const char* boundary = "---------------------------90519141544843365972754266";
    mpm_context_t ctx;
    read_file_buffer_t buf = {0};
    buf.file = fopen("..\\..\\mpart.txt","rb");
    mpm_init(boundary,0,buffered_file_read,&buf ,&ctx);
    char buffer[1025];
    size_t size = sizeof(buffer)-1;
    mpm_node_t node;
    while((node=mpm_parse(&ctx,buffer,&size))>0) {
        switch(node) {
            case MPM_HEADER_NAME_PART:
            case MPM_HEADER_VALUE_PART:
            case MPM_CONTENT_PART:
                buffer[size]='\0';
                fputs(buffer,stdout);
                break;
            case MPM_HEADER_NAME_END:
                fputs(": ",stdout);
                break;
            case MPM_HEADER_VALUE_END:
                fputs(" (HEADER)\r\n",stdout);
                break;
            case MPM_CONTENT_END:
                fputs("<END CONTENT>\r\n",stdout);
                break;
        }
        size = sizeof(buffer)-1;
    }
    fclose(buf.file);
    return 0;
}
```

platformio.ini:
```
[env:node32s]
platform = espressif32
board = node32s
framework = arduino
lib_deps = 
	codewitch-honey-crisis/htcw_mpm_parser
```