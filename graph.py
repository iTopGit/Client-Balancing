import matplotlib.pyplot as plt
import statistics

# 3 nodes

compute_data = {
    "node 1": {
        "balance": [0.3800], 
        "normal": [0.4979]
    },
    "node 2": {
        "balance": [0.5302], 
        "normal": [0.7454]
    },
    "node 3": {
        "balance": [0.8056, 0.5888], 
        "normal": [0.7786]
    },
    "node 4": {
        "balance": [0.5419, 0.6350], 
        "normal": [1.2213, 1.3318]
    }
}

return_data = {
    "node 1": {
        "balance": [3.4916], 
        "normal": [3.4857]
    },
    "node 2": {
        "balance": [5.8644], 
        "normal": [5.1424]
    },
    "node 3": {
        "balance": [8.7434, 8.7481], 
        "normal": [7.9953]
    },
    "node 4": {
        "balance": [0], 
        "normal": [0]
    }
}
    
def barGraph() :
    # Sample data
    categories = ['Balance ', 'Normal']  # Categories for X-axis
    values = [bm, nm]  # Values for Y-axis

    # Plot the bar graph
    plt.bar(categories, values, color='skyblue')

    # Add titles and labels
    plt.title('Sample Bar Graph')
    plt.xlabel('Categories')
    plt.ylabel('Values')

    # Show grid
    plt.grid(axis='y', linestyle='--', alpha=0.7)

    # Display the graph
    plt.show()

def processingTime() :
    # Sample data
    x = [1, 2, 3, 4]
    
    yB = [0.301534, 0.420082, 0.379774, 0.427029]  # Line 1 data
    yN = [0.340942, 0.54479, 0.562765, 0.662598]   # Line 2 data

    # Plot the first line
    plt.plot(x, yB, marker='o', linestyle='-', color='b', label='Balance')

    # Plot the second line
    plt.plot(x, yN, marker='s', linestyle='--', color='r', label='Normal')

    plt.xticks(x)
    plt.yticks(sorted([0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8]))
    
    # Add titles and labels
    plt.title('Server Processing Time\ndifferent subnet with UDP', fontsize=12)
    plt.xlabel('number of nodes')
    plt.ylabel('time in milliseconds')
    plt.legend()  # Add legend

    # Show grid
    plt.grid(True)

    # Display the graph
    plt.show()
    
def resultTime() :
    # Sample data
    x = [1, 2, 3, 4]
    
    yB = [3.49817, 4.48052, 3.95728, 4.05743]  # Line 1 data
    yN = [3.97726, 3.83604, 4.06254, 4.25237]   # Line 2 data

    # Plot the first line
    plt.plot(x, yB, marker='o', linestyle='-', color='b', label='Balance')

    # Plot the second line
    plt.plot(x, yN, marker='s', linestyle='--', color='r', label='Normal')

    plt.xticks(x)
    plt.yticks([3.0, 3.5, 4.0, 4.5, 5.0])
    
    # Add titles and labels
    plt.title('Return Result Time\ndifferent subnet with UDP', fontsize=12)
    plt.xlabel('number of nodes')
    plt.ylabel('time in milliseconds')
    plt.legend()  # Add legend

    # Show grid
    plt.grid(True)

    # Display the graph
    plt.show()

resultTime()
processingTime()