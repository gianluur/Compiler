#include "./compiler.hpp"

int main(int argc, char* argv[]){
  auto start = std::chrono::high_resolution_clock::now();

  Compiler compiler(argc, argv);

  auto end = std::chrono::high_resolution_clock::now();

  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  double seconds = std::chrono::duration<double>(duration).count();
  cout << "Compiling took: " << seconds << " seconds\n";

  return 0;
}