
#include "GL/glut.h"

#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <vector>
#include <iostream>
#include <cassert>

GLuint width = 800, height = 480;
GLdouble width_step = 2.0f / width;
GLdouble height_step = 2.0f / height;

const int MAX_RAY_DEPTH = 3;

template<typename T>
class Vec3
{
public:
    T x, y, z;

    Vec3() : x(T(0)), y(T(0)), z(T(0)) {}
    Vec3(T xx) : x(xx), y(xx), z(xx) {}
    Vec3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}

    Vec3& normalize()
    {
        T nor = x * x + y * y + z * z;
        if (nor > 1) {
            T invNor = 1 / sqrt(nor);
            x *= invNor, y *= invNor, z *= invNor;
        }
        return *this;
    }

    Vec3<T> operator * (const T &f) const { return Vec3<T>(x * f, y * f, z * f); }
    Vec3<T> operator * (const Vec3<T> &v) const { return Vec3<T>(x * v.x, y * v.y, z * v.z); }
    T dot(const Vec3<T> &v) const { return x * v.x + y * v.y + z * v.z; }
    Vec3<T> operator - (const Vec3<T> &v) const { return Vec3<T>(x - v.x, y - v.y, z - v.z); }
    Vec3<T> operator + (const Vec3<T> &v) const { return Vec3<T>(x + v.x, y + v.y, z + v.z); }
    Vec3<T>& operator += (const Vec3<T> &v) { x += v.x, y += v.y, z += v.z; return *this; }
    Vec3<T>& operator *= (const Vec3<T> &v) { x *= v.x, y *= v.y, z *= v.z; return *this; }
    Vec3<T> operator - () const { return Vec3<T>(-x, -y, -z); }   

};

template<typename T>
class Sphere
{
public:
    // Sphere variables.
    Vec3<T> center;                         /// position of the sphere
    T radius, radius2;                      /// sphere radius and radius^2
    Vec3<T> surfaceColor, emissionColor;    /// surface color and emission (light)
    T transparency, reflection;             /// surface transparency and reflectivity

    // Sphere constructor.
    // position(c), radius(r), surface color(sc), reflectivity(refl), transparency(transp), emission color(ec)
    Sphere(const Vec3<T> &c, const T &r, const Vec3<T> &sc, 
        const T &refl = 0, const T &transp = 0, const Vec3<T> &ec = 0) : 
        center(c), radius(r), surfaceColor(sc), reflection(refl),
        transparency(transp), emissionColor(ec), radius2(r * r)
    {}


    bool intersect(const Vec3<T> &rayorig, const Vec3<T> &raydir, T *t0 = NULL, T *t1 = NULL) const
    {

        Vec3<T> l = center - rayorig;

        T tca = l.dot(raydir);

        if (tca < 0) return false;
        
        T d2 = l.dot(l) - (tca * tca);

        if (d2 > radius2) return false;

        T thc = sqrt(radius2 - d2);
        if (t0 != NULL && t1 != NULL) {

            *t0 = tca - thc;

            *t1 = tca + thc;
        }

        return true;
    }
};

std::vector<Sphere<double> *> spheres;

// function to mix 2 T varables.
template<typename T>
T mix(const T &a, const T &b, const T &mix)
{
    return b * mix + a * (T(1) - mix);
}

template<typename T>
Vec3<T> trace(const Vec3<T> &rayorig, const Vec3<T> &raydir, 
    const std::vector<Sphere<T> *> &spheres, const int &depth)
{
    T tnear = INFINITY;
    const Sphere<T> *sphere = NULL;

    for (unsigned i = 0; i < spheres.size(); ++i) {
        T t0 = INFINITY, t1 = INFINITY;
        if (spheres[i]->intersect(rayorig, raydir, &t0, &t1)) {

            if (t0 < 0) t0 = t1;

            if (t0 < tnear) {

                tnear = t0;
                sphere = spheres[i];
            }
        }
    }

    if (!sphere) return Vec3<T>(.5); // Grey background color.

    Vec3<T> surfaceColor = 0; // initialisation of the color of the ray/surface of the object intersected by the ray.
    Vec3<T> phit = rayorig + (raydir * tnear); // point of intersection.
    Vec3<T> nhit = phit - sphere->center; // normal at the intersection point.

    if (raydir.dot(nhit) > 0) nhit = -nhit;
    nhit.normalize(); // normalize normal direction

    T bias = 1e-5; // add some bias to the point from which we will be tracing

    if ((sphere->transparency > 0 || sphere->reflection > 0) && depth < MAX_RAY_DEPTH) {
        T IdotN = raydir.dot(nhit); // raydir.normal

        T facingratio = std::max(T(0), -IdotN);

        T fresneleffect = mix<T>(pow(1 - facingratio, 3), 1, 0.1);

        Vec3<T> refldir = raydir - nhit * 2 * raydir.dot(nhit);
        Vec3<T> reflection = trace(phit + (nhit * bias), refldir, spheres, depth + 1);
        Vec3<T> refraction = 0;

        if (sphere->transparency) {
            T ior = 1.2, eta = 1 / ior;
            T k = 1 - eta * eta * (1 - IdotN * IdotN);
            Vec3<T> refrdir = raydir * eta - nhit * (eta *  IdotN + sqrt(k));
            refraction = trace(phit - nhit * bias, refrdir, spheres, depth + 1);
        }

        surfaceColor = (reflection * fresneleffect + refraction * (1 - fresneleffect) * sphere->transparency) * sphere->surfaceColor;
    }
    else {

        double shadow = 1.0;
        for (unsigned i = 0; i < spheres.size(); ++i) {
            if (spheres[i]->emissionColor.x > 0) {
                // this is a light
                Vec3<T> transmission = 1.0;
                Vec3<T> lightDirection = spheres[i]->center - phit;
                lightDirection.normalize();
                T light_angle = (acos(raydir.dot(lightDirection)) / ( sqrt(raydir.dot(raydir)) * sqrt(lightDirection.dot(lightDirection))));
                T light_incidence = sin(light_angle);
                for (unsigned j = 0; j < spheres.size(); ++j) {
                    if (i != j) {
                        T t0, t1;

                        if (spheres[j]->intersect(phit + (nhit * bias), lightDirection, &t0, &t1)) {
                            shadow = std::max(0.0, shadow - (1.0 - spheres[j]->transparency));
                            transmission = transmission * spheres[j]->surfaceColor * shadow;
                            //break;
                        }
                    }
                }
Z
                surfaceColor += sphere->surfaceColor * transmission * 
                    std::max(T(0), nhit.dot(lightDirection)) * spheres[i]->emissionColor;
            }
        }
    }
    return surfaceColor + sphere->emissionColor;
}

Vec3<double> *image = new Vec3<double>[width * height];
static Vec3<double> cam_pos = Vec3<double>(0);
template<typename T>
void render(const std::vector<Sphere<T> *> &spheres)
{
    Vec3<T> *pixel = image;
    T invWidth = 1 / T(width), invHeight = 1 / T(height);
    T fov = 30, aspectratio = T(width) / T(height);
    T angle = tan(M_PI * 0.5 * fov / T(180));
    // Trace rays
    for (GLuint y = 0; y < height; ++y) {
        for (GLuint x = 0; x < width; ++x, ++pixel) {
            T xx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectratio;
            T yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle;
            Vec3<T> raydir(xx, yy, -1);
            raydir.normalize();
            *pixel = trace(cam_pos, raydir, spheres, 0);
        }
    }
}

//********************************** OPEN GL ***********************************************

void advanceDisplay(void)
{
    cam_pos.z = cam_pos.z - 2;
    glutPostRedisplay();
}

void backDisplay(void)
{
    cam_pos.z = cam_pos.z + 2;
    glutPostRedisplay();
}

void resetDisplay(void)
{
    Vec3<double> new_cam_pos;
    new_cam_pos = cam_pos;
    cam_pos = new_cam_pos;
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
    switch (button)
    {
    case GLUT_LEFT_BUTTON:
        if(state == GLUT_DOWN)
        {
            glutIdleFunc(advanceDisplay);
        }
        break;

    case GLUT_MIDDLE_BUTTON:
        if(state == GLUT_DOWN)
        {
            glutIdleFunc(resetDisplay);
        }
        break;

    case GLUT_RIGHT_BUTTON:
        if(state == GLUT_DOWN)
        {
            glutIdleFunc(backDisplay);
        }
        break;
    }
}

GLuint tex = 0;
void display(void)
{
    int i;
    float x, y;
    render<double>(spheres); // Creates the image and put it to memory in image[].

    std::vector< unsigned char > buf;
    buf.reserve( width * height * 3 );
    for( size_t y = 0; y < height; ++y )
    {
        for( size_t x = 0; x < width; ++x )
        {
            // flip vertically (height-y) because the OpenGL texture origin is in the lower-left corner
            // flip horizontally (width-x) because...the original code did so
            size_t i = (height-y) * width + (width-x);
            buf.push_back( (unsigned char)( std::min(double(1), image[i].x) * 255.0 ) );
            buf.push_back( (unsigned char)( std::min(double(1), image[i].y) * 255.0 ) );
            buf.push_back( (unsigned char)( std::min(double(1), image[i].z) * 255.0 ) );
        }
    }

    /* clear all pixels */
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode( GL_PROJECTION ); 
    glLoadIdentity(); 

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity(); 

    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, tex );
    glTexSubImage2D
        (
        GL_TEXTURE_2D, 0,
        0, 0,
        width, height,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        &buf[0]
        );

    glBegin( GL_QUADS );
    glTexCoord2i( 0, 0 );
    glVertex2i( -1, -1 );
    glTexCoord2i( 1, 0 );
    glVertex2i(  1, -1 );
    glTexCoord2i( 1, 1 );
    glVertex2i(  1,  1 );
    glTexCoord2i( 0, 1 );
    glVertex2i( -1,  1 );
    glEnd();

    glutSwapBuffers();
}

int main(int argc, char **argv)
{ 
    // position, radius, surface color, reflectivity, transparency, emission color
    spheres.push_back(new Sphere<double>(Vec3<double>(0, -10004, -20), 10000, Vec3<double>(0.2), 0.0, 0.0));

    spheres.push_back(new Sphere<double>(Vec3<double>(3, 0, -15), 2, Vec3<double>(1.00, 0.1, 0.1), 0.65, 0.95));
    spheres.push_back(new Sphere<double>(Vec3<double>(1, -1, -18), 1, Vec3<double>(1.0, 1.0, 1.0), 0.9, 0.9));
    spheres.push_back(new Sphere<double>(Vec3<double>(-2, 2, -15), 2, Vec3<double>(0.1, 0.1, 1.0), 0.05, 0.5));
    spheres.push_back(new Sphere<double>(Vec3<double>(-4, 3, -18), 1, Vec3<double>(0.1, 1.0, 0.1), 0.3, 0.7));

    spheres.push_back(new Sphere<double>(Vec3<double>(-4, 0, -25), 1, Vec3<double>(1.00, 0.1, 0.1), 0.65, 0.95));
    spheres.push_back(new Sphere<double>(Vec3<double>(-1, 1, -25), 2, Vec3<double>(1.0, 1.0, 1.0), 0.0, 0.0));
    spheres.push_back(new Sphere<double>(Vec3<double>(2, 2, -25), 1, Vec3<double>(0.1, 0.1, 1.0), 0.05, 0.5));
    spheres.push_back(new Sphere<double>(Vec3<double>(5, 3, -25), 2, Vec3<double>(0.1, 1.0, 0.1), 0.3, 0.7));

    // light
    spheres.push_back(new Sphere<double>(Vec3<double>(-10, 20, 0), 3, Vec3<double>(0), 0, 0, Vec3<double>(3)));
    spheres.push_back(new Sphere<double>(Vec3<double>(0, 10, 0), 3, Vec3<double>(0), 0, 0, Vec3<double>(1)));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(10,10);
    glutCreateWindow(argv[0]);
    glutDisplayFunc(display);
    glutMouseFunc(mouse);

    glGenTextures( 1, &tex );
    glBindTexture( GL_TEXTURE_2D, tex );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
    glTexImage2D( GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL );    

    glutMainLoop();

    delete [] image;
    while (!spheres.empty()) {
        Sphere<double> *sph = spheres.back();
        spheres.pop_back();
        delete sph;
    }
    return 0;
}
