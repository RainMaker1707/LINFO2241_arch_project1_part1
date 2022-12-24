import matplotlib.pyplot as plt


def parse_txt(filename):
    """
    :param filename: filename of client output to parse
    :return: tab: a list of the response time in µs
    """
    tab = []
    with open(filename, 'r') as file:
        for line in file:
            if "Elapsed time" in line:
                tab.append(int(line.split(':')[1].split()[0]))
    return tab


def make_graph(data1, data2):
    """
    Graph title:
    Graph type: violin plot
    Axes
     - x: types of run
     - y: response time
    """
    plt.boxplot([data1, data2], showmeans=True, showfliers=True)
    plt.title("Response time difference between the two float versions")
    plt.xlabel("Types of run")
    plt.ylabel("Response time (µs)")
    plt.savefig("graph.png", format="png", dpi=125)


if __name__ == "__main__":
    float_server = parse_txt("server_float_output.txt")
    avx_server = parse_txt("server_avx_output.txt")
    make_graph(float_server, avx_server)


