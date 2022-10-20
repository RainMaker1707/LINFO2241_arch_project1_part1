
def parse(filename: str) -> dict:
    all_data = dict()
    nb_test = -1
    with open(filename, 'r') as file:
        for line in file:
            # basics infos
            if "Total threads" in line:
                all_data[nb_test]["thread_number"] = int(line[22:-1])
            elif "Request rate" in line:
                all_data[nb_test]["request_rate"] = int(line[14:-1])
            elif "Key size" in line:
                all_data[nb_test]["key_size"] = int(line[13:-1])
            elif "Request time" in line:
                all_data[nb_test]["request_time"] = int(line[14:-1])
            # All ok run time store and convert in seconds
            elif "Elapsed time" in line:
                all_data[nb_test]["run_times"].append(int(line[41:-4])/1000000)
            # file size was equal to 0
            elif "No file received" in line:
                all_data[nb_test]["no_file"] += 1
            # when server sent error code
            elif "error code" in line:
                all_data[nb_test]["errors"] += 1
            # New test result
            elif "Client running..." in line:
                nb_test += 1
                all_data[nb_test] =  {"run_times": [], "errors": 0, "no_file": 0}
    return all_data
