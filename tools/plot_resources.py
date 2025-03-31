#!/usr/bin/env python3

"""
This is a helper script to visualize cpu and mem usage over time, as written out
by the ResourceLogger class in genesis. It is meant as a helper for investigating
large scale behaviour of a program. Provide it with a table as printed by the class.
"""

import pandas as pd
import matplotlib.pyplot as plt
import argparse

def compute_ylim(data):
    """
    Compute y-axis limits such that the lower bound is 0 and the upper bound
    is the maximum value plus a margin. The margin is set to 10% of the range.

    If the data is constant, a default margin is used (10% of the value, or 1 if the value is 0).
    Still does not work properly in my tests though, because matplotlib just ignores the margins.
    """
    lower = 0
    dmax = data.max()
    dmin = data.min()
    if dmax == dmin:
        margin = dmax * 0.1 if dmax != 0 else 1
    else:
        margin = 0.1 * (dmax - dmin)
    upper = dmax + margin
    return lower, upper

def main():
    # Parse command-line arguments
    parser = argparse.ArgumentParser(
        description="Plot CPU and memory usage over time from a resource log file."
    )
    parser.add_argument(
        "file", type=str, help="Path to the tab-separated resource log file."
    )
    args = parser.parse_args()

    # Read the data from the tab-separated file
    df = pd.read_csv(args.file, sep="\t")

    # Create a datetime column from 'date' and 'time'
    df["datetime"] = pd.to_datetime(df["date"] + " " + df["time"])
    df.sort_values("datetime", inplace=True)

    # Convert relevant columns to NumPy arrays
    x = df["datetime"].to_numpy()
    cpu = df["cpu"].to_numpy()
    mem = df["mem"].to_numpy()

    # Compute dynamic y-axis limits for CPU and memory
    cpu_lower, cpu_upper = compute_ylim(cpu)
    mem_lower, mem_upper = compute_ylim(mem)

    # Create the figure and primary axis for CPU data
    fig, ax1 = plt.subplots(figsize=(10, 6))
    color_cpu = "tab:blue"
    ax1.set_xlabel("Time")
    ax1.set_ylabel("CPU", color=color_cpu)
    ax1.plot(x, cpu, color=color_cpu, label="CPU")
    ax1.tick_params(axis="y", labelcolor=color_cpu)
    ax1.set_ylim(cpu_lower, cpu_upper)

    # Create the secondary axis for Memory data using twinx
    ax2 = ax1.twinx()
    color_mem = "tab:red"
    ax2.set_ylabel("Memory", color=color_mem)
    ax2.plot(x, mem, color=color_mem, label="Memory")
    ax2.tick_params(axis="y", labelcolor=color_mem)
    ax2.set_ylim(mem_lower, mem_upper)

    # Finalize layout and show the plot
    fig.tight_layout()
    plt.title("Resource Usage Over Time")
    plt.show()

if __name__ == "__main__":
    main()
