#include "entity.h"
#include "mesh.h"
#include "image.h"
#include <cstring> 

Entity::Entity() {
    modelMatrix.SetIdentity();
    this->mesh = mesh;
    this->translate = false;
    this->rotate = false;
    this->escalate = false;
    this->scalingUp = true;
    this->movingUp = true;

}

Entity::Entity(Matrix44 modelMatrix) {
    setModelMatrix(modelMatrix);
}

Entity::Entity(Matrix44 modelMatrix, Mesh mesh) {
    setModelMatrix(modelMatrix);
}

Entity::Entity(Matrix44 modelMatrix, Mesh mesh, Texture* texture, Shader* shader) {
    setModelMatrix(modelMatrix);
    this->texture = texture;
    this->shader = shader;
}

void Entity::setModelMatrix(Matrix44 modelMatrix) {
    this->modelMatrix = modelMatrix;
}


void Entity::setMesh(Mesh mesh) {
    this->mesh = mesh;
}

Matrix44 Entity::getModelMatrix() {
    return modelMatrix;
}

Mesh Entity::getMesh() {
    return mesh;
}

void Entity::setRotate(bool rotate) {
    this->rotate = rotate;
}

void Entity::setTranslate(bool translate) {
    this->translate = translate;
}

void Entity::setEscalate(bool escalate) {
    this->escalate = escalate;
}

void Entity::setTranslationSpeed(float value) {
    this->translationSpeed = value;
}

/*
void Entity::Render(Image* framebuffer, Camera* camera, const Color& c, FloatImage* zBuffer,Image* texture, float c1, float z1, float t1) {

    const std::vector<Vector3>& meshVertices = mesh.GetVertices();

    const std::vector<Vector2>& uvs = mesh.GetUVs();

    for (size_t i = 0; i < meshVertices.size(); i += 3) {
        Vector3 vec1;
        Vector3 vec2;
        Vector3 vec3;

        //PAS 1: LOCAL SPACE TO WORLD SPACE -------------------------------------------------------------------------------
        vec1 = modelMatrix * mesh.GetVertices()[i];
        vec2 = modelMatrix * mesh.GetVertices()[i + 1];
        vec3 = modelMatrix * mesh.GetVertices()[i + 2];
        //-----------------------------------------------------------------------------------------------------------------

        //PAS 2: WORLD SPACE TO VIEW SPACE TO CLIP SPACE ------------------------------------------------------------------
        bool negZ0, negZ1, negZ2;
        Vector3 clipPos0 = camera->ProjectVector(vec1, negZ0);
        Vector3 clipPos1 = camera->ProjectVector(vec2, negZ1);
        Vector3 clipPos2 = camera->ProjectVector(vec3, negZ2);

        //Si algun vèrtex surt del frustum skipegem el triangle sencer.
        if (negZ0 || negZ1 || negZ2) {
            continue;
        }
        //-----------------------------------------------------------------------------------------------------------------

        //PAS 3:CLIP SPACE TO SCREEN SPACE ----------------------------------------------------------------------------------
        int Width = framebuffer->width;
        int Height = framebuffer->height;
        Vector3 screenPos0 = Vector3((clipPos0.x + 1.0f) * 0.5f * Width, (clipPos0.y + 1.0f) * 0.5f * Height, clipPos0.z);
        Vector3 screenPos1 = Vector3((clipPos1.x + 1.0f) * 0.5f * Width, (clipPos1.y + 1.0f) * 0.5f * Height, clipPos0.z);
        Vector3 screenPos2 = Vector3((clipPos2.x + 1.0f) * 0.5f * Width, (clipPos2.y + 1.0f) * 0.5f * Height, clipPos0.z);

        Vector3 v1 = Vector3(screenPos0.x, screenPos0.y, screenPos0.z);
        Vector3 v2 = Vector3(screenPos1.x, screenPos1.y, screenPos0.z);
        Vector3 v3 = Vector3(screenPos2.x, screenPos2.y, screenPos0.z);

        /*
        Vector2 uv_0 = uvs[i];
        Vector2 uv_1 = uvs[i+1];
        Vector2 uv_2 = uvs[i+2];


        Vector2 uv_0 = Vector2(uv_0.x * (framebuffer->width - 1), uv_0.y * (framebuffer->height - 1));
        Vector2 uv_1 = Vector2(uv_1.x * (framebuffer->width - 1), uv_1.y * (framebuffer->height - 1));
        Vector2 uv_2 = Vector2(uv_2.x * (framebuffer->width - 1), uv_2.y * (framebuffer->height - 1));

        if (c1 == true) {
            if (z1 == true) {
                /*
                if (t1 == true) {
                    framebuffer->DrawTriangleInterpolated4(v1, v2, v3, Color::RED, Color::GREEN, Color::BLUE, zBuffer, texture, uv_0, uv_1, uv_2);
                }else{

                    framebuffer->DrawTriangleInterpolated3(v1, v2, v3, Color::RED, Color::GREEN, Color::BLUE, zBuffer);
            }
            else {
                framebuffer->DrawTriangleInterpolated2(v1, v2, v3, Color::RED, Color::GREEN, Color::BLUE);

            }
        }
        else {
            //printf("c1 false");
            Vector2 v12 = Vector2(screenPos0.x, screenPos0.y);
            Vector2 v22 = Vector2(screenPos1.x, screenPos1.y);
            Vector2 v32 = Vector2(screenPos2.x, screenPos2.y);
            framebuffer->DrawTriangle(v12, v22, v32, c, true, c);

        }
    }
}
*/
void Entity::Render(Camera* camera) {
    shader->Enable();
    shader->SetMatrix44("u_model", modelMatrix);
    shader->SetMatrix44("u_viewprojection", camera->viewprojection_matrix);
    shader->SetTexture("u_face_texture", texture);
    mesh.Render(); // falta entrar paràmetre però no se exactament quin. És un int
    shader->Disable();

}
void Entity::Update(float seconds_elapsed) {
    if (rotate == true) {
        Vector3 rotation_axis(0.0f, 1.0f, 0.0f);
        modelMatrix.RotateLocal(seconds_elapsed * (PI / 10.0f), rotation_axis);
    }
    if (escalate == true) {
        if (scalingUp) {
            if (modelMatrix._11 < 2.0f) {
                modelMatrix._11 += seconds_elapsed;
                modelMatrix._22 += seconds_elapsed;
                modelMatrix._33 += seconds_elapsed;
            }
            else {
                scalingUp = false;
            }
        }
        else {
            if (modelMatrix._11 > 0.3f) {
                modelMatrix._11 -= seconds_elapsed;
                modelMatrix._22 -= seconds_elapsed;
                modelMatrix._33 -= seconds_elapsed;
            }
            else {
                scalingUp = true;
            }
        }
    }
    if (translate) {
        float translationDistance = translationSpeed * seconds_elapsed;

        if (movingUp) {
            modelMatrix.Translate(0.0f, translationDistance, 0.0f);
        }
        else {
            modelMatrix.Translate(0.0f, -translationDistance, 0.0f);
        }
        if (modelMatrix.GetTranslation().y >= 0.3f) {
            movingUp = false;
        }
        else if (modelMatrix.GetTranslation().y <= -0.2f) {
            movingUp = true;
        }
    }
}
