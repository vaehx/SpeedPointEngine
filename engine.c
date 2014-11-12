



Rendering Pipeline:






- Vertex Shader Stage
- Rasterization Stage
- Pixel Shader Stage
- Outputmerger Stage


struct SVertex
{
  struct
  {
    double x, y, z;
  } P;
  struct
  {  
    double nx, ny, nz;
  } N;
  struct
  {
    double tu, tv;
  } TC;   
};

struct SObject
{
  SVertex* pVertices;
  unsigned int nVertices;
};

struct SColor
{
  float r, g, b;
};

struct SScreenPixel
{
  SColor color;
  double z;   // for z-testing. double precision 
};

struct SPixel
{
  SColor color;
  struct
  {
    int x, y, d;
  } screenSpacePosition;  
  struct
  {
    double u, v;
  } texCoord;
  struct
  {
    double x, y, z;
  } N;       
};


void main()
{
  SObject* pObjects = ...;
  SColor* pScreenPixels = ...;
  for (unsigned int iObject = 0; iObject < nObjects; ++iObjects)
    RenderPipeline(pObjects[iObject], pScreenPixels);
}

void RenderForward(const SObject* pObjects, const SScreenPixel* pScreenPixels)
{

}

void GraphicsPipeline(const SObject& obj, SColor* pScreenPixels)
{    

  // 1. Vertex Shader Stage
  SVertex* pVertexBuffer = new SVertex[obj.nVertices];             
  for (unsigned int iVtx = 0; iVtx < obj.nVertices; ++iVtx)
    pVertexBuffer[iVtx] = VertexShader(obj.pVertices[iVtx]);
  
  // 2. Rasterization Stage  
  unsigned int nPixels;
  SPixel* pPixels = Rasterize(pVertexBuffer, obj.nVertices, &nPixels);
  
  // 3. Pixel Shader Stage
  for (unsigned int iPxl = 0; iPxl < nPixels; ++iPxl)
    pPixels[iPxl] = PixelShader(pPixels[iPxl]);        
  
  // 4. Outputmerger Stage
  for (unsigned int iPxl = 0; iPxl < nPixels; ++iPxl)
  {
    unsigned int iAccordingScreenPixelIndex =
      GetScreenPixelIndex(pPixels[iPxl].screenSpacePosition.x, pPixels[iPxl].screenSpacePosition.y); 
      
    // perform z-test
    if (pScreenPixels[iAccordingScreenPixel].z > pPixels[iPxl].screenSpacePosition.d)
      continue;
    
    pScreenPixels[iAccordingScreenPixelIndex].color = pPixels[iPxl].color;
    pScreenPixels[iAccordingScreenPixelIndex].z = pPixels[iPxl].screenSpacePosition.d;
  }
  
}

SVertex VertexShader(SVertex vtx)
{
  // Take care to use 4-Component vectors for position here!
  vtx.P = mul(worldMtx, vtx.P);
  vtx.P = mul(viewMtx, vtx.P);
  vtx.P = mul(projMtx, vtx.P);
  
  return vtx;
}

SPixel PixelShader(SPixel pxl)
{
  pxl.color = SampleTex2D(tex, pxl.texCoord, ...);
  ...
  return pxl;
}
