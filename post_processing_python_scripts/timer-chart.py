import numpy as np
import matplotlib.pyplot as plt

# specify the data address
address = '../output_dir/'

# Load data from four .dat files
data1 = np.loadtxt(address + 'computeSchemeTimer.dat')
data2 = np.loadtxt(address + 'reziTimer.dat')
data3 = np.loadtxt(address + 'cellDtTimer.dat')
data4 = np.loadtxt(address + 'updateCellsTimer.dat')
data5 = np.loadtxt(address + 'boundsIteratorTimer.dat')

# Extract the second column (times) from each dataset
times1 = data1[:, 1]
times2 = data2[:, 1]
times3 = data3[:, 1]
times4 = data4[:, 1]
times5 = data5[:, 1]

# Create x-axis (assuming indices match across files)
iterations = data1[:, 0]

# Create stacked area chart
plt.figure(figsize=(12, 6))
plt.stackplot(iterations, times1, times2, times3, times4, times5,
              labels=['computeScheme()', 'computeRezi()', 'updateCellDT()', 'updateCells()', 'updateBounds()'],
              alpha=0.75)

# Add legend and labels
plt.title('Summed Execution Time per Iteration')
plt.xlabel('Iteration')
plt.ylabel('Time (ms)')
plt.legend(loc='upper left')
plt.grid(True)

# Save plot
plt.savefig(address + 'timerChart.pdf')

# Show plot
# plt.show()
