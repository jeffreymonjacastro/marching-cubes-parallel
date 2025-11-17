#include "mc.h"

int main() {
  MarchingCubes mc;
  
  // Ejemplo 1: Esfera con centro en (128, 128, 128) y radio 50
  SphereFunction sphere(128, 128, 128, 50);
  
  // Ejemplo 2: Toro con centro en (128, 128, 128), radio mayor 60, radio menor 20
  TorusFunction torus(128, 128, 128, 60, 20);
  
  // Ejemplo 3: Cubo redondeado
  RoundedCubeFunction rounded_cube(128, 128, 128, 80, 10);
  
  // Usar la función con la clase
  int case_index = mc.generate_case(100, 100, 100, 10, &sphere);
  std::cout << "Case index para esfera: " << case_index << std::endl;
  
  case_index = mc.generate_case(100, 100, 100, 10, &torus);
  std::cout << "Case index para toro: " << case_index << std::endl;
  
  case_index = mc.generate_case(100, 100, 100, 10, &rounded_cube);
  std::cout << "Case index para cubo redondeado: " << case_index << std::endl;
  
  return 0;
}