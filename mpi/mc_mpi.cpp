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

  // Cartesian Grid Setup
  int dims[3] = {0, 0, 0};
  MPI_Dims_create(size, 3, dims);

  int periods[3] = {0, 0, 0};
  MPI_Comm cart_comm;
  MPI_Cart_create(MPI_COMM_WORLD, 3, dims, periods, 1, &cart_comm);

  int coords[3];
  MPI_Cart_coords(cart_comm, rank, 3, coords);

  int global_divs = domain / delta;

  // Domain Decomposition
  int base_x = global_divs / dims[0]; int extra_x = global_divs % dims[0];
  int local_start_i = coords[0] * base_x + min(coords[0], extra_x);
  int local_end_i = local_start_i + base_x + (coords[0] < extra_x ? 1 : 0);

  int base_y = global_divs / dims[1]; int extra_y = global_divs % dims[1];
  int local_start_j = coords[1] * base_y + min(coords[1], extra_y);
  int local_end_j = local_start_j + base_y + (coords[1] < extra_y ? 1 : 0);

  int base_z = global_divs / dims[2]; int extra_z = global_divs % dims[2];
  int local_start_k = coords[2] * base_z + min(coords[2], extra_z);
  int local_end_k = local_start_k + base_z + (coords[2] < extra_z ? 1 : 0);

  // Overlap for stitching
  if (coords[0] > 0) local_start_i--;
  if (coords[1] > 0) local_start_j--;
  if (coords[2] > 0) local_start_k--;

  MPI_Barrier(cart_comm);
  double start_time = MPI_Wtime();

  MarchingCubes mc(domain, delta, filename, func);
  mc.generateLocalMesh(local_start_i, local_end_i, local_start_j, local_end_j, local_start_k, local_end_k);

  // Gather Triangles
  MPI_Datatype triangle_type;
  MPI_Type_contiguous(sizeof(Triangle) / sizeof(double), MPI_DOUBLE, &triangle_type);
  MPI_Type_commit(&triangle_type);

  vector<Triangle>& local_triangles = mc.getTriangles();
  int local_triangle_count = local_triangles.size();
  vector<int> recv_counts;

  if (rank == 0) recv_counts.resize(size);

  MPI_Gather(&local_triangle_count, 1, MPI_INT, recv_counts.data(), 1, MPI_INT, 0, cart_comm);

  vector<Triangle> all_triangles;
  vector<int> displacements;

  if (rank == 0) {
    displacements.resize(size);
    displacements[0] = 0;
    int total_triangles = 0;
    if (!recv_counts.empty()) {
      total_triangles = recv_counts[0];
      for (int i = 1; i < size; ++i) {
        total_triangles += recv_counts[i];
        displacements[i] = displacements[i - 1] + recv_counts[i - 1];
      }
    }
    all_triangles.resize(total_triangles);
  }

  MPI_Gatherv(local_triangles.data(), local_triangle_count, triangle_type,
              all_triangles.data(), recv_counts.data(), displacements.data(),
              triangle_type, 0, cart_comm);

  if (rank == 0) {
    mc.setTriangles(all_triangles);
    mc.exportPly();
  }

  MPI_Barrier(cart_comm);
  double end_time = MPI_Wtime();

  if (rank == 0) {
    cout << "Total execution time: " << (end_time - start_time) << " seconds." << endl;
    cout << "Total triangles: " << all_triangles.size() << endl;
  }

  MPI_Type_free(&triangle_type);
  MPI_Comm_free(&cart_comm);

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