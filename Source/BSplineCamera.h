//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 28/1/15
//
// Copyright (c) 2014-2015 Concordia University. All rights reserved.
//

#pragma once

#include "Camera.h"
#include "BSpline.hpp"

class BSplineCamera : public Camera
{
public:
    BSplineCamera(BSpline* spline, float speed);
    virtual ~BSplineCamera();

    virtual void Update(float dt);
    virtual glm::mat4 GetViewMatrix() const;

private:
    glm::vec3 mPosition;
    glm::vec3 mLookAt;
    glm::vec3 mUp;

    BSpline* mpSpline;
    float mSplineParameterT;
    float mSpeed;
};
