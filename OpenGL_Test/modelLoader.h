#include <iostream>
#include <vector>
#include <stdio.h>
#include "defines.h"
#include "string.h"
#include "libs/glm/glm/glm.hpp"
#include "libs/glm/glm/ext/matrix_transform.hpp"
#include "libs/glm/glm/gtc/matrix_transform.hpp"

void ObjLoader(const char* filename, std::vector <Vertex>* vertecies, std::vector <uint32>* indices){
    std::vector <float32> vert;
    Vertex vertexBuffer;
    char* temp;
    float tempVert[3] = {0};
    int tempFace[9] = {0};
    char buffer[20];
    int length;

    FILE* fp, *newFp;
    fp = fopen(filename, "r");
    if(fp == NULL){
        std::cout << "Error: File" << filename << "could not be opened" << std::endl;
        std::cin.get();
        return;
    }

    fseek(fp, SEEK_CUR, SEEK_END);
    length = (int)ftell(fp);
    temp = new char[length + 1];
    rewind(fp);
    fread(temp, sizeof(char), length, fp);
    rewind(fp);
        for(int i = 0; i < length; i++){
            fseek(fp, i, SEEK_SET);
             if((temp[i] == 'v') && (temp[i + 1] == ' ')){
                fseek(fp, i + 1, SEEK_SET);
                int a = fscanf(fp, "%f %f %f\n", &tempVert[0], &tempVert[1], &tempVert[2]);
                for(int k = 0; k < 3; k++)
                vert.push_back(tempVert[k]);
            }

            else if(temp[i] == 'f' && temp[i + 1] == ' '){
                fseek(fp, i + 1, SEEK_SET);              
				int a = fscanf(fp, " %d/%d/%d %d/%d/%d %d/%d/%d\n", &tempFace[0], &tempFace[1], &tempFace[2], &tempFace[3], &tempFace[4], &tempFace[5], &tempFace[6], &tempFace[7], &tempFace[8]);
				indices->push_back(tempFace[0]);
                indices->push_back(tempFace[3]);
                indices->push_back(tempFace[6]);
            }
        }
        
    delete []temp;
    fclose(fp);

	for (int i = 0; i < vert.size(); i += 3) {
        vertexBuffer.x = vert.at(i);
        vertexBuffer.y = vert.at(i + 1);
        vertexBuffer.z = vert.at(i + 2);
        vertecies->push_back(vertexBuffer);
    }
    return;
}