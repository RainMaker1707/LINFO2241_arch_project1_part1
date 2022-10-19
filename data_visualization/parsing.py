def parse(filename):
    infos = {"thread_number": 0, "request_rate": 0, "request_time": 0, "run_times": []}
    data = []
    with open(filename, 'r') as file:
        for line in file:
            if "Elapsed time" in line:
                data.append(int(line[39:-4])/1000000)  # store and convert in seconds
            elif "Total threads" in line:
                infos["thread_number"] = int(line[22:-1])
            elif "Request rate" in line:
                infos["request_rate"] = int(line[14:-1])
            elif "Request time" in line:
                infos["request_time"] = int(line[14:-1])
    infos["run_times"] = data
    return infos
