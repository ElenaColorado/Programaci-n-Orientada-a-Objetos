#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <limits>
#include <map>
using namespace std;

/* =========================
   MODELOS (Tablas en 3FN)
   ========================= */
struct Autor {
    int id; string nombre; string nacionalidad;
};
struct Libro {
    int id; string titulo; string isbn; int anio; int id_autor; bool disponible; // true=se puede prestar
};
struct Estudiante {
    int id; string nombre; string grado;
};
struct Prestamo {
    int id; int id_libro; int id_estudiante; string fecha_prestamo; string fecha_devolucion; // ""=activo
};


class BibliotecaDB {
public:
    vector<Autor> autores;
    vector<Libro> libros;
    vector<Estudiante> estudiantes;
    vector<Prestamo> prestamos;

    // nombres de archivos
    const string F_AUTOR      = "autores.txt";
    const string F_LIBRO      = "libros.txt";
    const string F_ESTUDIANTE = "estudiantes.txt";
    const string F_PRESTAMO   = "prestamos.txt";

    /* -------- helpers genéricos -------- */
    static vector<string> split(const string& s, char sep='|') {
        vector<string> out; string item; stringstream ss(s);
        while (getline(ss, item, sep)) out.push_back(item);
        return out;
    }
    static string join(const vector<string>& cols, char sep='|') {
        ostringstream os;
        for (size_t i=0;i<cols.size();++i) { if(i) os<<sep; os<<cols[i]; }
        return os.str();
    }
    static int safeStoi(const string& s, int def=-1) {
        try{ if(s.empty()) return def; size_t i=0; int v=stoi(s,&i); return v; }catch(...){ return def; }
    }

    /* ------------- CARGA ------------- */
    void cargarTodo() {
        autores.clear(); libros.clear(); estudiantes.clear(); prestamos.clear();
        string line;

        // Autores
        ifstream fa(F_AUTOR); 
        if (fa) while (getline(fa,line)) if(!line.empty()) {
            auto c=split(line);
            if (c.size()>=3) autores.push_back({ safeStoi(c[0]), c[1], c[2] });
        }

        // Libros
        ifstream fl(F_LIBRO);
        if (fl) while (getline(fl,line)) if(!line.empty()) {
            auto c=split(line);
            if (c.size()>=6) libros.push_back({ safeStoi(c[0]), c[1], c[2], safeStoi(c[3],0), safeStoi(c[4]), safeStoi(c[5],1)!=0 });
        }

        // Estudiantes
        ifstream fe(F_ESTUDIANTE);
        if (fe) while (getline(fe,line)) if(!line.empty()) {
            auto c=split(line);
            if (c.size()>=3) estudiantes.push_back({ safeStoi(c[0]), c[1], c[2] });
        }

        // Préstamos
        ifstream fp(F_PRESTAMO);
        if (fp) while (getline(fp,line)) if(!line.empty()) {
            auto c=split(line);
            if (c.size()>=5) prestamos.push_back({ safeStoi(c[0]), safeStoi(c[1]), safeStoi(c[2]), c[3], c[4] });
        }

        // Recalcular disponibilidad por préstamos activos
        for (auto& l : libros) l.disponible = true;
        for (const auto& p : prestamos) if (p.fecha_devolucion.empty()) {
            for (auto& l : libros) if (l.id==p.id_libro) { l.disponible=false; break; }
        }
    }

    /* ------------- GUARDADO ------------- */
    bool guardarTodo() {
        ofstream fa(F_AUTOR); if(!fa) return false;
        for (auto& a: autores) fa << join({to_string(a.id), a.nombre, a.nacionalidad}) << "\n";

        ofstream fl(F_LIBRO); if(!fl) return false;
        for (auto& l: libros) fl << join({to_string(l.id), l.titulo, l.isbn, to_string(l.anio), to_string(l.id_autor), to_string(l.disponible?1:0)}) << "\n";

        ofstream fe(F_ESTUDIANTE); if(!fe) return false;
        for (auto& e: estudiantes) fe << join({to_string(e.id), e.nombre, e.grado}) << "\n";

        ofstream fp(F_PRESTAMO); if(!fp) return false;
        for (auto& p: prestamos) fp << join({to_string(p.id), to_string(p.id_libro), to_string(p.id_estudiante), p.fecha_prestamo, p.fecha_devolucion}) << "\n";
        return true;
    }

    /* ---------- búsquedas y validaciones ---------- */
    Autor*       findAutor(int id){ for(auto& a:autores) if(a.id==id) return &a; return nullptr; }
    Libro*       findLibro(int id){ for(auto& l:libros) if(l.id==id) return &l; return nullptr; }
    Estudiante*  findEst(int id)  { for(auto& e:estudiantes) if(e.id==id) return &e; return nullptr; }
    Prestamo*    findPrest(int id){ for(auto& p:prestamos) if(p.id==id) return &p; return nullptr; }

    bool isbnExiste(const string& isbn, int ignorarLibro=-1) const {
        for (auto& l:libros) if (l.isbn==isbn && l.id!=ignorarLibro) return true;
        return false;
    }
    bool tienePrestamoActivo(int idLibro) const {
        for (auto& p:prestamos) if (p.id_libro==idLibro && p.fecha_devolucion.empty()) return true;
        return false;
    }

    /* ===================== CRUD AUTOR ===================== */
    bool crearAutor(const Autor& a, string& err){
        if (findAutor(a.id)) { err="ID autor duplicado."; return false; }
        autores.push_back(a); return true;
    }
    bool actualizarAutor(const Autor& a, string& err){
        for (auto& x: autores) if (x.id==a.id){ x=a; return true; }
        err="Autor no encontrado."; return false;
    }
    bool borrarAutor(int id, string& err){
        for (auto& l:libros) if (l.id_autor==id){ err="No borrar: autor con libros asociados."; return false; }
        auto it=remove_if(autores.begin(),autores.end(),[&](const Autor& x){return x.id==id;});
        if (it==autores.end()){ err="Autor no encontrado."; return false; }
        autores.erase(it,autores.end()); return true;
    }

    /* ===================== CRUD LIBRO ===================== */
    bool crearLibro(const Libro& l, string& err){
        if (findLibro(l.id)) { err="ID libro duplicado."; return false; }
        if (!findAutor(l.id_autor)) { err="Autor inexistente (FK)."; return false; }
        if (isbnExiste(l.isbn)) { err="ISBN duplicado."; return false; }
        libros.push_back(l); return true;
    }
    bool actualizarLibro(const Libro& l, string& err){
        if (!findAutor(l.id_autor)) { err="Autor inexistente (FK)."; return false; }
        if (isbnExiste(l.isbn, l.id)) { err="ISBN duplicado en otro libro."; return false; }
        for (auto& x:libros) if (x.id==l.id){ x=l; return true; }
        err="Libro no encontrado."; return false;
    }
    bool borrarLibro(int id, string& err){
        // bloquear si tiene historial de préstamos
        for (auto& p:prestamos) if (p.id_libro==id){ err="No borrar: libro con historial de préstamos."; return false; }
        auto it=remove_if(libros.begin(),libros.end(),[&](const Libro& x){return x.id==id;});
        if (it==libros.end()){ err="Libro no encontrado."; return false; }
        libros.erase(it,libros.end()); return true;
    }

    /* =================== CRUD ESTUDIANTE ================== */
    bool crearEstudiante(const Estudiante& e, string& err){
        if (findEst(e.id)) { err="ID estudiante duplicado."; return false; }
        estudiantes.push_back(e); return true;
    }
    bool actualizarEstudiante(const Estudiante& e, string& err){
        for (auto& x:estudiantes) if (x.id==e.id){ x=e; return true; }
        err="Estudiante no encontrado."; return false;
    }
    bool borrarEstudiante(int id, string& err){
        for (auto& p:prestamos) if (p.id_estudiante==id){ err="No borrar: estudiante con préstamos."; return false; }
        auto it=remove_if(estudiantes.begin(),estudiantes.end(),[&](const Estudiante& x){return x.id==id;});
        if (it==estudiantes.end()){ err="Estudiante no encontrado."; return false; }
        estudiantes.erase(it,estudiantes.end()); return true;
    }

    /* =================== PRÉSTAMOS =================== */
    bool crearPrestamo(Prestamo p, string& err){
        if (findPrest(p.id)) { err="ID préstamo duplicado."; return false; }
        Libro* l=findLibro(p.id_libro); if(!l){ err="Libro inexistente (FK)."; return false; }
        if (!findEst(p.id_estudiante)) { err="Estudiante inexistente (FK)."; return false; }
        if (!l->disponible || tienePrestamoActivo(p.id_libro)) { err="Libro no disponible."; return false; }
        p.fecha_devolucion=""; prestamos.push_back(p); l->disponible=false; return true;
    }
    bool devolverPrestamo(int id, const string& fechaDev, string& err){
        Prestamo* p=findPrest(id); if(!p){ err="Préstamo no encontrado."; return false; }
        if (!p->fecha_devolucion.empty()){ err="Ya estaba devuelto."; return false; }
        p->fecha_devolucion=fechaDev; if (auto* l=findLibro(p->id_libro)) l->disponible=true; return true;
    }
    bool borrarPrestamo(int id, string& err){
        Prestamo* p=findPrest(id); if(!p){ err="Préstamo no encontrado."; return false; }
        if (p->fecha_devolucion.empty()){ err="No borrar préstamos activos. Devuélvelo primero."; return false; }
        auto it=remove_if(prestamos.begin(),prestamos.end(),[&](const Prestamo& x){return x.id==id;});
        prestamos.erase(it, prestamos.end()); return true;
    }
    bool actualizarPrestamo(int id, int nuevoLibro, int nuevoEst, const string& nuevaFecha, string& err){
        Prestamo* p=findPrest(id); if(!p){ err="Préstamo no encontrado."; return false; }
        Libro* lNuevo=findLibro(nuevoLibro); if(!lNuevo){ err="Libro inexistente."; return false; }
        if (!findEst(nuevoEst)) { err="Estudiante inexistente."; return false; }

        // si sigue activo, liberar el libro anterior
        if (p->fecha_devolucion.empty()) {
            if (!lNuevo->disponible && lNuevo->id!=p->id_libro) { err="Nuevo libro no disponible."; return false; }
            if (Libro* lOld=findLibro(p->id_libro)) lOld->disponible=true;
            lNuevo->disponible=false;  // ocuparlo
        } else {
            // si estaba devuelto, lo reactivamos
            if (!lNuevo->disponible) { err="Nuevo libro no disponible."; return false; }
            lNuevo->disponible=false;
            p->fecha_devolucion.clear();
        }
        p->id_libro=nuevoLibro; p->id_estudiante=nuevoEst; p->fecha_prestamo=nuevaFecha;
        return true;
    }

    /* ============ Consultas (joins con bucles) ============ */
    void listarPrestamosActivos() const {
        cout << "Préstamos ACTIVOS:\n";
        for (auto& p : prestamos) if (p.fecha_devolucion.empty()) {
            const Libro* l=nullptr; const Estudiante* e=nullptr;
            for (auto& x : libros) if (x.id==p.id_libro) { l=&x; break; }
            for (auto& y : estudiantes) if (y.id==p.id_estudiante) { e=&y; break; }
            cout << " - #" << p.id << " | " << (l?l->titulo:"?") << " | " << (e?e->nombre:"?")
                 << " | desde " << p.fecha_prestamo << "\n";
        }
    }
    void librosPorEstudiante(int idEst) const {
        const Estudiante* e=nullptr; for(auto& x:estudiantes) if(x.id==idEst){e=&x;break;}
        if(!e){ cout<<"Estudiante no encontrado.\n"; return; }
        cout << "Préstamos de " << e->nombre << ":\n";
        for (auto& p:prestamos) if (p.id_estudiante==idEst) {
            const Libro* l=nullptr; for(auto& x:libros) if(x.id==p.id_libro){l=&x;break;}
            cout << " - " << (l?l->titulo:"?") << " | " << p.fecha_prestamo
                 << " | Devuelto: " << (p.fecha_devolucion.empty()? "NO":p.fecha_devolucion) << "\n";
        }
    }
    void topAutores(int n=5) const {
        map<int,int> c; for (auto& l:libros) c[l.id_autor]++;
        vector<pair<int,int>> v(c.begin(),c.end());
        sort(v.begin(),v.end(),[](auto&a,auto&b){return a.second>b.second;});
        cout << "TOP " << n << " autores por # libros:\n";
        int i=0; for (auto& [idA,cnt] : v) {
            const Autor* a=nullptr; for (auto& x:autores) if(x.id==idA){a=&x;break;}
            cout << " - " << (a? a->nombre:("Autor#"+to_string(idA))) << " ("<<cnt<<")\n";
            if (++i>=n) break;
        }
    }
};

/* ============== UI / Consola ============== */
int leerInt(const string& prompt){
    cout<<prompt; int x;
    while(!(cin>>x)){ cin.clear(); cin.ignore(numeric_limits<streamsize>::max(),'\n'); cout<<"Inválido. Intenta de nuevo: "; }
    cin.ignore(numeric_limits<streamsize>::max(),'\n'); return x;
}
string leerStr(const string& prompt){ cout<<prompt; string s; getline(cin,s); return s; }

void menuAutores(BibliotecaDB& db){
    while(true){
        cout << "\n--- AUTORES ---\n1.Crear  2.Listar  3.Actualizar  4.Borrar  0.Volver\n> ";
        int op=leerInt("");
        if(op==0)break;
        string err;
        if(op==1){ Autor a{}; a.id=leerInt("ID: "); a.nombre=leerStr("Nombre: "); a.nacionalidad=leerStr("Nacionalidad: ");
                   if(!db.crearAutor(a,err)) cout<<"Error: "<<err<<"\n"; }
        else if(op==2){ if(db.autores.empty()) cout<<"(vacío)\n"; for(auto& a:db.autores) cout<<"["<<a.id<<"] "<<a.nombre<<" ("<<a.nacionalidad<<")\n"; }
        else if(op==3){ Autor a{}; a.id=leerInt("ID a actualizar: "); a.nombre=leerStr("Nuevo nombre: "); a.nacionalidad=leerStr("Nueva nacionalidad: ");
                        if(!db.actualizarAutor(a,err)) cout<<"Error: "<<err<<"\n"; }
        else if(op==4){ int id=leerInt("ID a borrar: "); if(!db.borrarAutor(id,err)) cout<<"Error: "<<err<<"\n"; }
    }
}
void menuLibros(BibliotecaDB& db){
    while(true){
        cout << "\n--- LIBROS ---\n1.Crear  2.Listar  3.Actualizar  4.Borrar  0.Volver\n> ";
        int op=leerInt("");
        if(op==0)break;
        string err;
        if(op==1){ Libro l{}; l.id=leerInt("ID: "); l.titulo=leerStr("Titulo: "); l.isbn=leerStr("ISBN (UNIQUE): ");
                   l.anio=leerInt("Año: "); l.id_autor=leerInt("ID Autor (FK): "); l.disponible=true;
                   if(!db.crearLibro(l,err)) cout<<"Error: "<<err<<"\n"; }
        else if(op==2){ if(db.libros.empty()) cout<<"(vacío)\n"; 
                        for(auto& l:db.libros){ cout<<"["<<l.id<<"] "<<l.titulo<<" | ISBN "<<l.isbn<<" | Año "<<l.anio
                                                   <<" | AutorID "<<l.id_autor<<" | "<<(l.disponible?"Disponible":"Prestado")<<"\n"; } }
        else if(op==3){ Libro l{}; l.id=leerInt("ID a actualizar: "); l.titulo=leerStr("Nuevo titulo: "); l.isbn=leerStr("Nuevo ISBN: ");
                        l.anio=leerInt("Nuevo año: "); l.id_autor=leerInt("Nuevo ID Autor: "); l.disponible = db.findLibro(l.id)? db.findLibro(l.id)->disponible : true;
                        if(!db.actualizarLibro(l,err)) cout<<"Error: "<<err<<"\n"; }
        else if(op==4){ int id=leerInt("ID a borrar: "); if(!db.borrarLibro(id,err)) cout<<"Error: "<<err<<"\n"; }
    }
}
void menuEstudiantes(BibliotecaDB& db){
    while(true){
        cout << "\n--- ESTUDIANTES ---\n1.Crear  2.Listar  3.Actualizar  4.Borrar  0.Volver\n> ";
        int op=leerInt("");
        if(op==0)break;
        string err;
        if(op==1){ Estudiante e{}; e.id=leerInt("ID: "); e.nombre=leerStr("Nombre: "); e.grado=leerStr("Grado: ");
                   if(!db.crearEstudiante(e,err)) cout<<"Error: "<<err<<"\n"; }
        else if(op==2){ if(db.estudiantes.empty()) cout<<"(vacío)\n"; for(auto& e:db.estudiantes) cout<<"["<<e.id<<"] "<<e.nombre<<" | "<<e.grado<<"\n"; }
        else if(op==3){ Estudiante e{}; e.id=leerInt("ID a actualizar: "); e.nombre=leerStr("Nuevo nombre: "); e.grado=leerStr("Nuevo grado: ");
                        if(!db.actualizarEstudiante(e,err)) cout<<"Error: "<<err<<"\n"; }
        else if(op==4){ int id=leerInt("ID a borrar: "); if(!db.borrarEstudiante(id,err)) cout<<"Error: "<<err<<"\n"; }
    }
}
void menuPrestamos(BibliotecaDB& db){
    while(true){
        cout << "\n--- PRÉSTAMOS ---\n1.Crear  2.Devolver  3.Activos  4.Actualizar  5.Borrar (devueltos)  0.Volver\n> ";
        int op=leerInt("");
        if(op==0)break;
        string err;
        if(op==1){ Prestamo p{}; p.id=leerInt("ID préstamo: "); p.id_libro=leerInt("ID libro: ");
                   p.id_estudiante=leerInt("ID estudiante: "); p.fecha_prestamo=leerStr("Fecha (YYYY-MM-DD): ");
                   if(!db.crearPrestamo(p,err)) cout<<"Error: "<<err<<"\n"; }
        else if(op==2){ int id=leerInt("ID préstamo a devolver: "); string f=leerStr("Fecha devolución (YYYY-MM-DD): ");
                        if(!db.devolverPrestamo(id,f,err)) cout<<"Error: "<<err<<"\n"; }
        else if(op==3){ db.listarPrestamosActivos(); }
        else if(op==4){ int id=leerInt("ID préstamo a actualizar: "); int nl=leerInt("Nuevo ID libro: ");
                        int ne=leerInt("Nuevo ID estudiante: "); string fp=leerStr("Nueva fecha préstamo: ");
                        if(!db.actualizarPrestamo(id,nl,ne,fp,err)) cout<<"Error: "<<err<<"\n"; }
        else if(op==5){ int id=leerInt("ID préstamo a borrar (debe estar devuelto): ");
                        if(!db.borrarPrestamo(id,err)) cout<<"Error: "<<err<<"\n"; }
    }
}
void menuConsultas(BibliotecaDB& db){
    while(true){
        cout << "\n--- CONSULTAS ---\n1.Libros por estudiante  2.TOP autores  0.Volver\n> ";
        int op=leerInt("");
        if(op==0)break;
        if(op==1){ int id=leerInt("ID estudiante: "); db.librosPorEstudiante(id); }
        else if(op==2){ int n=leerInt("¿Cuántos mostrar?: "); db.topAutores(n); }
    }
}

int main(){
    ios::sync_with_stdio(false); cin.tie(nullptr);
    BibliotecaDB db; db.cargarTodo();

    while(true){
        cout << "\n===== BIBLIOTECA (3FN) =====\n"
             << "1.Autores  2.Libros  3.Estudiantes  4.Préstamos  5.Consultas  6.Guardar  0.Salir\n> ";
        int op=0; if(!(cin>>op)){cin.clear();cin.ignore(numeric_limits<streamsize>::max(),'\n'); continue;}
        cin.ignore(numeric_limits<streamsize>::max(),'\n');

        if(op==0) break;
        else if(op==1) menuAutores(db);
        else if(op==2) menuLibros(db);
        else if(op==3) menuEstudiantes(db);
        else if(op==4) menuPrestamos(db);
        else if(op==5) menuConsultas(db);
        else if(op==6) cout << (db.guardarTodo() ? "Datos guardados.\n" : "Error al guardar.\n");
    }

    db.guardarTodo(); // seguridad
    cout << "¡Hasta luego!\n";
    return 0;
}
