- Loop through all parsed materials and add them to the material manager
- If a material is already loaded, override it (discarding previous info)
- Load textures of the material if not yet loaded (we'll be doing streaming later)

```cpp
struct IStreamingTexture : public ITexture
{
    virtual bool IsLoaded() const = 0;
    virtual const string& GetName() const = 0;
};

struct IResourcePool
{
    // Returns a pointer to a streaming-texture object with the given specification.
    // The specification is the filename of the texture!
    // If there is no such object, it will be created, loaded and the pointer to it is returned.
    // Returns:
    //     - 0 if the given specification is not valid (length 0)
    //
    // TODO: Do not load the texture immediately. Instead, add this streaming-texture object to the
    //      loading-queue, which is processed in a separate streaming-thread.
    virtual ITexture* GetStreamingTexture(const string& specification) = 0;
};

void LoadMaterials(const char* smbFile)
{
    vector<SSMBMaterial> loadedMaterials;
    smbLoader.ReadSMBFile(smbFile, loadedMaterials);
    for (auto itMaterial = loadedMaterials.begin(); itMaterial != loadedMaterials.end; ++itMaterial)
    {
        // Remove material if existing
        matMgr.RemoveMaterial(itMaterial->name);

        // Add material
        IMaterial* pNewMat = matMgr.CreateMaterial(itMaterial->name);
        pNewMat->pTextureMap = pResourcePool->GetStreamingTexture(itMaterial->textureMap);
        pNewMat->pNormalMap = pResourcePool->GetStreamingTexture(itMaterial->normalMap);
        ...
    }
}

// Call this after LoadMaterials() to fix missing pointers in all objects
void Scene::ReassignMaterials()
{
    for (auto itObject = objects.begin(); itObject != objects.end(); ++itObject)
    {
        pRenderable = ...;
        for (each itSubset in pRenderable->GetGeometry())
        {
            if (IsValidMaterialName(itSubset->materialName))
                itSubset->pMaterial = pMatMgr->FindMaterial(itSubset->materialName);
        }
    }
}
```