#include "mc.h"

int main() {
  int domain = 512;
  int delta = 8;
  string filename = "rounded_cube.ply";

  // Crear una instancia de la función implícita deseada
  // Sphere sphere(domain / 2.0, domain / 2.0, domain / 2.0, domain * (100.0 / 256.0));
  // TorusFunction torus(domain / 2.0, domain / 2.0, domain / 2.0, 70.0, 20.0);
  RoundedCubeFunction roundedCube(domain / 2.0, domain / 2.0, domain / 2.0, 150.0, 15.0);

  // MarchingCubes mc(domain, delta, filename, &sphere);
  // MarchingCubes mc(domain, delta, filename, &torus);
  MarchingCubes mc(domain, delta, filename, &roundedCube);

  mc.generateMesh();
  mc.exportPly();

  return 0;
}