#include <iostream>
#include <string>
#include <regex>

#include <libxml/tree.h>

#include "xmlmod.h"


using namespace std;


void parse_xml(string xml, bool T, bool a, bool u)
{
    xml = remove_invalid_chars(xml);

    xmlDocPtr doc;
    xmlNodePtr cur;
    
    // zo stringu xml dostaneme ukazatel na xml dokument
    doc = xmlParseMemory(xml.c_str(), xml.length());
    if (doc == NULL)
    {
        cerr << "Error when parsing xml" <<endl;
        return;
    }

    // hlada sa root element
    cur = xmlDocGetRootElement(doc);
    if (cur == NULL)
    {
        cerr << "Error when finding root xml element" <<endl;
        return;
    }
    // ak sa najde root element feed, jedna sa o dokument atom
    if (is_atom(cur))
    {
        cur = cur->xmlChildrenNode;

        // prechádzanie children nodov z xml
        while (cur != NULL)
        {
            // ziskanie nazvu feedu z elementu <feed><title></....
            if ((!xmlStrcmp(cur->name, (const xmlChar *)"title")))
            {
                cout << "*** ";
                cout << xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
                cout << " ***" << endl;
            // ziskavanie udajov z kazdeho elementu <feed><entry
            } else if ((!xmlStrcmp(cur->name, (const xmlChar *)"entry")))
            {
                struct entry item = find_entry_data(doc, cur->xmlChildrenNode, T, a, u);
                print_entry_data(item, T, a, u);
            }
            cur = cur->next;
        }
    // ak sa najde root element rss, jedna sa o rss 1.0
    } else if (is_rss1(cur))
    {
        cur = cur->xmlChildrenNode;

        while (cur != NULL)
        {
            if ((!xmlStrcmp(cur->name, (const xmlChar *)"channel")))
            {
                xmlNodePtr tmp = cur->xmlChildrenNode;
                while (tmp != NULL)
                {
                    if ((!xmlStrcmp(tmp->name, (const xmlChar *)"title")))
                    {
                        cout << "*** ";
                        cout << xmlNodeListGetString(doc, tmp->xmlChildrenNode, 1);
                        cout << " ***" << endl;
                    }
                    tmp = tmp->next;
                }
            }
            if ((!xmlStrcmp(cur->name, (const xmlChar *)"item")))
            {
                struct entry item = find_item_data(doc, cur->xmlChildrenNode, T, a, u);
                print_entry_data(item, T, a, u);
            }

            cur = cur->next;
        }
    // RSS 2.0
    } else if (is_rss2(cur))
    {
        cur = cur->xmlChildrenNode;

        while (cur != NULL)
        {
            if ((!xmlStrcmp(cur->name, (const xmlChar *)"channel")))
            {
                xmlNodePtr tmp = cur->xmlChildrenNode;
                while (tmp != NULL)
                {
                    if ((!xmlStrcmp(tmp->name, (const xmlChar *)"title")))
                    {
                        cout << "*** ";
                        cout << xmlNodeListGetString(doc, tmp->xmlChildrenNode, 1);
                        cout << " ***" << endl;
                    }
                    if ((!xmlStrcmp(tmp->name, (const xmlChar *)"item")))
                    {
                        struct entry item = find_item_data(doc, tmp->xmlChildrenNode, T, a, u);
                        print_entry_data(item, T, a, u);
                    }
                    tmp = tmp->next;
                }
            }
            cur = cur->next;
        }
    }
}

// Vypise vsetky data na vystup
void print_entry_data(struct entry item, bool T, bool a, bool u)
{
    cout << item.title << endl;

    if (T) {
        cout << "Aktualizace: " << item.time << endl;
    }
    if (a) {
        cout << "Autor: " << item.author << endl;
    }
    if (u) {
        cout << "URL: " << item.url << endl;
    }
    if (T || a || u)
        cout << endl;
}

// Pokud je hodnota xml elementu prazdna, vrati prazdny string
string get_value(xmlDocPtr doc, xmlNodePtr cur)
{
    if (xmlNodeListGetString(doc, cur->xmlChildrenNode, 1) != NULL)
    {
        return (char *)xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
    }
    return "";
}

// najdeni informaci o item a naplneni struktury entry, pouze RSS
struct entry find_item_data(xmlDocPtr doc, xmlNodePtr cur, bool T, bool a, bool u)
{
    struct entry item;
    while (cur != NULL)
    {
        if ((!xmlStrcmp(cur->name, (const xmlChar *)"title")))
        {
            item.title = get_value(doc, cur);
        } else if ((T && (!xmlStrcmp(cur->name, (const xmlChar *)"pubDate"))) || (T && (!xmlStrcmp(cur->name, (const xmlChar *)"date"))))
        {
            item.time = get_value(doc, cur);

        } else if ((a && (!xmlStrcmp(cur->name, (const xmlChar *)"author"))) || (a && (!xmlStrcmp(cur->name, (const xmlChar *)"creator"))))
        {
            item.author = get_value(doc, cur);
        } else if (u && (!xmlStrcmp(cur->name, (const xmlChar *)"link")))
        {
            item.url = get_value(doc, cur);
        }

        cur = cur->next;
    }
    return item;
}

// najdeni informaci o entry a naplneni struktury entry, pouze  ATOM
struct entry find_entry_data(xmlDocPtr doc, xmlNodePtr cur, bool T, bool a, bool u)
{
    struct entry item;
    while (cur != NULL)
    {
        if ((!xmlStrcmp(cur->name, (const xmlChar *)"title")))
        {
            item.title = get_value(doc, cur);

        } else if (T && (!xmlStrcmp(cur->name, (const xmlChar *)"updated")))
        {
            item.time = get_value(doc, cur);

        } else if (a && (!xmlStrcmp(cur->name, (const xmlChar *)"author")))
        {
            xmlNodePtr tmp = cur->xmlChildrenNode;
            while (tmp != NULL)
            {
                if ((!xmlStrcmp(tmp->name, (const xmlChar *)"name")))
                {
                    item.author = get_value(doc, tmp);
                }
                tmp = tmp->next;
            }
        } else if (u && (!xmlStrcmp(cur->name, (const xmlChar *)"link")))
        {
            item.url = (char *)xmlGetProp(cur, (xmlChar *)"href");
        }
        cur = cur->next;
    }
    return item;
}

// hleda se root element s nazvem feed, pokud se najde, jedna se o atom
int is_atom(xmlNodePtr cur)
{
    if (xmlStrcmp(cur->name, (const xmlChar *) "feed"))
    {
        return 0;
    }
    return 1;
}

// hleda se root element s nazvem rss
int is_rss1(xmlNodePtr cur)
{
    if (xmlStrcmp(cur->name, (const xmlChar *) "RDF"))
    {
        return 0;
    }
    return 1;
}

// hleda se rot element s nazvem rdf
int is_rss2(xmlNodePtr cur)
{
    if (xmlStrcmp(cur->name, (const xmlChar *) "rss"))
    {
        return 0;
    }
    return 1;
}

// odstrani znaky ktere delaly problem 
string remove_invalid_chars(string xml)
{
    xml.erase(remove(xml.begin(), xml.end(), '\0'), xml.end());
    return xml;
}
