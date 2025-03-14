/*!
 * @file
 * @brief This file contains implementation of gpu
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <student/gpu.hpp>

uint32_t computeVertexID(VertexArray const& vao, uint32_t shaderInvocation) {
    if (!vao.indexBuffer)return shaderInvocation;

    // we want UINT32
    if (vao.indexType == IndexType::UINT32) {
        uint32_t* ind = (uint32_t*)vao.indexBuffer;
        return ind[shaderInvocation];
    }
    // we want UINT16
    if (vao.indexType == IndexType::UINT16) {
        uint16_t* ind = (uint16_t*)vao.indexBuffer;
        return ind[shaderInvocation];
    }
    // we want UINT8
    if (vao.indexType == IndexType::UINT8) {
        uint8_t* ind = (uint8_t*)vao.indexBuffer;
        return ind[shaderInvocation];
    }
}

//! [drawTrianglesImpl]
void drawTrianglesImpl(GPUContext &ctx,uint32_t nofVertices){
  (void)ctx;
  (void)nofVertices;
  for (int i = 0; i < nofVertices; i++) {//smyčka přes vrcholy
      InVertex  inVertex; // vrchol, co vstupuje do vertex shader
      OutVertex outVertex;// vrchol, co leze ven z vertex shaderu
      inVertex.gl_VertexID = computeVertexID(ctx.vao, i);
      
      // for cycle to maxAttributes
      for (int j = 0; j < maxAttributes; j++) {
          // type = FLOAT
          if (ctx.vao.vertexAttrib[j].type == AttributeType::FLOAT) {
              float atrbt;
              uint64_t stride = ctx.vao.vertexAttrib[j].stride;
              uint64_t offset = ctx.vao.vertexAttrib[j].offset;
              atrbt = *(float*)ctx.vao.vertexAttrib[j].bufferData + offset + stride * inVertex.gl_VertexID;
              inVertex.attributes[j].v1 = atrbt / 4;
          }
          // type = VEC2
          if (ctx.vao.vertexAttrib[j].type == AttributeType::VEC2) {
              char* atrbt;
              uint64_t stride = ctx.vao.vertexAttrib[j].stride;
              uint64_t offset = ctx.vao.vertexAttrib[j].offset;
              atrbt = (char*)ctx.vao.vertexAttrib[j].bufferData + offset + stride * inVertex.gl_VertexID;
              inVertex.attributes[j].v2 = *(glm::vec2*)atrbt;
          }
          // type = VEC3
          if (ctx.vao.vertexAttrib[j].type == AttributeType::VEC3) {
              char* atrbt;
              uint64_t stride = ctx.vao.vertexAttrib[j].stride;
              uint64_t offset = ctx.vao.vertexAttrib[j].offset;
              atrbt = (char*)ctx.vao.vertexAttrib[j].bufferData + offset + stride * inVertex.gl_VertexID;
              inVertex.attributes[j].v3 = *(glm::vec3 *)atrbt;
          }
          // type = VEC4
          if (ctx.vao.vertexAttrib[j].type == AttributeType::VEC4) {
              char* atrbt;
              uint64_t stride = ctx.vao.vertexAttrib[j].stride;
              uint64_t offset = ctx.vao.vertexAttrib[j].offset;
              atrbt = (char*)ctx.vao.vertexAttrib[j].bufferData + offset + stride * inVertex.gl_VertexID;
              inVertex.attributes[j].v4 = *(glm::vec4*)atrbt;
          }
      }
      ctx.prg.vertexShader(outVertex, inVertex, ctx.prg.uniforms);
  }
  /// \todo Tato funkce vykreslí trojúhelníky podle daného nastavení.<br>
  /// ctx obsahuje aktuální stav grafické karty.
  /// Parametr "nofVertices" obsahuje počet vrcholů, který by se měl vykreslit (3 pro jeden trojúhelník).<br>
  /// Bližší informace jsou uvedeny na hlavní stránce dokumentace.
}
//! [drawTrianglesImpl]

/**
 * @brief This function reads color from texture.
 *
 * @param texture texture
 * @param uv uv coordinates
 *
 * @return color 4 floats
 */
glm::vec4 read_texture(Texture const&texture,glm::vec2 uv){
  if(!texture.data)return glm::vec4(0.f);
  auto uv1 = glm::fract(uv);
  auto uv2 = uv1*glm::vec2(texture.width-1,texture.height-1)+0.5f;
  auto pix = glm::uvec2(uv2);
  //auto t   = glm::fract(uv2);
  glm::vec4 color = glm::vec4(0.f,0.f,0.f,1.f);
  for(uint32_t c=0;c<texture.channels;++c)
    color[c] = texture.data[(pix.y*texture.width+pix.x)*texture.channels+c]/255.f;
  return color;
}

/**
 * @brief This function clears framebuffer.
 *
 * @param ctx GPUContext
 * @param r red channel
 * @param g green channel
 * @param b blue channel
 * @param a alpha channel
 */
void clear(GPUContext&ctx,float r,float g,float b,float a){
  auto&frame = ctx.frame;
  auto const nofPixels = frame.width * frame.height;
  for(size_t i=0;i<nofPixels;++i){
    frame.depth[i] = 10e10f;
    frame.color[i*4+0] = static_cast<uint8_t>(glm::min(r*255.f,255.f));
    frame.color[i*4+1] = static_cast<uint8_t>(glm::min(g*255.f,255.f));
    frame.color[i*4+2] = static_cast<uint8_t>(glm::min(b*255.f,255.f));
    frame.color[i*4+3] = static_cast<uint8_t>(glm::min(a*255.f,255.f));
  }
}

