#include <Arduino.h>
#include <mpm_parser.h>

const char* boundary ="---------------------------9051914041544843365972754266";
const char* content = "-----------------------------9051914041544843365972754266\r\n"
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

char buffer[1025];
typedef struct {
    const char* input;
} string_state_t;
static int string_read(void* state) {
    string_state_t* ss = (string_state_t*)state;
    if(ss->input==NULL) { return -1; }
    if(!*ss->input) {
        return -1;
    }
    return *(ss->input++);
}
void setup() {
    Serial.begin(115200);
    Serial.println("booted");
    mpm_context_t ctx;
    string_state_t buf = {0};
    buf.input = content;
    mpm_init(boundary,0,string_read,&buf ,&ctx);
    char buffer[1025];
    size_t size = sizeof(buffer)-1;
    mpm_node_t node;
    char disposition = 0;
    while((node=mpm_parse(&ctx,buffer,&size))>0) {
        switch(node) {
            case MPM_HEADER_NAME_PART:
            case MPM_HEADER_VALUE_PART:
            case MPM_CONTENT_PART:
                buffer[size]='\0';
                Serial.print(buffer);
                break;
            case MPM_HEADER_NAME_END:
                Serial.print(": ");
                break;
            case MPM_HEADER_VALUE_END:
                Serial.println(" (HEADER)");
                break;
            case MPM_CONTENT_END:
                Serial.println("<END CONTENT>");
                break;
        }
        size = sizeof(buffer)-1;
    }
}

void loop() {
  
}
