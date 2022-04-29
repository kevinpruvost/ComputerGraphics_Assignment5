/*****************************************************************//**
 * \file   Quaternion.cpp
 * \brief  Quaternion source code
 * 
 * \author Kevin Pruvost (pruvostkevin0@gmail.com)
 * \date   April, 10 2022
 *********************************************************************/
#include "Quaternion.hpp"

Quaternion::Quaternion(const glm::vec3 & rotation)
    : glm::quat(glm::radians(rotation))
{
}

Quaternion::~Quaternion()
{
}

Quaternion & Quaternion::operator=(const glm::quat & quat)
{
    *(dynamic_cast<glm::quat *>(this)) = quat;
    return *this;
}

Quaternion::operator glm::quat & ()
{
    return *(dynamic_cast<glm::quat *>(this));
}

Quaternion::operator const glm::quat & () const
{
    return *(dynamic_cast<const glm::quat *>(this));
}

void Quaternion::Rotate(const glm::vec3 & rotation)
{
    if (abs(rotation.x) > FLT_EPSILON) RotateX(rotation.x);
    if (abs(rotation.y) > FLT_EPSILON) RotateY(rotation.y);
    if (abs(rotation.z) > FLT_EPSILON) RotateZ(rotation.z);
}

void Quaternion::RotateX(const float rotation)
{
    constexpr const glm::vec3 X(1.0f, 0.0f, 0.0f);
    *this = glm::rotate(*this, glm::radians(rotation), X);
//    eulerAngles.x = std::fmod(eulerAngles.x + rotation, 360.0f - std::numeric_limits<float>::epsilon());
}

void Quaternion::RotateY(const float rotation)
{
    constexpr const glm::vec3 Y(0.0f, 1.0f, 0.0f);
    *this = glm::rotate(*this, glm::radians(rotation), Y);
//    eulerAngles.y = std::fmod(eulerAngles.y + rotation, 360.0f - std::numeric_limits<float>::epsilon());
}

void Quaternion::RotateZ(const float rotation)
{
    constexpr const glm::vec3 Z(0.0f, 0.0f, 1.0f);
    *this = glm::rotate(*this, glm::radians(rotation), Z);
//    eulerAngles.z = std::fmod(eulerAngles.z + rotation, 360.0f - std::numeric_limits<float>::epsilon());
}