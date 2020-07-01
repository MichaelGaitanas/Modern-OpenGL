//A header to import models for rendering. More specifically, the header reads
//the .txt files at which the model data are stored. The data are : vertices (x,y,z),
//normal vectors (nx,ny,nz) and indices. The indices are such to form triangles
//with a corresponding normal vector each.

#ifndef MESH_H
#define MESH_H

#include<iostream>
#include<fstream>
#include<vector>
#include<GL/glew.h>
using namespace std;

class mesh
{
public:
    unsigned int VAO;
    //The following buffer that will have the data (vertices and normals)
    //appropriately formatted for glDrawArrays() to render. We want the final
    //form of the buffer to be : {x1,y1,z1, nx1,ny1,nz1, x2,y2,z2, nx2,ny2,nz2, ...}
    vector<float> mainBuff;
    //Number of vertices stored in mainBuff.
    //We need to know that number for glDrawArrays().
    unsigned int mainBuffVerts;

    //constructor 1: Load vertices, normals and indices of the mesh.
    mesh(const char *vertsPath, const char *normsPath, const char *indsPath)
    {
        int i; //Of all the files that will be read, i will be the current row of a file.

        ////////////////// vertices //////////////////

        ifstream fVerts;
        fVerts.open(vertsPath);
        if (!fVerts.is_open())
        {
            cout << "Vertices file not found. Exiting...\n";
            exit(EXIT_FAILURE);
        }
        vector<vector<float>> verts;
        string vertsStr;
        float x,y,z;
        vector<float> vertsRow;
        i = 0;
        while (fVerts >> vertsStr >> x >> y >> z)
        {
            verts.push_back(vertsRow);
            verts[i].push_back(x);
            verts[i].push_back(y);
            verts[i].push_back(z);
            i++;
        }

        ////////////////// normals //////////////////

        ifstream fNorms;
        fNorms.open(normsPath);
        if (!fNorms.is_open())
        {
            cout << "Normals file not found. Exiting...\n";
            exit(EXIT_FAILURE);
        }
        vector<vector<float>> norms;
        string normsStr;
        float nx,ny,nz;
        vector<float> normsRow;
        i = 0;
        while (fNorms >> normsStr >> nx >> ny >> nz)
        {
            norms.push_back(normsRow);
            norms[i].push_back(nx);
            norms[i].push_back(ny);
            norms[i].push_back(nz);
            i++;
        }

        ////////////////// vertex and normal indices //////////////////

        ifstream fInds;
        fInds.open(indsPath);
        if (!fInds.is_open())
        {
            cout << "Indices file not found. Exiting...\n";
            exit(EXIT_FAILURE);
        }
        vector<vector<unsigned int>> inds;
        string indsStr;
        string slash1, slash2, slash3;
        unsigned int i11, i12,
                     i21, i22,
                     i31, i32;
        vector<unsigned int> indsRow;
        i = 0;
        while (fInds >> indsStr
                     >> i11 >> slash1 >> i12
                     >> i21 >> slash2 >> i22
                     >> i31 >> slash3 >> i32)
        {
            inds.push_back(indsRow);
            inds[i].push_back(i11-1);
            inds[i].push_back(i12-1);

            inds[i].push_back(i21-1);
            inds[i].push_back(i22-1);

            inds[i].push_back(i31-1);
            inds[i].push_back(i32-1);
            i++;
        }

        //Now combine verts[][], norms[][] and inds[][] to construct the main
        //buffer mainBuff[] which will have all the data needed for rendering.
        for (i = 0; i < inds.size(); ++i)
        {
            mainBuff.push_back( verts[ inds[i][0] ][0] );
            mainBuff.push_back( verts[ inds[i][0] ][1] );
            mainBuff.push_back( verts[ inds[i][0] ][2] );
            mainBuff.push_back( norms[ inds[i][1] ][0] );
            mainBuff.push_back( norms[ inds[i][1] ][1] );
            mainBuff.push_back( norms[ inds[i][1] ][2] );

            mainBuff.push_back( verts[ inds[i][2] ][0] );
            mainBuff.push_back( verts[ inds[i][2] ][1] );
            mainBuff.push_back( verts[ inds[i][2] ][2] );
            mainBuff.push_back( norms[ inds[i][3] ][0] );
            mainBuff.push_back( norms[ inds[i][3] ][1] );
            mainBuff.push_back( norms[ inds[i][3] ][2] );

            mainBuff.push_back( verts[ inds[i][4] ][0] );
            mainBuff.push_back( verts[ inds[i][4] ][1] );
            mainBuff.push_back( verts[ inds[i][4] ][2] );
            mainBuff.push_back( norms[ inds[i][5] ][0] );
            mainBuff.push_back( norms[ inds[i][5] ][1] );
            mainBuff.push_back( norms[ inds[i][5] ][2] );
        }
        //mainBuff[] has now the form :
        //mainBuff[] = {x1,y1,z1, nx1,ny1,nz1, x2,y2,z2, nx2,ny2,nz2, ...}

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        unsigned int VBO;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, mainBuff.size()*sizeof(float), &mainBuff[0], GL_STATIC_DRAW);
        //vertices attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        //normals attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);
        mainBuffVerts = mainBuff.size()/6;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////

    //constructor 2: Load vertices and indices of the mesh.
    mesh(const char *vertsPath, const char *indsPath)
    {
        int i; //Of all the files that will be read, i will be the current row of a file.

        ////////////////// vertices //////////////////

        ifstream fVerts;
        fVerts.open(vertsPath);
        if (!fVerts.is_open())
        {
            cout << "Vertices file not found. Exiting...\n";
            exit(EXIT_FAILURE);
        }
        vector<vector<float>> verts;
        string vertsStr;
        float x,y,z;
        vector<float> vertsRow;
        i = 0;
        while (fVerts >> vertsStr >> x >> y >> z)
        {
            verts.push_back(vertsRow);
            verts[i].push_back(x);
            verts[i].push_back(y);
            verts[i].push_back(z);
            i++;
        }

        ////////////////// vertex indices //////////////////

        ifstream fInds;
        fInds.open(indsPath);
        if (!fInds.is_open())
        {
            cout << "Indices file not found. Exiting...\n";
            exit(EXIT_FAILURE);
        }
        vector<vector<unsigned int>> inds;
        string indsStr;
        string slash1, slash2, slash3;
        unsigned int i11, i12,
                     i21, i22,
                     i31, i32;
        vector<unsigned int> indsRow;
        i = 0;
        while (fInds >> indsStr
                     >> i11 >> slash1 >> i12
                     >> i21 >> slash2 >> i22
                     >> i31 >> slash3 >> i32)
        {
            inds.push_back(indsRow);
            inds[i].push_back(i11-1);
            inds[i].push_back(i21-1);
            inds[i].push_back(i31-1);
            i++;
        }

        //Now combine verts[][] and inds[][] to construct the main
        //buffer mainBuff[] which will have all the data needed for rendering.
        for (i = 0; i < inds.size(); ++i)
        {
            mainBuff.push_back( verts[ inds[i][0] ][0] );
            mainBuff.push_back( verts[ inds[i][0] ][1] );
            mainBuff.push_back( verts[ inds[i][0] ][2] );

            mainBuff.push_back( verts[ inds[i][1] ][0] );
            mainBuff.push_back( verts[ inds[i][1] ][1] );
            mainBuff.push_back( verts[ inds[i][1] ][2] );

            mainBuff.push_back( verts[ inds[i][2] ][0] );
            mainBuff.push_back( verts[ inds[i][2] ][1] );
            mainBuff.push_back( verts[ inds[i][2] ][2] );
        }
        //mainBuff[] has now the form :
        //mainBuff[] = {x1,y1,z1, x2,y2,z2, x3,y3,z3, ...}
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        unsigned int VBO;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, mainBuff.size()*sizeof(float), &mainBuff[0], GL_STATIC_DRAW);
        //vertices attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        mainBuffVerts = mainBuff.size()/3;
    }
};
#endif
