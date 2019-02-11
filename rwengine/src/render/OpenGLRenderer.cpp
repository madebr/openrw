#include "render/OpenGLRenderer.hpp"

#include <cstring>
#include <sstream>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <core/Profiler.hpp>
#include <gl/DrawBuffer.hpp>
#include <rw/debug.hpp>

namespace {
constexpr GLuint kUBOIndexScene = 1;
constexpr GLuint kUBOIndexDraw = 2;
}

GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);

    while (true) {
        GLenum err = glGetError();
        if (err == GL_NO_ERROR) {
            break;
        }
        std::cerr << __FUNCTION__ << ":" << __LINE__ << ": ERROR glGetError() -> " << std::hex << "0x" << static_cast<unsigned>(err) << '\n';
    }
    if (shader == 0) {
        RW_ERROR("[OGL] Failed to create shader object");
        throw std::runtime_error("Compiling shader failed");
    }

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    while (true) {
        GLenum err = glGetError();
        if (err == GL_NO_ERROR) {
            break;
        }
        std::cerr << __FUNCTION__ << ":" << __LINE__ << ": ERROR glGetError() -> " << std::hex << "0x" << static_cast<unsigned>(err) << '\n';
    }
    if (status != GL_TRUE) {
        RW_ERROR("[OGL] Shader Compilation Failed.\nFailing shader type=0x" << std::hex << static_cast<unsigned>(type) << "\nsrc=" << source);
    }

    GLint len;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
    if (len > 1) {
        auto buffer = std::make_unique<GLchar[]>(len);
        glGetShaderInfoLog(shader, len, nullptr, buffer.get());

        GLint sourceLen;
        glGetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &sourceLen);
        auto sourceBuff = std::make_unique<GLchar[]>(sourceLen);
        glGetShaderSource(shader, sourceLen, nullptr, sourceBuff.get());

        RW_ERROR("[OGL] Shader InfoLog(" << shader << "):\n"
                  << buffer.get() << "\nSource:\n"
                  << sourceBuff.get());
    }

    if (status != GL_TRUE) {
        throw std::runtime_error("Compiling shader failed");
    }

    return shader;
}

GLuint compileProgram(const char* vertex, const char* fragment) {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertex);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragment);

    GLuint prog = glCreateProgram();

    glAttachShader(prog, vertexShader);
    glAttachShader(prog, fragmentShader);
    glLinkProgram(prog);

    glDetachShader(prog, vertexShader);
    glDetachShader(prog, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLint status;
    glGetProgramiv(prog, GL_LINK_STATUS, &status);

    if (status != GL_TRUE) {
        RW_ERROR("[OGL] Program Link Failed");
    }

    GLint len;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
    if (len > 1) {
        auto buffer = std::make_unique<GLchar[]>(len);
        glGetProgramInfoLog(prog, len, nullptr, buffer.get());

        RW_ERROR("[OGL] Program InfoLog(" << prog << "):\n"
                  << buffer.get());
    }

    if (status != GL_TRUE) {
        throw std::runtime_error("Linking shaders failed");
    }

    return prog;
}

void Renderer::setViewport(const glm::ivec2& vp) {
    viewport = vp;

    projection2D = glm::ortho(0.f, static_cast<float>(viewport.x),
                              static_cast<float>(viewport.y), 0.f, -1.f, 1.f);
}

void Renderer::swap() {
    drawCounter = 0;
    textureCounter = 0;
    bufferCounter = 0;
}

int Renderer::getDrawCount() {
    return drawCounter;
}

int Renderer::getBufferCount() {
    return bufferCounter;
}

int Renderer::getTextureCount() {
    return textureCounter;
}

const Renderer::SceneUniformData& Renderer::getSceneData() const {
    return lastSceneData;
}

void OpenGLRenderer::useDrawBuffer(DrawBuffer* dbuff) {
    if (dbuff != currentDbuff) {
        glBindVertexArray(dbuff->getVAOName());
        currentDbuff = dbuff;
        bufferCounter++;
#ifdef RW_GRAPHICS_STATS
        if (currentDebugDepth > 0) {
            profileInfo[currentDebugDepth - 1].buffers++;
        }
#endif
    }
}

void OpenGLRenderer::useTexture(GLuint unit, GLuint tex) {
    if (currentTextures[unit] != tex) {
        if (currentUnit != unit) {
            glActiveTexture(GL_TEXTURE0 + unit);
            currentUnit = unit;
        }
        glBindTexture(GL_TEXTURE_2D, tex);
        currentTextures[unit] = tex;
        textureCounter++;
#ifdef RW_GRAPHICS_STATS
        if (currentDebugDepth > 0) {
            profileInfo[currentDebugDepth - 1].textures++;
        }
#endif
    }
}

void OpenGLRenderer::setBlend(BlendMode mode) {
    if (mode!=BlendMode::BLEND_NONE && blendMode==BlendMode::BLEND_NONE)//To don't call glEnable again when it already enabled
        glEnable(GL_BLEND);

    if (mode!=blendMode) {
        switch (mode) {
        default:
            assert(false);
            break;
        case BlendMode::BLEND_NONE:
            glDisable(GL_BLEND);
            break;
        case BlendMode::BLEND_ALPHA:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case BlendMode::BLEND_ADDITIVE:
            glBlendFunc(GL_ONE, GL_ONE);
            break;

        }
    }

    blendMode = mode;
}

void OpenGLRenderer::setDepthMode(DepthMode mode) {
    if (mode != depthMode) {
        if (depthMode == DepthMode::OFF) glEnable(GL_DEPTH_TEST);
        switch(mode) {
        case DepthMode::OFF: glDisable(GL_DEPTH_TEST); break;
        case DepthMode::LESS: glDepthFunc(GL_LESS); break;
        }
        depthMode = mode;
    }
}

void OpenGLRenderer::setDepthWrite(bool enable) {
    if (enable != depthWriteEnabled) {
        glDepthMask(enable ? GL_TRUE : GL_FALSE);
        depthWriteEnabled = enable;
    }
}

void OpenGLRenderer::useProgram(Renderer::ShaderProgram* p) {
    if (p != currentProgram) {
        currentProgram = static_cast<OpenGLShaderProgram*>(p);
        glUseProgram(currentProgram->getName());
    }
}

namespace {
static bool supportsShaders() {
    GLboolean b;
    glGetBooleanv(GL_SHADER_COMPILER, &b);
    return b != GL_FALSE;
}
//GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam
void RWGL_MessageCallback(
        GLenum /*source*/,
        GLenum type,
        GLuint /*id*/,
        GLenum severity,
        GLsizei /*length*/,
        const GLchar* message,
        const void* /*userParam*/)
{
    std::cerr << "GL CALLBACK:" << ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" )
              << " severity=" << std::hex << "0x" << static_cast<unsigned>(severity) << ", message=" << message << "\n";//std::hex << "0x" << static_cast<unsigned>(source) << '\n';
}
}

OpenGLRenderer::OpenGLRenderer() {
    // We need to query for some profiling exts.
    ogl_CheckExtensions();

    if (!supportsShaders()) {
        std::cerr << "ERROR: OpenGL implementation does not support shader compilation (throw?)\n";
    }
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(RWGL_MessageCallback, this);

    glGenQueries(1, &debugQuery);

    createUBO(UBOScene, sizeof(SceneUniformData), sizeof(SceneUniformData));
    glBindBufferBase(GL_UNIFORM_BUFFER, kUBOIndexScene, UBOScene.name);

    GLint MaxUBOSize;
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &MaxUBOSize);

    createUBO(UBOObject, MaxUBOSize, sizeof(ObjectUniformData));

    swap();
}

std::string OpenGLRenderer::getIDString() const {
    std::stringstream ss;
    ss << "OpenGL Renderer";
    ss << " Version: " << glGetString(GL_VERSION);
    ss << " (GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION) << ")";
    ss << " Vendor: " << glGetString(GL_VENDOR);
    return ss.str();
}

std::unique_ptr<Renderer::ShaderProgram> OpenGLRenderer::createShader(const std::string& vert,
                                                      const std::string& frag) {
    return std::make_unique<OpenGLShaderProgram>(compileProgram(vert.c_str(), frag.c_str()));
}

void OpenGLRenderer::setProgramBlockBinding(Renderer::ShaderProgram* p,
                                            const std::string& name,
                                            GLint point) {
    OpenGLShaderProgram* glsh = static_cast<OpenGLShaderProgram*>(p);

    auto ubi = glGetUniformBlockIndex(glsh->getName(), name.c_str());
    glUniformBlockBinding(glsh->getName(), ubi, point);
}

void OpenGLRenderer::setUniformTexture(Renderer::ShaderProgram* p,
                                       const std::string& name, GLint tex) {
    useProgram(p);

    glUniform1i(currentProgram->getUniformLocation(name), tex);
}

void OpenGLRenderer::setUniform(Renderer::ShaderProgram* p,
                                const std::string& name, const glm::mat4& m) {
    useProgram(p);

    glUniformMatrix4fv(currentProgram->getUniformLocation(name.c_str()), 1,
                       GL_FALSE, glm::value_ptr(m));
}

void OpenGLRenderer::setUniform(Renderer::ShaderProgram* p,
                                const std::string& name, const glm::vec4& m) {
    useProgram(p);

    glUniform4fv(currentProgram->getUniformLocation(name.c_str()), 1,
                 glm::value_ptr(m));
}

void OpenGLRenderer::setUniform(Renderer::ShaderProgram* p,
                                const std::string& name, const glm::vec3& m) {
    useProgram(p);

    glUniform3fv(currentProgram->getUniformLocation(name.c_str()), 1,
                 glm::value_ptr(m));
}

void OpenGLRenderer::setUniform(Renderer::ShaderProgram* p,
                                const std::string& name, const glm::vec2& m) {
    useProgram(p);

    glUniform2fv(currentProgram->getUniformLocation(name.c_str()), 1,
                 glm::value_ptr(m));
}

void OpenGLRenderer::setUniform(Renderer::ShaderProgram* p,
                                const std::string& name, float f) {
    useProgram(p);

    glUniform1fv(currentProgram->getUniformLocation(name.c_str()), 1, &f);
}

void OpenGLRenderer::clear(const glm::vec4& colour, bool clearColour,
                           bool clearDepth) {
    auto flags = 0;
    if (clearColour) {
        flags |= GL_COLOR_BUFFER_BIT;
        glClearColor(colour.r, colour.g, colour.b, colour.a);
    }
    bool depthWriteWasEnabled = depthWriteEnabled;
    if (clearDepth) {
        flags |= GL_DEPTH_BUFFER_BIT;
        setDepthWrite(true);
    }

    glClear(flags);

    if (depthWriteWasEnabled != depthWriteEnabled) {
        setDepthWrite(depthWriteWasEnabled);
    }
}

void OpenGLRenderer::setSceneParameters(
    const Renderer::SceneUniformData& data) {
    uploadUBO(UBOScene, data);
    lastSceneData = data;
}

void OpenGLRenderer::setDrawState(const glm::mat4& model, DrawBuffer* draw,
                                  const Renderer::DrawParameters& p) {
    useDrawBuffer(draw);

    for (GLuint u = 0; u < p.textures.size(); ++u) {
        useTexture(u, p.textures[u]);
    }

    setBlend(p.blendMode);
    setDepthWrite(p.depthWrite);
    setDepthMode(p.depthMode);

    ObjectUniformData objectData{model,
                             glm::vec4(p.colour.r / 255.f, p.colour.g / 255.f,
                                       p.colour.b / 255.f, p.colour.a / 255.f),
                             1.f, 1.f, p.visibility};
    uploadUBO(UBOObject, objectData);

    drawCounter++;
#ifdef RW_GRAPHICS_STATS
    if (currentDebugDepth > 0) {
        profileInfo[currentDebugDepth - 1].draws++;
        profileInfo[currentDebugDepth - 1].primitives += p.count;
    }
#endif
}

void OpenGLRenderer::draw(const glm::mat4& model, DrawBuffer* draw,
                          const Renderer::DrawParameters& p) {
    setDrawState(model, draw, p);

    glDrawElements(draw->getFaceType(), static_cast<GLsizei>(p.count),
                   GL_UNSIGNED_INT,
                   reinterpret_cast<void*>(sizeof(RenderIndex) * p.start));
}

void OpenGLRenderer::drawArrays(const glm::mat4& model, DrawBuffer* draw,
                                const Renderer::DrawParameters& p) {
    setDrawState(model, draw, p);

    glDrawArrays(draw->getFaceType(), static_cast<GLint>(p.start), static_cast<GLsizei>(p.count));
}

void OpenGLRenderer::drawBatched(const RenderList& list) {
    RW_PROFILE_SCOPE(__func__);
#if 0  // Needs shader changes
	// Determine how many batches we need to process the entire list
	auto entries = list.size();
	glBindBuffer(GL_UNIFORM_BUFFER, UBOObject);
	for (int b = 0; b < entries; b += maxObjectEntries)
	{
		auto toConsume = std::min((GLuint)entries, b + maxObjectEntries) - b;
		std::vector<ObjectUniformData> uploadBuffer;
		uploadBuffer.resize(toConsume);
		for (int d = 0; d < toConsume; ++d)
		{
			auto& draw = list[b+d];
			uploadBuffer[d] = {
				draw.model,
				glm::vec4(draw.drawInfo.colour.r/255.f,
				draw.drawInfo.colour.g/255.f,
				draw.drawInfo.colour.b/255.f, 1.f),
				1.f,
				1.f,
				draw.drawInfo.colour.a/255.f
			};
		}
		glBufferData(GL_UNIFORM_BUFFER,
					 toConsume * sizeof(ObjectUniformData),
					 uploadBuffer.data(),
					 GL_STREAM_DRAW);

		// Dispatch individual draws
		for (int d = 0; d < toConsume; ++d)
		{
			auto& draw = list[b+d];
			useDrawBuffer(draw.dbuff);

			for( GLuint u = 0; u < draw.drawInfo.textures.size(); ++u )
			{
				useTexture(u, draw.drawInfo.textures[u]);
			}

			glDrawElements(draw.dbuff->getFaceType(), draw.drawInfo.count, GL_UNSIGNED_INT,
                           reinterpret_cast<void*>(sizeof(RenderIndex) * draw.drawInfo.start));
		}
	}
#else
    for (auto& ri : list) {
        draw(ri.model, ri.dbuff, ri.drawInfo);
    }
#endif
}

void OpenGLRenderer::invalidate() {
    currentDbuff = nullptr;
    currentProgram = nullptr;
    currentTextures.clear();
    currentUBO = 0;
    setBlend(BlendMode::BLEND_NONE);
    setDepthMode(DepthMode::OFF);
}

bool OpenGLRenderer::createUBO(Buffer &out, GLsizei size, GLsizei entrySize)
{
    glGenBuffers(1, &out.name);
    glBindBuffer(GL_UNIFORM_BUFFER, out.name);
    glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_STREAM_DRAW);

    if (entrySize != size) {
        GLint UBOAlignment;
        glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &UBOAlignment);
        RW_ASSERT(UBOAlignment > 0);
        entrySize = ((entrySize + (UBOAlignment-1))/UBOAlignment) * UBOAlignment;
    }

    out.bufferSize = size;
    out.entrySize = entrySize;
    out.entryCount = size / entrySize;

    return true;
}

void OpenGLRenderer::attachUBO(GLuint buffer) {
    if (currentUBO != buffer) {
        glBindBuffer(GL_UNIFORM_BUFFER, buffer);
        currentUBO = buffer;
    }
}

void OpenGLRenderer::uploadUBOEntry(Buffer &buffer, const void *data, size_t size)
{
    attachUBO(buffer.name);
    if (buffer.entryCount > 1) {
        RW_ASSERT(size <= buffer.entrySize);
        if (buffer.currentEntry >= buffer.entryCount) {
            // Orphan the buffer, we don't want it anymore
            glBufferData(GL_UNIFORM_BUFFER, buffer.bufferSize, nullptr,
                         GL_STREAM_DRAW);
            buffer.currentEntry = 0;
        }
        const auto offset = buffer.currentEntry * buffer.entrySize;
        const auto flags = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT
                           | GL_MAP_UNSYNCHRONIZED_BIT;
        void* dst = glMapBufferRange(GL_UNIFORM_BUFFER, offset,
                                     buffer.entrySize, flags);
        RW_ASSERT(dst != nullptr);
        memcpy(dst, data, size);
        glUnmapBuffer(GL_UNIFORM_BUFFER);
        glBindBufferRange(GL_UNIFORM_BUFFER, kUBOIndexDraw, buffer.name, offset, size);
        buffer.currentEntry++;
    }
    else {
        glBufferData(GL_UNIFORM_BUFFER, size, data, GL_DYNAMIC_DRAW);
    }
}

void OpenGLRenderer::pushDebugGroup(const std::string& title) {
#ifdef RW_GRAPHICS_STATS
    if (ogl_ext_KHR_debug) {
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, title.c_str());
        ProfileInfo& prof = profileInfo[currentDebugDepth];
        prof.buffers = prof.draws = prof.textures = prof.uploads =
            prof.primitives = 0;

        glQueryCounter(debugQuery, GL_TIMESTAMP);
        glGetQueryObjectui64v(debugQuery, GL_QUERY_RESULT, &prof.timerStart);

        currentDebugDepth++;
        RW_ASSERT(currentDebugDepth < MAX_DEBUG_DEPTH);
    }
#else
    RW_UNUSED(title);
#endif
}

const Renderer::ProfileInfo& OpenGLRenderer::popDebugGroup() {
#ifdef RW_GRAPHICS_STATS
    if (ogl_ext_KHR_debug) {
        glPopDebugGroup();
        currentDebugDepth--;
        RW_ASSERT(currentDebugDepth >= 0);

        ProfileInfo& prof = profileInfo[currentDebugDepth];

        glQueryCounter(debugQuery, GL_TIMESTAMP);
        GLuint64 current_time;
        glGetQueryObjectui64v(debugQuery, GL_QUERY_RESULT, &current_time);

        prof.duration = current_time - prof.timerStart;

        // Add counters to the parent group
        if (currentDebugDepth > 0) {
            ProfileInfo& p = profileInfo[currentDebugDepth - 1];
            p.draws += prof.draws;
            p.buffers += prof.buffers;
            p.primitives += prof.primitives;
            p.textures += prof.textures;
            p.uploads += prof.uploads;
        }

        return prof;
    }
#endif
    return profileInfo[0];
}

Renderer::ShaderProgram::~ShaderProgram() = default;

GLint OpenGLRenderer::OpenGLShaderProgram::getUniformLocation(const std::string &name) {
    auto c = uniforms.find(name.c_str());
    GLint loc = -1;
    if (c == uniforms.end()) {
        loc = glGetUniformLocation(program, name.c_str());
        uniforms[name] = loc;
    } else {
        loc = c->second;
    }
    return loc;
}
