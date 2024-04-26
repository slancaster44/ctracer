#include "scene.h"
#include "shape.h"
#include "material.h"

#include <stdio.h>
#include <string.h>
#include <cjson/cJSON.h>

#define READ_FILE(BUFFER, FILE_NAME)                                            \
    FILE* fp = fopen(FILE_NAME, "r");                                           \
    if (fp == NULL) {                                                           \
        printf("Error: unable to open file '%s'\n", file);                      \
        exit(1);                                                                \
    }                                                                           \
                                                                                \
    fseek(fp, 0L, SEEK_END);                                                    \
    unsigned long file_size = sizeof(char) * ((unsigned long) ftell(fp) + 1);   \
    rewind(fp);                                                                 \
                                                                                \
    BUFFER = alloca(file_size);                                                 \
    fread(BUFFER, 1, file_size, fp);                                            \
    fclose(fp);                                                                 \
                                                                                \
    BUFFER[file_size-1] = '\0';                                                 \

void FatalDataCheck(void* object, const char* msg) {
    if (object == NULL) { 
        printf("Error: %s\n", msg);
        const char *error_ptr = cJSON_GetErrorPtr(); 
        if (error_ptr != NULL) { 
            printf("cJSON Error Message: %s\n", error_ptr); 
        } 
        exit(1);
    }
}

void GetFloatScalar(double* output, cJSON* json, const char* name) {
    cJSON* data = cJSON_GetObjectItem(json, name);
    FatalDataCheck(data, name);
    *output = (double) cJSON_GetNumberValue(data);
}

void GetIntegerScalar(int* output, cJSON* json, const char* name) {
    cJSON* data = cJSON_GetObjectItem(json, name);
    FatalDataCheck(data, name);
    *output = (int) cJSON_GetNumberValue(data);
}

void GetFloatListItem(double* out, cJSON* json, int index) {
    cJSON* data = cJSON_GetArrayItem(json, index);
    FatalDataCheck(data, "Failed to get list item");
    *out = (double) cJSON_GetNumberValue(data);
}

void GetTuple(Tuple3* output, cJSON* json, const char* name) {
    cJSON* data = cJSON_GetObjectItem(json, name);
    FatalDataCheck(data, name);

    int size = cJSON_GetArraySize(data);
    if (size != 3) {
        printf("Error: Expected array of size 3");
        exit(1);
    }

    for (int i = 0; i < 3; i++) {
        GetFloatListItem(&((*output)[i]), data, i);
    }
}

void GetVector(Tuple3* output, cJSON* json, const char* name) {
    GetTuple(output, json, name);
    (*output)[3] = 0;
}

void GetPoint(Tuple3* output, cJSON* json, const char* name) {
    GetTuple(output, json, name);
    (*output)[3] = 1;
}

void GetCamera(Camera* c, cJSON* json) {
    cJSON* camera_data = cJSON_GetObjectItem(json, "camera");
    FatalDataCheck(camera_data, "Camera data not found");

    Tuple3 from = NewTuple3(0, 0, 0, 0);
    GetPoint(&from, camera_data, "from");

    Tuple3 to = NewTuple3(0, 0, 0, 0);
    GetPoint(&to, camera_data, "to");

    Tuple3 up = NewTuple3(0, 0, 0, 0);
    GetVector(&up, camera_data, "up");

    int width;
    GetIntegerScalar(&width, camera_data, "width");

    int height;
    GetIntegerScalar(&height, camera_data, "height");

    double fov;
    GetFloatScalar(&fov, camera_data, "fov");

    Camera local_camera = NewCamera((unsigned) width, (unsigned) height, fov);
    Matrix4x4 transform = ViewMatrix(from, to, up);
    CameraApplyTransformation(&local_camera, transform);

    memcpy(c, &local_camera, sizeof(Camera));
}

void GetLight(Light* l, cJSON* json) {
    cJSON* light_data = cJSON_GetObjectItem(json, "light");
    FatalDataCheck(light_data, "Light data not found");

    GetPoint(&l->origin, light_data, "origin");
    GetPoint(&l->color, light_data, "color");
}

void GetShapeType(SHAPE_TYPE* type, cJSON* json) {
    cJSON* shape_type = cJSON_GetObjectItem(json, "type");
    FatalDataCheck(shape_type, "Shape type tag not found");

    char* shape_type_name = cJSON_GetStringValue(shape_type);
    FatalDataCheck(shape_type_name, "Could not get shape type information");
    if (strncmp(shape_type_name, "sphere", 6) == 0) {
        *type = SPHERE;
    } else if (strncmp(shape_type_name, "plane", 5) == 0) {
        *type = PLANE;
    } else {
        printf("Unkown shape type tag\n");
        exit(1);
    }
}

void GetMatrix(Matrix4x4* m, cJSON* json) {
    int matrix_size = cJSON_GetArraySize(json);
    if (matrix_size != 4) {
        printf("Expected matrix of 4 rows");
        exit(1);
    }

    for (int i = 0; i < 4; i++) {
        cJSON* row = cJSON_GetArrayItem(json, i);
        FatalDataCheck(row, "Failed to get matrix row");

        int row_size = cJSON_GetArraySize(row);
        if (row_size != 4) {
            printf("Expected array of size '4' but got array of size '%d'\n", row_size);
            exit(1);
        }

        for (int j = 0; j < 4; j++) {
            GetFloatListItem(&(m->contents[i][j]), row, j);
        }
    }
}

void GetShapeTransform(Matrix4x4* transform, cJSON* json) {
    cJSON* shape_transform_json = cJSON_GetObjectItem(json, "transform");
    FatalDataCheck(shape_transform_json, "Shape transform not found");
    GetMatrix(transform, shape_transform_json);
}

void GetPattern(Material* m, cJSON* json) {
    cJSON* pattern_json = cJSON_GetObjectItem(json, "pattern");
    FatalDataCheck(pattern_json, "Shape pattern required");

    GetPoint(&(m->pattern.color_a), pattern_json, "color_a");
    GetPoint(&(m->pattern.color_b), pattern_json, "color_b");
    GetShapeTransform(&(m->pattern.transform), pattern_json);

    m->pattern.inverse_transform = MatrixInvert(m->pattern.transform);

    cJSON* pattern_type_json = cJSON_GetObjectItem(pattern_json, "type");
    FatalDataCheck(pattern_type_json, "Pattern tag not found");

    char* pattern_name = cJSON_GetStringValue(pattern_type_json);
    FatalDataCheck(pattern_name, "Could get pattern type");
    if (strncmp(pattern_name, "solid", 5) == 0) {
        m->pattern.type = SOLID;
    } else if (strncmp(pattern_name, "striped", 7) == 0) {
        m->pattern.type = STRIPED;
    } else if (strncmp(pattern_name, "checkered", 9) == 0) {
        m->pattern.type = CHECKERED; 
    } else if (strncmp(pattern_name, "ringed", 6) == 0) {
        m->pattern.type = RINGED;
    } else if (strncmp(pattern_name, "gradient", 8) == 0) {
        m->pattern.type = GRADIENT;
    } else {
        printf("Unkown pattern '%s'\n", pattern_name);
        exit(1);
    }

}

void GetMaterial(Material* m, cJSON* json) {
    cJSON* shape_material_json = cJSON_GetObjectItem(json, "material");
    FatalDataCheck(shape_material_json, "Shape material not found");

    GetPattern(m, shape_material_json);
    GetFloatScalar(&(m->ambient_reflection), shape_material_json, "ambient");
    GetFloatScalar(&(m->diffuse_reflection), shape_material_json, "diffuse");
    GetFloatScalar(&(m->specular_reflection), shape_material_json, "specular");
    GetFloatScalar(&(m->shininess), shape_material_json, "shininess");
    GetFloatScalar(&(m->general_reflection), shape_material_json, "general");
    GetFloatScalar(&(m->refractive_index), shape_material_json, "refractive_index");
    GetFloatScalar(&(m->transparency), shape_material_json, "transparency");


    cJSON* shader_json = cJSON_GetObjectItem(shape_material_json, "shader");
    FatalDataCheck(shader_json, "Shader tag not found");
    
    char* shader_name = cJSON_GetStringValue(shader_json);
    FatalDataCheck(shader_name, "Could get shader information");
    if (strncmp(shader_name, "phong", 5) == 0) {
        m->shader = PhongShader;
    } else {
        printf("Unkown shader '%s'\n", shader_name);
        exit(1);
    }

}

void GetShapes(Set* shapes, cJSON* json) {
    ConstructSet(shapes, sizeof(Shape));

    cJSON* shapes_list = cJSON_GetObjectItem(json, "shapes");
    if (shapes_list == NULL) {
        return;
    }
    
    int num_shapes = cJSON_GetArraySize(shapes_list);
    for (int i = 0; i < num_shapes; i++) {
        Shape this_shape;
        cJSON* this_shape_json = cJSON_GetArrayItem(shapes_list, i);

        GetShapeType(&this_shape.type, this_shape_json);
        
        GetShapeTransform(&this_shape.transformation, this_shape_json);
        this_shape.inverse_transform = MatrixInvert(this_shape.transformation);

        GetMaterial(&this_shape.material, this_shape_json);

        AppendValue(shapes, &this_shape);
    }
}

void ReadScene(Scene* s, const char* file) {
    char* file_contents;
    READ_FILE(file_contents, file);

    cJSON* json = cJSON_Parse(file_contents);
    FatalDataCheck(json, "Could not parse jsson");

    GetCamera(&s->camera, json);
    GetLight(&s->light, json);
    GetShapes(&s->shapes, json);

    cJSON_Delete(json);
}