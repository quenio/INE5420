#pragma once

#include "obj.h"
#include "display.h"

inline Coord3D coord_3d(const Obj::Vertex &vertex)
{
    return Coord3D(vertex.x(), vertex.y(), vertex.z());
}

inline void face_to_segments(list<Segment3D> &segments, Obj::Face &face, const vector<shared_ptr<Obj::Vertex>> &vertices)
{
    list<size_t> &references = face.references();
    shared_ptr<Obj::Vertex> previous = vertices[references.back()-1];

    for (size_t ref: face.references())
    {
        const shared_ptr<Obj::Vertex> &current = vertices[ref-1];

        segments.push_back({ coord_3d(*previous), coord_3d(*current) });

        previous = current;
    }
}

inline shared_ptr<Object3D> as_object_3d(const Obj::File &file)
{
    const vector<shared_ptr<Obj::Vertex>> vertices = file.vertices();
    const list<shared_ptr<Obj::Face>> faces = file.faces();

    printf("Vertices: %lu\n", vertices.size());
    printf("Faces: %lu\n", faces.size());

    list<Segment3D> segments;
    for (auto &face: faces)
    {
        face_to_segments(segments, *face, vertices);
    }

    printf("Segments: %lu\n", segments.size());

    return make_shared<Object3D>(segments);
}

inline vector<shared_ptr<Coord3D>> as_vertices(const vector<shared_ptr<Obj::Vertex>> &file_vertices)
{
    vector<shared_ptr<Coord3D>> vertices;

    for (auto &file_vertex: file_vertices)
    {
        vertices.push_back(make_shared<Coord3D>(coord_3d(*file_vertex)));
    }

    return vertices;
}

inline shared_ptr<Face3D> as_face(Obj::Face &face, const vector<shared_ptr<Coord3D>> &vertices)
{
    list<shared_ptr<Coord3D>> face_vertices;

    for (size_t ref: face.references())
    {
        assert(ref <= vertices.size());

        face_vertices.push_back(vertices[ref-1]);
    }

    return make_shared<Face3D>(face_vertices);
}

inline vector<shared_ptr<Face3D>> as_faces(const vector<shared_ptr<Coord3D>> &vertices, const list<shared_ptr<Obj::Face>> &file_faces)
{
    vector<shared_ptr<Face3D>> faces;

    for (auto &file_face: file_faces)
    {
        faces.push_back(as_face(*file_face, vertices));
    }

    return faces;
}

inline shared_ptr<Group3D> as_group_3d(const Obj::File &file)
{
    const vector<shared_ptr<Obj::Vertex>> file_vertices = file.vertices();
    const list<shared_ptr<Obj::Face>> file_faces = file.faces();

    printf("File Vertices: %lu\n", file_vertices.size());
    printf("File Faces: %lu\n", file_faces.size());

    const vector<shared_ptr<Coord3D>> vertices = as_vertices(file_vertices);
    const vector<shared_ptr<Face3D>> faces = as_faces(vertices, file_faces);

    printf("Group Vertices: %lu\n", vertices.size());
    printf("Group Faces: %lu\n", faces.size());

    return make_shared<Group3D>(vertices, faces);
}

inline list<shared_ptr<DisplayFile<Coord3D>::Command>> as_display_commands(shared_ptr<Draw3DCommand::Object> object)
{
    list<shared_ptr<DisplayFile<Coord3D>::Command>> commands;

    commands.push_back(make_shared<Draw3DCommand>(object));

    return commands;
}

