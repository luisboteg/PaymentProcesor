cmake_minimum_required(VERSION 3.14)
project(WorldlineProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # Desactiva extensiones específicas del compilador para asegurar la portabilidad

# Buscar la biblioteca libpqxx
find_package(libpqxx REQUIRED)

# Crear una biblioteca para el código común
add_library(PaymentProcessorLib src/PaymentProcessor.cpp)
target_link_libraries(PaymentProcessorLib PRIVATE pqxx)

# Definir el ejecutable principal
add_executable(PaymentProcessor src/main.cpp)
target_link_libraries(PaymentProcessor PRIVATE PaymentProcessorLib)

# Habilitar pruebas
enable_testing()

# Buscar Google Test
find_package(GTest REQUIRED)

# Definir el ejecutable de pruebas
add_executable(TestPayment tests/test_payment.cpp)
target_link_libraries(TestPayment PRIVATE GTest::GTest GTest::Main PaymentProcessorLib)

# Añadir la prueba al sistema de pruebas de CMake
add_test(NAME TestPayment COMMAND TestPayment)