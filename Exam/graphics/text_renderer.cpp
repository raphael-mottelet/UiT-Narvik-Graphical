#include "text_renderer.h"
#include "stb_easy_font.h"

#include <vector>
#include <string>
#include <iostream>

// Local shader compiler for the text renderer using OpenGL forward rasterization
static GLuint compileShaderLocal(GLenum type, const char* src)
{
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);

    GLint ok = 0;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint logLen = 0;
        glGetShaderiv(s, GL_INFO_LOG_LENGTH, &logLen);
        std::string log(logLen, '\0');
        glGetShaderInfoLog(s, logLen, nullptr, log.data());
        std::cerr << "[TextRenderer] shader compile error:\n"
                  << log << "\n";
        glDeleteShader(s);
        return 0;
    }
    return s;
}

// Links a minimal vertex and fragment shader program dedicated to 2D text rendering in screen space
static GLuint makeProgramLocal(const char* vsSrc, const char* fsSrc)
{
    GLuint vs = compileShaderLocal(GL_VERTEX_SHADER, vsSrc);
    if (!vs) return 0;

    GLuint fs = compileShaderLocal(GL_FRAGMENT_SHADER, fsSrc);
    if (!fs) {
        glDeleteShader(vs);
        return 0;
    }

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);

    glDeleteShader(vs);
    glDeleteShader(fs);

    GLint ok = 0;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        GLint logLen = 0;
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLen);
        std::string log(logLen, '\0');
        glGetProgramInfoLog(prog, logLen, nullptr, log.data());
        std::cerr << "[TextRenderer] program link error:\n"
                  << log << "\n";
        glDeleteProgram(prog);
        return 0;
    }

    return prog;
}

// Initializes GPU resources and shader program used to render bitmap text via OpenGL 3.3 forward rasterization
bool initTextRenderer(TextRenderer& tr, unsigned int /*screenWidth*/)
{
    static const char* kTextVS = R"(
        #version 330 core
        layout(location = 0) in vec2 aPos;
        uniform vec2 uScreenSize;
        void main()
        {
            vec2 ndc;
            ndc.x = (aPos.x / uScreenSize.x) * 2.0 - 1.0;
            ndc.y = 1.0 - (aPos.y / uScreenSize.y) * 2.0;
            gl_Position = vec4(ndc, 0.0, 1.0);
        }
    )";

    static const char* kTextFS = R"(
        #version 330 core
        uniform vec4 uColor;
        out vec4 FragColor;
        void main()
        {
            FragColor = uColor;
        }
    )";

    tr.program = makeProgramLocal(kTextVS, kTextFS);
    if (!tr.program) {
        std::cerr << "[TextRenderer] Failed to create program\n";
        return false;
    }

    tr.uColorLoc      = glGetUniformLocation(tr.program, "uColor");
    tr.uScreenSizeLoc = glGetUniformLocation(tr.program, "uScreenSize");

    glGenVertexArrays(1, &tr.vao);
    glGenBuffers(1, &tr.vbo);

    glBindVertexArray(tr.vao);
    glBindBuffer(GL_ARRAY_BUFFER, tr.vbo);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    // Attribute 0 stores 2D positions in pixel space before conversion to normalized device coordinates
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                          sizeof(glm::vec2), (void*)0);

    glBindVertexArray(0);
    return true;
}

// Releases OpenGL objects allocated for the text renderer and resets the struct
void shutdownTextRenderer(TextRenderer& tr)
{
    if (tr.vbo)     glDeleteBuffers(1, &tr.vbo);
    if (tr.vao)     glDeleteVertexArrays(1, &tr.vao);
    if (tr.program) glDeleteProgram(tr.program);

    tr = TextRenderer{};
}

// Internal helper that converts stb_easy_font quads to triangle vertices and submits them for rasterization
namespace {
    void drawTextInternal(const TextRenderer& tr,
                          float x, float y,
                          float scale,
                          const std::string& text,
                          const glm::vec4& color)
    {
        if (!tr.program || !tr.vao || !tr.vbo || text.empty())
            return;

        static char buffer[60000]; // Vertex storage for generated quads

        // Raw text width in pixels before applying scale
        int rawWidth = stb_easy_font_width((char*)text.c_str());

        // Center the text horizontally around the supplied x coordinate
        float baseX = x - 0.5f * rawWidth * scale;
        float baseY = y;

        // stb_easy_font generates quads with origin at (0,0) in pixel coordinates
        int numQuads = stb_easy_font_print(0.0f, 0.0f,
                                           (char*)text.c_str(),
                                           nullptr,
                                           buffer,
                                           sizeof(buffer));
        if (numQuads <= 0)
            return;

        std::vector<glm::vec2> vertices;
        vertices.reserve(numQuads * 6); // Each quad is converted into 2 triangles

        for (int q = 0; q < numQuads; ++q) {
            const char* quadBase = buffer + q * 4 * 16; // 4 vertices per quad, 16 bytes per vertex

            glm::vec2 p[4];
            for (int k = 0; k < 4; ++k) {
                const float* v = (const float*)(quadBase + k * 16);
                p[k].x = baseX + v[0] * scale;
                p[k].y = baseY + v[1] * scale;
            }

            // Triangulate quad as two triangles for the rasterization pipeline
            vertices.push_back(p[0]);
            vertices.push_back(p[1]);
            vertices.push_back(p[2]);

            vertices.push_back(p[0]);
            vertices.push_back(p[2]);
            vertices.push_back(p[3]);
        }

        // Query the current viewport to convert pixel positions to normalized device coordinates
        GLint vp[4];
        glGetIntegerv(GL_VIEWPORT, vp);
        float screenW = (float)vp[2];
        float screenH = (float)vp[3];

        // Store depth and blending state to restore after 2D text rendering
        GLboolean depthWasEnabled = glIsEnabled(GL_DEPTH_TEST);
        GLboolean blendWasEnabled = glIsEnabled(GL_BLEND);

        if (depthWasEnabled) glDisable(GL_DEPTH_TEST);
        if (!blendWasEnabled) glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glUseProgram(tr.program);
        glUniform2f(tr.uScreenSizeLoc, screenW, screenH);
        glUniform4f(tr.uColorLoc, color.r, color.g, color.b, color.a);

        glBindVertexArray(tr.vao);
        glBindBuffer(GL_ARRAY_BUFFER, tr.vbo);
        glBufferData(GL_ARRAY_BUFFER,
                     (GLsizeiptr)(vertices.size() * sizeof(glm::vec2)),
                     vertices.data(),
                     GL_DYNAMIC_DRAW);

        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.size());

        glBindVertexArray(0);

        // Restore previous blending and depth test state after drawing text overlays
        if (!blendWasEnabled) glDisable(GL_BLEND);
        if (depthWasEnabled)  glEnable(GL_DEPTH_TEST);
    }
}

// Public interface used by the game to draw a single text line with optional drop shadow in front of the 3D scene
void drawTextLine(const TextRenderer& tr,
                  unsigned int /*screenWidth*/,
                  int x, int y,
                  float scale,
                  float shadowAlpha,
                  const std::string& text,
                  const glm::vec4& color)
{
    if (text.empty())
        return;

    // Optional black shadow behind the main text to improve readability over bright rasterized backgrounds
    if (shadowAlpha > 0.0f) {
        if (shadowAlpha > 1.0f) shadowAlpha = 1.0f;
        glm::vec4 shadowColor(0.0f, 0.0f, 0.0f, shadowAlpha);

        float offset = 1.5f * scale; // Shadow offset proportional to text scale
        drawTextInternal(tr,
                         (float)x + offset,
                         (float)y + offset,
                         scale,
                         text,
                         shadowColor);
    }

    // Main colored text drawn on top using the same screen-space rasterization path
    drawTextInternal(tr,
                     (float)x,
                     (float)y,
                     scale,
                     text,
                     color);
}
