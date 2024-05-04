#ifndef __shader_h__
#define __shader_h__

#include <GL/gl.h>
#include <cstdio>
#include <cstdlib>

GLuint shader_load(GLenum type, char *filename)
{
    FILE *file = fopen(filename, "r");
    long file_size;
    
    if(!file) {
        printf("Error during file reading!\n");
        exit(0);
    }

    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *content = (char*)malloc(file_size+1);
    fread(content, sizeof(char), file_size, file);
    content[file_size] = 0;

    fclose(file);

}

#endif