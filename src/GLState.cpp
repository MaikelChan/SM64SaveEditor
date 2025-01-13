#include "GLState.h"
#include <cstdint>

VertexAttrib::VertexAttrib()
{
	enabled = GL_FALSE;
	size = 4;
	type = GL_FLOAT;
	normalized = GL_FALSE;
	stride = 0;
	pointer = 0;
}

GLState::GLState()
{
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
	shaderProgram = 0;
	boundTexture2D = 0;
	boundArrayBuffer = 0;
	boundElementArrayBuffer = 0;
	boundVao = 0;
	boundSampler = 0;

	int maxVertexAttribs;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs);
	vertexAttribs = new VertexAttrib[maxVertexAttribs];
}

GLState::~GLState()
{
	delete[] vertexAttribs;
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

void GLState::SetShaderProgram(const GLuint value)
{
	if (shaderProgram == value) return;
	shaderProgram = value;

	glUseProgram(value);
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

void GLState::BindVao(const GLuint vao)
{
	if (boundVao == vao) return;
	boundVao = vao;

	glBindVertexArray(vao);
}

void GLState::BindSampler(const GLuint sampler)
{
	if (boundSampler == sampler) return;
	boundSampler = sampler;

	glBindSampler(0, sampler);
}


void GLState::EnableVertexAttribArray(const GLuint index, const GLboolean enable)
{
	// TODO: This is not entirely correct, as it assumes the same VAO is always bound.

	if (vertexAttribs[index].enabled == enable) return;
	vertexAttribs[index].enabled = enable;

	if (enable) glEnableVertexAttribArray(index);
	else glDisableVertexAttribArray(index);
}

void GLState::VertexAttribPointer(const GLuint index, const GLint size, const GLenum type, const GLboolean normalized, const GLsizei stride, const void* pointer)
{
	// TODO: This is not entirely correct, as it assumes the same VAO is always bound.

	if (vertexAttribs[index].size == size &&
		vertexAttribs[index].type == type &&
		vertexAttribs[index].normalized == normalized &&
		vertexAttribs[index].stride == stride &&
		vertexAttribs[index].pointer == pointer) return;

	vertexAttribs[index].size = size;
	vertexAttribs[index].type = type;
	vertexAttribs[index].normalized = normalized;
	vertexAttribs[index].stride = stride;
	vertexAttribs[index].pointer = pointer;

	glVertexAttribPointer(index, size, type, normalized, stride, pointer);
}