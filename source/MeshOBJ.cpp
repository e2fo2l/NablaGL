#include "MeshOBJ.h"

#include <GL/glew.h>
#include <iostream>
#include <stb_image.h>

#include "utils/GLShader.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "utils/tinyobj/tiny_obj_loader.h"

MeshOBJ::MeshOBJ() : Mesh() {
}

MeshOBJ::~MeshOBJ() {
    for(const auto& tex : textureMap) {
        glDeleteTextures(1, &tex.second);
    }
}

bool MeshOBJ::LoadFile(const std::string &filename) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> obj_materials;
    std::string err;

    std::string objBaseDir = filename.substr(0, filename.find_last_of("/\\") + 1);
    std::string objBaseName = filename.substr(filename.find_last_of("/\\") + 1);

    if (!tinyobj::LoadObj(&attrib, &shapes, &obj_materials, &err, filename.c_str(), objBaseDir.c_str())) {
        std::cout << "Error loading OBJ: " << err;
        return false;
    }

    if(shapes.size() < materials.size()) {
        std::cout << objBaseName << ": There are less shapes than materials." <<
                                    "TinyOBJLoader doesn't handle that case well." <<
                                    "For better results, re-export this OBJ with the" <<
                                    "\"Material Groups\" option ticked in Blender.\n";
    }

    /* Load textures */
    obj_materials.push_back(tinyobj::material_t()); /* Add a default material for shapes without one */

    stbi_set_flip_vertically_on_load(true);
    for (const auto &material : obj_materials) {
        materials.push_back({
            material.diffuse_texname,
            {material.ambient[0], material.ambient[1], material.ambient[2]},
            {material.specular[0], material.specular[1], material.specular[2]},
            material.shininess
        });

        if (material.diffuse_texname
                .empty()) /* Ignore materials that don't have a texture name */
            continue;
        if (textureMap.find(material.diffuse_texname) !=
            textureMap.end()) /* No need to load a texture twice */
            continue;

        int w, h, c;
        uint8_t *image = stbi_load(
            (objBaseDir + material.diffuse_texname).c_str(), &w, &h, &c, 0);
        if (!image) {
            std::cout << "Could not load texture " << material.diffuse_texname
                      << "for " << objBaseName << "\n";
            continue;
        }

        unsigned int textureID;

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0,
                     c == 3 ? GL_SRGB8 : GL_SRGB8_ALPHA8,
                     w, h, 0,
                     c == 3 ? GL_RGB : GL_RGBA,
                     GL_UNSIGNED_BYTE, image);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(image);

        textureMap.insert(std::make_pair(material.diffuse_texname, textureID));
    }
    stbi_set_flip_vertically_on_load(false);

    /* Load geometry */
    for (const auto& shape : shapes) {
        DrawShape ds;
        /* Since tinyobjloader only supports one material per shape, make sure to tick
           "Material Groups" when exporting as OBJ in Blender */

        ds.elementStart = vertices.size();
        for (const auto& index : shape.mesh.indices) {
            tinyobj::real_t vx = attrib.vertices[3*index.vertex_index+0];
            tinyobj::real_t vy = attrib.vertices[3*index.vertex_index+1];
            tinyobj::real_t vz = attrib.vertices[3*index.vertex_index+2];

            tinyobj::real_t nx = attrib.normals[3*index.normal_index+0];
            tinyobj::real_t ny = attrib.normals[3*index.normal_index+1];
            tinyobj::real_t nz = attrib.normals[3*index.normal_index+2];

            tinyobj::real_t tx = attrib.texcoords[2*index.texcoord_index+0];
            tinyobj::real_t ty = attrib.texcoords[2*index.texcoord_index+1];

            vertices.push_back({{vx,vy,vz},{nx,ny,nz},{tx,ty}});
        }
        ds.elementSize = vertices.size() - ds.elementStart;

        if (shape.mesh.material_ids.size() != 0)
            ds.materialID = shape.mesh.material_ids[0];
        else
            ds.materialID = materials.size() - 1; /* Default material */

        drawShapes.push_back(ds);
    }

    glBindVertexArray(0);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    /* Vertices */
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

    /* Normals */
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));

    /* Texture coords */
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texcoord));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return true;
}

void MeshOBJ::Draw(GLShader *shader) {
    if(VAO == 0 || VBO == 0)
        return;

    auto program = shader->GetProgram();
    glUseProgram(program);
    glBindVertexArray(VAO);

    GLint Ka = glGetUniformLocation(program, "u_Ka");
    GLint Kd = glGetUniformLocation(program, "u_Kd");
    GLint Ks = glGetUniformLocation(program, "u_Ks");
    GLint Ns = glGetUniformLocation(program, "u_Ns");
    glUniform1i(Kd, 0); /* GL_TEXTURE0 slot */

    glActiveTexture(GL_TEXTURE0);
    for(const auto& ds : drawShapes) {
        const auto& mat = materials[ds.materialID];
        if(!mat.diffuse.empty() && textureMap.find(mat.diffuse) != textureMap.end()) {
            glBindTexture(GL_TEXTURE_2D, textureMap.at(mat.diffuse));
        }
        // Material binding
        if(mat.ambient.x == 1.0f && mat.ambient.y == 1.0f && mat.ambient.z == 1.0f)
            glUniform3f(Ka, 0.1f, 0.1f, 0.1f); // Default ambient color
        else
            glUniform3f(Ka, mat.ambient.x, mat.ambient.y, mat.ambient.z); // Ambient color
        glUniform3f(Ks, mat.specular.x, mat.specular.y, mat.specular.z); // Specular
        glUniform1f(Ns, mat.shininess); // Shininess

        glDrawArrays(GL_TRIANGLES, ds.elementStart, ds.elementSize);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    glBindVertexArray(0);
}

//くコ:彡