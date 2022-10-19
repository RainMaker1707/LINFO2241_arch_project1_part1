def parse(filename):
    infos = {"thread_number": 0, "request_rate": 0, "request_time": 0, "run_times": [], "errors": 0, "no_file": 0}
    data = []
    with open(filename, 'r') as file:
        for line in file:
            # basics infos
            if "Total threads" in line:
                infos["thread_number"] = int(line[22:-1])
            elif "Request rate" in line:
                infos["request_rate"] = int(line[14:-1])
            elif "Request time" in line:
                infos["request_time"] = int(line[14:-1])
            # All ok run time store and convert in seconds
            elif "Elapsed time" in line:
                data.append(int(line[39:-4])/1000000)
            # file size was equal to 0
            elif "No file received" in line:
                infos["no_file"] += 1
            # when server sent error code
            elif "error code" in line:
                infos["errors"] += 1
    infos["run_times"] = data
    return infos
