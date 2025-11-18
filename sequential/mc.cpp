#include "mc.h"

int main() {
  int domain = 512;
  int delta = 4;  
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
  vector<Point> centers = {
    Point(200, 256, 256),
    Point(312, 256, 256),
    Point(256, 200, 300),
    Point(256, 312, 300),
    Point(256, 256, 200)
  };
  vector<double> radii = {40.0, 35.0, 45.0, 38.0, 42.0};
  MetaballFunction metaballs(centers, radii, 1.5);
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