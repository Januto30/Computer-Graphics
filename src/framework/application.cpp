#include "application.h"
#include "mesh.h"
#include "shader.h"
#include "utils.h" 
#include "button.h"
#include "framework.h"
#include "particlesystem.h"
#include "entity.h"
#include "camera.h"

int a1 = 1;

float a, b, c, d, e, f, lletra, u_aspectRatio, aug;
bool c1 = false;
bool z1 = false;
bool t1 = false;
bool ind = false;
bool mult = false;
bool tecla = false;

Image* textura1 = new Image();
Image* textura2 = new Image();
Image* textura3 = new Image();
Shader* shader = nullptr;

Camera camera = Camera();

Entity myEntity = Entity();
Entity myEntity2 = Entity();
Entity myEntity3 = Entity();
Entity myEntity4 = Entity();

Mesh quad = Mesh();
Mesh myMesh = Mesh();
Mesh myMesh2 = Mesh();
Mesh myMesh3 = Mesh();
Mesh myMesh4 = Mesh();

Matrix44 modelMatrix;
Matrix44 modelMatrix2;
Matrix44 modelMatrix3;
Matrix44 modelMatrix4;

//-----------------
Shader* s = new Shader();
Texture* t = new Texture();
Texture* face_texture = new Texture();
//-----------------

Application::Application(const char* caption, int width, int height)
{
	this->window = createWindow(caption, width, height);

	int w, h;
	SDL_GetWindowSize(window, &w, &h);

	this->mouse_state = 0;
	this->time = 0.f;
	this->window_width = w;
	this->window_height = h;
	this->keystate = SDL_GetKeyboardState(nullptr);

	this->framebuffer.Resize(w, h);


}

Application::~Application()
{

}

void Application::Init(void)
{
	a = false;
	lletra = 3.0f;
	aug = 0.f;
	s = Shader::Get("shaders/quad.vs", "shaders/quad.fs");
	quad.CreateQuad();

	//Exercici filtres:
	t = Texture::Get("images/fruits.png");

	//Exercici mesh
	shader = Shader::Get("shaders/raster.vs", "shaders/raster.fs");
	face_texture = Texture::Get("textures/lee_color_specular.tga");
	myMesh.LoadOBJ("meshes/lee.obj");
	Entity myEntity = Entity(modelMatrix, myMesh, face_texture, shader);



	/*
	camera.right = framebuffer.width / 2;
	camera.left = -framebuffer.width / 2;
	camera.top = framebuffer.height / 2;
	camera.bottom = -framebuffer.height / 2;


	//Assignem objectes a les nostres malles
	std::cout << "Initiating app..." << std::endl;
	myMesh.LoadOBJ("meshes/cleo.obj");
	myMesh2.LoadOBJ("meshes/anna.obj");
	myMesh3.LoadOBJ("meshes/lee.obj");
	myMesh4.LoadOBJ("meshes/lee.obj");

	textura2->LoadTGA("textures/lee_color_specular.tga", false);
	textura1->LoadTGA("textures/cleo_color_specular.tga", false);
	textura3->LoadTGA("textures/anna_color_specular.tga", false);

	myEntity4.setTexture(*textura2);
	myEntity3.setTexture(*textura2);
	myEntity2.setTexture(*textura3);
	myEntity.setTexture(*textura1);

	//Assignem malles a les respectives entitats
	myEntity.setMesh(myMesh);
	myEntity2.setMesh(myMesh2);
	myEntity3.setMesh(myMesh3);
	myEntity4.setMesh(myMesh4);


	//ROTACIÓ-------------------------------------------------------------
	myEntity3.setRotate(true);
	myEntity2.setEscalate(true);
	myEntity.setTranslate(true);
	myEntity.setTranslationSpeed(1.0f);
	Vector3 rotation_axis(0.0f, 1.0f, 0.0f);
	modelMatrix4.RotateLocal(1 * (PI / 10.0f), rotation_axis);

	//BUFFER--------------------------------------------------------------
	zBuffer = new FloatImage(this->window_width, this->window_height);

	//TRANSLACIÓ----------------------------------------------------------
	modelMatrix.SetIdentity();
	modelMatrix2.SetIdentity();
	modelMatrix3.SetIdentity();

	//TRANSLACIÓ----------------------------------------------------------
	modelMatrix.Translate(-0.65, 0, 0);
	modelMatrix3.Translate(-0.1, -0.8, -1.0);
	modelMatrix2.Translate(+0.7, -0.2, 0);
	modelMatrix4.Translate(-0.1, -0.8, -8.0);

	//ESCALA--------------------------------------------------------------
	modelMatrix._11 = 1.2;
	modelMatrix._22 = 1.2;
	modelMatrix._33 = 1.2;

	modelMatrix3._11 = 2.2;
	modelMatrix3._22 = 2.2;
	modelMatrix3._33 = 2.2;

	modelMatrix4._11 = 3;
	modelMatrix4._22 = 3;
	modelMatrix4._33 = 3;

	myEntity.setModelMatrix(modelMatrix);
	myEntity2.setModelMatrix(modelMatrix2);
	myEntity3.setModelMatrix(modelMatrix3);
	myEntity4.setModelMatrix(modelMatrix4);

	//CÀMERA--------------------------------------------------------------
	//camera.SetPerspective(45 o en radiants, framebuffer.width/framebuffer.height, 0.01f, 1000.0f);
	camera.SetOrthographic(camera.left, camera.right, camera.top, camera.bottom, camera.near_plane, camera.far_plane);
	*/

}

void Application::Render(void)
{
	if (lletra != 4.0) {
		s->Enable();
		s->SetFloat("u_aspectRatio", framebuffer.width / framebuffer.height);
		s->SetTexture("u_texture", t);
		s->SetFloat("aug_value", aug);
		quad.Render();
		Option();
		s->Disable();
	}
	if (lletra == 4.0) {
		glEnable(GL_DEPTH_TEST);
		myEntity.Render(&camera);
	}

	/*
	zBuffer->Fill(INT_MAX);

	if (ind == true) {	//Tecla "1"
		myEntity4.Render(&framebuffer, &camera, Color::PURPLE, tecla, zBuffer, c1,z1,t1);

	}
	if (mult == true) {	//Tecla "2"
		myEntity.Render(&framebuffer, &camera, Color::BLUE, tecla, zBuffer, c1, z1, t1);
		myEntity2.Render(&framebuffer, &camera, Color::GREEN,tecla, zBuffer, c1, z1, t1);
		myEntity3.Render(&framebuffer, &camera, Color::RED, tecla, zBuffer, c1, z1, t1);
	}
	framebuffer.Render();
	*/
}

void Application::Update(float seconds_elapsed)
{
	/*
	framebuffer.Fill(Color::BLACK);
	myEntity.Update(0.01f);
	myEntity2.Update(0.05f);
	myEntity3.Update(0.1f);
	*/
}

void Application::OnKeyPressed(SDL_KeyboardEvent event)
{
	// KEY CODES: https://wiki.libsdl.org/SDL2/SDL_Keycode
	switch (event.keysym.sym) {
	case SDLK_ESCAPE: exit(0); break; // ESC key, kill the app
	case SDLK_1:printf("1"); lletra = 1.0f; break;
	case SDLK_2:printf("2"); lletra = 2.0f; break;
	case SDLK_3:printf("3"); lletra = 3.0f; break;
	case SDLK_4:printf("4"); lletra = 4.0f; break;


	case SDLK_a:printf("a"); a = true; b = c = d = e = f = false; break;
	case SDLK_b:printf("b"); b = true; a = c = d = e = f = false; break;
	case SDLK_c:printf("c"); c = true; b = a = d = e = f = false; break;
	case SDLK_d:printf("d"); d = true; b = c = a = e = f = false; break;
	case SDLK_e:printf("e"); e = true; b = c = d = a = f = false; break;
	case SDLK_f:printf("f"); f = true; b = c = d = e = a = false; break;

	case SDLK_p:printf("p"); aug += 100.0f; break;
	case SDLK_o:printf("o"); aug -= 100.0f; break;

	}
}

void Application::OnMouseButtonDown(SDL_MouseButtonEvent event) {

	if (event.button == SDL_BUTTON_LEFT) {

	}

}

void Application::OnMouseButtonUp(SDL_MouseButtonEvent event)
{
	if (event.button == SDL_BUTTON_LEFT) {

	}
}


void Application::OnMouseMove(SDL_MouseButtonEvent event)
{
	if (event.button == SDL_BUTTON_LEFT) {
		camera.Orbit(-mouse_delta.x * 0.01, Vector3::UP);
		camera.Orbit(-mouse_delta.y * 0.01, Vector3::RIGHT);
	}
}

void Application::OnWheel(SDL_MouseWheelEvent event)
{
	float dy = event.preciseY;
	camera.Zoom(dy < 0 ? 1.1 : 0.9);
}


void Application::OnFileChanged(const char* filename)
{
	Shader::ReloadSingleShader(filename);

}

void Application::Option() {
	if (lletra == 1) {
		s->SetFloat("lletra_value", 1.0f);
	}
	else if (lletra == 2) {
		s->SetFloat("lletra_value", 2.0f);
	}
	else if (lletra == 3) {
		s->SetFloat("lletra_value", 3.0f);
	}

	if (a == true) {
		s->SetFloat("b_value", 0.0f);
		s->SetFloat("c_value", 0.0f);
		s->SetFloat("d_value", 0.0f);
		s->SetFloat("e_value", 0.0f);
		s->SetFloat("f_value", 0.0f);

		s->SetFloat("u_value", 1.0f);
	}
	else if (b == true) {
		s->SetFloat("u_value", 0.0f);
		s->SetFloat("c_value", 0.0f);
		s->SetFloat("d_value", 0.0f);
		s->SetFloat("e_value", 0.0f);
		s->SetFloat("f_value", 0.0f);

		s->SetFloat("b_value", 1.0f);
	}
	else if (c == true) {
		s->SetFloat("b_value", 0.0f);
		s->SetFloat("u_value", 0.0f);
		s->SetFloat("d_value", 0.0f);
		s->SetFloat("e_value", 0.0f);
		s->SetFloat("f_value", 0.0f);

		s->SetFloat("c_value", 1.0f);
	}
	else if (d == true) {
		s->SetFloat("b_value", 0.0f);
		s->SetFloat("c_value", 0.0f);
		s->SetFloat("u_value", 0.0f);
		s->SetFloat("e_value", 0.0f);
		s->SetFloat("f_value", 0.0f);

		s->SetFloat("d_value", 1.0f);
	}
	else if (e == true) {
		s->SetFloat("b_value", 0.0f);
		s->SetFloat("c_value", 0.0f);
		s->SetFloat("d_value", 0.0f);
		s->SetFloat("u_value", 0.0f);
		s->SetFloat("f_value", 0.0f);

		s->SetFloat("e_value", 1.0f);
	}
	else if (f == true) {
		s->SetFloat("b_value", 0.0f);
		s->SetFloat("c_value", 0.0f);
		s->SetFloat("d_value", 0.0f);
		s->SetFloat("e_value", 0.0f);
		s->SetFloat("u_value", 0.0f);

		s->SetFloat("f_value", 1.0f);
	}
}