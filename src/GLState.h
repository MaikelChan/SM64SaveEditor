#pragma once

#include <glad/gl.h>

struct VertexAttrib
{
	GLboolean enabled;
	GLint size;
	GLenum type;
	GLboolean normalized;
	GLsizei stride;
	const void* pointer;

	VertexAttrib();
};

class GLState
{
private:
	GLfloat clearColorR, clearColorG, clearColorB, clearColorA;
	GLboolean blendEnabled;
	GLboolean cullFaceEnabled;
	GLboolean depthTestEnabled;
	GLboolean stencilTestEnabled;
	GLboolean scissorTestEnabled;
	GLboolean primitiveRestartEnabled;
	GLenum blendEquation;
	GLenum bfsSrcRgb, bfsDstRgb, bfsSrcAlpha, bfsDstAlpha;
	GLint viewportX, viewportY;
	GLsizei viewportW, viewportH;
	GLint scissorX, scissorY;
	GLsizei scissorW, scissorH;
	GLenum frontPolygonMode, backPolygonMode;
	GLuint shaderProgram;
	GLuint boundTexture2D;
	GLuint boundArrayBuffer;
	GLuint boundElementArrayBuffer;
	VertexAttrib vertexAttribs[8];
	GLuint boundVao;
	GLuint boundSampler;

public:
	GLState();

	void ClearColor(const GLfloat r, const GLfloat g, const GLfloat b, const GLfloat a);
	void EnableBlend(const GLboolean enable);
	void EnableCullFace(const GLboolean enable);
	void EnableDepthTest(const GLboolean enable);
	void EnableStencilTest(const GLboolean enable);
	void EnableScissorTest(const GLboolean enable);
	void EnablePrimitiveRestart(const GLboolean enable);
	void BlendEquation(const GLenum value);
	void BlendFuncSeparate(const GLenum srcRgb, const GLenum dstRgb, const GLenum srcAlpha, const GLenum dstAlpha);
	void Viewport(const GLint x, const GLint y, const GLsizei w, const GLsizei h);
	void Scissor(const GLint x, const GLint y, const GLsizei w, const GLsizei h);
	void PolygonMode(const GLenum face, const GLenum mode);
	void SetShaderProgram(const GLuint value);
	void BindTexture2D(const GLuint texture2d);
	void BindArrayBuffer(const GLuint buffer);
	void BindElementArrayBuffer(const GLuint buffer);
	void EnableVertexAttribArray(const GLuint index, const GLboolean enable);
	void VertexAttribPointer(const GLuint index, const GLint size, const GLenum type, const GLboolean normalized, const GLsizei stride, const void* pointer);
	void BindVao(const GLuint vao);
	void BindSampler(const GLuint sampler);
};