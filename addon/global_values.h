#ifndef GLOBAL_VALUES_H
#define GLOBAL_VALUES_H

int g_value = 99;

// bool debug = true;
bool debug = false;

// Data
vector<double> d_computational_time;
vector<double> d_result_time;

// Results
double avg_compute_time;
double avg_result_time;
double med_compute_time;
double med_result_time;
vector<std::tuple<int, double, double, double, double>> results;

// Parameters

string g_send_type = "basic";
string g_subnet = "same";
string g_protocol = "UDP";

// string g_distance = "hop";
string g_distance = "traffic";

int g_n_client = 4;

int g_matrix_size = 16;
int g_value_range = 127;


#endif // GLOBAL_VALUES_H