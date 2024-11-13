#pragma once

#include <string.h>
#include <stdio.h>
#include <iostream>
#include "framework.h"
#include "image.h"


//remove unsafe warnings
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)
#endif

class Button {
private:
    Image buttonImage;
    Vector2 position;

public:
    Button(const char* direc, const Vector2& position);

    bool IsMouseInside(const Vector2& mousePosition) const;

    Image getImage();
};

