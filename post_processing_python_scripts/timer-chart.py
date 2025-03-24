import numpy as np
import matplotlib.pyplot as plt
import sys

# Specify the data address
address = sys.argv[1]

# Load data from .dat files
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

# Compute average times (in milliseconds)
avg1 = np.mean(times1)
avg2 = np.mean(times2)
avg3 = np.mean(times3)
avg4 = np.mean(times4)
avg5 = np.mean(times5)

# Create x-axis (assuming indices match across files)
iterations = data1[:, 0]

# Create stacked area chart
plt.figure(figsize=(12, 6))
plt.stackplot(iterations, times1, times2, times3, times4, times5,
              labels=[
                  f'computeScheme() - {avg1:.3f} ms',
                  f'computeRezi() - {avg2:.3f} ms',
                  f'updateCellDT() - {avg3:.3f} ms',
                  f'updateCells() - {avg4:.3f} ms',
                  f'updateBounds() - {avg5:.3f} ms'
              ],
              alpha=0.75)

# Set y-axis range
plt.ylim(0, 6)

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

