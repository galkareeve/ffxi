// datReader.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "myStruct.h"
#include "IDatBase.h"
#include "Dat29.h"

#include <stdlib.h>
#include "glGlobal.h"
#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>


/*
 * Global data used by our render callback:
 */
IDatBase *g_pDat=nullptr;
gl_resources g_resources;
bool isLoaded=false;

/*
 * Functions for creating OpenGL objects:
 */
static GLuint make_buffer(
    GLenum target,
    const void *buffer_data,
    GLsizei buffer_size
) {
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(target, buffer);
    glBufferData(target, buffer_size, buffer_data, GL_STATIC_DRAW);
    return buffer;
}
/*
static GLuint make_texture(const char *filename)
{
    int width, height;
    void *pixels = read_tga(filename, &width, &height);
    GLuint texture;

    if (!pixels)
        return 0;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
    glTexImage2D(
        GL_TEXTURE_2D, 0,           //* target, level 
        GL_RGB8,                    //* internal format 
        width, height, 0,           //* width, height, border 
        GL_BGR, GL_UNSIGNED_BYTE,   //* external format, type 
        pixels                      //* pixels
    );
    free(pixels);
    return texture;
}
*/

//static GLuint make_texture()
//{
//	GLuint texture[40];
//    glGenTextures(40, texture);
//
//    char buf[16];
//	bool isDDS=false;
//	unsigned int width,height,mipmap;
//	char *pStorage=nullptr;
//	int totalDDS = g_pDat->getOriginalTotalDDS();
//	if( totalDDS==0 )
//		exit(0);
//
//	//limit
//	if( totalDDS>40 ) totalDDS=40;
//	for(int i=0,j=0; i<totalDDS; ++i) {	
//		if( g_pDat->getDDS(i,width,height,mipmap, buf,isDDS, pStorage)) {
//			glBindTexture(GL_TEXTURE_2D, texture[j]);
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//			if( isDDS )
//				loader.loadDDS(pStorage, width, height, mipmap);
//			else
//				loader.loadImage(pStorage, width, height);
//			delete[] pStorage;
//			
//			g_pDat->addDDSTextureID(texture[j], buf);
//			j++;
//		}
//	}
//	return texture[0];
//}

static void show_info_log(
    GLuint object,
    PFNGLGETSHADERIVPROC glGet__iv,
    PFNGLGETSHADERINFOLOGPROC glGet__InfoLog
)
{
    GLint log_length;
    char *log;

    glGet__iv(object, GL_INFO_LOG_LENGTH, &log_length);
    log = (char*)malloc(log_length);
    glGet__InfoLog(object, log_length, NULL, log);
    fprintf(stderr, "%s", log);
    free(log);
}

static GLuint make_shader(GLenum type, const char *filename)
{
    GLint length;
	GLuint shader;
    GLint shader_ok;

	std::ifstream ifs(filename, std::ifstream::in);
	ifs.seekg (0, ifs.end);
    length = ifs.tellg();
    ifs.seekg (0, ifs.beg);

    GLchar *source = new char [length+1];
	if (!source)
        return 0;

	 // read data as a block:
    ifs.read (source,length);
	ifs.close();
	((char*)source)[length] = '\0';

    shader = glCreateShader(type);
    glShaderSource(shader, 1, (const GLchar**)&source, &length);
    delete[] source;

    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_ok);
    if (!shader_ok) {
        fprintf(stderr, "Failed to compile %s:\n", filename);
        show_info_log(shader, glGetShaderiv, glGetShaderInfoLog);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

static GLuint make_program(GLuint vertex_shader, GLuint fragment_shader)
{
    GLint program_ok;

    GLuint program = glCreateProgram();

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &program_ok);
    if (!program_ok) {
        fprintf(stderr, "Failed to link shader program:\n");
        show_info_log(program, glGetProgramiv, glGetProgramInfoLog);
        glDeleteProgram(program);
        return 0;
    }
    return program;
}

/*
 * Data used to seed our vertex array and element array buffers:
 */
static const GLfloat g_vertex_buffer_data[] = { 
    -1.0f, -1.0f, 0.0f, 1.0f,
     1.0f, -1.0f, 0.0f, 1.0f,
    -1.0f,  1.0f, 0.0f, 1.0f,
     1.0f,  1.0f, 0.0f, 1.0f
};
static const GLushort g_element_buffer_data[] = { 0, 1, 2, 3 };

/*
 * Load and create all of our resources:
 */
static int make_resources(const char *datFN, bool writeBMP)
{
	//create Dat
	g_pDat = new CDat29(datFN);
	if( !g_pDat )
		return 0;
	if(writeBMP)
		g_pDat->setWriteBMP();

	g_pDat->loadDat();
	if( g_pDat->getTotalDDS()>0 ) {
		std::string name = g_pDat->firstImage();
		glutSetWindowTitle(name.c_str());
	}
	else
		glutSetWindowTitle("No Image");

    g_resources.vertex_buffer = make_buffer(
        GL_ARRAY_BUFFER,
        g_vertex_buffer_data,
        sizeof(g_vertex_buffer_data)
    );
    g_resources.element_buffer = make_buffer(
        GL_ELEMENT_ARRAY_BUFFER,
        g_element_buffer_data,
        sizeof(g_element_buffer_data)
    );

//    g_resources.textures[0] = make_texture();

//    if (g_resources.textures[0] == 0 )
//       return 0;

    g_resources.vertex_shader = make_shader(
        GL_VERTEX_SHADER,
        "datReader.v.glsl"
    );
    if (g_resources.vertex_shader == 0)
        return 0;

    g_resources.fragment_shader = make_shader(
        GL_FRAGMENT_SHADER,
        "datReader.f.glsl"
    );
    if (g_resources.fragment_shader == 0)
        return 0;

    g_resources.program = make_program(g_resources.vertex_shader, g_resources.fragment_shader);
    if (g_resources.program == 0)
        return 0;

    g_resources.uniforms.timer
        = glGetUniformLocation(g_resources.program, "timer");
    g_resources.uniforms.textures[0]
        = glGetUniformLocation(g_resources.program, "textures[0]");

    g_resources.attributes.position
        = glGetAttribLocation(g_resources.program, "position");

    return 1;
}

/*
 * GLUT callbacks:
 */
static void update_timer(void)
{
    int milliseconds = glutGet(GLUT_ELAPSED_TIME);
    g_resources.timer = (float)milliseconds * 0.001f;
    glutPostRedisplay();
}

static void render(void)
{
	if(!isLoaded)
		return;

//    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(g_resources.program);
    glUniform1f(g_resources.uniforms.timer, g_resources.timer);
    
//	glEnable(GL_BLEND);
	//1-source ==> not premultiplied
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	glBlendFunc(GL_SRC_ALPHA, GL_ZERO);
//	glBlendFunc(GL_ONE, GL_ZERO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_resources.textures[0]);
    glUniform1i(g_resources.uniforms.textures[0], 0);

    glBindBuffer(GL_ARRAY_BUFFER, g_resources.vertex_buffer);
    glVertexAttribPointer(
        g_resources.attributes.position,  /* attribute */
        4,                                /* size */
        GL_FLOAT,                         /* type */
        GL_FALSE,                         /* normalized? */
        sizeof(GLfloat)*4,                /* stride */
        (void*)0                          /* array buffer offset */
    );
    glEnableVertexAttribArray(g_resources.attributes.position);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_resources.element_buffer);
    glDrawElements(
        GL_TRIANGLE_STRIP,  /* mode */
        4,                  /* count */
        GL_UNSIGNED_SHORT,  /* type */
        (void*)0            /* element array buffer offset */
    );

    glDisableVertexAttribArray(g_resources.attributes.position);
    glutSwapBuffers();
}

//Called whenever a key on the keyboard was pressed.
//The key is given by the ''key'' parameter, which is in ASCII.
//It's often a good idea to have the escape key (ASCII value 27) call glutLeaveMainLoop() to 
//exit the program.
static void keyboard(unsigned char key, int x, int y)
{
	std::string n;
	switch (key)
	{
	case 27:
		glutLeaveMainLoop();
		break;
	case 'n':
		if(g_pDat->getTotalDDS()>0) {
			n = g_pDat->nextDDS();
			glutSetWindowTitle(n.c_str());
		}
		break;
	case 'b':
		if(g_pDat->getTotalDDS()>0) {
			n = g_pDat->prevDDS();
			glutSetWindowTitle(n.c_str());
		}
		break;

	case 'm':
//		g_pDat->parseMesh();
//		std::cout << "done parseMesh\n" << std::endl;
		break;
	case 'f':
//		g_pDat->parseFrame();
		break;
	case 'p':	isLoaded=false;
				g_pDat->nextDatFile();
				if(g_pDat->getTotalDDS()>0) {
					n = g_pDat->firstImage();
					glutSetWindowTitle(n.c_str());
				}
		break;
	case 'o':	isLoaded=false;
				g_pDat->prevDatFile();
				if(g_pDat->getTotalDDS()>0) {
					n = g_pDat->firstImage();
					glutSetWindowTitle(n.c_str());
				}
		break;
	case 's': g_pDat->skipNext();
				std::cout << "skipped file: " << g_pDat->getCurDatFile() << std::endl;
		break;
	}
}

/*
 * Entry point
 */
int main(int argc, char** argv)
{
	bool writeBMP=false;
	if((argc>2) && (strcmp(argv[2],"1")==0))
		writeBMP=true;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(400, 300);
    glutCreateWindow("Hello World");
    glutIdleFunc(&update_timer);
    glutDisplayFunc(&render);
	glutKeyboardFunc(&keyboard);

    glewInit();
    if (!GLEW_VERSION_2_0) {
        fprintf(stderr, "OpenGL 2.0 not available\n");
        return 1;
    }

    if (!make_resources(argc >= 2 ? argv[1] : "3034", writeBMP)) {
        fprintf(stderr, "Failed to load resources\n");
        return 1;
    }

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glutMainLoop();
    return 0;
}


