#include "mc.h"

int main(int argc, char **argv) {
  if (argc == 2 && string(argv[1]) == "help") {
    cout << "Usage: " << argv[0] << " <domain> <delta> <function_name>" << endl;
    cout << "Available functions:" << endl;
    cout << "  sphere" << endl;
    cout << "  torus" << endl;
    cout << "  rounded_cube" << endl;
    cout << "  gyroid" << endl;
    cout << "  metaballs" << endl;
    return 0;
  }

  if (argc != 4) {
    cout << "Invalid arguments. Use '" << argv[0] << " help' for usage information." << endl;
    return 1;
  }

  int domain = stoi(argv[1]);
  int delta = stoi(argv[2]);
  string function_name = argv[3];
  string filename = function_name;

  ImplicitFunction *func = nullptr;
  Sphere *sphere = nullptr;
  TorusFunction *torus = nullptr;
  RoundedCubeFunction *roundedCube = nullptr;
  GyroidFunction *gyroid = nullptr;
  MetaBalls *metaballs = nullptr;

  // Factor de escala global basado en un dominio de referencia de 512
  double s = domain / 512.0;

  if (function_name == "sphere") {
    sphere = new Sphere(domain / 2.0, domain / 2.0, domain / 2.0, domain * (100.0 / 256.0));
    func = sphere;
  } else if (function_name == "torus") {
    torus = new TorusFunction(domain / 2.0, domain / 2.0, domain / 2.0, 70.0 * s, 20.0 * s);
    func = torus;
  } else if (function_name == "rounded_cube") {
    roundedCube = new RoundedCubeFunction(domain / 2.0, domain / 2.0, domain / 2.0, 150.0 * s, 15.0 * s);
    func = roundedCube;
  } else if (function_name == "gyroid") {
    gyroid = new GyroidFunction(domain / 2.0, domain / 2.0, domain / 2.0, 0.06 / s, 0.5);
    func = gyroid;
  } else if (function_name == "metaballs") {
    metaballs = new MetaBalls();
    double cx = domain / 2.0, cy = domain / 2.0, cz = domain / 2.0;
    metaballs->addSphere(cx - 120 * s, cy, cz, 70 * s);
    metaballs->addSphere(cx + 130 * s, cy, cz, 65 * s);
    metaballs->addSphere(cx, cy + 110 * s, cz, 75 * s);
    metaballs->addSphere(cx + 90 * s, cy - 80 * s, cz, 55 * s);
    metaballs->addSphere(cx + 50 * s, cy - 50 * s, cz, 50 * s);
    metaballs->addSphere(cx - 50 * s, cy, cz + 110 * s, 70.0 * s);
    metaballs->addSphere(cx + 30 * s, cy - 30 * s, cz - 110 * s, 80.0 * s);
    metaballs->addSphere(cx, cy - 60 * s, cz + 15 * s, 65.0 * s);
    func = metaballs;
  } else {
    cout << "Unknown function: " << function_name << endl;
    return 1;
  }

  MarchingCubes mc(domain, delta, filename, func);
  mc.generateMesh();
  mc.exportPly();

  if (sphere)
    delete sphere;
  if (torus)
    delete torus;
  if (roundedCube)
    delete roundedCube;
  if (gyroid)
    delete gyroid;
  if (metaballs)
    delete metaballs;

  return 0;
}