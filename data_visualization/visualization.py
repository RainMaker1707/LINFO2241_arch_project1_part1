import parsing
import sys
import matplotlib


def visualization(data):

    return


if __name__ == "__main__":
    argv = sys.argv
    if argv[1] == "-d":  # directory option (several files)
        raise NotImplementedError
    elif argv[1] == "-f":  # uniq file option
        parsed_data = parsing.parse(argv[2])
        visualization(parsed_data)
        print(parsed_data)
