import matplotlib.pyplot as plt


def create_chart_from_dat_file(dat_file_path, output_folder, file_name):
    # Read data from .dat file
    with open(dat_file_path, 'r') as file:
        data = file.readlines()
    x_values = []
    y_values = []
    for line in data:
        x, y = map(float, line.split())
        x_values.append(x)
        y_values.append(y)

    # Create the plot
    plt.figure(figsize=(8, 6))
    plt.plot(x_values, y_values, linestyle='-')
    plt.xlabel('Iterace')
    plt.ylabel('Log(Rezi)')
    # plt.title('Data from .dat File')

    # Save the plot as PDF
    output_file_path = output_folder + '/' + file_name + ".pdf"
    plt.savefig(output_file_path)

    print(f"Chart saved as '{output_file_path}'")


if __name__ == "__main__":
    # Specify the full path of the .dat file
    instructionFilePath = "../output_dir/instructions.dat"

    # Initialize an empty list to store the string variables
    string_variables = []

    # Open the .dat file in read mode
    with open(instructionFilePath, 'r') as file:
        # Read lines from the file
        lines = file.readlines()

        inputDirectory = lines[0].strip()
        inputFileName = lines[2].strip()
        inputNacaDirPlusName = lines[0].strip() + "/" + lines[5].strip()

        outputFileName1 = lines[6].strip() + ".png"
        outputFileName2 = lines[7].strip() + ".png"
        outputDirectory = lines[1].strip()

        machUpper = float(lines[10].strip())
        machLower = float(lines[11].strip())
        cpUpper = float(lines[12].strip())
        cpLower = float(lines[13].strip())

        reziFile = lines[4].strip()
        reziName = lines[8].strip()

    print("rezi chart created under name: ", outputFileName1)

    # Create the chart
    create_chart_from_dat_file(inputDirectory + reziFile, outputDirectory, reziName)
