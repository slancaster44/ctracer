#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

#include "scene.h"
#include "read_file.h"

bool IsDoublePart(char* c)
{
    return *c == '.' || *c == '+' || *c == '-' || (*c >= '0' && *c <= '9');
}

void ReadDoubles(Set *output, char *file_contents, unsigned long *index)
{
    char *ptr = &file_contents[*index];

    do
    {
        double this_double = strtod(ptr, &ptr);
        AppendValue(output, &this_double);

        while (*ptr == ' ') 
        {
            ptr++;
        }
    } 
    while (IsDoublePart(ptr));

    long difference = ptr - (&file_contents[*index]);
    *index += (unsigned long) difference;
}

void ReadObj(Scene *s, const char *filename)
{
    char *file_contents;
    unsigned long file_length;
    READ_FILE(file_contents, file_length, filename);

    Set verticies;
    ConstructSet(&verticies, sizeof(Tuple3));

    Set faces;
    ConstructSet(&faces, sizeof(Tuple3));

    for (unsigned long i = 0; i < file_length;)
    {


        if (file_contents[i] == 'f')
        {
            Set doubles;
            ConstructSet(&doubles, sizeof(double));

            i++;
            ReadDoubles(&doubles, file_contents, &i);
            if (doubles.length == 3)
            {
                double* v1_idx = Index(&doubles, 0);
                double* v2_idx = Index(&doubles, 1);
                double* v3_idx = Index(&doubles, 2);

                Tuple3* v1 = Index(&verticies, (unsigned long) *v1_idx-1);
                Tuple3* v2 = Index(&verticies, (unsigned long) *v2_idx-1);
                Tuple3* v3 = Index(&verticies, (unsigned long) *v3_idx-1);

                Shape triangle = NewTriangle(*v1, *v2, *v3);
                AddShape(s, triangle);
            }
            else
            {
                printf("Ignoring non-triangle face\n");
            }

            DeconstructSet(&doubles);

        }
        else if (file_contents[i] == 'v')
        {
            Set doubles;
            ConstructSet(&doubles, sizeof(double));

            i++;
            ReadDoubles(&doubles, file_contents, &i);
            if (doubles.length != 3)
            {
                printf("Unable to parse vector");
                continue;
            }

            Tuple3 vec;

            double* x_ptr = Index(&doubles, 0);
            double* y_ptr = Index(&doubles, 1);
            double* z_ptr = Index(&doubles, 2);

            vec[0] = *x_ptr;
            vec[1] = *y_ptr;
            vec[2] = *z_ptr;
            vec[3] = 1.0; //Mark as point

            AppendValue(&verticies, &vec);
            DeconstructSet(&doubles);
        }
        else
        {
            i++;
        }
    }

    DeconstructSet(&verticies);
    DeconstructSet(&faces);
}
