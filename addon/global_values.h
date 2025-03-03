#ifndef GLOBAL_VALUES_H
#define GLOBAL_VALUES_H

int g_value = 99;

// bool debug = true;
bool debug = false;

bool nRound = true;
// bool nRound = false;

// Data
vector<double> d_computational_time;
vector<double> d_result_time;
vector<uint32_t> d_packet_size;
vector<int> d_packet_loss;

// Packet Loss
int g_packet_loss_count = 0;


// Results
double avg_compute_time;
double avg_result_time;
double avg_packet_size;
double avg_packet_loss;
double med_compute_time;
double med_result_time;
double med_packet_size;
double med_packet_loss;
vector<std::tuple<int, double, double, double, double, double, double>> results;
int r_num = 0;

// Parameters

string g_send_type = "basic";
string g_subnet = "same";
string g_protocol = "UDP";

string g_distance = "hop";
// string g_distance = "traffic";

int g_n_client = 4;

int g_matrix_size = 8;
int g_value_range = 127;


#endif // GLOBAL_VALUES_H