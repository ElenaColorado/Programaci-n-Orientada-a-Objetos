#include <iostream>
#include <string>
#include <vector>

// --- Definición de la clase Libro (mejorada) ---
class Libro {
public:
    std::string titulo;
    std::string autor;
    int anioPublicacion;
    bool estaDisponible; // Nuevo atributo

    // Constructor para inicializar los valores fácilmente
    Libro() : anioPublicacion(0), estaDisponible(true) {}

    // Método para mostrar todos los detalles del libro
    void mostrarDetallesCompletos() {
        std::cout << "-----------------------------\n";
        std::cout << "Título: " << titulo << "\n";
        std::cout << "Autor: " << autor << "\n";
        std::cout << "Año de publicación: " << anioPublicacion << "\n";
        std::cout << "Disponibilidad: " << (estaDisponible ? "Disponible" : "Prestado") << "\n";
    }
};

// --- Definición de la clase Biblioteca ---
class Biblioteca {
private:
    std::vector<Libro> coleccion;

public:
    // Añadir un libro (con validación para evitar duplicados)
    void agregarLibro(const Libro& nuevoLibro) {
        for (const auto& libro : coleccion) {
            if (libro.titulo == nuevoLibro.titulo) {
                std::cout << "Ya existe un libro con ese título.\n";
                return;
            }
        }
        coleccion.push_back(nuevoLibro);
        std::cout << "Libro agregado exitosamente.\n";
    }

    // Mostrar todos los libros del inventario
    void mostrarInventario() {
        if (coleccion.empty()) {
            std::cout << "La biblioteca está vacía.\n";
            return;
        }
        for (const auto& libro : coleccion) {
            libro.mostrarDetallesCompletos();
        }
    }

    // Buscar un libro por título
    Libro* buscarLibro(const std::string& tituloBuscado) {
        for (auto& libro : coleccion) {
            if (libro.titulo == tituloBuscado) {
                return &libro;
            }
        }
        return nullptr;
    }

    // Prestar un libro
    void prestarLibro(const std::string& tituloPrestamo) {
        Libro* libro = buscarLibro(tituloPrestamo);
        if (libro == nullptr) {
            std::cout << "Libro no encontrado.\n";
        } else if (!libro->estaDisponible) {
            std::cout << "El libro ya está prestado.\n";
        } else {
            libro->estaDisponible = false;
            std::cout << "Libro prestado exitosamente.\n";
        }
    }

    // Devolver un libro
    void devolverLibro(const std::string& tituloDevolucion) {
        Libro* libro = buscarLibro(tituloDevolucion);
        if (libro == nullptr) {
            std::cout << "Libro no encontrado.\n";
        } else if (libro->estaDisponible) {
            std::cout << "Ese libro ya estaba disponible.\n";
        } else {
            libro->estaDisponible = true;
            std::cout << "Libro devuelto exitosamente.\n";
        }
    }
};

// --- Función Principal con el Menú ---
int main() {
    Biblioteca miBiblioteca;
    int opcion = 0;

    // Agregar algunos libros de ejemplo para empezar
    Libro libroInicial;
    libroInicial.titulo = "El Hobbit";
    libroInicial.autor = "J.R.R. Tolkien";
    libroInicial.anioPublicacion = 1937;
    miBiblioteca.agregarLibro(libroInicial);

    while (opcion != 5) {
        std::cout << "\n--- BIBLIOTECA DIGITAL ---" << std::endl;
        std::cout << "1. Añadir libro" << std::endl;
        std::cout << "2. Mostrar inventario" << std::endl;
        std::cout << "3. Prestar libro" << std::endl;
        std::cout << "4. Devolver libro" << std::endl;
        std::cout << "5. Salir" << std::endl;
        std::cout << "Seleccione una opción: ";
        std::cin >> opcion;

        // Limpiar el buffer de entrada para futuras lecturas de texto
        std::cin.ignore();

        if (opcion == 1) {
            Libro nuevoLibro;
            std::cout << "Ingrese título del libro: ";
            std::getline(std::cin, nuevoLibro.titulo);
            std::cout << "Ingrese autor del libro: ";
            std::getline(std::cin, nuevoLibro.autor);
            std::cout << "Ingrese año de publicación: ";
            std::cin >> nuevoLibro.anioPublicacion;
            std::cin.ignore();
            miBiblioteca.agregarLibro(nuevoLibro);
        }
        else if (opcion == 2) {
            miBiblioteca.mostrarInventario();
        }
        else if (opcion == 3) {
            std::string titulo;
            std::cout << "Ingrese el título del libro a prestar: ";
            std::getline(std::cin, titulo);
            miBiblioteca.prestarLibro(titulo);
        }
        else if (opcion == 4) {
            std::string titulo;
            std::cout << "Ingrese el título del libro a devolver: ";
            std::getline(std::cin, titulo);
            miBiblioteca.devolverLibro(titulo);
        }
        else if (opcion == 5) {
            std::cout << "¡Hasta luego!" << std::endl;
        }
        else {
            std::cout << "Opción no válida. Intente nuevamente." << std::endl;
        }
    }

    return 0;
}
