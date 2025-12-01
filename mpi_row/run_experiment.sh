#!/bin/bash
#SBATCH --job-name=mc_mpi_exp
#SBATCH --output=log_mpi.out
#SBATCH --partition=standard
#SBATCH --ntasks=32
#SBATCH --cpus-per-task=1
#SBATCH --time=05:00:00
#SBATCH --mem=16G
#SBATCH --mail-type=END,FAIL
#SBATCH --mail-user=jeffrey.monja@utec.edu.pe

OUTPUT="tiempos_mpi.csv"
echo "Procesos,Delta,Tiempo,Triangulos" > $OUTPUT

# --- CONFIGURACIÓN ---
DOMAIN=2048
FUNC="metaballs"
# Deltas deben ser enteros. Ajusta según necesites.
DELTAS=(32 16 8 4 2 1)
# Lista de número de procesos a probar
PROCS=(2 4 8 16 32)

echo "Iniciando Experimento MPI: Dominio $DOMAIN, Función $FUNC..."

for D in "${DELTAS[@]}" 
do
    for P in "${PROCS[@]}"
    do
        echo "-------------------------------------------"
        echo "Procesando P=$P, Delta=$D"
        
        # Ejecutar con MPI (mpirun suele ser más seguro si srun no está configurado con PMI)
        SALIDA=$(mpirun -np $P ./mc_mpi $DOMAIN $D $FUNC)
        
        # Extraer el tiempo de la salida (tomamos solo la primera ocurrencia por seguridad)
        TIEMPO=$(echo "$SALIDA" | grep "Total execution time:" | awk '{print $4}')
        TRIANGULOS=$(echo "$SALIDA" | grep "Total triangles:" | awk '{print $3}')
        
        echo "  -> Tiempo: $TIEMPO s"
        echo "  -> Triangulos: $TRIANGULOS"
        
        echo "$P,$D,$TIEMPO,$TRIANGULOS" >> $OUTPUT
    done
done

echo "Experimento finalizado."
