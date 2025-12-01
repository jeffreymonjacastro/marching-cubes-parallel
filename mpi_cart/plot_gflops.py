import pandas as pd
import matplotlib.pyplot as plt
import os

# Configuration
DOMAIN = 2048
# FLOP counts from code analysis (MetaBalls)
FLOPS_PER_EVAL = 89
FLOPS_PER_CELL_CASE = 8 * FLOPS_PER_EVAL # 712
FLOPS_PER_INTERSECTION = 2 * FLOPS_PER_EVAL + 13 # 191
FLOPS_PER_NON_EMPTY_CELL_INTERSECTIONS = 12 * FLOPS_PER_INTERSECTION # 2292

def calculate_flops(row):
    delta = row['Delta']
    n_triangles = row['Triangulos']
    
    # Total cells in the grid
    total_cells = (DOMAIN / delta) ** 3
    
    # Cost for checking all cells (generateCase)
    flops_case = total_cells * FLOPS_PER_CELL_CASE
    
    # Cost for intersections
    flops_intersections = n_triangles * FLOPS_PER_NON_EMPTY_CELL_INTERSECTIONS
    
    total_flops = flops_case + flops_intersections
    return total_flops

def main():
    # Adjust path if running from root or mpi_row folder
    csv_path = 'mpi_cart/tiempos_mpi.csv'
    if not os.path.exists(csv_path):
        csv_path = 'tiempos_mpi.csv'
        
    if not os.path.exists(csv_path):
        print(f"Error: {csv_path} not found.")
        return

    print(f"Reading from {csv_path}")
    df = pd.read_csv(csv_path)
    
    # Calculate FLOPs
    df['FLOPs'] = df.apply(calculate_flops, axis=1)
    
    # Calculate GFLOPS (Giga Floating Point Operations Per Second)
    df['GFLOPS'] = df['FLOPs'] / df['Tiempo'] / 1e9
    
    # Print table
    pd.options.display.float_format = '{:.2f}'.format
    print(df[['Procesos', 'Delta', 'Tiempo', 'Triangulos', 'FLOPs', 'GFLOPS']])
    
    # Save processed data
    output_csv_path = 'mpi_cart/tiempos_mpi_flops.csv'
    if not os.path.exists('mpi_cart') and os.path.exists('tiempos_mpi.csv'):
         output_csv_path = 'tiempos_mpi_flops.csv'
        
    df.to_csv(output_csv_path, index=False)
    print(f"\nResults saved to {output_csv_path}")

    # Plotting
    plt.figure(figsize=(10, 6))
    
    deltas = df['Delta'].unique()
    deltas.sort()
    
    for delta in deltas:
        subset = df[df['Delta'] == delta].sort_values('Procesos')
        N = int(DOMAIN / delta)
        plt.plot(subset['Procesos'], subset['GFLOPS'], marker='o', label=f'N={N}')

    plt.title('GFLOPS vs Processes (MPI Cart Decomposition)')
    plt.xlabel('Number of Processes (Log Scale)')
    plt.ylabel('GFLOPS')
    plt.xscale('log', base=2)
    plt.xticks(df['Procesos'].unique(), df['Procesos'].unique())
    plt.grid(True, which="both", ls="-", alpha=0.5)
    plt.legend()
    
    plot_path = 'mpi_cart/gflops_plot_cart.png'
    if not os.path.exists('mpi_cart') and os.path.exists('tiempos_mpi.csv'):
        plot_path = 'gflops_plot.png'
        
    plt.savefig(plot_path)
    print(f"Plot saved to {plot_path}")

if __name__ == "__main__":
    main()
