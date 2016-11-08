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

inline list<shared_ptr<DisplayFile<Coord3D>::Command>> as_display_commands(shared_ptr<Object3D> object)
{
    list<shared_ptr<DisplayFile<Coord3D>::Command>> commands;

    commands.push_back(make_shared<Draw3DCommand>(object));

    return commands;
}

