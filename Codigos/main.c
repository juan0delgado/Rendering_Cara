// a continuacion el codigo: main.c


/**************************************************************
 * TRABAJO: Ray Tracing Cara 3D                               *
 * AUTOR: [ Juan David Delgado Burbano ]                      *
 * FECHA: [12/04/2025]                                        *
 * AUTOR CODIGO:   [ Juan David Delgado Burbano ]             *                                        
 **************************************************************/









#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include "EDM.h"

#include <sys/stat.h>
#include <string.h> 
#include "stl_loader.h"


//-------puesto 6:30--------------

#include <unistd.h>
//------------------------------



// 512  pero 120segundos por frame, mientras que   256 son 20 seg por frame funciono pero baja calidad 
#define SIDE 512    // Resolución de la imagen (512x512 píxeles)
#define SCALE 2048  // Escala de color (0-2047 para PGM)
#define SKY 1e+30  // Valor "infinito" para distancias no intersectadas
#define TOTAL_FRAMES 120  // Número total de frames en la animación



// Representa un rayo con origen (o) y dirección (d)
typedef struct {
    VEC3_T o, d;
} RAY_T;

// Representa una esfera con centro (c) y radio (r)  [no se uso en este trabajo]
typedef struct {
    VEC3_T c;
    float r;
} SPHERE_T;


// Representa un cilindro con base, eje, radio y altura [no se uso en este trabajo]
typedef struct {
    VEC3_T base;
    VEC3_T axis;
    float radius;
    float height;
} CYLINDER_T;


// Representa un cono con vértice, eje, radio y altura [no se uso en este trabajo]
typedef struct {
    VEC3_T apex;
    VEC3_T axis;
    float radius;
    float height;
} CONE_T;





void center_and_scale_model(STL_MODEL* model, float target_size) {
    // Calcular centro y dimensiones del modelo
    VEC3_T min = {INFINITY, INFINITY, INFINITY};
    VEC3_T max = {-INFINITY, -INFINITY, -INFINITY};


    // Recorrer todos los triángulos del modelo
    for (int i = 0; i < model->count; i++) {
        TRIANGLE_T tri = model->triangles[i];
        // Actualizar mínimos y máximos con los 3 vértices
        for (int j = 0; j < 3; j++) {
            VEC3_T v = j == 0 ? tri.v0 : (j == 1 ? tri.v1 : tri.v2);

            // Actualizar mínimo y máximo de cada eje
            min.x = fminf(min.x, v.x);
            min.y = fminf(min.y, v.y);
            min.z = fminf(min.z, v.z);
            max.x = fmaxf(max.x, v.x);
            max.y = fmaxf(max.y, v.y);
            max.z = fmaxf(max.z, v.z);
        }
    }


    // Calcular el centro del modelo (promedio entre el mínimo y el máximo en cada eje)
    VEC3_T center = {
        (min.x + max.x) / 2,
        (min.y + max.y) / 2,
        (min.z + max.z) / 2
    };


    // Calcular el tamaño del modelo en cada eje
    VEC3_T size = {
        max.x - min.x,
        max.y - min.y,
        max.z - min.z
    };


    // Obtener la dimensión más grande (ancho, alto o profundidad)
    float max_dim = fmaxf(size.x, fmaxf(size.y, size.z));

    // Calcular el factor de escala para ajustar el modelo al tamaño deseado
    float scale = target_size / max_dim;

    // Aplicar transformación a todos los triángulos
    for (int i = 0; i < model->count; i++) {
        model->triangles[i].v0 = VEC3_scale(VEC3_sub(model->triangles[i].v0, center), scale);
        model->triangles[i].v1 = VEC3_scale(VEC3_sub(model->triangles[i].v1, center), scale);
        model->triangles[i].v2 = VEC3_scale(VEC3_sub(model->triangles[i].v2, center), scale);
    }
}






bool ray_triangle_intersect(RAY_T ray, TRIANGLE_T triangle, float* t) {
    const float EPSILON = 1e-6f;

    // edge1 y edge2 son los dos vectores que forman el triángulo desde el vértice v0
    VEC3_T edge1 = VEC3_sub(triangle.v1, triangle.v0);
    VEC3_T edge2 = VEC3_sub(triangle.v2, triangle.v0);

    // h es perpendicular al rayo y a edge2
    VEC3_T h = VEC3_cross(ray.d, edge2);

    // a es el determinante (producto escalar entre edge1 y h)
    float a = VEC3_dot(edge1, h);

    // Primera comprobación de paralelismo porque Si a se acerca a  0, el rayo es paralelo al triángulo y no hay intersección (0.000001) 
    if (a > -EPSILON && a < EPSILON)
        return false;
    // Inverso del determinante
    float f = 1.0f / a;
    // s es el vector desde v0 hasta el origen del rayo
    VEC3_T s = VEC3_sub(ray.o, triangle.v0);
    // u determina si la intersección cae dentro del triángulo en el eje edge1
    float u = f * VEC3_dot(s, h);
    // Si u está fuera del rango [0,1], no hay intersección dentro del triángulo
    if (u < 0.0f || u > 1.0f)
        return false;

    VEC3_T q = VEC3_cross(s, edge1);
    float v = f * VEC3_dot(ray.d, q);

    if (v < 0.0f || u + v > 1.0f)
        return false;

    *t = f * VEC3_dot(edge2, q);

    // Retorna true solo si la intersección está al frente del rayo 
    return (*t > EPSILON);
}



VEC3_T get_light_position(VEC3_T camera_position) {
    // Ajusta estos valores para cambiar el ángulo de iluminación
    VEC3_T light_offset = {200.0, 500.0, -200.0}; // Posición de la luz 200.0, 300.0, -200.0
    return VEC3_add(camera_position, light_offset);
}



void create_directory_if_not_exists(const char *dir) {
    struct stat st = {0};
    if (stat(dir, &st) == -1) {
        mkdir(dir, 0700);  // Crear directorio con permisos 0700 si no existe
    }
}



float Shading(float dst, RAY_T ray, VEC3_T normal, VEC3_T light, int objectType) {
    if (dst == SKY) return 0.0;
    float ambient = 0.0; // valor de luz ambiental 
    VEC3_T normal_normalized = VEC3_normalize(normal);

    // difuse es la intencidad de la luz en el punto segun el angulo enque inside si el angulo entre la luz y la normal es pequeño el valor es cercano a uno ( hay mucha luz), si el angulo es grande osea entra de lado la luz,hay poca luz, y si es negativo la luz viene de atras y no hay luz
    float diffuse = VEC3_dot(normal_normalized, light);
    diffuse = (diffuse < 0.0) ? 0.0 : diffuse;
    float intensity = (objectType == 2) ? 1.5 : 1.2; // intensidad de la luz  2.0 : 1.5 menos=mas sombra
    return (ambient + diffuse * intensity);
}



void rotate_scene(float angle_deg, VEC3_T *light) {
    float angle = angle_deg * M_PI / 180.0;
    float cos_theta = cos(angle);
    float sin_theta = sin(angle);

    // Rotar la luz junto con la cámara
    float light_x = light->x;
    float light_z = light->z;
        
    light->x = light_x * cos_theta - light_z * sin_theta;
    light->z = light_x * sin_theta + light_z * cos_theta;
}






int main() {
    create_directory_if_not_exists("frames");

    // 1. Cargar modelo con verificación mejorada
    STL_MODEL head_model;
    if (!load_stl_ascii("cara2.stl", &head_model)) {
        fprintf(stderr, "Error cargando modelo STL\n");
        return 1;
    }

    // 2. Configuración para comenzar desde frame específico
    const int start_frame = 0;  // Comenzar desde frame 58
    printf("Iniciando renderizado desde el frame %d\n", start_frame);

    // Escalado seguro del modelo
    printf("Escalando modelo...\n");
    center_and_scale_model(&head_model, 500.0);
    printf("Modelo cargado: %d triángulos\n", head_model.count);

    // 3. Configuración de renderizado
    const float camera_radius = 800.0;
    const float camera_height = 300.0;
    const VEC3_T up = {0.0, 1.0, 0.0};
    const float inv_side = 1.0f / (SIDE / 2.0f);  // Precalcular

    // 4. Bucle principal de renderizado (comienza desde start_frame)
    for (int frame = start_frame; frame < TOTAL_FRAMES; ++frame) {
        printf("Renderizando frame %d/%d...\n", frame+1, TOTAL_FRAMES);
        char filename[64];
        sprintf(filename, "frames/frame_%03d.pgm", frame);

        FILE *fp = fopen(filename, "w");
        if (!fp) {
            perror("Error al crear archivo");
            free(head_model.triangles);
            return 1;
        }

        // ------------------------------------------------------------
        // CONFIGURACIÓN EXACTA DE CÁMARA PARA ESTE FRAME ESPECÍFICO
        // ------------------------------------------------------------

        // Ángulo exacto para este frame
        float angle = (2 * M_PI * frame) / TOTAL_FRAMES;

        // Posición de cámara (orbita perfecta alrededor de (0,0,0))
        VEC3_T camera_position = {
            camera_radius * cosf(angle),
            camera_height,
            camera_radius * sinf(angle)
        };

        // Punto de mira fijo en el origen
        const VEC3_T look_at = {0.0, 0.0, 0.0};

        // Sistema de coordenadas de cámara
        VEC3_T forward = VEC3_normalize(VEC3_sub(look_at, camera_position));
        VEC3_T right = VEC3_normalize(VEC3_cross(forward, up));
        VEC3_T real_up = VEC3_normalize(VEC3_cross(right, forward));

        // Configuración de iluminación (posición relativa a cámara)
        VEC3_T light_pos = {
            camera_position.x + 200.0f,   // Offset lateral
            camera_position.y + 500.0f,   // Offset vertical
            camera_position.z - 200.0f    // Offset profundidad
        };
        VEC3_T light_dir = VEC3_normalize(VEC3_sub(light_pos, look_at));

        // ------------------------------------------------------------
        //                     RENDERIZADO 
        // ------------------------------------------------------------
        fprintf(fp, "P2\n%d %d\n%d\n", SIDE, SIDE, SCALE);

        #pragma omp parallel for schedule(dynamic)
        for (int y = 0; y < SIDE; y++) {
            char pixel_buffer[SIDE * 12];
            int buf_pos = 0;
            int screen_y = SIDE - 1 - y;

            for (int x = 0; x < SIDE; x++) {
                float px = (x - SIDE/2.0f) * inv_side;
                float py = (screen_y - SIDE/2.0f) * inv_side;

                VEC3_T ray_dir = VEC3_normalize(VEC3_add(
                    VEC3_add(VEC3_scale(right, px), VEC3_scale(real_up, py)),
                    forward
                ));

                RAY_T ray = {camera_position, ray_dir};

                // Búsqueda de intersección
                float closest_t = SKY;
                VEC3_T hit_normal = {0};
                int intersected = 0;

                for (int i = 0; i < head_model.count; i++) {
                    float t;
                    if (ray_triangle_intersect(ray, head_model.triangles[i], &t) && t < closest_t) {
                        closest_t = t;
                        hit_normal = head_model.triangles[i].normal;
                        intersected = 1;
                    }
                }

                // Cálculo de iluminación
                float shade = 0.1f;
                if (intersected) {
                    VEC3_T hit_point = VEC3_add(ray.o, VEC3_scale(ray.d, closest_t));

                    // Rayo de sombra
                    RAY_T shadow_ray = {
                        VEC3_add(hit_point, VEC3_scale(hit_normal, 0.001f)),
                        light_dir
                    };

                    int in_shadow = 0;
                    for (int i = 0; i < head_model.count && !in_shadow; i++) {
                        float t;
                        if (ray_triangle_intersect(shadow_ray, head_model.triangles[i], &t) && t > 0.001f) {
                            in_shadow = 1;
                        }
                    }

                    if (!in_shadow) {
                        float diffuse = VEC3_dot(hit_normal, light_dir);
                        shade += 0.9f * fmaxf(0.0f, diffuse);
                    }
                }

                // Escribir pixel
                int pixel_value = (int)(fminf(shade, 1.0f) * SCALE);
                buf_pos += sprintf(pixel_buffer + buf_pos, "%d ", pixel_value);
            }

            #pragma omp critical
            {
                fprintf(fp, "%s\n", pixel_buffer);
            }
        }

        fclose(fp);
        printf("Frame %d completado\n", frame+1);
    }

    free(head_model.triangles);
    printf("Renderizado completado desde frame %d!\n", start_frame);
    return 0;
}

/*

int main() {
    create_directory_if_not_exists("frames");

    // 1. Cargar modelo con verificación mejorada
    STL_MODEL head_model;
    if (!load_stl_ascii("cara2.stl", &head_model)) {
        fprintf(stderr, "Error cargando modelo STL\n");
        return 1;
    }


    
    // Escalado seguro del modelo
    printf("Escalando modelo...\n");
    center_and_scale_model(&head_model, 500.0);
    printf("Modelo cargado: %d triángulos\n", head_model.count);

    // 2. Configuración de renderizado
    const float camera_radius = 800.0;
    const float camera_height = 300.0;
    const VEC3_T look_at = {0.0, 0.0, 500.0};
    const VEC3_T up = {0.0, 1.0, 0.0};
    const float inv_side = 1.0f / (SIDE / 2.0f);  // Precalcular

    // 3. Bucle principal de renderizado

    //    int frame = 0; frame < TOTAL_FRAMES; ++frame
    for (int frame = 0; frame < TOTAL_FRAMES; ++frame) {
        printf("Iniciando frame %d/%d...\n", frame+1, TOTAL_FRAMES);
        char filename[64];
        sprintf(filename, "frames/frame_%03d.pgm", frame);

        FILE *fp = fopen(filename, "w");
        if (!fp) {
            perror("Error al crear archivo");
            free(head_model.triangles);
            return 1;
        }

        // Configuración de cámara
        float angle = (2 * M_PI * frame) / TOTAL_FRAMES;
        VEC3_T camera_position = {
            camera_radius * cosf(angle),
            camera_height,
            camera_radius * sinf(angle) // + look_at.z  es lo que tenia ahi pegado
        };
        const VEC3_T look_at = {0.0, 0.0, 0.0};
// const VEC3_T look_at = {0.0, 0.0, 0.0}; es la ultima implementacion 



        
        // Sistema de coordenadas de cámara
        VEC3_T forward = VEC3_normalize(VEC3_sub(look_at, camera_position));
        VEC3_T right = VEC3_normalize(VEC3_cross(forward, up));
        VEC3_T real_up = VEC3_normalize(VEC3_cross(right, forward));

        // Configuración de iluminación
        VEC3_T light_pos = {
            camera_position.x + 200.0f,
            camera_position.y + 500.0f,
            camera_position.z - 200.0f
        };
        VEC3_T light_dir = VEC3_normalize(VEC3_sub(light_pos, look_at));

        // Escribir encabezado PGM
        fprintf(fp, "P2\n%d %d\n%d\n", SIDE, SIDE, SCALE);

        // Renderizado paralelo con buffers por hilo
        #pragma omp parallel for schedule(dynamic)
        for (int y = 0; y < SIDE; y++) {
            char pixel_buffer[SIDE * 12];  // Buffer para una fila completa
            int buf_pos = 0;
            int screen_y = SIDE - 1 - y;  // Convertir coordenada Y

            for (int x = 0; x < SIDE; x++) {
                // Coordenadas normalizadas del píxel
                float px = (x - SIDE/2.0f) * inv_side;
                float py = (screen_y - SIDE/2.0f) * inv_side;

                // Generar dirección del rayo
                VEC3_T ray_dir = VEC3_normalize(VEC3_add(
                    VEC3_add(VEC3_scale(right, px), VEC3_scale(real_up, py)),
                    forward
                ));

                RAY_T ray = {camera_position, ray_dir};

                // Búsqueda de intersección
                float closest_t = SKY;
                VEC3_T hit_normal = {0};
                int intersected = 0;

                for (int i = 0; i < head_model.count; i++) {
                    float t;
                    if (ray_triangle_intersect(ray, head_model.triangles[i], &t) && t < closest_t) {
                        closest_t = t;
                        hit_normal = head_model.triangles[i].normal;
                        intersected = 1;
                    }
                }

                // Cálculo de iluminación
                float shade = 0.1f;  // Componente ambiental
                if (intersected) {
                    VEC3_T hit_point = VEC3_add(ray.o, VEC3_scale(ray.d, closest_t));

                    // Rayo de sombra con pequeño offset
                    RAY_T shadow_ray = {
                        VEC3_add(hit_point, VEC3_scale(hit_normal, 0.001f)),
                        light_dir
                    };

                    int in_shadow = 0;
                    for (int i = 0; i < head_model.count && !in_shadow; i++) {
                        float t;
                        if (ray_triangle_intersect(shadow_ray, head_model.triangles[i], &t) && t > 0.001f) {
                            in_shadow = 1;
                        }
                    }

                    if (!in_shadow) {
                        float diffuse = VEC3_dot(hit_normal, light_dir);
                        shade += 0.9f * fmaxf(0.0f, diffuse);
                    }
                }

                // Escribir en buffer
                int pixel_value = (int)(fminf(shade, 1.0f) * SCALE);
                buf_pos += sprintf(pixel_buffer + buf_pos, "%d ", pixel_value);
            }

            // Escribir fila completa (sección crítica)
            #pragma omp critical
            {
                fprintf(fp, "%s\n", pixel_buffer);
            }
        }

        fclose(fp);
        printf("Frame %d completado\n", frame+1);
    }

    free(head_model.triangles);
    printf("Renderizado completado!\n");
    return 0;
}




*/

    