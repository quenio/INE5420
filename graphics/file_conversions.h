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

inline list<shared_ptr<DisplayFile<Coord3D>::Command>> as_display_commands(const Obj::File &file)
{
    vector<shared_ptr<Obj::Vertex>> vertices = file.vertices();
    list<shared_ptr<DisplayFile<Coord3D>::Command>> commands;
    list<Segment3D> segments;

    for (auto &face: file.faces())
    {
        face_to_segments(segments, *face, vertices);
    }

    const shared_ptr<Object3D> object = make_shared<Object3D>(segments);

    object->transform(y_rotation(30) * x_rotation(45) * scaling(1.6, 1.6, 1.6) * translation(-0.5, -0.5, +10));

    commands.push_back(make_shared<Draw3DCommand>(object));

    return commands;
}

