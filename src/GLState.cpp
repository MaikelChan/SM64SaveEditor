#include "GLState.h"
#include <cstdint>
#include <stdexcept>
#include <cstring>

VertexAttrib::VertexAttrib()
{
	enabled = GL_FALSE;
	size = 4;
	type = GL_FLOAT;
	normalized = GL_FALSE;
	stride = 0;
	pointer = 0;
}

VertexArrayObject::VertexArrayObject(const GLint vertexAttribCount)
{
	vertexAttribs = new VertexAttrib[vertexAttribCount];
}

VertexArrayObject::~VertexArrayObject()
{
	delete[] vertexAttribs;
}

GLState::GLState()
{
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs);

	clearColorR = 0.0f;
	clearColorG = 0.0f;
	clearColorB = 0.0f;
	clearColorA = 0.0f;
	blendEnabled = GL_FALSE;
	cullFaceEnabled = GL_FALSE;
	depthTestEnabled = GL_FALSE;
	stencilTestEnabled = GL_FALSE;
	scissorTestEnabled = GL_FALSE;
	primitiveRestartEnabled = GL_FALSE;
	blendEquation = GL_FUNC_ADD;
	bfsSrcRgb = GL_ONE;
	bfsDstRgb = GL_ZERO;
	bfsSrcAlpha = GL_ONE;
	bfsDstAlpha = GL_ZERO;
	viewportX = 0;
	viewportY = 0;
	viewportW = -1;
	viewportH = -1;
	scissorX = 0;
	scissorY = 0;
	scissorW = -1;
	scissorH = -1;
	frontPolygonMode = GL_FILL;
	backPolygonMode = GL_FILL;
	boundShaderProgram = 0;
	boundTexture2D = 0;
	boundArrayBuffer = 0;
	boundElementArrayBuffer = 0;
	boundPixelUnpackBuffer = 0;
	boundVao = 0;
	boundSampler = 0;
}

GLState::~GLState()
{
	for (auto vao : vertexArrayObjects) delete vao.second;
	vertexArrayObjects.clear();

	for (auto shaderProgram : shaderPrograms) delete shaderProgram.second;
	shaderPrograms.clear();
}

void GLState::ClearColor(const GLfloat r, const GLfloat g, const GLfloat b, const GLfloat a)
{
	if (clearColorR == r && clearColorG == g && clearColorB == b && clearColorA == a) return;
	clearColorR = r;
	clearColorG = g;
	clearColorB = b;
	clearColorA = a;

	glClearColor(r, g, b, a);
}

void GLState::EnableBlend(const GLboolean enable)
{
	if (blendEnabled == enable) return;
	blendEnabled = enable;

	if (enable) glEnable(GL_BLEND);
	else glDisable(GL_BLEND);
}

void GLState::EnableCullFace(const GLboolean enable)
{
	if (cullFaceEnabled == enable) return;
	cullFaceEnabled = enable;

	if (enable) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);
}

void GLState::EnableDepthTest(const GLboolean enable)
{
	if (depthTestEnabled == enable) return;
	depthTestEnabled = enable;

	if (enable) glEnable(GL_DEPTH_TEST);
	else glDisable(GL_DEPTH_TEST);
}

void GLState::EnableStencilTest(const GLboolean enable)
{
	if (stencilTestEnabled == enable) return;
	stencilTestEnabled = enable;

	if (enable) glEnable(GL_STENCIL_TEST);
	else glDisable(GL_STENCIL_TEST);
}

void GLState::EnableScissorTest(const GLboolean enable)
{
	if (scissorTestEnabled == enable) return;
	scissorTestEnabled = enable;

	if (enable) glEnable(GL_SCISSOR_TEST);
	else glDisable(GL_SCISSOR_TEST);
}

void GLState::EnablePrimitiveRestart(const GLboolean enable)
{
	if (primitiveRestartEnabled == enable) return;
	primitiveRestartEnabled = enable;

	if (enable) glEnable(GL_PRIMITIVE_RESTART);
	else glDisable(GL_PRIMITIVE_RESTART);
}

void GLState::BlendEquation(const GLenum value)
{
	if (blendEquation == value) return;
	blendEquation = value;

	glBlendEquation(value);
}

void GLState::BlendFuncSeparate(const GLenum srcRgb, const GLenum dstRgb, const GLenum srcAlpha, const GLenum dstAlpha)
{
	if (bfsSrcRgb == srcRgb && bfsDstRgb == dstRgb && bfsSrcAlpha == srcAlpha && bfsDstAlpha == dstAlpha) return;
	bfsSrcRgb = srcRgb;
	bfsDstRgb = dstRgb;
	bfsSrcAlpha = srcAlpha;
	bfsDstAlpha = dstAlpha;

	glBlendFuncSeparate(srcRgb, dstRgb, srcAlpha, dstAlpha);
}

void GLState::Viewport(const GLint x, const GLint y, const GLsizei w, const GLsizei h)
{
	if (viewportX == x && viewportY == y && viewportW == w && viewportH == h) return;
	viewportX = x;
	viewportY = y;
	viewportW = w;
	viewportH = h;

	glViewport(x, y, w, h);
}

void GLState::Scissor(const GLint x, const GLint y, const GLsizei w, const GLsizei h)
{
	if (scissorX == x && scissorY == y && scissorW == w && scissorH == h) return;
	scissorX = x;
	scissorY = y;
	scissorW = w;
	scissorH = h;

	glScissor(x, y, w, h);
}

void GLState::PolygonMode(const GLenum face, const GLenum mode)
{
	switch (face)
	{
		case GL_FRONT:

			if (frontPolygonMode == mode) return;
			frontPolygonMode = mode;

			glPolygonMode(GL_FRONT, mode);
			break;

		case GL_BACK:

			if (backPolygonMode == mode) return;
			backPolygonMode = mode;

			glPolygonMode(GL_BACK, mode);
			break;

		case GL_FRONT_AND_BACK:

			if (frontPolygonMode == mode && backPolygonMode == mode) return;
			frontPolygonMode = mode;
			backPolygonMode = mode;

			glPolygonMode(GL_FRONT_AND_BACK, mode);
			break;
	}
}

void GLState::BindTexture2D(const GLuint texture2d)
{
	if (boundTexture2D == texture2d) return;
	boundTexture2D = texture2d;

	glBindTexture(GL_TEXTURE_2D, texture2d);
}

void GLState::BindArrayBuffer(const GLuint buffer)
{
	if (boundArrayBuffer == buffer) return;
	boundArrayBuffer = buffer;

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
}

void GLState::BindElementArrayBuffer(const GLuint buffer)
{
	if (boundElementArrayBuffer == buffer) return;
	boundElementArrayBuffer = buffer;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
}

void GLState::BindPixelUnpackBuffer(const GLuint buffer)
{
	if (boundPixelUnpackBuffer == buffer) return;
	boundPixelUnpackBuffer = buffer;

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer);
}

void GLState::BindSampler(const GLuint sampler)
{
	if (boundSampler == sampler) return;
	boundSampler = sampler;

	glBindSampler(0, sampler);
}

void GLState::GenVertexArrays(const GLsizei count, GLuint* arrays)
{
	glGenVertexArrays(count, arrays);

	for (GLsizei i = 0; i < count; i++)
	{
		if (vertexArrayObjects.count(arrays[i]) > 0)
		{
			throw std::runtime_error("VertexArrayObject already exists.");
		}

		vertexArrayObjects.insert(std::make_pair(arrays[i], new VertexArrayObject(maxVertexAttribs)));
	}
}

void GLState::DeleteVertexArrays(const GLsizei count, const GLuint* arrays)
{
	for (GLsizei i = 0; i < count; i++)
	{
		if (vertexArrayObjects.count(arrays[i]) == 0)
		{
			throw std::runtime_error("VertexArrayObject doesn't exist.");
		}

		VertexArrayObject* vao = vertexArrayObjects.at(arrays[i]);
		delete vao;
		vertexArrayObjects.erase(arrays[i]);
	}

	glDeleteVertexArrays(count, arrays);
}

void GLState::BindVertexArray(const GLuint vao)
{
	if (boundVao == vao) return;
	boundVao = vao;

	glBindVertexArray(vao);
}

void GLState::EnableVertexAttribArray(const GLuint index, const GLboolean enable)
{
	VertexArrayObject* vao = vertexArrayObjects.at(boundVao);

	if (vao->vertexAttribs[index].enabled == enable) return;
	vao->vertexAttribs[index].enabled = enable;

	if (enable) glEnableVertexAttribArray(index);
	else glDisableVertexAttribArray(index);
}

void GLState::VertexAttribPointer(const GLuint index, const GLint size, const GLenum type, const GLboolean normalized, const GLsizei stride, const void* pointer)
{
	VertexArrayObject* vao = vertexArrayObjects.at(boundVao);

	if (vao->vertexAttribs[index].size == size &&
		vao->vertexAttribs[index].type == type &&
		vao->vertexAttribs[index].normalized == normalized &&
		vao->vertexAttribs[index].stride == stride &&
		vao->vertexAttribs[index].pointer == pointer) return;

	vao->vertexAttribs[index].size = size;
	vao->vertexAttribs[index].type = type;
	vao->vertexAttribs[index].normalized = normalized;
	vao->vertexAttribs[index].stride = stride;
	vao->vertexAttribs[index].pointer = pointer;

	glVertexAttribPointer(index, size, type, normalized, stride, pointer);
}

GLuint GLState::CreateProgram()
{
	GLuint program = glCreateProgram();

	if (shaderPrograms.count(program) > 0)
	{
		throw std::runtime_error("Shader program already exists.");
	}

	shaderPrograms.insert(std::make_pair(program, new ShaderProgram()));

	return program;
}

void GLState::DeleteProgram(const GLuint program)
{
	if (shaderPrograms.count(program) == 0)
	{
		throw std::runtime_error("Shader program doesn't exist.");
	}

	ShaderProgram* sp = shaderPrograms.at(program);
	delete sp;
	shaderPrograms.erase(program);

	glDeleteProgram(program);
}

void GLState::UseProgram(const GLuint value)
{
	if (boundShaderProgram == value) return;
	boundShaderProgram = value;

	glUseProgram(value);
}

void GLState::Uniform1i(const GLint location, const GLint value)
{
	ShaderProgram* sp = shaderPrograms.at(boundShaderProgram);

	if (sp->uniforms.count(location) == 0)
	{
		Uniform u;
		sp->uniforms.insert(std::make_pair(location, u));
	}

	Uniform* uniform = &sp->uniforms.at(location);

	if (uniform->u_1i == value) return;
	uniform->u_1i = value;

	glUniform1i(location, value);
}

void GLState::UniformMatrix4fv(const GLint location, const GLsizei count, const GLboolean transpose, const GLfloat* value)
{
	ShaderProgram* sp = shaderPrograms.at(boundShaderProgram);

	if (sp->uniforms.count(location) == 0)
	{
		Uniform u;
		sp->uniforms.insert(std::make_pair(location, u));
	}

	Uniform* uniform = &sp->uniforms.at(location);

	if (memcmp(value, uniform->u_matrix4fv, sizeof(GLfloat) * 16) == 0) return;
	memcpy(uniform->u_matrix4fv, value, sizeof(GLfloat) * 16);

	glUniformMatrix4fv(location, count, transpose, value);
}
