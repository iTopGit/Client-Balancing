import pandas as pd
import matplotlib.pyplot as plt
import os

# Load CSV directory
file_path = "/home/ntk/workspace/ns-allinone-3.42/ns-3.42/scratch/project/saves/"

# Define save path for images
save_path = "/home/ntk/workspace/ns-allinone-3.42/ns-3.42/scratch/project/images/packet_loss"

# Ensure save directory exists
os.makedirs(save_path, exist_ok=True)

# Function to create a graph for packet loss
def plot_packet_loss(subnet, protocol, distance):
    balance_file = f"{file_path}balance_{subnet}_{protocol}_{distance}.csv"
    basic_file = f"{file_path}basic_{subnet}_{protocol}_{distance}.csv"

    try:
        df_balance = pd.read_csv(balance_file)
        df_basic = pd.read_csv(basic_file)
    except FileNotFoundError:
        print(f"Missing file for {subnet}, {protocol}, {distance}. Skipping.")
        return
    
    if "avg_packet_loss" not in df_balance.columns or "avg_packet_loss" not in df_basic.columns:
        print(f"Column 'avg_packet_loss' not found in {balance_file} or {basic_file}. Skipping.")
        return

    plt.figure()
    plt.plot(df_balance["n_nodes"], df_balance["avg_packet_loss"], marker='o', linestyle='-', label="Balance")
    plt.plot(df_basic["n_nodes"], df_basic["avg_packet_loss"], marker='o', linestyle='--', label="Basic")
    
    plt.title(f"Packet Loss ({subnet}, {protocol}, {distance})")
    plt.xlabel("Number of Nodes")
    plt.ylabel("Average Packet Loss")
    plt.xticks([1, 2, 3, 4])  # Adjust based on node configurations
    plt.legend()
    plt.grid(True)

    # Save the plot
    filename = f"packet_loss_{subnet}_{protocol}_{distance}.png"
    plt.savefig(os.path.join(save_path, filename), format='png', dpi=300)
    plt.close()
    print(f"Saved: {filename}")

# Parameters to iterate over
subnets = ["diff", "same"]
protocols = ["TCP", "UDP"]
distances = ["hop"]

def generate_packet_loss_graphs():
    for subnet in subnets:
        for protocol in protocols:
            for distance in distances:
                plot_packet_loss(subnet, protocol, distance)

generate_packet_loss_graphs()
