import pandas as pd
import matplotlib.pyplot as plt
import os

# Load CSV files
file = "/home/ntk/workspace/ns-allinone-3.42/ns-3.42/scratch/project/saves/"

# Define save path
save_path = "/home/ntk/workspace/ns-allinone-3.42/ns-3.42/scratch/project/images/"

# Ensure the save directory exists
os.makedirs(save_path, exist_ok=True)

# Function to create a new figure for each graph
def create_graph(title, subnet, protocol, distance, col_name):
    
    X_file = file + "balance" + "_" + subnet + "_" + protocol + "_" + distance + ".csv"
    Y_file = file + "basic" + "_" + subnet + "_" + protocol + "_" + distance + ".csv"
    
    dfx = pd.read_csv(X_file)
    dfy = pd.read_csv(Y_file)
    
    plt.figure()  # Creates a new figure for each graph
    plt.plot(dfx["n_nodes"], dfx[col_name], marker='o', linestyle='-', label="balance")
    plt.plot(dfy["n_nodes"], dfy[col_name], marker='o', linestyle='--', label="basic")
    plt.title(title)
    plt.xlabel("Number of Nodes")
    plt.ylabel(col_name + " (ms)")
    plt.xticks([1, 2, 3, 4])  # Add xticks
    plt.legend()
    plt.grid(True)
    # Save the plot
    filename = title + ".png"
    plt.savefig(os.path.join(save_path, filename), format='png', dpi=300)
    plt.close()  # Close the figure to free memory

subnets = ["diff", "same"]
protocols = ["TCP", "UDP"]
distances = ["hop", "traffic"]
time = ["avg_compute_time", "avg_result_time", "med_compute_time", "med_result_time"]

for subnet in subnets:
    for protocol in protocols:
        for distance in distances:
            if distance == "traffic":
                continue
            for t in time:
                filename = f"{subnet}_{protocol}_{distance}_{t}"
                print(filename)
                create_graph(filename, subnet, protocol, distance, t)

