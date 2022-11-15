import matplotlib.pyplot as plt
import numpy as np
import copy
import pandas as pd
from pandas import DataFrame
import seaborn as sns

from IPython.display import display, HTML

def parsing_client(data:list) -> dict:
    """
    Parse client logs (i.e. response times) and return response times for each parameter set
    for each run
    :param data: All log output from clien
    :return: Dictionary with keys = parameter set, value = response times for each run
    """
    result = dict()
    last_parameters = tuple()
    last_run = 0
    for line in data:
        line = line.strip()

        # Blank lines
        if line == "":
            continue

        splited = line.split(" ")
        if len(splited) == 2:
            # New parameters set
            last_parameters = tuple(splited)
            result[last_parameters] = dict()
            last_run = 0
            result[last_parameters][last_run] = list()
        elif len(splited) == 4:
            # End of one run
            last_run += 1
            if last_run < 10:
                result[last_parameters][last_run] = list()
        elif len(splited) == 1:
            # One data
            result[last_parameters][last_run].append(int(line))
    return result

def parsing_data(server_data:list, client_data:list) -> DataFrame:
    stats = ["L1-dcache-load-misses","L1-dcache-loads","L1-icache-load-misses","cache-references","cache-misses","dTLB-load-misses","dTLB-loads","iTLB-load-misses","iTLB-loads","cycle_activity.stalls_total","cycle_activity.stalls_mem_any","mem_load_retired.l3_miss","cycles","branches","instructions","mem_load_retired.l2_miss","sys","L1-dcache-load-misses-rate","cache-misses-rate","dTLB-load-misses-rate","iTLB-load-misses-rate","instructions-rate"]
    df = DataFrame(columns=["version","key_size","mean-response-time","nb-requests"]+stats,index=list(range(80)))

    # Parse server data
    last_parameters = tuple()
    last_run = -1
    for line in server_data:
        splited = line.split()
        if len(splited) == 0 or splited[0] == "Performance" or splited[0] == "<not" or "user" in splited or "elapsed" in splited:
            # Useless lines
            continue
        elif splited[0] == "#":
            # New run
            last_run += 1
        elif len(splited) == 2:
            # New parameter set
            last_parameters = tuple(splited)
        elif "sys" in splited:
            # Time line
            df.loc[last_run]["sys"] = float(splited[0])
        else:
            df.loc[last_run]["version"] = last_parameters[0]
            df.loc[last_run]["key_size"] = last_parameters[1]
            line_stat = splited[1]
            df.loc[last_run][line_stat] = int(splited[0].replace(",",""))
            if "#" in splited:
                df.loc[last_run][line_stat+"-rate"] = float(splited[splited.index("#")+1].replace("%",""))

    # Parse client data
    last_run = 0
    current_run_times = list()
    for line in client_data:
        line = line.strip()

        # Blank lines
        if line == "":
            continue

        splited = line.split(" ")
        if len(splited) == 2:
            # New parameters set
            continue
        elif len(splited) == 4:
            # End of one run
            df.loc[last_run]["mean-response-time"] = np.mean(current_run_times)
            df.loc[last_run]["nb-requests"] = int(splited[-1])
            current_run_times = list()
            last_run += 1
        elif len(splited) == 1:
            # One data
            current_run_times.append(int(line))

    # Scale data by number of requests
    for col in df.columns:
        if col not in ["L1-dcache-load-misses-rate","cache-misses-rate","dTLB-load-misses-rate","iTLB-load-misses-rate","instructions-rate","version","key_size","mean-response-time","nb-requests"]:
            df[col] = (df[col] / df["nb-requests"]) * 50

    return df

if __name__ == '__main__':
    # Import data
    with open("data/client.txt","r") as f:
        client_data = f.readlines()
    with open("data/server.txt","r") as f:
        server_data = f.readlines()

    df = parsing_data(server_data,client_data)

    df["stall-rate"] = df["cycle_activity.stalls_total"] / df["cycles"]
    # Plot boxplot L1-dcache-misses-rate compared to version hue key_size
    plt.figure()
    sns.boxplot(data=df[df["key_size"]=="8"], x="version", y="instructions-rate")
    plt.figure()
    sns.boxplot(data=df[df["key_size"]=="8"], x="version", y="mem_load_retired.l2_miss")
    plt.figure()
    sns.boxplot(data=df[df["key_size"]=="8"], x="version", y="cycles")
    plt.figure()
    sns.boxplot(data=df[df["key_size"]=="8"], x="version", y="instructions")
    plt.figure()
    sns.boxplot(data=df[df["key_size"]=="8"], x="version", y="L1-dcache-load-misses-rate")
    plt.figure()
    sns.boxplot(data=df[df["key_size"]=="8"], x="version", y="mean-response-time")
    plt.figure()
    sns.boxplot(data=df[df["key_size"]=="8"], x="version", y="cycle_activity.stalls_total")
    plt.figure()
    sns.boxplot(data=df[df["key_size"]=="8"], x="version", y="stall-rate")
    plt.savefig("figures/stall8.pdf")
    plt.figure()
    sns.boxplot(data=df[df["key_size"]=="128"], x="version", y="stall-rate")
    # plt.title("8")
    plt.savefig("figures/stall128.pdf")
    plt.figure()
    sns.boxplot(data=df[df["key_size"]=="8"], x="version", y="mean-response-time")
    # plt.title("128")
    # plt.savefig("figures/time128.pdf")
    plt.figure()
    sns.boxplot(data=df[df["key_size"]=="8"], x="version", y="cache-references")
    plt.figure()
    sns.boxplot(data=df[df["key_size"]=="8"], x="version", y="cache-misses")

    plt.show()

