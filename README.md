#### Sistema de Gestión de Biblioteca Escolar
Proyecto en C++ que implementa un sistema de biblioteca escolar con un modelo relacional simplificado.
Se utilizan struct + vector<> para simular tablas en memoria y archivos .txt (CSV simple con |) para persistencia.

## Entidades gestionadas
- Autor (id, nombre, nacionalidad)
- Libro (id, título, ISBN único, año, id_autor (FK), disponible)
- Estudiante (id, nombre, grado)
- Préstamo (id, id_libro (FK), id_estudiante (FK), fecha_prestamo, fecha_devolucion)

El diseño corresponde a tu modelo en 3FN: sin datos repetidos y con relaciones mediante IDs.

### Archivos de datos (en el mismo directorio)
- autores.txt
- libros.txt
- estudiantes.txt
- prestamos.txt

Formato CSV con | como separador.
Los archivos pueden comenzar vacíos; el programa los crea/actualiza al Guardar o al Salir.

## Requisitos
- Compilador C++ con C++17 (recomendado: g++)
- Sin librerías externas (solo <iostream>, <vector>, <string>, <fstream>, <sstream>, <algorithm>, <limits>)

# Compilar y ejecutar
  # Compilar
  g++ -std=c++17 Reto3_Biblioteca.cpp -o Reto3.exe

  # Ejecutar (Windows)
  Reto3.exe

  # Ejecutar (Linux/Mac)
  ./Reto3.exe

Ejecuta el binario en la misma carpeta donde están los .txt.

# Funcionalidades (por módulo)

  # Autores
    - Crear, listar, actualizar, eliminar.
    - Regla de integridad: no se permite eliminar un autor que todavía tiene libros asociados.


  # Libros
    - Crear, listar, actualizar, eliminar.
    - ISBN único (bloquea duplicados).
    - FK Autor (verifica existencia del autor).
    - Disponibilidad: Disponible / Prestado (se actualiza con los préstamos).
    - Regla (según tu implementación): no se elimina si existe historial de préstamos.

  # Estudiantes
    - Crear, listar, actualizar, eliminar.
    - Regla: no se elimina si tiene préstamos (activos o registrados).

  # Préstamos
    - Crear solo si el libro está disponible (bloquea segundo préstamo activo del mismo libro).
    - Devolver (pone el libro en Disponible).
    - Listar préstamos activos / históricos.
    - Actualizar (valida FKs y disponibilidad).
    - Eliminar solo si ya fue devuelto (no permite borrar préstamos activos).

# Consultas
- Libros prestados por estudiante (simula JOIN con bucles).
-  Top autores por cantidad de libros.
-  Buscar libro por ISBN.

# Formato de los archivos (CSV |)
- autores.txt: id|nombre|nacionalidad
- libros.txt: id|titulo|isbn|anio|id_autor|disponible(1/0)
- estudiantes.txt: id|nombre|grado
- prestamos.txt: id|id_libro|id_estudiante|fecha_prestamo|fecha_devolucion
  
Evita el carácter | dentro de los textos. Si lo necesitas, cambia el separador en el código.

# Ejemplo de uso (flujo rápido)
- Crea un Autor, un Estudiante y un Libro (con ISBN único).
- Crea un Préstamo del libro. Si intentas volver a prestarlo sin devolver, el sistema bloquea por no disponible.
- Devuelve el préstamo: el libro queda Disponible de nuevo.


Intenta crear un Libro con ISBN duplicado: el sistema bloquea.


Intenta borrar un Autor con libros o un Libro con historial: el sistema bloquea según las reglas.
