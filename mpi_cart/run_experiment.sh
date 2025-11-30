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
echo "Procesos,Delta,Tiempo" > $OUTPUT

# --- CONFIGURACIÓN ---
DOMAIN=2048
FUNC="metaballs"
# Deltas deben ser enteros. Ajusta según necesites.
DELTAS=(32 16 8 4 2 1)
# Lista de número de procesos a probar
PROCS=(2 4 8 16 32)

echo "Iniciando Experimento MPI: Dominio $DOMAIN, Función $FUNC..."

for P in "${PROCS[@]}"
do
    for D in "${DELTAS[@]}"
    do
        echo "-------------------------------------------"
        echo "Procesando P=$P, Delta=$D"
        
        # Ejecutar con MPI (mpirun suele ser más seguro si srun no está configurado con PMI)
        SALIDA=$(mpirun -np $P ./mc_mpi_v2 $DOMAIN $D $FUNC)
        
        # Extraer el tiempo de la salida (tomamos solo la primera ocurrencia por seguridad)
        TIEMPO=$(echo "$SALIDA" | grep "Total execution time:" | awk '{print $4}')
        
        echo "  -> Tiempo: $TIEMPO s"
        
        echo "$P,$D,$TIEMPO" >> $OUTPUT
    done
done

echo "Experimento finalizado."
