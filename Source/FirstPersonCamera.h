//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#pragma once

#include "Camera.h"

class FirstPersonCamera : public Camera
{
public:
	FirstPersonCamera(glm::vec3 position);
	virtual ~FirstPersonCamera();

	virtual void Update(float dt);

	virtual glm::mat4 GetViewMatrix() const;
    
    virtual void CameraSkake(float dt);
    
    virtual void ToggleShakeFlag();
    virtual void SetShakeFlag(bool flag);

private:
	glm::vec3 mPosition;
    glm::vec3 mPositionCache;
	float mHorizontalAngle; // horizontal angle
	float mVerticalAngle;   // vertical angle

	float mSpeed;			// World units per second
	float mAngularSpeed;    // Degrees per pixel
    
    bool mSkakeFlag;
    
    float mAmplitude = 5.0f;
    float mAdjustAmplitudePerSecond = -1.0f;
    float mTimeElapsed = 0.0f;

	glm::vec3 mLookAt;
};
