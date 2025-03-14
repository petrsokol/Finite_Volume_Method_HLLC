import numpy as np
import matplotlib.pyplot as plt

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
    inputNacaDirPlusName = lines[0].strip() + lines[5].strip()

    outputFileName1 = lines[6].strip() + ".png"
    outputFileName2 = lines[7].strip() + ".png"
    outputDirectory = lines[1].strip()
    chartInput = lines[3].strip()
    chartName = lines[9].strip() + ".pdf"

wallDataFilePath = inputDirectory + chartInput

# Extract relevant data
x_data = np.loadtxt(wallDataFilePath, usecols=(0,))
mach_data = np.loadtxt(wallDataFilePath, usecols=(3,))
cp_data = np.loadtxt(wallDataFilePath, usecols=(4,))

# Plotting
fig, ax = plt.subplots(1, 2, figsize=(12, 5))

# Plot 1
ax[0].plot(x_data, mach_data)
# ax[0].set_title('Machovo číslo')
ax[0].set_xlabel('X')
ax[0].set_ylabel('M')
ax[0].set_xlim(min(x_data), max(x_data))  # Set x-axis limits
ax[0].set_ylim(np.floor(10 * min(mach_data)) / 10.0, np.ceil(10 * max(mach_data)) / 10.0)  # Set y-axis limits
ax[0].spines['top'].set_visible(False)  # Hide top spine
ax[0].spines['right'].set_visible(False)  # Hide right spine
ax[0].grid(color='lightgray', linestyle='-', linewidth=0.5)  # Add light gray grid lines

# Plot 2 (Just for illustration, you can modify this)
ax[1].plot(x_data, cp_data, color='r')
# ax[1].set_title('tlakový koeficient')
ax[1].set_xlabel('X')
ax[1].set_ylabel('c_p')
ax[1].set_xlim(min(x_data), max(x_data))  # Set x-axis limits
ax[1].set_ylim(np.ceil(10 * max(cp_data)) / 10.0, np.floor(10 * min(cp_data)) / 10.0)  # Set y-axis limits
ax[1].spines['top'].set_visible(False)  # Hide top spine
ax[1].spines['right'].set_visible(False)  # Hide right spine
ax[1].grid(color='lightgray', linestyle='-', linewidth=0.5)  # Add light gray grid lines

# Adjust layout to add gap between subplots
plt.subplots_adjust(wspace=0.35)

# Save as PDF
print(np.ceil(10 * max(cp_data)) / 10.0)
plt.savefig(outputDirectory + chartName)
