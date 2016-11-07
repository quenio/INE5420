#pragma once

#include "obj.h"
#include "display.h"

inline Coord3D coord_3d(const Obj::Vertex &vertex)
{
    return Coord3D(vertex.x(), vertex.y(), vertex.z());
}

inline list<Segment3D> face_to_segments(Obj::Face &face, const vector<shared_ptr<Obj::Vertex>> &vertices)
{
    list<Segment3D> segments;
    list<size_t> &references = face.references();
    shared_ptr<Obj::Vertex> previous = vertices[references.back()-1];

    for (size_t ref: face.references())
    {
        const shared_ptr<Obj::Vertex> &current = vertices[ref-1];

        segments.push_back({ coord_3d(*previous), coord_3d(*current) });

        previous = current;
    }

    return segments;
}

inline list<shared_ptr<DisplayFile<Coord3D>::Command>> as_display_commands(const Obj::File &file)
{
    vector<shared_ptr<Obj::Vertex>> vertices = file.vertices();
    list<shared_ptr<DisplayFile<Coord3D>::Command>> commands;

    for (auto &face: file.faces())
    {
        commands.push_back(make_shared<Draw3DCommand>(make_shared<Object3D>(face_to_segments(*face, vertices))));
    }

    return commands;
}

