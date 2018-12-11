#ifndef XSMOD_INCLUDE
#define XSMOD_INCLUDE

#include <libxml/tree.h>

std::string remove_invalid_chars(std::string xml);
void parse_xml(std::string xml, bool T, bool a, bool u);
void print_entry_data(struct entry item, bool T, bool a, bool u);
std::string get_value(xmlDocPtr doc, xmlNodePtr cur);
struct entry find_item_data(xmlDocPtr doc, xmlNodePtr cur, bool T, bool a, bool u);
struct entry find_entry_data(xmlDocPtr doc, xmlNodePtr cur, bool T, bool a, bool u);
int is_atom(xmlNodePtr cur);
int is_rss1(xmlNodePtr cur);
int is_rss2(xmlNodePtr cur);
struct entry {
    std::string title;
    std::string author;
    std::string time;
    std::string url;
};

#endif
