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
#include "image.h"


Image::Image() {
	width = 0; height = 0;
	pixels = NULL;
}

Image::Image(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;
	pixels = new Color[width*height];
	memset(pixels, 0, width * height * sizeof(Color));
}

// Copy constructor
Image::Image(const Image& c)
{
	pixels = NULL;
	width = c.width;
	height = c.height;
	bytes_per_pixel = c.bytes_per_pixel;
	if(c.pixels)
	{
		pixels = new Color[width*height];
		memcpy(pixels, c.pixels, width*height*bytes_per_pixel);
	}
}

// Assign operator
Image& Image::operator = (const Image& c)
{
	if(pixels) delete pixels;
	pixels = NULL;

	width = c.width;
	height = c.height;
	bytes_per_pixel = c.bytes_per_pixel;

	if(c.pixels)
	{
		pixels = new Color[width*height*bytes_per_pixel];
		memcpy(pixels, c.pixels, width*height*bytes_per_pixel);
	}
	return *this;
}

Image::~Image()
{
	if(pixels) 
		delete pixels;
}

void Image::Render()
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glDrawPixels(width, height, bytes_per_pixel == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}

// Change image size (the old one will remain in the top-left corner)
void Image::Resize(unsigned int width, unsigned int height)
{
	Color* new_pixels = new Color[width*height];
	unsigned int min_width = this->width > width ? width : this->width;
	unsigned int min_height = this->height > height ? height : this->height;

	for(unsigned int x = 0; x < min_width; ++x)
		for(unsigned int y = 0; y < min_height; ++y)
			new_pixels[ y * width + x ] = GetPixel(x,y);

	delete pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}

// Change image size and scale the content
void Image::Scale(unsigned int width, unsigned int height)
{
	Color* new_pixels = new Color[width*height];

	for(unsigned int x = 0; x < width; ++x)
		for(unsigned int y = 0; y < height; ++y)
			new_pixels[ y * width + x ] = GetPixel((unsigned int)(this->width * (x / (float)width)), (unsigned int)(this->height * (y / (float)height)) );

	delete pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}

Image Image::GetArea(unsigned int start_x, unsigned int start_y, unsigned int width, unsigned int height)
{
	Image result(width, height);
	for(unsigned int x = 0; x < width; ++x)
		for(unsigned int y = 0; y < height; ++x)
		{
			if( (x + start_x) < this->width && (y + start_y) < this->height) 
				result.SetPixel( x, y, GetPixel(x + start_x,y + start_y) );
		}
	return result;
}

void Image::FlipY()
{
	int row_size = bytes_per_pixel * width;
	Uint8* temp_row = new Uint8[row_size];
#pragma omp simd
	for (int y = 0; y < height * 0.5; y += 1)
	{
		Uint8* pos = (Uint8*)pixels + y * row_size;
		memcpy(temp_row, pos, row_size);
		Uint8* pos2 = (Uint8*)pixels + (height - y - 1) * row_size;
		memcpy(pos, pos2, row_size);
		memcpy(pos2, temp_row, row_size);
	}
	delete[] temp_row;
}

bool Image::LoadPNG(const char* filename, bool flip_y)
{
	std::string sfullPath = absResPath(filename);
	std::ifstream file(sfullPath, std::ios::in | std::ios::binary | std::ios::ate);

	// Get filesize
	std::streamsize size = 0;
	if (file.seekg(0, std::ios::end).good()) size = file.tellg();
	if (file.seekg(0, std::ios::beg).good()) size -= file.tellg();

	if (!size)
		return false;

	std::vector<unsigned char> buffer;

	// Read contents of the file into the vector
	if (size > 0)
	{
		buffer.resize((size_t)size);
		file.read((char*)(&buffer[0]), size);
	}
	else
		buffer.clear();

	std::vector<unsigned char> out_image;

	if (decodePNG(out_image, width, height, buffer.empty() ? 0 : &buffer[0], (unsigned long)buffer.size(), true) != 0)
		return false;

	size_t bufferSize = out_image.size();
	unsigned int originalBytesPerPixel = (unsigned int)bufferSize / (width * height);
	
	// Force 3 channels
	bytes_per_pixel = 3;

	if (originalBytesPerPixel == 3) {
		pixels = new Color[bufferSize];
		memcpy(pixels, &out_image[0], bufferSize);
	}
	else if (originalBytesPerPixel == 4) {

		unsigned int newBufferSize = width * height * bytes_per_pixel;
		pixels = new Color[newBufferSize];

		unsigned int k = 0;
		for (unsigned int i = 0; i < bufferSize; i += originalBytesPerPixel) {
			pixels[k] = Color(out_image[i], out_image[i + 1], out_image[i + 2]);
			k++;
		}
	}

	// Flip pixels in Y
	if (flip_y)
		FlipY();

	return true;
}

// Loads an image from a TGA file
bool Image::LoadTGA(const char* filename, bool flip_y)
{
	unsigned char TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	unsigned char TGAcompare[12];
	unsigned char header[6];
	unsigned int imageSize;
	unsigned int bytesPerPixel;

    std::string sfullPath = absResPath( filename );

	FILE * file = fopen( sfullPath.c_str(), "rb");
   	if ( file == NULL || fread(TGAcompare, 1, sizeof(TGAcompare), file) != sizeof(TGAcompare) ||
		memcmp(TGAheader, TGAcompare, sizeof(TGAheader)) != 0 ||
		fread(header, 1, sizeof(header), file) != sizeof(header))
	{
		std::cerr << "File not found: " << sfullPath.c_str() << std::endl;
		if (file == NULL)
			return NULL;
		else
		{
			fclose(file);
			return NULL;
		}
	}

	TGAInfo* tgainfo = new TGAInfo;
    
	tgainfo->width = header[1] * 256 + header[0];
	tgainfo->height = header[3] * 256 + header[2];
    
	if (tgainfo->width <= 0 || tgainfo->height <= 0 || (header[4] != 24 && header[4] != 32))
	{
		fclose(file);
		delete tgainfo;
		return NULL;
	}
    
	tgainfo->bpp = header[4];
	bytesPerPixel = tgainfo->bpp / 8;
	imageSize = tgainfo->width * tgainfo->height * bytesPerPixel;
    
	tgainfo->data = new unsigned char[imageSize];
    
	if (tgainfo->data == NULL || fread(tgainfo->data, 1, imageSize, file) != imageSize)
	{
		if (tgainfo->data != NULL)
			delete tgainfo->data;
            
		fclose(file);
		delete tgainfo;
		return false;
	}

	fclose(file);

	// Save info in image
	if(pixels)
		delete pixels;

	width = tgainfo->width;
	height = tgainfo->height;
	pixels = new Color[width*height];

	// Convert to float all pixels
	for (unsigned int y = 0; y < height; ++y) {
		for (unsigned int x = 0; x < width; ++x) {
			unsigned int pos = y * width * bytesPerPixel + x * bytesPerPixel;
			// Make sure we don't access out of memory
			if( (pos < imageSize) && (pos + 1 < imageSize) && (pos + 2 < imageSize))
				SetPixel(x, height - y - 1, Color(tgainfo->data[pos + 2], tgainfo->data[pos + 1], tgainfo->data[pos]));
		}
	}

	// Flip pixels in Y
	if (flip_y)
		FlipY();

	delete tgainfo->data;
	delete tgainfo;

	return true;
}

// Saves the image to a TGA file
bool Image::SaveTGA(const char* filename)
{
	unsigned char TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	std::string fullPath = absResPath(filename);
	FILE *file = fopen(fullPath.c_str(), "wb");
	if ( file == NULL )
	{
		perror("Failed to open file: ");
		return false;
	}

	unsigned short header_short[3];
	header_short[0] = width;
	header_short[1] = height;
	unsigned char* header = (unsigned char*)header_short;
	header[4] = 24;
	header[5] = 0;

	fwrite(TGAheader, 1, sizeof(TGAheader), file);
	fwrite(header, 1, 6, file);

	// Convert pixels to unsigned char
	unsigned char* bytes = new unsigned char[width*height*3];
	for(unsigned int y = 0; y < height; ++y)
		for(unsigned int x = 0; x < width; ++x)
		{
			Color c = pixels[y*width+x];
			unsigned int pos = (y*width+x)*3;
			bytes[pos+2] = c.r;
			bytes[pos+1] = c.g;
			bytes[pos] = c.b;
		}

	fwrite(bytes, 1, width*height*3, file);
	fclose(file);

	return true;
}

#ifndef IGNORE_LAMBDAS

// You can apply and algorithm for two images and store the result in the first one
// ForEachPixel( img, img2, [](Color a, Color b) { return a + b; } );
template <typename F>
void ForEachPixel(Image& img, const Image& img2, F f) {
	for(unsigned int pos = 0; pos < img.width * img.height; ++pos)
		img.pixels[pos] = f( img.pixels[pos], img2.pixels[pos] );
}

#endif

FloatImage::FloatImage(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;
	pixels = new float[width * height];
	memset(pixels, 0, width * height * sizeof(float));
}

// Copy constructor
FloatImage::FloatImage(const FloatImage& c) {
	pixels = NULL;

	width = c.width;
	height = c.height;
	if (c.pixels)
	{
		pixels = new float[width * height];
		memcpy(pixels, c.pixels, width * height * sizeof(float));
	}
}

// Assign operator
FloatImage& FloatImage::operator = (const FloatImage& c)
{
	if (pixels) delete pixels;
	pixels = NULL;

	width = c.width;
	height = c.height;
	if (c.pixels)
	{
		pixels = new float[width * height * sizeof(float)];
		memcpy(pixels, c.pixels, width * height * sizeof(float));
	}
	return *this;
}

FloatImage::~FloatImage()
{
	if (pixels)
		delete pixels;
}

// Change image size (the old one will remain in the top-left corner)
void FloatImage::Resize(unsigned int width, unsigned int height)
{
	float* new_pixels = new float[width * height];
	unsigned int min_width = this->width > width ? width : this->width;
	unsigned int min_height = this->height > height ? height : this->height;

	for (unsigned int x = 0; x < min_width; ++x)
		for (unsigned int y = 0; y < min_height; ++y)
			new_pixels[y * width + x] = GetPixel(x, y);

	delete pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}

void Image::DrawLineDDA(int x0, int y0, int x1, int y1, const Color& c) {
	float dx = x1 - x0;
	float dy = y1 - y0;

	float steps = (abs(dx) > abs(dy)) ? abs(dx) : abs(dy);

	float xInc = dx / steps;
	float yInc = dy / steps;

	float x = x0;
	float y = y0;

	for (int i = 0; i <= steps; ++i) {
		SetPixel(static_cast<int>(x), static_cast<int>(y), c);
		x += xInc;
		y += yInc;
	}
}


void Image::DrawRect(int x, int y, int w, int h, const Color& borderColor, int borderWidth, bool isFilled, const Color& fillColor) {
	for (int i = 0; i < borderWidth; ++i) {
		for (int j = 0; j < w; ++j) {
			SetPixel(x + j, y + i, borderColor);
			SetPixel(x + j, y + h - 1 - i, borderColor);
		}

		for (int j = 0; j < h; ++j) {
			SetPixel(x + i, y + j, borderColor);
			SetPixel(x + w - 1 - i, y + j, borderColor);
		}
	}

	if (isFilled) {
		for (int i = borderWidth; i < w - borderWidth; ++i) {
			for (int j = borderWidth; j < h - borderWidth; ++j) {
				SetPixel(x + i, y + j, fillColor);
			}
		}
	}
}

void Image::DrawImage(const Image& image, int x, int y, bool top) {
	
	for (int i = 0; i < image.width; ++i) {
		for (int j = 0; j < image.height; ++j) {
			int destX = x + i;
			int destY = y + j;

			if (destX >= 0 && destX < width && destY >= 0 && destY < height) {
				Color pixelColor = image.GetPixel(i, j);

				if (top) {
					SetPixel(destX, destY, pixelColor);
				}
				else {
					SetPixel(destX, destY, pixelColor + GetPixel(destX, destY));
				}
			}
		}
	}
}

//No hem sapigut fer que funcioni
void Image::Eraser(unsigned int startX, unsigned int startY)
{
	if (startX < width && startY < height)
	{
		unsigned int areaWidth = 10;
		unsigned int areaHeight = 10;

		for (unsigned int x = startX; x < startX + areaWidth && x < width; ++x)
		{
			for (unsigned int y = startY; y < startY + areaHeight && y < height; ++y)
			{
				SetPixel(x, y, Color::CYAN); 
			}
		}
	}
}

void Image::DrawCircle(int x, int y, int r, const Color& borderColor, int borderWidth, bool isFilled, const Color& fillColor) {
	if (isFilled) {
		// Obtener el ancho y el alto de la imagen
		int width = Image::height;
		int height = Image::width;

		// Iterar sobre todos los puntos de la imagen
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				// Calcular la distancia entre el punto (i, j) y el centro del círculo
				int distance = sqrt((i - x) * (i - x) + (j - y) * (j - y));

				// Si el punto está dentro del círculo, pintarlo con el color de relleno
				if (distance <= r) {
					SetPixel(i, j, fillColor);
				}
			}
		}
	}

	// Algoritmo de círculo de punto medio
	int d = 1 - r;
	int x_c = 0;
	int y_c = r;

	// Dibujar el primer punto
	SetPixel(x + x_c, y + y_c, borderColor);

	// Calcular los puntos simétricos y dibujar
	while (x_c < y_c) {
		if (d < 0) {
			d += 2 * x_c + 3;
		}
		else {
			d += 2 * (x_c - y_c) + 5;
			y_c--;
		}

		x_c++;

		// Agregar los puntos simétricos
		SetPixel(x + x_c, y + y_c, borderColor);
		SetPixel(x - x_c, y + y_c, borderColor);
		SetPixel(x + x_c, y - y_c, borderColor);
		SetPixel(x - x_c, y - y_c, borderColor);
		SetPixel(x + y_c, y + x_c, borderColor);
		SetPixel(x - y_c, y + x_c, borderColor);
		SetPixel(x + y_c, y - x_c, borderColor);
		SetPixel(x - y_c, y - x_c, borderColor);
	}
}

void Image::ScanLineDDA(int x0, int y0, int x1, int y1, std::vector<Cell>& table) {

	float dx = x1 - x0;
	float dy = y1 - y0;

	int steps = (abs(dx) > abs(dy)) ? abs(dx) : abs(dy);


	float xInc = dx / steps;
	float yInc = dy / steps;

	float x = x0;
	float y = y0;

	for (int i = 0; i <= steps; ++i) {
		if (x < table[y].minx) {
			table[y].minx = x;
		}

		if (x > table[y].maxx) {
			table[y].maxx = x;
		}
		x += xInc;
		y += yInc;
	}

}

void Image::DrawTriangle(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Color& borderColor, bool isFilled, const Color& fillColor) {

	// Encontrar el valor máximo y mínimo de la altura del triángulo para realizar el AET
	if (isFilled) {
		int h_max = std::max({ static_cast<int>(p0.y), static_cast<int>(p1.y), static_cast<int>(p2.y) });
		int h_min = std::min({ static_cast<int>(p0.y), static_cast<int>(p1.y), static_cast<int>(p2.y) });

		int h = h_max + 1;

		std::vector<Cell> table(h); // no se si ha de ser l'alçada del framebuffer o del triangle

		ScanLineDDA(p0.x, p0.y, p1.x, p1.y, table);
		ScanLineDDA(p1.x, p1.y, p2.x, p2.y, table);
		ScanLineDDA(p2.x, p2.y, p0.x, p0.y, table);


		for (int y = h_min; y < table.size(); y++) {
			DrawLineDDA(table[y].minx, y, table[y].maxx, y, fillColor);
		}
	}
	DrawLineDDA(p0.x, p0.y, p1.x, p1.y, borderColor);
	DrawLineDDA(p1.x, p1.y, p2.x, p2.y, borderColor);
	DrawLineDDA(p2.x, p2.y, p0.x, p0.y, borderColor);
}


//3.4--------------
void Image::DrawTriangleInterpolated4(const Vector3& p0, const Vector3& p1, const Vector3& p2,
	const Color& c0, const Color& c1, const Color& c2,
	FloatImage* zbuffer, Image* texture,
	const Vector2& uv0, const Vector2& uv1, const Vector2& uv2) {

	//Inicialitzem algunes variables i matrius necessàries
	Vector3 sortedP0 = p0;         Vector3 sortedP1 = p1;    Vector3 sortedP2 = p2;
	Color interpolatedColor, color;    float zInterp;            std::vector<Cell> table(height);
	Matrix44 m;

	//Omplim la matriu
	m.M[0][0] = p0.x;    m.M[0][1] = p0.y;    m.M[0][2] = 1;
	m.M[1][0] = p1.x;    m.M[1][1] = p1.y;    m.M[1][2] = 1;
	m.M[2][0] = p2.x;    m.M[2][1] = p2.y;    m.M[2][2] = 1;
	m.Inverse();

	ScanLineDDA(p0.x, p0.y, p1.x, p1.y, table);
	ScanLineDDA(p1.x, p1.y, p2.x, p2.y, table);
	ScanLineDDA(p2.x, p2.y, p0.x, p0.y, table);

	//Recorrem l'àrea del nostre triangle
	for (int i = 0; i < height; i++)
	{
		if (table[i].minx <= table[i].maxx) {
			for (int j = table[i].minx; j < table[i].maxx; j++)
			{
				//Transformem el punt actual a coordenades bariocèntriques
				Vector3 point(j, i, 1);
				Vector3 b = m * point;
				b.Normalize();
				b.Clamp(0, 1);

				//Calculem coordenades bariocèntriques i la coordenada z interpolada
				float u = b.x;
				float v = b.y;
				float w = b.z;
				float z = p0.z * u + p1.z * v + p2.z * w;

				//Comprovem si el punt està dins del triangle i si està davant en el z-buffer
				if (zbuffer->GetPixel(j, i) >= z) {
					zbuffer->SetPixel(j, i, z);

					
					if (u >= 0 && v >= 0 && w >= 0) {
						if (texture == nullptr) {
							color = c0 * u + c1 * v + c2 * w;

						}
						else {
							Vector2 uvss = uv0 * u + uv1 * v + uv2 * w;
							uvss.normalize();
							uvss.Clamp(0, 1);
							color = texture->GetPixel(floor(uvss.x * (texture->width - 1)), floor(uvss.y * (texture->height - 1)));

						}

						//Assignem el color calculat al píxel de la imatge
						SetPixel(j, i, color);
					}
				}
			}
		}
	}
}

//3.3--------------
void Image::DrawTriangleInterpolated3(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Color& c0, const Color& c1, const Color& c2, FloatImage* zbuffer) {

	Vector3 sortedP0 = p0;
	Vector3 sortedP1 = p1;
	Vector3 sortedP2 = p2;
	Matrix44 m;

	//Forma "matricial"
	m.M[0][0] = p0.x;	m.M[0][1] = p1.x;	m.M[0][2] = p2.x;
	m.M[1][0] = p0.y;	m.M[1][1] = p1.y;	m.M[1][2] = p2.y;
	m.M[2][0] = p0.z;	m.M[2][1] = p1.z;	m.M[2][2] = p2.z;
	m.Inverse();

	// Calculem bounding box.
	int minX = std::min({ static_cast<int>(sortedP0.x), static_cast<int>(sortedP1.x), static_cast<int>(sortedP2.x) });
	int minY = std::min({ static_cast<int>(sortedP0.y), static_cast<int>(sortedP1.y), static_cast<int>(sortedP2.y) });
	int maxX = std::max({ static_cast<int>(sortedP0.x), static_cast<int>(sortedP1.x), static_cast<int>(sortedP2.x) });
	int maxY = std::max({ static_cast<int>(sortedP0.y), static_cast<int>(sortedP1.y), static_cast<int>(sortedP2.y) });

	// Recorrem l'àrea del nostre triangle.
	for (int y = minY; y <= maxY; ++y)
	{
		for (int x = minX; x <= maxX; ++x)
		{
			// Calculem les coordenades baricéntriques	
			float denom = (p1.y - p2.y) * (p0.x - p2.x) + (p2.x - p1.x) * (p0.y - p2.y);
			float u = ((p1.y - p2.y) * (x - p2.x) + (p2.x - p1.x) * (y - p2.y)) / denom;
			float v = ((p2.y - p0.y) * (x - p2.x) + (p0.x - p2.x) * (y - p2.y)) / denom;
			float w = 1.0f - u - v;

			//No m'acabem de sortir la forma matricial de les slides
			//--Vector3 barycentric_coords_p0 = m.Inverse() * p0;
			//--Vector3 barycentric_coords_p1 = m.Inverse() * p1;
			//--Vector3 barycentric_coords_p2 = m.Inverse() * p2;

			// Verifiquem que el punt estigui dintre el triangle
			if (u >= 0 && v >= 0 && w >= 0)
			{
				Color interpolatedColor = c0 * u + c1 * v + c2 * w;
				float zInterp = p0.z * u + p1.z * v + p2.z * w;

				// Corregir la verificación del Z-Buffer
				if (zInterp <= zbuffer->GetPixel(x, y)) {
					SetPixel(x, y, interpolatedColor);
					zbuffer->SetPixel(x, y, zInterp);
				}
			}
		}
	}
}

//3.2--------------
void Image::DrawTriangleInterpolated2(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Color& c0, const Color& c1, const Color& c2) {

	Vector3 v0 = p0, v1 = p1, v2 = p2;

	int minX = std::min({ v0.x, v1.x, v2.x });
	int minY = std::min({ v0.y, v1.y, v2.y });
	int maxX = std::max({ v0.x, v1.x, v2.x });
	int maxY = std::max({ v0.y, v1.y, v2.y });

	//Recorrem l'àrea del nostre triangle.
	for (int y = minY; y <= maxY; ++y)
	{
		for (int x = minX; x <= maxX; ++x)
		{
			// Calculem les coordenades baricéntriques	
			float denom = (p1.y - p2.y) * (p0.x - p2.x) + (p2.x - p1.x) * (p0.y - p2.y);
			float u = ((p1.y - p2.y) * (x - p2.x) + (p2.x - p1.x) * (y - p2.y)) / denom;
			float v = ((p2.y - p0.y) * (x - p2.x) + (p0.x - p2.x) * (y - p2.y)) / denom;
			float w = 1.0f - u - v;

			//Verifiquem que el punt estigui dintre el triangle
			if (u >= 0 && v >= 0 && w >= 0)
			{
				// Interpolem el color amb les coordenades baricèntriques
				Color interpolatedColor = c0 * u + c1 * v + c2 * w;
				SetPixel(x, y, interpolatedColor);
			}
		}
	}
}
