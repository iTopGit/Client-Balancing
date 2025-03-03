#ifndef RESULT_H
#define RESULT_H

#include "global_values.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string>
#include <vector>

using namespace std;

void
computeStatistics()
{
    // Computational Time
    if (!d_computational_time.empty())
    {
        avg_compute_time =
            accumulate(d_computational_time.begin(), d_computational_time.end(), 0.0) /
            d_computational_time.size();

        vector<double> sortedData = d_computational_time;
        sort(sortedData.begin(), sortedData.end());

        med_compute_time =
            sortedData.size() % 2 == 0
                ? (sortedData[sortedData.size() / 2 - 1] + sortedData[sortedData.size() / 2]) / 2.0
                : sortedData[sortedData.size() / 2];
    }

    // Result Time
    if (!d_result_time.empty())
    {
        avg_result_time =
            accumulate(d_result_time.begin(), d_result_time.end(), 0.0) / d_result_time.size();

        vector<double> sortedData = d_result_time;
        sort(sortedData.begin(), sortedData.end());

        med_result_time =
            sortedData.size() % 2 == 0
                ? (sortedData[sortedData.size() / 2 - 1] + sortedData[sortedData.size() / 2]) / 2.0
                : sortedData[sortedData.size() / 2];
    }

    // Packet Size
    if (!d_packet_size.empty())
    {
        avg_packet_size =
            accumulate(d_packet_size.begin(), d_packet_size.end(), 0.0) / d_packet_size.size();

        vector<uint32_t> sortedData = d_packet_size;
        sort(sortedData.begin(), sortedData.end());

        med_packet_size =
            sortedData.size() % 2 == 0
                ? (sortedData[sortedData.size() / 2 - 1] + sortedData[sortedData.size() / 2]) / 2.0
                : sortedData[sortedData.size() / 2];
    }

    if (!d_packet_loss.empty())
    {
        avg_packet_loss =
            accumulate(d_packet_loss.begin(), d_packet_loss.end(), 0.0) / d_packet_loss.size();

        vector<int> sortedData = d_packet_loss;
        sort(sortedData.begin(), sortedData.end());

        med_packet_loss =
            sortedData.size() % 2 == 0
                ? (sortedData[sortedData.size() / 2 - 1] + sortedData[sortedData.size() / 2]) / 2.0
                : sortedData[sortedData.size() / 2];
    }
}

void
displayResult()
{
    string parameter = g_send_type + ", " + g_subnet + ", " + g_protocol + ", " + g_distance +
                       ", " + to_string(g_n_client);
    cout << "Simulation : \n" << parameter << endl;

    if (debug)
    {
        cout << "Computational Time : \n";
        copy(d_computational_time.begin(),
             d_computational_time.end(),
             ostream_iterator<double>(cout, " "));
        cout << endl;

        cout << "Result Time : \n";
        copy(d_result_time.begin(), d_result_time.end(), ostream_iterator<double>(cout, " "));
        cout << endl;
    }

    computeStatistics();

    cout << "Average Computational Time : " << avg_compute_time << endl;
    cout << "Median Computational Time : " << med_compute_time << endl;
    cout << "Average Result Time : " << avg_result_time << endl;
    cout << "Median Result Time : " << med_result_time << endl;
}

void
resetResult()
{
    d_computational_time.clear();
    d_result_time.clear();
    d_packet_loss.clear();
    d_packet_size.clear();
}

void
displayStoredResult()
{
    for (const auto& result : results)
    {
        std::cout << "n_nodes: " << std::get<0>(result)
                  << ", avg_computational_time: " << std::get<1>(result)
                  << ", median_computational_time: " << std::get<2>(result)
                  << ", avg_result_time: " << std::get<3>(result)
                  << ", median_result_time: " << std::get<4>(result) << std::endl;
    }
}

#include <fstream>

void
saveResultsToCSV(
    const std::string& file_name,
    const std::vector<std::tuple<int, double, double, double, double, double, double>>& results)
{
    const string path = "scratch/project/saves/";
    std::ofstream file(path + file_name + ".csv");

    if (!file)
    {
        std::cerr << "Error: Unable to open file at " << path << std::endl;
        return;
    }

    // Write CSV Header
    file << "n_nodes,avg_compute_time,med_compute_time,avg_result_time,med_result_time,avg_packet_"
            "loss,avg_packet_size\n";

    // Write all simulation results
    for (const auto& result : results)
    {
        file << std::get<0>(result) << ","   // n_nodes
             << std::get<1>(result) << ","   // avg_compute_time
             << std::get<2>(result) << ","   // med_compute_time
             << std::get<3>(result) << ","   // avg_result_time
             << std::get<4>(result) << ","   // med_result_time
             << std::get<5>(result) << ","   // avg_packet_loss
             << std::get<6>(result) << "\n"; // avg_packet_size
    }

    file.close();
    // std::cout << "Results saved at: " << path << "/simulation_results.csv" << std::endl;
}

#endif // RESULT_H