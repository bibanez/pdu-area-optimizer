import csv
import matplotlib.pyplot as plt
import numpy as np

def read_csv(filename):
    with open(filename, 'r') as csvfile:
        reader = csv.reader(csvfile)
        table = [[int(cell) for cell in row] for row in reader]
    return table

def plot_table(table):
    """
    Plots the table as a colored grid.
    """
    table_array = np.array(table)
    plt.imshow(table_array, cmap='tab20', interpolation='none')
    plt.colorbar(ticks=range(np.max(table_array) + 1))
    plt.title('Area Distribution')
    plt.show()

if __name__ == "__main__":
    filename = 'output.csv'
    table = read_csv(filename)
    plot_table(table)
