#include "mc_mpi.h"

int main(int argc, char **argv) {
  // MPI Initialization
  int rank, size;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (rank == 0) {
    cout << "Number of MPI processes: " << size << endl;
  }

  if (argc == 2 && string(argv[1]) == "help") {
    if (rank == 0) {
      cout << "Usage: " << argv[0] << " <domain> <delta> <function_name>" << endl;
      cout << "Available functions:" << endl;
      cout << "  sphere" << endl;
      cout << "  torus" << endl;
      cout << "  rounded_cube" << endl;
      cout << "  gyroid" << endl;
      cout << "  metaballs" << endl;
    }
    MPI_Finalize();
    return 0;
  }

  if (argc != 4) {
    if (rank == 0)
      cout << "Invalid arguments. Use '" << argv[0] << " help' for usage information." << endl;

    MPI_Finalize();
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

  if (function_name == "sphere") {
    sphere = new Sphere(domain / 2.0, domain / 2.0, domain / 2.0, domain * (100.0 / 256.0));
    func = sphere;
  } else if (function_name == "torus") {
    torus = new TorusFunction(domain / 2.0, domain / 2.0, domain / 2.0, 70.0, 20.0);
    func = torus;
  } else if (function_name == "rounded_cube") {
    roundedCube = new RoundedCubeFunction(domain / 2.0, domain / 2.0, domain / 2.0, 150.0, 15.0);
    func = roundedCube;
  } else if (function_name == "gyroid") {
    gyroid = new GyroidFunction(domain / 2.0, domain / 2.0, domain / 2.0, 0.06, 0.5);
    func = gyroid;
  } else if (function_name == "metaballs") {
    metaballs = new MetaBalls();
    double cx = domain / 2.0, cy = domain / 2.0, cz = domain / 2.0;
    metaballs->addSphere(cx - 120, cy, cz, 70);
    metaballs->addSphere(cx + 130, cy, cz, 65);
    metaballs->addSphere(cx, cy + 110, cz, 75);
    metaballs->addSphere(cx + 90, cy - 80, cz, 55);
    metaballs->addSphere(cx + 50, cy - 50, cz, 50);
    metaballs->addSphere(cx - 50, cy, cz + 110, 70.0);
    metaballs->addSphere(cx + 30, cy - 30, cz - 110, 80.0);
    metaballs->addSphere(cx, cy - 60, cz + 15, 65.0);
    func = metaballs;
  } else {
    cout << "Unknown function: " << function_name << endl;
    return 1;
  }

  double start_time = MPI_Wtime();

  MarchingCubes mc(domain, delta, filename, func);

  double end_time = MPI_Wtime();

  if (rank == 0) {
    cout << "Total execution time: " << (end_time - start_time) << " seconds." << endl;
  }

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

  MPI_Finalize();
  return 0;
}