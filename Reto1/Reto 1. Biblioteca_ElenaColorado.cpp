#include <iostream>
#include <string>
#include <vector>

class Libro {
public:
    std::string titulo;
    std::string autor;
    int anioPublicacion;
    bool estaDisponible; 

    Libro() : anioPublicacion(0), estaDisponible(true) {}

    void mostrarDetallesCompletos() {
        std::cout << "-----------------------------" << std::endl;
        std::cout << "Título: " << titulo << std::endl;
        std::cout << "Autor: " << autor << std::endl;
        std::cout << "Año de publicación: " << anioPublicacion << std::endl;
        std::cout << "Disponibilidad: " << (estaDisponible ? "Disponible" : "Prestado") << std::endl;
    }
};

class Biblioteca {
private:
    std::vector<Libro> coleccion;

public:
    void agregarLibro(const Libro& nuevoLibro) {
        for (const auto& libro : coleccion) {
            if (libro.titulo == nuevoLibro.titulo) {
                std::cout << "Ya existe un libro con ese título." << std::endl;
                return;
            }
        }
        coleccion.push_back(nuevoLibro);
        std::cout << "Libro agregado." << std::endl;
    }

    void mostrarInventario() {
        if (coleccion.empty()) {
            std::cout << "La biblioteca está vacía." << std::endl;
            return;
        }
        for (const auto& libro : coleccion) {
            libro.mostrarDetallesCompletos();
        }
    }

    Libro* buscarLibro(const std::string& tituloBuscado) {
        for (auto& libro : coleccion) {
            if (libro.titulo == tituloBuscado) {
                return &libro;
            }
        }
        return nullptr;
    }

    void prestarLibro(const std::string& tituloPrestamo) {
        Libro* libro = buscarLibro(tituloPrestamo);
        if (libro == nullptr) {
            std::cout << "Libro no encontrado." << std::endl;
        } else if (!libro->estaDisponible) {
            std::cout << "El libro ya está prestado." << std::endl;
        } else {
            libro->estaDisponible = false;
            std::cout << "Libro prestado." << std::endl;
        }
    }

    void devolverLibro(const std::string& tituloDevolucion) {
        Libro* libro = buscarLibro(tituloDevolucion);
        if (libro == nullptr) {
            std::cout << "Libro no encontrado." << std::endl;
        } else if (libro->estaDisponible) {
            std::cout << "Ese libro ya estaba disponible." << std::endl;
        } else {
            libro->estaDisponible = true;
            std::cout << "Libro devuelto." << std::endl;
        }
    }
};

int main() {
    Biblioteca miBiblioteca;
    int opcion = 0;

    Libro libroInicial;
    libroInicial.titulo = "El Hobbit";
    libroInicial.autor = "J.R.R. Tolkien";
    libroInicial.anioPublicacion = 1937;
    miBiblioteca.agregarLibro(libroInicial);

    while (opcion != 5) {
        std::cout << "--- BIBLIOTECA DIGITAL ---" << std::endl;
        std::cout << "1. Añadir libro" << std::endl;
        std::cout << "2. Mostrar inventario" << std::endl;
        std::cout << "3. Prestar libro" << std::endl;
        std::cout << "4. Devolver libro" << std::endl;
        std::cout << "5. Salir" << std::endl;
        std::cout << "Selecciona una opción: ";
        std::cin >> opcion;

        std::cin.ignore();

        if (opcion == 1) {
            Libro nuevoLibro;
            std::cout << "Ingresa el título del libro: ";
            std::getline(std::cin, nuevoLibro.titulo);
            std::cout << "Ingresa el autor del libro: ";
            std::getline(std::cin, nuevoLibro.autor);
            std::cout << "Ingresa el año de publicación: ";
            std::cin >> nuevoLibro.anioPublicacion;
            std::cin.ignore();
            miBiblioteca.agregarLibro(nuevoLibro);
        }
        else if (opcion == 2) {
            miBiblioteca.mostrarInventario();
        }
        else if (opcion == 3) {
            std::string titulo;
            std::cout << "Ingrese el título del libro que quiere prestar: ";
            std::getline(std::cin, titulo);
            miBiblioteca.prestarLibro(titulo);
        }
        else if (opcion == 4) {
            std::string titulo;
            std::cout << "Ingrese el título del libro que va a devolver: ";
            std::getline(std::cin, titulo);
            miBiblioteca.devolverLibro(titulo);
        }
        else if (opcion == 5) {
            std::cout << "¡Adios!" << std::endl;
        }
        else {
            std::cout << "La opción no es válida. Intenta de nuevo." << std::endl;
        }
    }

    return 0;
}
