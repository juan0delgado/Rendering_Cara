



// a continuacion el codigo: EDM.c

/**************************************************************
 * TRABAJO: Ray Tracing Cara 3D                               *
 * AUTOR: [ Juan David Delgado Burbano]                       *
 * FECHA: [12/04/2025]                                        *
 * FUENTE del Codigo EDM.c : [Juan Carlos Giraldo Carvajal]   *
 * (este codigo se tomo de una monitoria acargo de            *
 *  Juan Carlos Giraldo Carvajal)                             *
 * DESCRIPCIÓN: Implementación de operaciones vectoriales 3D  *
 **************************************************************/

/*
 * Archivo: EDM.c
 * Contiene las implementaciones de operaciones básicas con vectores 3D
 * utilizadas para el proyecto de Ray Tracing.
 */



#include <stdio.h>
#include <math.h>
#include "EDM.h"


// VEC3_add: Suma dos vectores componente a componente
VEC3_T VEC3_add(VEC3_T a, VEC3_T b) {
  return (VEC3_T){a.x + b.x, a.y + b.y, a.z + b.z};
}


// VEC3_sub: Resta dos vectores componente a componente
VEC3_T VEC3_sub(VEC3_T a, VEC3_T b) {
  return (VEC3_T){a.x - b.x, a.y - b.y, a.z - b.z};
}


// VEC3_dot: Calcula el producto punto entre dos vectores
float VEC3_dot(VEC3_T a, VEC3_T b) {
  return a.x*b.x + a.y*b.y + a.z*b.z;
}


// VEC3_cross: Calcula el producto cruz entre dos vectores
VEC3_T VEC3_cross(VEC3_T a, VEC3_T b) {
  return (VEC3_T){a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z,a.x*b.y - a.y*b.x};
}


// VEC3_normalize: Normaliza un vector (lo convierte en unitario)
VEC3_T VEC3_normalize(VEC3_T a) {
  float len = VEC3_lenght(a);
  return (len > 0) ? VEC3_scale(a, 1/len) : VEC3_zero();
}


// VEC3_scale: Escala un vector por un valor escalar
VEC3_T VEC3_scale(VEC3_T a, float b) {
  return (VEC3_T){a.x*b, a.y*b, a.z*b};
}


// VEC3_zero: Retorna un vector cero
VEC3_T VEC3_zero() {
  return (VEC3_T){0,0,0};
}


// VEC3_lenght: Calcula la magnitud (longitud) de un vector
float VEC3_lenght(VEC3_T a) {
  return sqrt(VEC3_dot(a,a));
}

// VEC3_set: Crea un vector con los componentes especificados

VEC3_T VEC3_set(float x, float y, float z) {
  return (VEC3_T){x,y,z};
}


// Funciones de acceso a componentes individuales (VEC3_get_x, VEC3_get_y, VEC3_get_z) Retorna: Componente respectiva del vector


float VEC3_get_x(VEC3_T a) {
  return a.x;
}

float VEC3_get_y(VEC3_T a) {
  return a.y;
}

float VEC3_get_z(VEC3_T a) {
  return a.z;
}


//  * Funciones para acceso a componentes como colores RGB (VEC3_get_R, VEC3_get_G, VEC3_get_B) Parámetros son a = Vector de entrada (interpretado como color)  Retorna: Componente de color respectiva

float VEC3_get_R(VEC3_T a) {
  return a.x;
}

float VEC3_get_G(VEC3_T a) {
  return a.y;
}


float VEC3_get_B(VEC3_T a) {
  return a.z;
}


// VEC3_print: Imprime un vector en formato (x, y, z)
void VEC3_print(VEC3_T a) {
  printf("(%f, %f, %f)\n", a.x, a.y, a.z);
}


