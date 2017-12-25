#include <cstring>
#include <cmath>
#include <iostream>
#include "Eigen/Dense"
#include "Eigen/SVD"
#include <Eigen/LU>
#include "distances.h"

using Eigen::MatrixXd;
using Eigen::Map;
using Eigen::JacobiSVD;

double **mem_distance;

void find_closest_point(vector<double> &closest_point, const vector<double> &curr_point, const vector<double> &lower_left_point, double delta) {
    int dim = curr_point.size();
    
    for (int i = 0; i < dim; ++i) {
        double val = curr_point[i] - lower_left_point[i] * delta;
        closest_point.push_back(lower_left_point[i]);
        
        if (val > delta / 2) {
            ++closest_point.back();
        }
    }
}

double euclidean_distance_square(const vector<double> &pnt_1, const vector<double> &pnt_2) {
    double dist = 0;
    
    for (int i = 0; i < (int)pnt_1.size(); ++i) {
        dist += (pnt_1[i] - pnt_2[i]) * (pnt_1[i] - pnt_2[i]);
    }
    
    return dist;
}

void mean_point(vector<double> &mean, const vector<double> &pnt_1, const vector<double> &pnt_2) {
    for (int i = 0; i < (int)pnt_1.size(); ++i) {
        double val = (pnt_1[i] + pnt_2[i]) / 2.0;
        mean.push_back(val);
    }
}

double discrete_frechet_distance(const Curve &curve_1, const Curve &curve_2, Curve &mean_traversal, bool path) {
    int N1 = curve_1.get_length();
    int N2 = curve_2.get_length();
    double **dp_solve = new double*[N1];
    double result, dist;
    
    for (int i = 0; i < N1; ++i) {
        dp_solve[i] = new double[N2];
    }
    
    dist = euclidean_distance_square(curve_1.get_point(0), curve_2.get_point(0));
    dp_solve[0][0] = dist;
    
    for (int i = 1; i < N1; ++i) {
        dist = euclidean_distance_square(curve_1.get_point(i), curve_2.get_point(0));
        dp_solve[i][0] = max(dp_solve[i - 1][0], dist);
    }
    
    for (int i = 1; i < N2; ++i) {
        dist = euclidean_distance_square(curve_1.get_point(0), curve_2.get_point(i));
        dp_solve[0][i] = max(dp_solve[0][i - 1], dist);
    }
    
    for (int i = 1; i < N1; ++i) {
        for (int j = 1; j < N2; ++j) {
            dist = euclidean_distance_square(curve_1.get_point(i), curve_2.get_point(j));
            double val = min(min(dp_solve[i - 1][j], dp_solve[i][j - 1]), dp_solve[i - 1][j - 1]);
            
            dp_solve[i][j] = max(val, dist);
        }
    }
    
    result = sqrt(dp_solve[N1 - 1][N2 - 1]);
    
    if (path) {
        int p1 = N1 - 1, p2 = N2 - 1;
        Curve reverse_mean_traversal;
        vector<double> mean;
        
        mean.clear();
        mean_point(mean, curve_1.get_point(p1), curve_2.get_point(p2));
        reverse_mean_traversal.insert_point(mean);
        
        while (p1 || p2) {
            if (!p2 || (p1 && dp_solve[p1 - 1][p2] < min(dp_solve[p1][p2 - 1], dp_solve[p1 - 1][p2 - 1]))) {
                mean.clear();
                mean_point(mean, curve_1.get_point(--p1), curve_2.get_point(p2));
            } else if (!p1 || (p2 && dp_solve[p1][p2 - 1] < dp_solve[p1 - 1][p2 - 1])) {
                mean.clear();
                mean_point(mean, curve_1.get_point(p1), curve_2.get_point(--p2));
            } else {
                mean.clear();
                mean_point(mean, curve_1.get_point(--p1), curve_2.get_point(--p2));
            }
            
            reverse_mean_traversal.insert_point(mean);
        }
        
        if (p1 || p2) {
            mean.clear();
            mean_point(mean, curve_1.get_point(0), curve_2.get_point(0));
            reverse_mean_traversal.insert_point(mean);
        }
        
        for (int i = (int)reverse_mean_traversal.get_length() - 1; i >= 0; --i) {
            mean_traversal.insert_point(reverse_mean_traversal.get_point(i));
        }
    }
    
    for (int i = 0; i < N1; ++i) {
        delete[] dp_solve[i];
    }
    
    delete[] dp_solve;
    return result;
}

double dynamic_time_wrapping(const Curve &curve_1, const Curve &curve_2) {
    int N1 = curve_1.get_length();
    int N2 = curve_2.get_length();
    double **dp_solve = new double*[N1];
    double result, dist;
    
    for (int i = 0; i < N1; ++i) {
        dp_solve[i] = new double[N2];
    }
    
    dist = euclidean_distance_square(curve_1.get_point(0), curve_2.get_point(0));
    dp_solve[0][0] = dist;
    
    for (int i = 1; i < N1; ++i) {
        dist = euclidean_distance_square(curve_1.get_point(i), curve_2.get_point(0));
        dp_solve[i][0] = dp_solve[i - 1][0] + dist;
    }
    
    for (int i = 1; i < N2; ++i) {
        dist = euclidean_distance_square(curve_1.get_point(0), curve_2.get_point(i));
        dp_solve[0][i] = dp_solve[0][i - 1] + dist;
    }
    
    for (int i = 1; i < N1; ++i) {
        for (int j = 1; j < N2; ++j) {
            dist = euclidean_distance_square(curve_1.get_point(i), curve_2.get_point(j));
            double val = min(min(dp_solve[i - 1][j], dp_solve[i][j - 1]), dp_solve[i - 1][j - 1]);
            
            dp_solve[i][j] = dist + val;
        }
    }
    
    result = sqrt(dp_solve[N1 - 1][N2 - 1]);
    
    for (int i = 0; i < N1; ++i) {
        delete[] dp_solve[i];
    }
    
    delete[] dp_solve;
    return result;
}

double c_rmsd(const Curve &curve_1, const Curve &curve_2) {
    vector<double> xc(curve_1.get_dimension());
    vector<double> yc(curve_2.get_dimension());
    vector<double> point;
    int N1 = curve_1.get_length();
    int N2 = curve_2.get_length();
    
    for (int i = 0; i < curve_1.get_length(); ++i) {
        for (int j = 0; j < curve_1.get_dimension(); ++j) {
            xc[j] = xc[j] + (double)(curve_1.get_coord_point(j, i) / curve_1.get_length());
        }
    }
    for (int i = 0; i < curve_2.get_length(); ++i) {
        for (int j = 0; j < curve_2.get_dimension(); ++j) {
            yc[j] = yc[j] + (double)(curve_2.get_coord_point(j, i) / curve_2.get_length());
        }
    }
    
    MatrixXd matrix_X(N1, curve_1.get_dimension());
    MatrixXd matrix_Y(N2, curve_2.get_dimension());
    
    for (int i = 0; i < curve_1.get_length(); ++i) {
        for (int j = 0; j < curve_1.get_dimension(); ++j) {
            matrix_X(i, j) = curve_1.get_coord_point(j, i) - xc[j];
        }
    }
    for (int i = 0; i < curve_2.get_length(); ++i) {
        for (int j = 0; j < curve_2.get_dimension(); ++j) {
            matrix_Y(i, j) = curve_2.get_coord_point(j, i) - yc[j];
        }
    }
    
    JacobiSVD<MatrixXd> svd(matrix_X.transpose() * matrix_Y, Eigen::ComputeFullU | Eigen::ComputeFullV);
    
    const Eigen::VectorXd S = svd.singularValues();
    
    if (S(2) <= 0.0) {
        return -1;
    }
    
    MatrixXd Q = svd.matrixU() * (svd.matrixV()).transpose();
    
    if (Q.determinant() < 0) {
        MatrixXd U = svd.matrixU();
        
        for (int i = 0; i < curve_1.get_dimension(); ++i) {
            U(i, 2) *= -1;
        }
        Q = U * (svd.matrixV()).transpose();
    }
    
    return (matrix_X * Q - matrix_Y).norm() / sqrt(curve_1.get_length());
}

double compute_distance(const Curve &curve_1, const Curve &curve_2, const char *dist_function) {
    if (curve_1.get_int_id() != -1 && curve_2.get_int_id() != -1) {
        if (mem_distance[curve_1.get_int_id()][curve_2.get_int_id()] != -1) {
            return mem_distance[curve_1.get_int_id()][curve_2.get_int_id()];
        }
    }
    
    double dist = 0.0;
    
    if (!strcmp(dist_function, "DFT")) {
        dist = discrete_frechet_distance(curve_1, curve_2);
    } else if (!strcmp(dist_function, "DTW")) {
        dist = dynamic_time_wrapping(curve_1, curve_2);
    }
    
    if (curve_1.get_int_id() != -1 && curve_2.get_int_id() != -1) {
        mem_distance[curve_1.get_int_id()][curve_2.get_int_id()] = dist;
    }
    
    return dist;
}
