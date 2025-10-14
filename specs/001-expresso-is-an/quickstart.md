# Quickstart Guide

This guide provides instructions on how to build and run the Expresso evaluator from source.

## Prerequisites

- A C++17 compliant compiler (e.g., GCC, Clang, MSVC)
- CMake (version 3.10 or higher)
- ANTLR4 (complete JAR) - [Download here](https://www.antlr.org/download.html)

## Build Instructions

1.  **Clone the repository**:
    ```sh
    git clone <repository-url>
    cd <repository-directory>
    ```

2.  **Generate the ANTLR Parser**:
    - Download the ANTLR 4 complete JAR and place it in the root of this repository (e.g., `antlr-4.13.1-complete.jar`).
    - Run the following command from the root of the repository to generate the C++ parser code. Replace `antlr-4.x-complete.jar` with the actual filename of the JAR.
      ```sh
      java -jar antlr-4.x-complete.jar -Dlanguage=Cpp -o src/parser/generated -visitor -no-listener src/parser/Expresso.g4
      ```

3.  **Configure with CMake**:
    - Create a build directory:
      ```sh
      mkdir build
      cd build
      ```
    - Run CMake to configure the project:
      ```sh
      cmake ..
      ```

4.  **Compile the project**:
    - From within the `build` directory, run your build tool:
      ```sh
      make
      # or on Windows with Visual Studio
      cmake --build .
      ```

## Running Expresso

- After a successful build, the `expresso` executable will be located in the `build` directory.
- Run it to start the interactive session:
  ```sh
  ./expresso
  ```
