import pandas as pd
import matplotlib.pyplot as plt
import os
import math

DOMAIN = 2048

# Path to the CSV file
csv_path = os.path.join(os.path.dirname(__file__), '../tiempos_mpi.csv')

# Read the CSV
try:
    df = pd.read_csv(csv_path)
except FileNotFoundError:
    print(f"Error: File not found at {csv_path}")
    exit(1)

# Clean column names (strip whitespace)
df.columns = df.columns.str.strip()

# Get unique Delta values
deltas = sorted(df['Delta'].unique())
num_deltas = len(deltas)

# Determine grid size (2 columns)
cols = 2
rows = math.ceil(num_deltas / cols)

# Create subplots
fig, axes = plt.subplots(rows, cols, figsize=(12, 5 * rows), constrained_layout=True)
axes = axes.flatten()

for i, delta in enumerate(deltas):
    ax = axes[i]
    subset = df[df['Delta'] == delta].sort_values(by='Procesos')
    
    ax.plot(subset['Procesos'], subset['Tiempo'], marker='o', color='b')
    
    ax.set_title(f'N = {int(DOMAIN / delta)}')
    ax.set_xlabel('Procesos')
    ax.set_ylabel('Tiempo (s)')
    
    # Log scale for X only
    ax.set_xscale('log', base=2)
    
    # Set X ticks to be the actual process numbers present in the data
    process_ticks = subset['Procesos'].unique()
    ax.set_xticks(process_ticks)
    ax.get_xaxis().set_major_formatter(plt.ScalarFormatter())
    
    # Y axis linear (default)
    ax.grid(True, which="both", ls="-", alpha=0.5)

# Hide unused subplots if any
for j in range(i + 1, len(axes)):
    axes[j].axis('off')

# Save the plot
output_path = os.path.join(os.path.dirname(__file__), 'tiempos_subplots.png')
plt.savefig(output_path)
print(f"Graph saved to {output_path}")

# --- New Combined Plot ---
plt.figure(figsize=(10, 6))

for delta in deltas:
    subset = df[df['Delta'] == delta].sort_values(by='Procesos')
    plt.plot(subset['Procesos'], subset['Tiempo'], marker='o', label=f'N = {int(DOMAIN / delta)}')

plt.xscale('log', base=2)
plt.yscale('log')
plt.xlabel('Procesos')
plt.ylabel('Tiempo (s)')
plt.title('Tiempo de Ejecución vs Procesos para Diferentes N (Escala Log-Log)')
plt.legend(title='N', bbox_to_anchor=(1.05, 1), loc='upper left')
plt.grid(True, which="both", ls="-", alpha=0.5)

# Set X ticks
all_processes = sorted(df['Procesos'].unique())
plt.xticks(all_processes, all_processes)
plt.gca().get_xaxis().set_major_formatter(plt.ScalarFormatter())

# Save the combined plot
output_path_combined = os.path.join(os.path.dirname(__file__), 'tiempos_combined.png')
plt.savefig(output_path_combined, bbox_inches='tight')
print(f"Combined graph saved to {output_path_combined}")
