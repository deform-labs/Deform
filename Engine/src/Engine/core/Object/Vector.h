// Vector.h
#pragma once

#include <Engine/EngineAPI.h>

// Two dimensional vector (X, Y) 
struct DEFORM_API Vector2D
{

    // This struct represents a two-dimensional vector with X and Y components. It includes constructors for initializing the vector and getter methods for accessing the individual components.
    Vector2D();

    // Constructor that initializes the two-dimensional vector with the specified X and Y values.
    Vector2D(float x, float y);

    // the X and Y components of the vector, which represent its position or direction in 2D space. These are public member variables that can be accessed directly or through the provided getter methods.
    float X, Y;

    // Gets the X component of the vector. This method returns the value of the X component, which can be used to determine the horizontal position or direction represented by the vector.
    float GetX() const;
    
    // Gets the Y component of the vector. This method returns the value of the Y component, which can be used to determine the vertical position or direction represented by the vector.
    float GetY() const;
};

 // This struct represents a three-dimensional vector with X, Y, and Z components. It includes constructors for initializing the vector and getter methods for accessing the individual components.
struct DEFORM_API Vector3D
{
    // this is the default constructor for the Vector3D struct. It initializes the X, Y, and Z components of the vector to default values (likely zero). This allows you to create a Vector3D instance without providing specific values for its components.
    Vector3D();

    // This constructor initializes the three-dimensional vector with the specified X, Y, and Z values. It allows you to create a Vector3D instance with specific coordinates or direction in 3D space.
    Vector3D(float x, float y, float z);

    // the X, Y, and Z components of the vector, which represent its position or direction in 3D space. These are public member variables that can be accessed directly or through the provided getter methods.
    float X, Y, Z;

    // gets the X component of the vector. This method returns the value of the X component, which can be used to determine the horizontal position or direction represented by the vector in 3D space.
    float GetX() const;
    // gets the Y component of the vector. This method returns the value of the Y component, which can be used to determine the vertical position or direction represented by the vector in 3D space.
    float GetY() const;
    // gets the Z component of the vector. This method returns the value of the Z component, which can be used to determine the depth position or direction represented by the vector in 3D space.
    float GetZ() const;
};