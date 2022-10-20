import parsing
import sys
import matplotlib.pyplot as plt
import seaborn as sns
import os
import pandas as pd


def visualization(data:dict):
    df = pd.DataFrame(columns=["run_times","request_rate","key_size","file_size","n_threads"])
    for test in data.values():
        for i in range(len(test["run_times"])):
            df = df.append({"run_times":test["run_times"][i],"request_rate":test["request_rate"],"key_size":test["key_size"],"file_size":2,"n_threads":1},ignore_index=True)

    fig, ax = plt.subplots()
    # sns.violinplot(data=df,y="run_times",x="request_rate")
    fig = sns.catplot(data=df,y="run_times",x="request_rate",kind="box", height=5, aspect=2)
    plt.xlabel("Requests per second [-]")
    plt.ylabel("Request time [s]")
    plt.title("Request time depending on client request rate")
    plt.tight_layout()
    # fig.set_size_inches(40,15)
    # plt.show()

    image_format = "pdf"
    image_name = "test.pdf"
    fig.savefig(image_name, format=image_format, dpi=1200)
    return


if __name__ == "__main__":
    # for k in k_list:
    #     for r in r_list:
    #         print(k,r)
    #         duration = int(50.0/r)
    #         os.system("./../client -k "+str(k)+" -r "+str(r)+" -t "+str(duration)+" 127.0.0.1:2241 >> output.txt")

    # for r in [2000,4000]:
    #     duration = int(100/r)
    #     os.system("./../client -k 2 -r "+str(r)+" -t 1 127.0.0.1:2241 >> output.txt")

    data = parsing.parse("output.txt")
    visualization(data)
