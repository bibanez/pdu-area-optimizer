import csv
import matplotlib.pyplot as plt
import numpy as np
import os

def read_csv(filename):
    with open(filename, 'r') as csvfile:
        reader = csv.reader(csvfile)
        table = [[int(cell) for cell in row] for row in reader]
    return table

def read_stats(filename):
    with open(filename, 'r') as csvfile:
        reader = csv.DictReader(csvfile)
        stats = list(reader)
    return stats

def plot_table(table, title, stats, output_filename):
    """
    Plots the table as a colored grid with title and stats as subtitle.
    """
    table_array = np.array(table)
    fig, ax = plt.subplots(figsize=(10, 8))
    im = ax.imshow(table_array, cmap='tab20', interpolation='none')
    plt.colorbar(im, ax=ax, ticks=range(np.max(table_array) + 1))
    
    ax.set_title(title, fontsize=14, fontweight='bold')
    
    # Create subtitle with stats
    stats_text = "Stats:\n"
    for stat in stats[:5]:  # Limit to first 5 stats for readability
        stats_text += f"Source {stat['Source']}: w/weight_sum={float(stat['w/weight_sum']):.4f}, a/area_sum={float(stat['a/area_sum']):.4f}\n"
    if len(stats) > 5:
        stats_text += "..."
    
    ax.text(0.5, -0.15, stats_text, transform=ax.transAxes, ha='center', va='center', fontsize=10)
    
    plt.tight_layout()
    plt.savefig(output_filename, dpi=300, bbox_inches='tight')
    plt.close()

def get_title(filename):
    titles = {
        "fss": "Few sources same weight strict layout",
        "fsr": "Few sources same weight random layout",
        "fds": "Few sources different weight strict layout",
        "fdr": "Few sources different weight random layout",
        "mss": "Many sources same weight strict layout",
        "msr": "Many sources same weight random layout",
        "mds": "Many sources different weight strict layout",
        "mdr": "Many sources different weight random layout"
    }
    key = filename.split('_')[1].split('.')[0]
    return titles.get(key, "Unknown layout")

def main():
    # Create the results folder if it doesn't exist
    results_folder = "results"
    if not os.path.exists(results_folder):
        os.makedirs(results_folder)

    table_files = [f for f in os.listdir() if f.startswith('table_') and f.endswith('.csv')]
    
    for table_file in table_files:
        stats_file = table_file.replace('table_', 'stats_')
        output_file = os.path.join(results_folder, table_file.replace('.csv', '.png'))
        
        table = read_csv(table_file)
        stats = read_stats(stats_file)
        title = get_title(table_file)
        
        plot_table(table, title, stats, output_file)
        print(f"Plotted {table_file} with stats from {stats_file}. Saved as {output_file}")

if __name__ == "__main__":
    main()
