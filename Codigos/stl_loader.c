
// A continuacion el codigo: stl_loader.c



/**************************************************************
 * TRABAJO: Ray Tracing Cara 3D                               *
 * AUTOR: [ Juan David Delgado Burbano ]                      *
 * FECHA: [12/04/2025]                                        *
 * AUTOR CODIGO:   [ Juan David Delgado Burbano ]             *                  
 * REFERENCIAS:                                               *
 *  Write an STL (ASCII) File in PYTHON ||TUTORIAL            *
 *  https://www.youtube.com/watch?v=5mspBMmyqqU               *
 * Codigo (leer_stl_binario de [Juan Carlos Giraldo Carvajal])*
 **************************************************************/

/**************************************************************
 * FUNCIÓN: load_stl_ascii                                    *
 * DESCRIPCIÓN: Carga un modelo 3D desde un archivo STL ASCII *
 * PARÁMETROS:                                                *
 *   - filename = Ruta del archivo STL a cargar                *
 *   - model = Puntero a estructura STL_MODEL donde almacenar  *
 * RETORNO: 1 si éxito, 0 si falló                            *
 **************************************************************/





#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "EDM.h"
#include <math.h> 




int load_stl_ascii(const char* filename, STL_MODEL* model) {

    // 1. Apertura del archivo
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error al abrir archivo STL");
        return 0;
    }

    // 2. Inicialización de variables
    char line[256]; // Buffer para leer cada línea
    int triangle_capacity = 10000; // Capacidad inicial mayor
    int triangle_count = 0 // Contador de triángulos cargados
    int line_number = 0; // numero de linea 
    int error_count = 0;


    // 3. Reserva inicial de memoria (arreglo dinamico para triángulos)
    TRIANGLE_T* triangles = malloc(triangle_capacity * sizeof(TRIANGLE_T));
    if (!triangles) {
        fclose(file);
        fprintf(stderr, "Error de memoria\n");
        return 0;
    }

    printf("Cargando modelo STL...\n");


    // 4. Procesamiento línea por línea
    while (fgets(line, sizeof(line), file)) {
        line_number++;

        // 5. Detección de triángulos (comienza con "facet normal")
        if (strstr(line, "facet normal")) {
            TRIANGLE_T tri;  // Triángulo temporal
            int vertices_read = 0;  // Vértices leídos para este triángulo

            // Leer normal triangulo
            if (sscanf(line, " facet normal %f %f %f", 
                      &tri.normal.x, &tri.normal.y, &tri.normal.z) != 3) {
                fprintf(stderr, "Error en línea %d: formato de normal incorrecto\n", line_number);
                error_count++;
                continue;
            }

            // Leer "outer loop"
            if (!fgets(line, sizeof(line), file)) {
                fprintf(stderr, "Error en línea %d: se esperaba 'outer loop'\n", line_number);
                error_count++;
                continue;
            }
            line_number++;

            // Leer 3 vértices
            for (int i = 0; i < 3; i++) {
                if (!fgets(line, sizeof(line), file)) {
                    fprintf(stderr, "Error en línea %d: vértice %d faltante\n", line_number, i+1);
                    error_count++;
                    break;
                }
                line_number++;
                
                // Selecciona el vértice correspondiente (v0, v1 o v2)
                VEC3_T* vertex = (i == 0) ? &tri.v0 : (i == 1) ? &tri.v1 : &tri.v2;

                // Parsea las coordenadas del vértice
                if (sscanf(line, " vertex %f %f %f", 
                          &vertex->x, &vertex->y, &vertex->z) != 3) {
                    fprintf(stderr, "Error en línea %d: formato de vértice %d incorrecto\n", line_number, i+1);
                    error_count++;
                }
                else {
                    vertices_read++;
                }
            }

            // Verificar que se leyeron los 3 vértices
            if (vertices_read != 3) {
                continue;
            }

            // Leer "endloop" y "endfacet"
            fgets(line, sizeof(line), file); line_number++; // endloop
            fgets(line, sizeof(line), file); line_number++; // endfacet

            // Validar triángulo y agrandar arreglo dinamico si se necesita 
            if (!isnan(tri.normal.x) && !isnan(tri.v0.x) && !isnan(tri.v1.x) && !isnan(tri.v2.x)) {
                if (triangle_count >= triangle_capacity) {
                    triangle_capacity *= 2;
                    TRIANGLE_T* temp = realloc(triangles, triangle_capacity * sizeof(TRIANGLE_T));
                    if (!temp) {
                        fprintf(stderr, "Error al redimensionar memoria\n");
                        free(triangles);
                        fclose(file);
                        return 0;
                    }
                    triangles = temp;
                }
                triangles[triangle_count++] = tri;   //  Almacenar triángulo válido
            }
        }
    }
    
    //  Cierre del archivo
    fclose(file);

    if (error_count > 0) {
        fprintf(stderr, "Advertencia: se encontraron %d errores durante la carga\n", error_count);
    }

    // Optimizar memoria al tamaño exacto 
    if (triangle_count > 0) {
        TRIANGLE_T* temp = realloc(triangles, triangle_count * sizeof(TRIANGLE_T));
        if (temp) triangles = temp;
    }
    // Asignación al modelo de salida
    model->triangles = triangles;
    model->count = triangle_count;

    printf("Modelo cargado: %d triángulos\n", triangle_count);
    return 1;
}


