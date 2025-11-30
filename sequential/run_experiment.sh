#!/bin/bash
#SBATCH --job-name=mc_rescue
#SBATCH --output=log_rescue.out
#SBATCH --partition=standard
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --time=02:00:00
#SBATCH --mem=16G                  # Pedimos 16GB de RAM para ir sobrados

OUTPUT="tiempos_rescue.csv"
echo "Delta,Tiempo,Triangulos" > $OUTPUT

# --- CONFIGURACIÓN DE RESCATE ---
DOMAIN=2048       # Bajamos a 1024 (Esto sí lo aguanta la RAM)
FUNC="metaballs"
DELTAS=(32 16 8 4 2 1 0.5 0.25)

echo "Iniciando MODO RESCATE: Dominio $DOMAIN, Función $FUNC..."

for D in "${DELTAS[@]}"
do
    echo "-------------------------------------------"
    echo "Procesando Delta: $D"
    
    SALIDA=$(./mc $DOMAIN $D $FUNC)
    
    TRIANGULOS=$(echo $SALIDA | awk '{print $4}')
    TIEMPO=$(echo $SALIDA | awk '{print $7}')
    
    echo "  -> Tiempo: $TIEMPO s | Triángulos: $TRIANGULOS"
    
    echo "$D,$TIEMPO,$TRIANGULOS" >> $OUTPUT
done

echo "Experimento finalizado."
