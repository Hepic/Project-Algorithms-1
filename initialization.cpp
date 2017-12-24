#include <iostream>
#include <cstdlib>
#include "cluster.h"
#include "curve.h"
#include "distances.h"
#include "help_functions.h"
#include "initialization.h"

void k_random_selection(vector<const Curve*> &centroids, int len) {
    vector<int> all_random_points(len);
    
    for (int i = 0; i < len; ++i) {
        all_random_points[i] = i;
    }
    
    for (int i = 0; i < num_of_clusters; ++i) {
        int pos = rand() % (len - i) + i;
        swap(all_random_points[pos], all_random_points[i]);
        
        centroids[i] = &input_curves[all_random_points[i]];
    }
}

void k_means_pp(vector<const Curve*> &centroids, int len, const char *metric) {
    vector<double> min_distance(len, -1);
    vector<bool> is_centroid(len, false);
    int pos;
    double max_sum = 0;
    
    centroids.reserve(num_of_clusters); 

    if (num_of_clusters > 0) {
        pos = rand() % len;
    
        is_centroid[pos] = true;
        centroids[0] = &input_curves[pos];
    }

    for (int t = 1; t < num_of_clusters; ++t) {
        for (int i = 0; i < len; ++i) {
            if (is_centroid[i]) {
                continue;
            }
            
            double dist = compute_distance(input_curves[i], input_curves[pos], metric);

            if (min_distance[i] == -1 || dist < min_distance[i]) {
                if (min_distance[i] != -1) {
                    max_sum -= min_distance[i] * min_distance[i];
                }
                
                max_sum += dist * dist;
                min_distance[i] = dist;
            }
        }
        
        double value = uniform_distribution(0, max_sum);
        double curr = 0;
        
        for (int i = 0; i < len; ++i) {
            if (is_centroid[i]) {
                continue;
            }
            
            curr += min_distance[i] * min_distance[i];
            
            if (curr >= value) {
                pos = i;
                break;
            }
        }
        
        is_centroid[pos] = true;
        centroids[t] = &input_curves[pos];
        
        max_sum -= min_distance[pos] * min_distance[pos];
    }
}
