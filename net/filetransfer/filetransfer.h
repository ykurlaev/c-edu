#include <netinet/in.h>

#define BUFFER_SIZE 512

int run_client(in_addr_t address, in_port_t port);
int run_server(in_port_t port);
