#include "hash_functions.h"
#include "distances.h"
#include "help_functions.h"

vector<int> vec_r;
vector<vector<vector<double> > > t_shift, vec_line;

void grid_hashing(vector<Curve> &modified_curves, double delta, const vector<double> &t_shift_grid) {
    vector<double> closest_point, lower_left_point, shifted_point;

    if (!input_curves.empty()) {
        lower_left_point.resize(input_curves[0].get_dimension());
        shifted_point.resize(input_curves[0].get_dimension());
    }

    for (int i = 0; i < (int)input_curves.size(); ++i) {
        modified_curves[i].clear_curve();

        for (int j = 0; j < input_curves[i].get_length(); ++j) {    
            for(int k = 0; k < input_curves[i].get_dimension(); ++k) {
                double value = input_curves[i].get_coord_point(k, j);
                double shifted_value = value - t_shift_grid[k];
                double image_value = (int)(shifted_value / delta);
                
                lower_left_point[k] = image_value;
                shifted_point[k] = shifted_value;
            }
            
            closest_point.clear();
            find_closest_point(closest_point, shifted_point, lower_left_point, delta);

            if (!modified_curves[i].is_empty() && modified_curves[i].get_last_point() == closest_point) { // remove duplicates
                continue;
            }
            
            modified_curves[i].insert_point(closest_point);
        }
    }
}

void grid_hashing_curve(Curve &modified_curve, double delta, const vector<double> &t_shift_grid, const Curve &curve) {
    vector<double> closest_point, lower_left_point, shifted_point;
    modified_curve.set_id(curve.get_id(), curve.get_int_id()); 
    
    lower_left_point.resize(curve.get_dimension());
    shifted_point.resize(curve.get_dimension());

    for (int i = 0; i < curve.get_length(); ++i) {
        for(int j = 0; j < curve.get_dimension(); ++j) {
            double value = curve.get_coord_point(j, i);
            double shifted_value = value - t_shift_grid[j];
            double image_value = (int)(shifted_value / delta);
            
            lower_left_point[j] = image_value;
            shifted_point[j] = shifted_value;
        }
        
        closest_point.clear();
        find_closest_point(closest_point, shifted_point, lower_left_point, delta);

        if (!modified_curve.is_empty() && modified_curve.get_last_point() == closest_point) { // remove duplicates
            continue;
        }
        
        modified_curve.insert_point(closest_point);
    }
}

void multiple_grids(vector<Curve> &concat_curves, double delta) {
    vector<Curve> modified_curves(input_curves.size(), Curve());
    vector<double> t_shift_grid;
    vector<vector<double> > t_shift_k_grids;
    int dim;
   
    if (!input_curves.empty()) {
        dim = input_curves[0].get_dimension();
    } 

    for (int i = 0; i < global_k; ++i) {
        t_shift_grid.clear();
    
        for (int j = 0; j < dim; ++j) {
            double val = uniform_distribution(0, dim);
            t_shift_grid.push_back(val);
        }

        t_shift_k_grids.push_back(t_shift_grid);
        grid_hashing(modified_curves, delta, t_shift_grid);

        for (int j = 0; j < (int)modified_curves.size(); ++j) {
            concat_curves[j].append_curve(modified_curves[j]);
        }
    } 

    t_shift.push_back(t_shift_k_grids);

    for (int i = 0; i < (int)input_curves.size(); ++i) {
        concat_curves[i].set_id(input_curves[i].get_id(), input_curves[i].get_int_id());
    }
}

void multiple_grids_curve(Curve &concat_curve, double delta, int hash_id, const Curve &curve) {
    concat_curve.set_id(curve.get_id(), curve.get_int_id());
    Curve modified_curve;

    for (int i = 0; i < global_k; ++i) { // create concatenated grid_curve after k grid_hashings
        modified_curve.clear_curve();
        grid_hashing_curve(modified_curve, delta, t_shift[hash_id][i], curve);

        concat_curve.append_curve(modified_curve);
    }
}

vector<double> prob_lsh_euclid_hashing(const vector<double> &vec, int w, int k_vec, int hash_id) {
    vector<double> vec_lsh;

    for (int i = 0; i < k_vec; ++i) {
        double dot = dot_product(vec, vec_line[hash_id][i]);
        double t = uniform_distribution(0, w);

        int val = (dot + t) / w;
        vec_lsh.push_back(val);
    }
    
    return vec_lsh;
}

long long vector_hashing(const vector<double> &vec) {
    long long sum = 0, MOD = 4294967291LL;

    for (int i = 0; i < (int)vec.size(); ++i) {
        if (i >= (int)vec_r.size()) {
            vec_r.push_back(rand() % MAX_R);
        }

        sum += (((long long)vec_r[i] + MOD) % MOD) * (((long long)vec[i] + MOD) % MOD);
        sum %= MOD;
    }
    
    sum = (sum + MOD) % MOD;
    return sum;
}
