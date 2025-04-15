


// A continuacion el codigo: stl_loader.h



/**************************************************************
 * TRABAJO: Ray Tracing Cara 3D                               *
 * AUTOR: [ Juan David Delgado Burbano ]                      *
 * FECHA: [12/04/2025]                                        *
 * AUTOR CODIGO:   [ Juan David Delgado Burbano ]             *                                        
 **************************************************************/

/**************************************************************
 * ARCHIVO: stl_loader.h                                      *
 * DESCRIPCIÓN: Cabecera para el cargador de archivos STL     *
 **************************************************************/

#ifndef STL_LOADER_H
#define STL_LOADER_H

#include "EDM.h"

int load_stl_ascii(const char* filename, STL_MODEL* model);

#endif