#include <iostream>
#include <string>
#include <sstream>

#include <stdio.h>

#include "openssl/bio.h"
#include "openssl/ssl.h"
#include "openssl/err.h"

using namespace std;

// odoslanie http requestu a ziskani odpovedi ktera se vraci
string get_response(BIO *bio, string file, string host)
{
    string response = "";
    string req = "GET " + file + " HTTP/1.0\r\n";
    req += "Host: " + host + "\r\n";
    req += "Connection: close\r\n";
    req += "Accept-Encoding: UTF-8\r\n";
    req += "User-agent: Feedreader-xkucer91\r\n\r\n";

    // odeslani requestu
    BIO_puts(bio, req.c_str());

    int x;
    char buff[1024];
    memset(buff, '\0', sizeof(buff));
    while (42)
    {
        x = BIO_read(bio, buff, sizeof(buff));
        if (x == 0)
            break;
        if (x < 0)
        {
            if (! BIO_should_retry(bio))
            {
                cerr << "Error failed BIO_read" << endl;
            }
            response.append(buff, sizeof(buff));
            break;
        }
        response.append(buff, sizeof(buff));
        memset(buff, '\0', sizeof(buff));
    }

    return response;
}

// Zo zadanej url adresy dostane hosta, file a zpusob(http|https)
int parse_url(char *url, string *host, string *file)
{
    string s = url;

    // http | https 
    string scheme = s.substr(0, s.find("://"));
    s.erase(0,scheme.length()+3);

    // host
    string h = s.substr(0, s.find("/"));
    s.erase(0, h.length());

    *host = h;
    *file = s;

    // TO DO http://host/file.php ?get=premenne....

    if (scheme == "https")
        return 1;
    else
        return 0;
}

void clean_up()
{
    FIPS_mode_set(0);
    EVP_cleanup();
    SSL_COMP_free_compression_methods();
    CRYPTO_cleanup_all_ex_data();
    ERR_remove_state(0);
    ERR_free_strings();
}


// Vytvori spojeni s danou url
string ssl_connect(char *url, char *c, char *C)
{
    // openssl init
    SSL_library_init();
    SSL_load_error_strings();
    ERR_load_BIO_strings();
    OpenSSL_add_all_algorithms();

    string host;
    string file;

    string response = "";

    if (parse_url(url, &host, &file) == 1)
    {
        // https, secured connection
        BIO * bio;
        SSL_CTX *ctx = SSL_CTX_new(SSLv23_client_method());
        SSL * ssl;

        // defaultni certifikaty, nezadane -c ani -C
        if (C == NULL && c == NULL)
        {
            if ( !SSL_CTX_set_default_verify_paths(ctx))
            {
                cerr << "Error verifying default certificates" << endl;
            }

        } else {

            if (C != NULL)
            {
                if ( !SSL_CTX_load_verify_locations(ctx, NULL, C))
                {
                    cerr << "Error verifying the certificate location" << endl;
                }
            }
            if (c != NULL)
            {
                if ( !SSL_CTX_load_verify_locations(ctx, c, NULL))
                {
                    cerr << "Error verifying the certificate location" << endl;
                }
            }
        }


        bio = BIO_new_ssl_connect(ctx);
        BIO_get_ssl(bio, &ssl);
        SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);

        string hostname = host + ":443";
        BIO_set_conn_hostname(bio, hostname.c_str());

        if (BIO_do_connect(bio) <= 0)
        {
            cerr << "Error creating secure connection" << endl;
        }

        if (SSL_get_verify_result(ssl) != X509_V_OK)
        {
            cerr << "Error certificate is not valid" << endl;
        } else { 
            response = get_response(bio, file, host);
        }

        BIO_reset(bio);
        BIO_free_all(bio);

    } else {
        // http , unsecured connection
        BIO * bio;

        host;
        string bio_connect = host + ":80";

        bio = BIO_new_connect(bio_connect.c_str());
        if (BIO_do_connect(bio) <= 0) 
        {
            cerr << "Error connecting to server" << endl;
            exit(1);
        }

        response = get_response(bio, file, host);

        BIO_reset(bio);
        BIO_free_all(bio);
    }

    clean_up();
    return response;
}
