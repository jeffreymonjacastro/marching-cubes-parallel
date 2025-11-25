#include "mc.h"

int main() {
  int domain = 512;
  int delta = 2;
  string filename = "heart.ply";

  // ========== FUNCIONES BÁSICAS ==========

  // 1. Esfera
  // Sphere sphere(domain / 2.0, domain / 2.0, domain / 2.0, domain * (100.0 / 256.0));
  // MarchingCubes mc(domain, delta, "sphere.ply", &sphere);

  // 2. Toro
  // TorusFunction torus(domain / 2.0, domain / 2.0, domain / 2.0, 70.0, 20.0);
  // MarchingCubes mc(domain, delta, "torus.ply", &torus);

  // 3. Cubo Redondeado
  // RoundedCubeFunction roundedCube(domain / 2.0, domain / 2.0, domain / 2.0, 150.0, 15.0);
  // MarchingCubes mc(domain, delta, "rounded_cube.ply", &roundedCube);

  // ========== FUNCIONES AVANZADAS ==========

  // 4. Gyroid (superficie mínima periódica)
  // GyroidFunction gyroid(domain / 2.0, domain / 2.0, domain / 2.0, 0.15, 0.2);
  // MarchingCubes mc(domain, delta, "gyroid.ply", &gyroid);

  // 5. Metaballs (blobs orgánicos)
  MetaBalls metaballs;

  // Centro del dominio
  double cx = 256, cy = 256, cz = 256;

  // 1. Grupo Central (ligeramente separadas)
  metaballs.addSphere(cx - 40, cy, cz, 40); // Izquierda
  metaballs.addSphere(cx + 40, cy, cz, 40); // Derecha
  metaballs.addSphere(cx, cy + 50, cz, 35); // Arriba

  // 2. Satélite Lejano (Para probar desconexión)
  // Ponlo lejos, casi en el borde del dominio visible
  metaballs.addSphere(cx + 120, cy - 80, cz, 30);

  // 3. Puente largo (Para probar tunelización)
  // Colocado entre el centro y el satélite, pero pequeño
  metaballs.addSphere(cx + 60, cy - 40, cz, 25);

  // Esfera 3: MUY AL FRENTE (Z grande)
  // Esta esfera estará "cerca" de la cara frontal del cubo
  metaballs.addSphere(cx - 50, cy, cz + 100, 58.0);

  // Esfera 4: MUY AL FONDO (Z pequeño)
  // Esta esfera estará "lejos", casi en el fondo del cubo
  metaballs.addSphere(cx + 40, cy - 40, cz - 120, 70.0);

  // Esfera 5: Flotando en medio
  metaballs.addSphere(cx, cy - 80, cz + 20, 50.0);

  MarchingCubes mc(domain, delta, "metaballs.ply", &metaballs);

  // 6. Mandelbulb (fractal 3D - requiere delta pequeño y puede ser lento)
  // int delta_fractal = 4;
  // MandelbulbFunction mandelbulb(domain / 2.0, domain / 2.0, domain / 2.0, 8.0, 15, 2.0);
  // MarchingCubes mc(domain, delta_fractal, "mandelbulb.ply", &mandelbulb);

  // 7. Corazón 3D (versión mejorada más estable)
  // HeartFunction heart(domain / 2.0, domain / 2.0, domain / 2.0, 100.0);
  // MarchingCubes mc(domain, delta, "heart.ply", &heart);

  // 7b. Corazón 3D versión simple (más robusta, prueba esta si la anterior tiene artefactos)
  // HeartFunctionSimple heart_simple(domain / 2.0, domain / 2.0, domain / 2.0, 100.0);
  // MarchingCubes mc(domain, delta, "heart_simple.ply", &heart_simple);

  // 8. Función Híbrida Compleja (la más compleja - combina toro, esferas, cubo, gyroid, ondas)
  // ComplexHybridFunction complexShape(domain / 2.0, domain / 2.0, domain / 2.0, 0.5);
  // MarchingCubes mc(domain, delta, "complex_hybrid.ply", &complexShape);

  // ========== GENERAR Y EXPORTAR ==========

  mc.generateMesh();
  mc.exportPly();

  return 0;
}