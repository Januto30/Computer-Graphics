#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "GL/glew.h"
#include "../extra/picopng.h"
#include "image.h"
#include "utils.h"
#include "camera.h"
#include "mesh.h"
#include <cmath>
#include "button.h"

Button::Button(const char* direc, const Vector2& position) : position(position) {
    if (!buttonImage.LoadPNG(direc)) {
        std::cerr << "Error loading image from file: " << direc << std::endl;
    }
}

Image Button::getImage() {
    return buttonImage;
}



bool Button::IsMouseInside(const Vector2& mousePosition) const {
    return (mousePosition.x >= position.x && mousePosition.x <= position.x + buttonImage.width &&
        mousePosition.y >= position.y && mousePosition.y <= position.y + buttonImage.height);
}
