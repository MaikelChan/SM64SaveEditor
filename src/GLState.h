#pragma once

#include <unordered_map>
#include <glad/gl.h>

struct VertexAttrib
{
	GLboolean enabled;
	GLboolean normalized;
	GLint size;
	GLenum type;
	GLsizei stride;
	const void* pointer;

	VertexAttrib();
};

struct VertexArrayObject
{
	VertexAttrib* vertexAttribs;

	VertexArrayObject(const GLint vertexAttribCount);
	~VertexArrayObject();
};

struct Uniform
{
	GLint u_1i{};
	GLfloat u_matrix4fv[4][4]{};
};

struct ShaderProgram
{
	std::unordered_map<GLint, Uniform> uniforms;
};

class GLState
{
private:
	GLint maxVertexAttribs;

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
	GLuint boundVao;
	GLuint boundSampler;

	std::unordered_map<GLuint, VertexArrayObject*> vertexArrayObjects;
	std::unordered_map<GLuint, ShaderProgram*> shaderPrograms;

public:
	GLState();
	~GLState();

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
	void BindVao(const GLuint vao);
	void BindSampler(const GLuint sampler);

	void EnableVertexAttribArray(const GLuint index, const GLboolean enable);
	void VertexAttribPointer(const GLuint index, const GLint size, const GLenum type, const GLboolean normalized, const GLsizei stride, const void* pointer);

	void Uniform1i(const GLint location, const GLint value);
	void UniformMatrix4fv(const GLint location, const GLsizei count, const GLboolean transpose, const GLfloat* value);
};