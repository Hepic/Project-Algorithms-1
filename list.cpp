#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "list.h"
#include "distances.h"

List::Node::Node(const Curve &curve, const Curve &grid_curve) {
    this->curve = curve;
    this->grid_curve = grid_curve;
    this->next = NULL;
}

List::List() {
    head = tail = NULL;
}

List::~List() {
    Node *node = head, *tmp;

    while (node != NULL) {
        tmp = node;
        node = node->next;

        delete tmp;
    }
}

void List::insert(const Curve &curve, const Curve &grid_curve) {
    Node *new_node = new Node(curve, grid_curve);

    if (head == NULL) {
        head = new_node;
        tail = head;
    } else {
        tail->next = new_node;
        tail = new_node;
    }
}

void List::print_list() const {
    Node *node = head;

    while (node != NULL) {
        cout << "Curve: \n";;
        node->curve.print_curve();
        
        cout << "Grid Curve: \n";;
        node->grid_curve.print_curve();

        node = node->next;
    }

    cout << "\n";
}

void List::search(vector<Curve> &closer_curves, const Curve &curve, const Curve &grid_curve, const char *hash_function, const char *dist_function, double R, vector<bool> &visited, bool check) const {
    Node *node = head;
    double dist;
    
    while (node != NULL) {
        if (visited[node->curve.get_int_id()]) {
            node = node->next;
            continue;
        }

        bool cmp = false;
        
        if (check) {
            cmp = grid_curve.equal_curves(node->grid_curve);
        }
        
        if (cmp || !check) {
            dist = compute_distance(node->curve, curve, dist_function);
            
            if (dist <= R) {
                closer_curves.push_back(node->curve);
            }
        }
        
        node = node->next;
    }
}

void List::print_list_in_file(ofstream &file) const {
    Node *node = head;

    while (node != NULL) {
        file << node->curve.get_int_id() << " ";
        node = node->next;
    }

    file << "\n";
}

bool List::is_empty() const {
    return head == NULL;
}

void List::insert_list_to_cluster(vector<int> &cluster) const {
    Node *node = head;

    while (node != NULL) {
        cluster.push_back(node->curve.get_int_id());
        node = node->next;
    }
}

int List::get_first_id() const {
    return head->curve.get_int_id();
}
