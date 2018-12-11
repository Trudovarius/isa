#ifndef SSLMOD_INCLUDE
#define SSLMOD_INCLUDE

#include "openssl/bio.h"
#include "openssl/ssl.h"
#include "openssl/err.h"

void clean_up();
std::string get_response(BIO *bio, std::string file, std::string host);
int parse_url(char *url, std::string *host, std::string *file);
std::string ssl_connect(char *url,char *c, char *C);

#endif
