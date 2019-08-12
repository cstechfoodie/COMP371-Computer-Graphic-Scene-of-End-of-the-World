//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 28/1/15
//
// Copyright (c) 2014-2015 Concordia University. All rights reserved.
//

#include "BSplineCamera.h"
#include "EventManager.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>
#include <algorithm>


using namespace glm;

BSplineCamera::BSplineCamera(BSpline* spline, float speed)
    : Camera(), mpSpline(spline), mSpeed(speed), mSplineParameterT(0.0f)
{
    assert(spline != nullptr);
    mPosition = mpSpline->GetPosition(mSplineParameterT);
    
    mLookAt = vec3(0.0f, 0.0f, -1.0f);
    mUp = vec3(0.0f, 1.0f, 0.0f);
}







BSplineCamera::~BSplineCamera()
{
}

void BSplineCamera::Update(float dt)
{
    // @TODO - Using the BSpline class, update the position on the spline
    //         considering the speed of the camera
    //       - Set the mLookAt vector pointing in front (tangent on spline)
    
    mLookAt = mpSpline->GetTangent(mSplineParameterT);
    
    float tangentMagnitude = length(mLookAt);
    mLookAt = normalize(mLookAt);
    
    mSplineParameterT += mSpeed * dt / tangentMagnitude;
    mPosition = mpSpline->GetPosition(mSplineParameterT);
    
//    mSplineParameterT += dt;
//    mPosition = mpSpline->GetPosition(mSplineParameterT);
}

glm::mat4 BSplineCamera::GetViewMatrix() const
{
    return glm::lookAt(mPosition, mLookAt, mUp);
}
