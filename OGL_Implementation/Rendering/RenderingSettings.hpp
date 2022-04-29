/*****************************************************************//**
 * \file   RenderingSettings.hpp
 * \brief  Rendering enum & settings code related
 * 
 * \author Kevin Pruvost (pruvostkevin0@gmail.com)
 * \date   April, 16 2022
 *********************************************************************/
#pragma once

// GLM includes
#include <glm\glm.hpp>

// C++ includes
#include <array>

enum RenderingMode
{
    VerticesMode          = 0b001,
    WireframeMode         = 0b010,
    FacesMode             = 0b100,
    FacesAndWireframeMode = 0b110
};

// Display Mode
extern RenderingMode DisplayMode; // 0 = Point, 1 = Face, 2 = Wireframe, 3 = Face + Wireframe
extern std::array<glm::vec3, 4> WireframeColors;