#ifndef GLOBAL_VALUES_H
#define GLOBAL_VALUES_H

int g_value = 99;

bool debug = true;

// Data
vector<double> d_computational_time;
vector<double> d_result_time;

// Results
double r_computational_time;
double r_result_time;

// Parameters

string g_send_type = "basic";
string g_subnet = "same";
string g_protocol = "UDP";
string g_distance = "hop";
int g_n_client = 4;

int g_matrix_size = 16;
int g_value_range = 127;




#endif // GLOBAL_VALUES_H