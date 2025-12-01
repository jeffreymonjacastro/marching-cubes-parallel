import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
from matplotlib.ticker import ScalarFormatter
import os
import math

# --- CONFIGURACIÓN ---
DOMAIN_SIZE = 2048
OUTPUT_DIR = 'outputs'

# Rutas relativas
PATH_SEQ = os.path.join('sequential', 'tiempos_rescue.csv')
PATH_ROW = os.path.join('mpi_row', 'tiempos_mpi.csv')
PATH_CART = os.path.join('mpi_cart', 'tiempos_mpi.csv')

if not os.path.exists(OUTPUT_DIR):
    os.makedirs(OUTPUT_DIR)

# --- 1. CARGA DE DATOS ---
def load_and_prep_data():
    try:
        df_row = pd.read_csv(PATH_ROW)
        df_cart = pd.read_csv(PATH_CART)
        try:
            df_seq = pd.read_csv(PATH_SEQ)
            df_seq.columns = df_seq.columns.str.strip()
            df_seq['N'] = (DOMAIN_SIZE / df_seq['Delta']).astype(int)
        except FileNotFoundError:
            df_seq = None
    except FileNotFoundError as e:
        print(f"Error: {e}")
        return None

    df_row.columns = df_row.columns.str.strip()
    df_cart.columns = df_cart.columns.str.strip()
    df_row['N'] = (DOMAIN_SIZE / df_row['Delta']).astype(int)
    df_cart['N'] = (DOMAIN_SIZE / df_cart['Delta']).astype(int)

    # Imputación Row P=8, N=2048
    mask_missing = (df_row['Procesos'] == 8) & (df_row['N'] == 2048)
    if df_row.loc[mask_missing].empty or df_row.loc[mask_missing, 'Tiempo'].isnull().any():
        try:
            t_p4 = df_row[(df_row['Procesos'] == 4) & (df_row['N'] == 2048)]['Tiempo'].values[0]
            t_p16 = df_row[(df_row['Procesos'] == 16) & (df_row['N'] == 2048)]['Tiempo'].values[0]
            imputed_val = np.sqrt(t_p4 * t_p16)
            if df_row.loc[mask_missing].empty:
                new_row = pd.DataFrame({'Procesos': [8], 'Delta': [1], 'Tiempo': [imputed_val], 'N': [2048]})
                df_row = pd.concat([df_row, new_row], ignore_index=True)
            else:
                df_row.loc[mask_missing, 'Tiempo'] = imputed_val
        except: pass

    # Extrapolación secuencial N=2048
    if df_seq is not None and 2048 not in df_seq['N'].values:
        if 1024 in df_seq['N'].values:
            t_1024 = df_seq[df_seq['N'] == 1024]['Tiempo'].values[0]
            t_2048_est = t_1024 * 8.0 
            new_seq_row = pd.DataFrame({'Delta': [1], 'Tiempo': [t_2048_est], 'Triangulos': [0], 'N': [2048]})
            df_seq = pd.concat([df_seq, new_seq_row], ignore_index=True)

    df_row['Strategy'] = 'MPI Row'
    df_cart['Strategy'] = 'MPI Cart'
    df_all = pd.concat([df_row, df_cart], ignore_index=True)

    # Inyección P=1
    if df_seq is not None:
        p1_rows = []
        for _, row in df_seq.iterrows():
            p1_rows.append({'Procesos': 1, 'Delta': row['Delta'], 'Tiempo': row['Tiempo'], 'N': row['N'], 'Strategy': 'MPI Row'})
            p1_rows.append({'Procesos': 1, 'Delta': row['Delta'], 'Tiempo': row['Tiempo'], 'N': row['N'], 'Strategy': 'MPI Cart'})
        df_all = pd.concat([df_all, pd.DataFrame(p1_rows)], ignore_index=True)
        df_all = df_all.sort_values(by=['Strategy', 'N', 'Procesos'])

    return df_all

# --- CÁLCULOS ---
def calculate_standard_metrics(data):
    t_base_row = data[data['Procesos'] == 1]
    if not t_base_row.empty:
        t_1 = t_base_row['Tiempo'].values[0]
    else:
        p_min = data['Procesos'].min()
        t_1 = data[data['Procesos'] == p_min]['Tiempo'].values[0] * p_min
    speedup = t_1 / data['Tiempo']
    efficiency = speedup / data['Procesos']
    return speedup, efficiency

# --- GRÁFICAS ---

def plot_time_subplots(df_all):
    """ Genera una grilla de gráficas de Tiempo vs Procesos (Una por N) """
    Ns = sorted(df_all['N'].unique(), reverse=True)
    cols = 2
    rows = math.ceil(len(Ns) / cols)
    
    fig, axes = plt.subplots(rows, cols, figsize=(12, 4 * rows), constrained_layout=True)
    axes = axes.flatten()
    
    for i, n in enumerate(Ns):
        ax = axes[i]
        for strategy, color, marker in [('MPI Row', 'blue', 'o'), ('MPI Cart', 'red', 's')]:
            data = df_all[(df_all['N'] == n) & (df_all['Strategy'] == strategy)].sort_values('Procesos')
            if not data.empty:
                ax.plot(data['Procesos'], data['Tiempo'], marker=marker, label=strategy, color=color)
        
        ax.set_title(f'Tiempo de Ejecución: N={n}')
        ax.set_xlabel('Procesos'); ax.set_ylabel('Tiempo (s)')
        ax.set_xscale('log', base=2)
        # Eje Y lineal para ver la magnitud real del tiempo
        ax.set_xticks([1, 2, 4, 8, 16, 32])
        ax.get_xaxis().set_major_formatter(ScalarFormatter())
        ax.grid(True, which="both", alpha=0.5)
        ax.legend()

    for j in range(i + 1, len(axes)): axes[j].axis('off')
    plt.savefig(os.path.join(OUTPUT_DIR, 'tiempos_subplots.png'), bbox_inches='tight', dpi=300)
    plt.close()

def plot_time_combined(df_all):
    """ Genera una sola gráfica log-log con todos los tiempos """
    plt.figure(figsize=(10, 6))
    Ns = sorted(df_all['N'].unique(), reverse=True)
    colors = cm.viridis(np.linspace(0, 1, len(Ns)))
    
    for i, n in enumerate(Ns):
        # Usamos MPI Cart como representativo para no saturar
        data = df_all[(df_all['N'] == n) & (df_all['Strategy'] == 'MPI Cart')].sort_values('Procesos')
        if not data.empty:
            plt.plot(data['Procesos'], data['Tiempo'], marker='o', label=f'N={n}', color=colors[i])

    plt.title('Tiempo de Ejecución vs Procesos (Log-Log)')
    plt.xlabel('Procesos'); plt.ylabel('Tiempo (s)')
    plt.xscale('log', base=2); plt.yscale('log', base=10)
    plt.xticks([1, 2, 4, 8, 16, 32]); plt.gca().get_xaxis().set_major_formatter(ScalarFormatter())
    plt.grid(True, which="both", alpha=0.3); plt.legend(title='Tamaño (N)')
    plt.savefig(os.path.join(OUTPUT_DIR, 'tiempos_combined.png'), bbox_inches='tight', dpi=300)
    plt.close()

def plot_efficiency_individual(df_all):
    strategies = ['MPI Row', 'MPI Cart']
    Ns = sorted(df_all['N'].unique(), reverse=True)
    colors = cm.viridis(np.linspace(0, 1, len(Ns)))

    for strategy in strategies:
        fig, ax = plt.subplots(figsize=(8, 6))
        ax.axhline(1.0, color='k', linestyle='--', label='Ideal', alpha=0.7)
        for i, n in enumerate(Ns):
            data = df_all[(df_all['N'] == n) & (df_all['Strategy'] == strategy)].sort_values('Procesos')
            if data.empty: continue
            _, efficiency = calculate_standard_metrics(data)
            ax.plot(data['Procesos'], efficiency, marker='o', label=f'N={n}', color=colors[i])

        ax.set_title(f'Eficiencia: {strategy}'); ax.set_xlabel('Procesos'); ax.set_ylabel('Eficiencia')
        ax.set_xscale('log', base=2); ax.set_xticks([1, 2, 4, 8, 16, 32])
        ax.get_xaxis().set_major_formatter(ScalarFormatter())
        ax.set_ylim(bottom=0); ax.grid(True, alpha=0.3); ax.legend(title='N', fontsize='small')
        plt.savefig(os.path.join(OUTPUT_DIR, f'efficiency_{strategy.replace(" ", "_")}.png'), bbox_inches='tight', dpi=300)
        plt.close()

def plot_speedup_individual(df_all):
    strategies = ['MPI Row', 'MPI Cart']
    Ns = sorted(df_all['N'].unique(), reverse=True)
    colors = cm.viridis(np.linspace(0, 1, len(Ns)))

    for strategy in strategies:
        fig, ax = plt.subplots(figsize=(8, 6))
        ax.plot([1, 32], [1, 32], 'k--', label='Ideal')
        for i, n in enumerate(Ns):
            data = df_all[(df_all['N'] == n) & (df_all['Strategy'] == strategy)].sort_values('Procesos')
            if data.empty: continue
            spd, _ = calculate_standard_metrics(data)
            ax.plot(data['Procesos'], spd, marker='o', label=f'N={n}', color=colors[i])
        
        ax.set_title(f'Speedup: {strategy}'); ax.set_xlabel('Procesos'); ax.set_ylabel('Speedup')
        ax.set_xscale('log', base=2); ax.set_yscale('log', base=2)
        ax.set_xticks([1, 2, 4, 8, 16, 32]); ax.get_xaxis().set_major_formatter(ScalarFormatter())
        ax.set_yticks([1, 2, 4, 8, 16, 32, 64]); ax.get_yaxis().set_major_formatter(ScalarFormatter())
        ax.grid(True, alpha=0.3); ax.legend(title='N', fontsize='small')
        plt.savefig(os.path.join(OUTPUT_DIR, f'speedup_{strategy.replace(" ", "_")}.png'), bbox_inches='tight', dpi=300)
        plt.close()

def plot_weak_scaling_cleaned(df_all):
    plt.rcParams.update({'font.size': 12})
    # --- CORRECCIÓN: Eliminamos la carga baja que se solapaba ---
    target_pairs = [
        {'label': 'Carga Muy Alta (P=1->8)', 'points': [(1, 512), (8, 1024)], 'marker': 'D', 'ls': ':'},
        {'label': 'Carga Alta (P=2->16)', 'points': [(2, 256), (16, 512)], 'marker': 's', 'ls': '-'},
        {'label': 'Carga Media (P=4->32)', 'points': [(4, 256), (32, 512)], 'marker': 'o', 'ls': '--'}
        # Eliminada Carga Baja
    ]

    for strategy in ['MPI Row', 'MPI Cart']:
        fig, ax = plt.subplots(figsize=(10, 7))
        ax.axhline(y=1.0, color='gray', linestyle=':', label='Ideal')

        for pair in target_pairs:
            x, t = [], []
            for p, n in pair['points']:
                row = df_all[(df_all['Procesos'] == p) & (df_all['N'] == n) & (df_all['Strategy'] == strategy)]
                if not row.empty: x.append(p); t.append(row['Tiempo'].values[0])
            
            if len(x) == 2:
                eff = [t[0]/val for val in t]
                ax.plot(x, eff, marker=pair['marker'], color='black', ls=pair['ls'], label=pair['label'])
                
                for i in range(2):
                    off = 1.1 if eff[i] < 0.8 else 0.9
                    va = 'bottom' if eff[i] < 0.8 else 'top'
                    ax.text(x[i], eff[i]*off, f'N={pair["points"][i][1]}', ha='center', va=va, fontsize=10, fontweight='bold')
                
                mid_x = np.exp((np.log(x[0])+np.log(x[1]))/2)
                mid_y = np.exp((np.log(eff[0])+np.log(eff[1]))/2)
                ax.text(mid_x, mid_y, f'{eff[1]:.2f}x', ha='center', va='center', bbox=dict(facecolor='white', edgecolor='black', boxstyle='round,pad=0.2'))

        ax.set_xscale('log', base=2); ax.set_yscale('log')
        ax.set_xticks([1, 2, 4, 8, 16, 32]); ax.get_xaxis().set_major_formatter(ScalarFormatter())
        ax.set_yticks([0.2, 0.5, 1, 2, 5, 10]); ax.get_yaxis().set_major_formatter(ScalarFormatter())
        ax.set_ylim(0.15, 15) 
        
        ax.set_xlabel('Procesos (P)'); ax.set_ylabel('Eficiencia Weak Scaling')
        ax.set_title(f'Weak Scaling Analysis: {strategy}'); ax.grid(True, ls=':', alpha=0.5)
        ax.legend()
        plt.savefig(os.path.join(OUTPUT_DIR, f'weak_scaling_style_{strategy.replace(" ", "_")}.png'), bbox_inches='tight', dpi=300)
        plt.close()

if __name__ == "__main__":
    print("Generando gráficas corregidas...")
    df = load_and_prep_data()
    if df is not None:
        plot_time_combined(df)
        plot_time_subplots(df)
        plot_efficiency_individual(df)
        plot_speedup_individual(df)
        plot_weak_scaling_cleaned(df)
        print("¡Listo! Revisa 'outputs/'.")