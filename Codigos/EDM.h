



// a continuacion el codigo: main.h



/**************************************************************
 * TRABAJO: Ray Tracing Cara 3D                               *
 * AUTOR: [ Juan David Delgado Burbano ]                      *
 * FECHA: [12/04/2025]                                        *
 * FUENTE del Codigo EDM.h : [Juan Carlos Giraldo Carvajal]   *
 * (este codigo se tomo de una monitoria acargo de            *
 *  Juan Carlos Giraldo Carvajal)                             *
 * DESCRIPCIÓN: Declaracion de operaciones vectoriales 3D     *
 **************************************************************/

/*
 * Archivo: EDM.h
 * Cabecera para operaciones con vectores 3D 
 * y estructuras para modelos STL  
 */


#ifndef EDM_H
#define EDM_H


// Estructura para representar un vector 3D
typedef struct {
  float x,y,z;
} VEC3_T;

// Estructura para representar un triángulo 3D
typedef struct {
    VEC3_T v0, v1, v2;    // Vértices del triángulo (cordendas)
    VEC3_T normal;         // Normal del triángulo (desde STL)
} TRIANGLE_T;

// Estructura para representar un modelo STL (compuesto por triangulos)
typedef struct {
    TRIANGLE_T* triangles; // Array de triángulos
    int count;             // Cantidad de triángulos
} STL_MODEL;

// Declaración de funciones para operaciones vectoriales 3D
VEC3_T VEC3_add(VEC3_T a, VEC3_T b);
VEC3_T VEC3_sub(VEC3_T a, VEC3_T b);
float VEC3_dot(VEC3_T a, VEC3_T b);
VEC3_T VEC3_cross(VEC3_T a, VEC3_T b);
VEC3_T VEC3_normalize(VEC3_T a);
VEC3_T VEC3_scale(VEC3_T a, float b);
VEC3_T VEC3_zero();
float VEC3_lenght(VEC3_T a);
VEC3_T VEC3_set(float x, float y, float z);
float VEC3_get_x(VEC3_T a);
float VEC3_get_y(VEC3_T a);
float VEC3_get_z(VEC3_T a);
float VEC3_get_R(VEC3_T a);
float VEC3_get_G(VEC3_T a);
float VEC3_get_B(VEC3_T a);
void VEC3_print(VEC3_T a);

#endif /* EDM_H */


