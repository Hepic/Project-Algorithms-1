#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "list.h"

class HashTable {
    int id, len;
    List *table;

    public:
        HashTable(int, int);
        HashTable(const HashTable&);
        ~HashTable();
        void set_id(int);
        int get_id() const;
        int get_length() const;
        long long get_index(const Curve &curve, const char*) const;
        void insert(const Curve&, const Curve&, const char*);
        void search(vector<Curve>&, const Curve&, const Curve&, const char*, const char*, double, vector<bool>&, bool = true) const;
        void print_bucket(int) const;
        void print_bucket_in_file(int, ofstream&) const;
        bool is_empty_bucket(int) const;
        void insert_bucket_to_cluster(int, vector<int>&) const;
        int get_first_id_bucket(int) const;
};

#endif
