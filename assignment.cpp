#include <iostream>
#include <cstdlib>
#include "cluster.h"
#include "curve.h"
#include "distances.h"
#include "help_functions.h"
#include "assignment.h"

double loyd_assignment(const vector<const Curve*> &centroids, vector<vector<int> > &clusters, const char *metric) {
    double min_dist, dist, value = 0;
    int p_centr;
    
    for (int i = 0; i < (int)clusters.size(); ++i) {
        clusters[i].clear();
    }
    
    for (int i = 0; i < (int)input_curves.size(); ++i) {
        min_dist = -1;
        
        for (int j = 0; j < (int)centroids.size(); ++j) {
            dist = compute_distance(input_curves[i], *centroids[j], metric);
            
            if (min_dist == -1 || dist < min_dist) {
                min_dist = dist;
                p_centr = j;
            }
        }
        
        clusters[p_centr].push_back(i);
        value += min_dist;
    }
    
    return value;
}

double range_search(const vector<HashTable> &hashtables, const vector<const Curve*> &centroids, vector<vector<int> > &clusters, double delta, const char *metric) {
    vector<int> assignment((int)input_curves.size(), -1);
    vector<set<Curve> > R_closest_curves((int)centroids.size());
    vector<bool> grid_curves_found((int)centroids.size(), false), visited((int)input_curves.size(), false);
    double minim = -1, value = 0;

    for (int i = 0; i < (int)clusters.size(); ++i) {
        clusters[i].clear();
    }

    for (int i = 0; i < (int)centroids.size(); ++i) {
        for (int j = i + 1; j < (int)centroids.size(); ++j) {
            double dist = compute_distance(*centroids[i], *centroids[j], metric);
            
            if (minim == -1 || dist < minim) {
                minim = dist;
            }
        }
    }
    
    double R = minim / 2.0;

    while (1) {
        general_search(hashtables, delta, R, "classic", metric, R_closest_curves, centroids, grid_curves_found, visited);
        bool found = false;
        
        for (int i = 0; i < (int)R_closest_curves.size(); ++i) {
            for (set<Curve>::const_iterator itr = R_closest_curves[i].begin(); itr != R_closest_curves[i].end(); ++itr) {
                int id = itr->get_int_id();
                
                if (assignment[id] == -1) {
                    assignment[id] = i;
                    found = true;
                } else {
                    int dist_1 = compute_distance(*itr, *centroids[i], metric);
                    int dist_2 = compute_distance(*itr, *centroids[assignment[id]], metric);
                    
                    if (dist_1 < dist_2) {
                        assignment[id] = i;
                        found = true;
                    }
                }
            }
        }
        
        if (!found) {
            break;
        }
        
        R *= 2;
    }
    
    // assignment for long points
    for (int i = 0; i < (int)assignment.size(); ++i) {
        if (assignment[i] == -1) {
            double minim = -1;
            
            for (int j = 0; j < (int)centroids.size(); ++j) {
                double dist = compute_distance(input_curves[i], *centroids[j], metric);
                
                if (minim == -1 || dist < minim) {
                    minim = dist;
                    assignment[i] = j;
                }
            }

            value += minim;
        } else {
            double dist = compute_distance(input_curves[i], *centroids[assignment[i]], metric);
            value += dist;
        }
        
        clusters[assignment[i]].push_back(i);
    }

    return value;
}
