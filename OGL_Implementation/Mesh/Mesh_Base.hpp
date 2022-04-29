/*****************************************************************//**
 * \file   Mesh_Base.hpp
 * \brief  Mesh_Base class
 * 
 * \author Kevin Pruvost (pruvostkevin0@gmail.com)
 * \date   April, 03 2022
 *********************************************************************/
#pragma once

// GLAD includes
#include <GLAD\glad.h>

/**
 * @brief Contains the real data of the mesh to be
 * stored in a static database.
*/
class Mesh_Base
{
protected:
    Mesh_Base();
public:
    virtual ~Mesh_Base();

    GLuint GetVerticesVAO() const;
    GLuint GetFacesVAO() const;
    GLuint GetVerticesVBO() const;
    GLuint GetFacesVBO() const;
    GLuint GetVerticesCount() const;
    GLuint GetFacesVerticesCount() const;

    virtual GLuint GetFacesEBO() const = 0;
    virtual bool IsUsingEBO() const = 0;

    enum class DrawMode : unsigned char
    {
        DrawArrays = 0,
        DrawElements = 1
    };
    virtual DrawMode GetDrawMode() const = 0;

private:

protected:
    GLuint __verticesVAO, __facesVAO;
    GLuint __verticesVBO, __facesVBO;
    GLuint __verticesNVert, __facesNVert;
};
