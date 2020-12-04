#include "SpriteBacth.h"
#include <algorithm>

SpriteBacth::SpriteBacth():_vbo(0),_vao(0)
{
}

void SpriteBacth::init() {
	createVertexArray();
}

void SpriteBacth::begin(GlyphSortType sortType) {
	_sortType = sortType;
	_renderBacthes.clear();
	for (int i = 0; i < _glyphs.size(); i++)
	{
		delete _glyphs[i];
	}
	_glyphs.clear();
}
void SpriteBacth::end() {
	sortGlyph();
	createRenderBatches();
}

void SpriteBacth::createRenderBatches() {
	std::vector<Vertex> vertices;
	vertices.resize(_glyphs.size() * 6);
	if (_glyphs.empty()) {
		return;
	}
	int cv = 0;
	int ofset = 0;
	_renderBacthes.emplace_back(ofset, 6, _glyphs[0]->texture);
	vertices[cv++] = _glyphs[0]->topLeft;
	vertices[cv++] = _glyphs[0]->bottomLeft;
	vertices[cv++] = _glyphs[0]->bottomRight;
	vertices[cv++] = _glyphs[0]->bottomRight;
	vertices[cv++] = _glyphs[0]->topRight;
	vertices[cv++] = _glyphs[0]->topLeft;
	ofset += 6;

	
	for (int cg = 1; cg < _glyphs.size(); cg++)
	{
		if (_glyphs[cg]->texture != _glyphs[cg-1]->texture) {
			_renderBacthes.emplace_back(ofset, 6, _glyphs[cg]->texture);
		}
		else {
			_renderBacthes.back()._numVertixes += 6;
		}
		
		vertices[cv++] = _glyphs[cg]->topLeft;
		vertices[cv++] = _glyphs[cg]->bottomLeft;
		vertices[cv++] = _glyphs[cg]->bottomRight;
		vertices[cv++] = _glyphs[cg]->bottomRight;
		vertices[cv++] = _glyphs[cg]->topRight;
		vertices[cv++] = _glyphs[cg]->topLeft;
		ofset += 6;
		
	}
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	//glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),vertices.data(), GL_DYNAMIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SpriteBacth::renderBatch() {
	glBindVertexArray(_vao);
	for (int i = 0; i < _renderBacthes.size(); i++)
	{
		glBindTexture(GL_TEXTURE_2D, _renderBacthes[i]._texture);
		glDrawArrays(GL_TRIANGLES, _renderBacthes[i]._offset, _renderBacthes[i]._numVertixes);
	}
	glBindVertexArray(0);
}

void SpriteBacth::draw(const glm::vec4& destRect, const glm::vec4& uvRect, GLuint texture, float depth, const Color& color) {
	Glyph* glyph = new Glyph;
	glyph->texture = texture;
	glyph->depth = depth;

	glyph->topLeft.color = color;
	glyph->topLeft.setPosition(destRect.x, destRect.y + destRect.w);
	glyph->topLeft.setUV(uvRect.x, uvRect.y + uvRect.w);

	glyph->bottomLeft.color = color;
	glyph->bottomLeft.setPosition(destRect.x, destRect.y);
	glyph->bottomLeft.setUV(uvRect.x, uvRect.y);

	glyph->bottomRight.color = color;
	glyph->bottomRight.setPosition(destRect.x + destRect.z, destRect.y);
	glyph->bottomRight.setUV(uvRect.x + uvRect.z, uvRect.y);

	glyph->topRight.color = color;
	glyph->topRight.setPosition(destRect.x + destRect.z, destRect.y + destRect.w);
	glyph->topRight.setUV(uvRect.x + uvRect.z, uvRect.y + uvRect.w);

	_glyphs.push_back(glyph);
}

void SpriteBacth::sortGlyph() {
	switch (_sortType)
	{
		case GlyphSortType::BACK_TO_FRONT:
			std::stable_sort(_glyphs.begin(), _glyphs.end(), compareBackToFront);
			break;
		case GlyphSortType::FRONT_TO_BACK:
			std::stable_sort(_glyphs.begin(), _glyphs.end(), compareFrontToBack);
			break;
		case GlyphSortType::TEXTURE:
			std::stable_sort(_glyphs.begin(), _glyphs.end(), compareTexture);
			break;
	}
}

bool SpriteBacth::compareFrontToBack(Glyph* a, Glyph* b) {
	return (a->depth < b->depth);
}

bool SpriteBacth::compareBackToFront(Glyph* a, Glyph* b) {
	return (a->depth > b->depth);
}

bool SpriteBacth::compareTexture(Glyph* a, Glyph* b) {
	return (a->texture < b->texture);
}

void SpriteBacth::createVertexArray() {
	if (_vao == 0) {
		glGenVertexArrays(1, &_vao);
	}
	glBindVertexArray(_vao);
	if (_vbo == 0) {
		glGenBuffers(1, &_vbo);
	}

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);


	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*)offsetof(Vertex, position));

	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex),
		(void*)offsetof(Vertex, color));

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*)offsetof(Vertex, uv));
	glBindVertexArray(0);
}


SpriteBacth::~SpriteBacth()
{
}

RenderBatch::RenderBatch(GLuint offset, GLuint numVertixex, GLuint texture):_offset(offset), _numVertixes(numVertixex), _texture(texture)
{
}
