#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <trigon/core/resources/mesh.h>
#include "trigon/app.h"
#include <vector>
#include <stdlib.h>
#include <ostream>
#include <fstream>

enum FLAGS {
    BINARY,
    HEADER,
    COUNT
};

void convert_mesh(aiMesh& mesh, mesh_t& out) {
    out.vertex_count    = mesh.mNumVertices;
    out.vertices        = new vertex_t[out.vertex_count];

    for (unsigned int i = 0; i < mesh.mNumVertices; i++) {
        out.vertices[i].pos[0] = static_cast<uint32_t>(mesh.mVertices[i].x);
        out.vertices[i].pos[1] = static_cast<uint32_t>(mesh.mVertices[i].y);
        out.vertices[i].pos[2] = static_cast<uint32_t>(mesh.mVertices[i].z);

        if (mesh.HasNormals()) {
            out.vertices[i].nor[0] = static_cast<uint32_t>(mesh.mNormals[i].x);
            out.vertices[i].nor[1] = static_cast<uint32_t>(mesh.mNormals[i].y);
            out.vertices[i].nor[2] = static_cast<uint32_t>(mesh.mNormals[i].z);
        }

        if (mesh.HasTextureCoords(0)) {
            out.vertices[i].uv[0] = static_cast<uint32_t>(mesh.mTextureCoords[0][i].x);
            out.vertices[i].uv[1] = static_cast<uint32_t>(mesh.mTextureCoords[0][i].y);
        }

        if (mesh.HasVertexColors(0)) {
            out.vertices[i].col[0] = static_cast<uint32_t>(mesh.mColors[0][i].r);
            out.vertices[i].col[1] = static_cast<uint32_t>(mesh.mColors[0][i].g);
            out.vertices[i].col[2] = static_cast<uint32_t>(mesh.mColors[0][i].b);
            out.vertices[i].col[3] = static_cast<uint32_t>(mesh.mColors[0][i].a);
        }

    }

    if (mesh.HasFaces()) {
        std::vector<uint32_t> indices;
        for (unsigned int i = 0; i < mesh.mNumFaces; i++) {
            const aiFace& face = mesh.mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        out.index_count = mesh.mNumVertices;
        out.indices     = new uint32_t[out.vertex_count];

        for (int i = 0; i < out.index_count; i++) {
            out.indices[i] = indices[i];
        }
    }
}

void write_mesh_to_header(const mesh_t& mesh, const std::string& mesh_name, const std::string& dir) {
    std::string filename = dir + "/" + mesh_name + ".h";
    std::ofstream output(filename);

    if (!output) {
        std::cerr << "Cannot open output file: " << filename << std::endl;
        return;
    }

    output << "#pragma once\n";
    output << "#include <inttypes.h>\n";
    output << "const uint32_t " + mesh_name + "_vertex_count = " << mesh.vertex_count << "; \n";
    output << "const uint32_t " + mesh_name + "_index_count = " << mesh.index_count << ";\n";

    output << "const vertex_t " + mesh_name + "_vertices[] = {";
    for (uint32_t i = 0; i < mesh.vertex_count; i++) {
        output << "{";
        output << "{" << mesh.vertices[i].pos[0] << "," << mesh.vertices[i].pos[1] << "," << mesh.vertices[i].pos[2] << "},";
        output << "{" << mesh.vertices[i].nor[0] << "," << mesh.vertices[i].nor[1] << "," << mesh.vertices[i].nor[2] << "},";
        output << "{" << mesh.vertices[i].uv[0] << "," << mesh.vertices[i].uv[1] << "},";
        output << "{" << mesh.vertices[i].col[0] << "," << mesh.vertices[i].col[1] << "," << mesh.vertices[i].col[2] << "," << mesh.vertices[i].col[3] << "}";
        output << "},";
    }
    output << "};\n";

    output << "const uint32_t " + mesh_name + "_indices[] = {";
    for (uint32_t i = 0; i < mesh.index_count; i++) {
        output << mesh.indices[i];
        if (i < mesh.index_count - 1) output << ",";
       // output << "\n";
    }
    output << "};\n";

    output.close();
}

void write_mesh_to_bin(const mesh_t& mesh, const std::string& mesh_name, const std::string& dir) {
    std::string filename = dir + "/" + mesh_name + ".mesh";
    std::ofstream output(filename);

    if (!output) {
        std::cerr << "Cannot open output file: " << filename << std::endl;
        return;
    }
    
    output << mesh.vertex_count;
    output << mesh.vertices;
    output << mesh.index_count;
    output << mesh.indices;
    output.close();

}

int main(int argc, const char* argv[]) {
    if (argc <= 1) {
        return 1;
    }

    bool flags[FLAGS::COUNT] = {};
    std::vector<std::string> cmds;
    for (uint32_t i = 1; i < argc; i++) {
        std::string cmd = argv[i];
        if (cmd == "-h") {
            flags[FLAGS::HEADER] = true;
        }
        else if (cmd == "-b") {
            flags[FLAGS::BINARY] = true;
        }
    }

    app_config_t config = {};
    config.app_name = "converter";
    config.version = version_t(1, 0, 0, 0);
    app_t::load(config);

    Assimp::Importer importer;

    std::string dir = app_t::get().current_path;
    std::string name = argv[1];
    std::string path = dir + '/' + name;

    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType |
        aiProcess_ImproveCacheLocality |
        aiProcess_RemoveRedundantMaterials |
        aiProcess_FixInfacingNormals |
        aiProcess_FindDegenerates |
        aiProcess_FindInvalidData |
        aiProcess_GenSmoothNormals
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Error: " << importer.GetErrorString() << std::endl;
        return -1;
    }

    aiMesh* mesh = scene->mMeshes[0]; 


    for (uint32_t i = 0; i < scene->mNumMeshes; i++) {
        mesh_t mesh;
        convert_mesh(*scene->mMeshes[i], mesh);

        if (flags[FLAGS::BINARY]) {
            write_mesh_to_bin(mesh, scene->mMeshes[i]->mName.C_Str(), dir);
        }
        else {
            write_mesh_to_header(mesh, scene->mMeshes[i]->mName.C_Str(), dir);
        }
    }
   

    return 0;
}