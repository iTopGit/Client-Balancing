import pandas as pd
import matplotlib.pyplot as plt
import os

# data_type = "avg"
data_type = "med"

# Load CSV files
file = "/home/ntk/workspace/ns-allinone-3.42/ns-3.42/scratch/project/saves/"

# Define save path
if data_type == "med":
    save_path = "/home/ntk/workspace/ns-allinone-3.42/ns-3.42/scratch/project/images/med_arrow/"
else:
    save_path = "/home/ntk/workspace/ns-allinone-3.42/ns-3.42/scratch/project/images/avg_arrow/"

# Ensure the save directory exists
os.makedirs(save_path, exist_ok=True)

# Function to create a new figure for each graph

def create_graph(title, subnet, protocol, col_name):
    base_X_file = file + "balance" + "_" + subnet + "_" + protocol + "_" + "hop" + ".csv"
    base_Y_file = file + "basic" + "_" + subnet + "_" + protocol + "_" + "hop" + ".csv"
    
    X_file = file + "balance" + "_" + subnet + "_" + protocol + "_" + "traffic" + ".csv"
    Y_file = file + "basic" + "_" + subnet + "_" + protocol + "_" + "traffic" + ".csv"
    
    dfx = pd.read_csv(X_file)
    dfy = pd.read_csv(Y_file)
    base_x = pd.read_csv(base_X_file)
    base_y = pd.read_csv(base_Y_file)
    
    # Determine markers and edge colors based on comparison with baseline data
    markers_x = []
    markers_y = []
    edge_colors_x = []
    edge_colors_y = []
    
    for i in range(len(dfx[col_name])):
        if dfx[col_name][i] > base_x[col_name][i]:
            markers_x.append('^')  # Triangle Up (Increase)
            edge_colors_x.append('red')
        elif dfx[col_name][i] < base_x[col_name][i]:
            markers_x.append('v')  # Triangle Down (Decrease)
            edge_colors_x.append('green')
        else:
            markers_x.append('o')  # Circle (No Change)
            edge_colors_x.append('black')
    
    for i in range(len(dfy[col_name])):
        if dfy[col_name][i] > base_y[col_name][i]:
            markers_y.append('^')  # Triangle Up (Increase)
            edge_colors_y.append('red')
        elif dfy[col_name][i] < base_y[col_name][i]:
            markers_y.append('v')  # Triangle Down (Decrease)
            edge_colors_y.append('green')
        else:
            markers_y.append('o')  # Circle (No Change)
            edge_colors_y.append('black')
    
    plt.figure(figsize=(8, 6))  # Creates a larger figure for better visibility
    for i in range(len(dfx["n_nodes"])):
        plt.scatter(dfx["n_nodes"][i], dfx[col_name][i], 
                    marker=markers_x[i], color='b', edgecolors=edge_colors_x[i], linewidths=2, s=100, label="balance" if i==0 else "")
    
    for i in range(len(dfy["n_nodes"])):
        plt.scatter(dfy["n_nodes"][i], dfy[col_name][i], 
                    marker=markers_y[i], color='orange', edgecolors=edge_colors_y[i], linewidths=2, s=100, label="basic" if i==0 else "")
    
    plt.plot(dfx["n_nodes"], dfx[col_name], linestyle='-', color='b')
    plt.plot(dfy["n_nodes"], dfy[col_name], linestyle='--', color='orange')
    
    plt.title(title, fontsize=14)
    plt.xlabel("Number of Nodes", fontsize=12)
    plt.ylabel(col_name + " (ms)", fontsize=12)
    plt.xticks([1, 2, 3, 4], fontsize=10)  # Add xticks
    plt.yticks(fontsize=10)
    plt.legend(fontsize=12)
    plt.grid(True, linestyle='--', alpha=0.7)
    
    # Save the plot
    filename = title + ".png"
    plt.savefig(os.path.join(save_path, filename), format='png', dpi=300)
    plt.close()  # Close the figure to free memory
    print(title)

subnets = ["diff", "same"]
protocols = ["TCP", "UDP"]

if data_type == "med":
    time = ["med_compute_time", "med_result_time"]
else:
    time = ["avg_compute_time", "avg_result_time"]

def create_all_graphs():
    for subnet in subnets:
        for protocol in protocols:
            for t in time:
                filename = f"{subnet}_{protocol}_traffic_{t}"
                create_graph(filename, subnet, protocol, t)
                    
create_all_graphs()