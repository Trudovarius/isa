#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#include <unistd.h>

#include "xmlmod.h"
#include "sslmod.h"

using namespace std;

vector<char *> args(char **f, char **u, char **C, int argc, char **argv);
vector<char *> feedfile(char *name, vector<char *> url);

bool a, u, T;

int main(int argc, char **argv) 
{
    // hodnoty z prepinacov
    char *f, *c, *C;
    f = c = C = NULL;
    a = u = T = false;
    vector<char *> url = args(&f, &c, &C, argc, argv);

    // TO DO: read feedfile
    if (f != NULL)
        url = feedfile(f, url);

    // Prochazeni nactenymi url
    for (char *i : url) {
        //cout << i << endl;
        string response = ssl_connect(i, c, C);
 
        size_t pos = response.find("<?xml");
        if (pos != string::npos)
        {
            string xml = response.substr(pos);

            parse_xml(xml, T, a, u);
            cout << endl;
        } else {
            pos = response.find("<rdf");
            if (pos != string::npos)
            {
                string xml = response.substr(pos);

                parse_xml(xml, T, a, u);
                cout << endl;
            }
        }
    }



    return 0;
}

// Spracovanie argumentov
// Funkce vraci vector ukazatelu na retezec kde kazdy predstavuje jednu nactenou url adresu
vector<char *> args(char **f, char **c, char **C, int argc, char **argv)
{
    int x;
    while ((x = getopt(argc, argv, "f:c:C:Tau")) != -1)
    {
        switch (x)
        {
            case 'f':
                *f = optarg;
                break;
            case 'c':
                *c = optarg;
                break;
            case 'C':
                *C = optarg;
                break;
            case 'T':
                T = true;
                break;
            case 'a':
                a = true;
                break;
            case 'u':
                u = true;
                break;
            case '?':
                cerr << "Error: wrong arguments.\nRight syntax is: feedreader <URL | -f <feedfile>> [-c <certfile>] [-C <certaddr>] [-T] [-a] [-u]" << endl;
                break;
        }
    }
    vector<char *> url;
    int i = 1;
    while (i < argc)
    {
        if (argv[i][0] == '-')
        {
            if (argv[i][1] == 'f' || argv[i][1] == 'c' ||
                argv[i][1] == 'C')
            {
                i += 2;
            } else {
                if (argv[i][1] == 'a') {
                   a = true;
                }
                if (argv[i][1] == 'u') {
                   u = true;
                }
                if (argv[i][1] == 'T') {
                   T = true;
                }
                i++;
            }
        } else {
            url.push_back(argv[i++]);
        }
    }
    return url;
}

// Nacte url ze suboru zadaneho -f feedfile a vlozi hodnoty do vectoru url adres
vector<char *> feedfile(char *name, vector<char *> url)
{
    ifstream feedfile;
    feedfile.open(name);

    
    if (!feedfile.is_open())
    {
        cerr << "Error opening file" << endl;
        return url;
    }

    string line; // nacteni suboru po 1 radku
    while (getline(feedfile, line))
    {
        // ignorujeme radky zacinajici na # a prazdne radky
        if (line[0] == '#' || line.length() == 0)
            continue;

        // radek se skopiruje na alokovane misto v pameti 
        // pak se ukazatel na tento retezec vlozi do vektoru ukazatelu url
        char *l = (char *)malloc (line.length() + 1);
        memcpy (l, line.c_str(), line.length() + 1);
        url.push_back(l);
    }
    feedfile.close();
    return url;
}
