#include "pch.h"
#include "ModelImporter.h"

Model ModelImporter::CreateFromFBX(path file)
{
	if (std::filesystem::exists(file)) {
		string pathString = file.string();
		// Change the following filename to a suitable filename value.
		const char* lFilename = pathString.c_str();

		// Initialize the SDK manager. This object handles memory management.
		fbxsdk::FbxManager* lSdkManager = fbxsdk::FbxManager::Create();

		// Create the IO settings object.
		fbxsdk::FbxIOSettings* ios = fbxsdk::FbxIOSettings::Create(lSdkManager, IOSROOT);
		lSdkManager->SetIOSettings(ios);

		// Create an importer using the SDK manager.
		fbxsdk::FbxImporter* lImporter = fbxsdk::FbxImporter::Create(lSdkManager, "");

		// Use the first argument as the filename for the importer.
		if (!lImporter->Initialize(lFilename, -1, lSdkManager->GetIOSettings())) {
			throw std::exception(("Call to FbxImporter::Initialize() failed. Error returned: " + string(lImporter->GetStatus().GetErrorString())).c_str());
		}
		// Create a new scene so that it can be populated by the imported file.
		fbxsdk::FbxScene* lScene = fbxsdk::FbxScene::Create(lSdkManager, "myScene");

		// Import the contents of the file into the scene.
		lImporter->Import(lScene);

		// The file is imported, so get rid of the importer.
		lImporter->Destroy();


		Import(lScene);

		// Destroy the SDK manager and all the other objects it was handling.
		lSdkManager->Destroy();

	}
}
void ModelImporter::ProcessNode(fbxsdk::FbxNode* node, std::vector<ModelMesh>& meshes)
{
	auto attribute = node->GetNodeAttribute();
	if (attribute) {
		auto type = attribute->GetAttributeType();
		// Attribute type determines how to process child nodes
		if (type == fbxsdk::FbxNodeAttribute::eMesh) {
			meshes.push_back(CreateMesh((fbxsdk::FbxMesh*)attribute));
		}
		else {
			ProcessNodeChildren(node, meshes);
		}
	}
	else {
		ProcessNodeChildren(node, meshes);
	}
}
void ModelImporter::ProcessNodeChildren(fbxsdk::FbxNode* node, std::vector<ModelMesh>& meshes)
{
	for (int i = 0; i < node->GetChildCount(); i++) {
		auto child = node->GetChild(i);
		ProcessNode(child, meshes);
	}
}
ModelMesh ModelImporter::CreateMesh(fbxsdk::FbxMesh* fbxMesh)
{
	ModelMesh mesh;
	int index = 0;
	for (int polyIndex = 0; polyIndex < fbxMesh->GetPolygonCount(); polyIndex++) {

		int vertexCount = fbxMesh->GetPolygonSize(polyIndex);
		int polygonIndexStart = fbxMesh->GetPolygonVertexIndex(polyIndex);
		int controlPointIndex = polygonIndexStart + polyIndex;
		for (int polyVertIndex = 0; polyVertIndex < vertexCount; polyVertIndex++) {
			ModelMeshVertex vertex;
			auto transform = fbxMesh->GetNode()->EvaluateGlobalTransform();
			auto control = transform.MultT(fbxMesh->GetControlPointAt(fbxMesh->GetPolygonVertex(polyIndex, polyVertIndex)).mData);
			//auto control = fbxMesh->GetControlPointAt(fbxMesh->GetPolygonVertex(polyIndex, polyVertIndex)).mData;
			vertex.Position.X = (float)control[0];
			vertex.Position.Y = (float)control[1];
			vertex.Position.Z = (float)control[2];
			fbxsdk::FbxVector4 normal;
			if (fbxMesh->GetPolygonVertexNormal(polyIndex, polyVertIndex, normal)) {
				normal = transform.MultT(normal) - transform.MultT(fbxsdk::FbxZeroVector4);
				vertex.Normal = Vector3(normal[0], normal[1], normal[2]);
				vertex.Normal.Normalize();

			}
			for (int uvIndex = 0; uvIndex < fbxMesh->GetElementUVCount(); uvIndex++) {
				auto uvElement = fbxMesh->GetElementUV(uvIndex);
				switch (uvElement->GetMappingMode())
				{
				case fbxsdk::FbxGeometryElement::eByControlPoint:
					break;
				case fbxsdk::FbxGeometryElement::eByPolygonVertex:
				{
					int lTextureUVIndex = fbxMesh->GetTextureUVIndex(polyIndex, polyVertIndex);
					switch (uvElement->GetReferenceMode())
					{
					case fbxsdk::FbxGeometryElement::eDirect:
					case fbxsdk::FbxGeometryElement::eIndexToDirect:
					{
						auto uv = uvElement->GetDirectArray().GetAt(lTextureUVIndex);
						vertex.TextureCoordinate.X = (float)uv[0];
						vertex.TextureCoordinate.Y = -(float)uv[1];
					}
					break;
					default:
						break; // other reference modes not shown here!
					}
				}
				break;
				case fbxsdk::FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
				case fbxsdk::FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
				case fbxsdk::FbxGeometryElement::eNone:       // doesn't make much sense for UVs
					break;
				default:
					break;
				}
			}
			mesh.Vertices.push_back(vertex);
			// Indices
			if (polyVertIndex >= 1 && polyVertIndex < vertexCount - 1) {
				// triangulate polygon

				mesh.Indices.push_back(index + polyVertIndex + 1);
				mesh.Indices.push_back(index + polyVertIndex);
				mesh.Indices.push_back(index);

			}
		}
		index += vertexCount;
	}

	// assign a material
	auto elementMaterial = fbxMesh->GetElementMaterial();
	if (elementMaterial) {
		fbxsdk::FbxSurfaceMaterial* fbxMaterial = fbxMesh->GetNode()->GetMaterial(elementMaterial->GetIndexArray().GetAt(0));
		// fbxMaterial->GetName();
	}

	return mesh;
}
void ModelImporter::ImportMaterials(fbxsdk::FbxScene* scene)
{
	for (int i = 0; i < scene->GetMaterialCount(); i++) {
		auto fbxMaterial = scene->GetMaterial(i);
		/*material.name = fbxMaterial->GetName();
		material.pixelShader = fbxMaterial->ShadingModel.Get() + ".cso";*/

		//Get the implementation to see if it's a hardware shader.
		const fbxsdk::FbxImplementation* lImplementation = GetImplementation(fbxMaterial, FBXSDK_IMPLEMENTATION_HLSL);
		fbxsdk::FbxString lImplemenationType = "HLSL";
		if (!lImplementation)
		{
			lImplementation = GetImplementation(fbxMaterial, FBXSDK_IMPLEMENTATION_CGFX);
			lImplemenationType = "CGFX";
		}
		if (!lImplementation)
		{
			lImplementation = GetImplementation(fbxMaterial, FBXSDK_IMPLEMENTATION_SFX);
			lImplemenationType = "SFX";
		}
		if (!lImplementation)
		{
			lImplementation = GetImplementation(fbxMaterial, FBXSDK_IMPLEMENTATION_OGS);
			lImplemenationType = "OGS";
		}
		if (!lImplementation)
		{
			lImplementation = GetImplementation(fbxMaterial, FBXSDK_IMPLEMENTATION_MENTALRAY);
			lImplemenationType = "MENTALRAY";
		}
		if (!lImplementation)
		{
			lImplementation = GetImplementation(fbxMaterial, FBXSDK_IMPLEMENTATION_PREVIEW);
			lImplemenationType = "PREVIEW";
		}
		if (!lImplementation)
		{
			lImplementation = GetImplementation(fbxMaterial, FBXSDK_IMPLEMENTATION_NONE);
			lImplemenationType = "NONE";
		}
		if (lImplementation)
		{
			//Now we have a hardware shader, let's read it
			const fbxsdk::FbxBindingTable* lRootTable = lImplementation->GetRootTable();
			fbxsdk::FbxString lFileName = lRootTable->DescAbsoluteURL.Get();
			/*material.pixelShader = lFileName.Buffer();*/
			fbxsdk::FbxString lTechniqueName = lRootTable->DescTAG.Get();
			size_t lEntryNum = lRootTable->GetEntryCount();
			for (int i = 0; i < (int)lEntryNum; ++i)
			{
				const fbxsdk::FbxBindingTableEntry& lEntry = lRootTable->GetEntry(i);
				const char* lEntrySrcType = lEntry.GetEntryType(true);
				fbxsdk::FbxProperty lFbxProp;
				fbxsdk::FbxString lTest = lEntry.GetSource();
				if (strcmp(fbxsdk::FbxPropertyEntryView::sEntryType, lEntrySrcType) == 0)
				{
					lFbxProp = fbxMaterial->FindPropertyHierarchical(lEntry.GetSource());
					if (!lFbxProp.IsValid())
					{
						lFbxProp = fbxMaterial->RootProperty.FindHierarchical(lEntry.GetSource());
					}
				}
				else if (strcmp(fbxsdk::FbxConstantEntryView::sEntryType, lEntrySrcType) == 0)
				{
					lFbxProp = lImplementation->GetConstants().FindHierarchical(lEntry.GetSource());
				}
				if (lFbxProp.IsValid())
				{
					if (lFbxProp.GetSrcObjectCount<fbxsdk::FbxTexture>() > 0)
					{
						//do what you want with the textures
						for (int j = 0; j < lFbxProp.GetSrcObjectCount<fbxsdk::FbxFileTexture>(); ++j)
						{
							fbxsdk::FbxFileTexture* lTex = lFbxProp.GetSrcObject<fbxsdk::FbxFileTexture>(j);
						}
						for (int j = 0; j < lFbxProp.GetSrcObjectCount<fbxsdk::FbxLayeredTexture>(); ++j)
						{
							fbxsdk::FbxLayeredTexture* lTex = lFbxProp.GetSrcObject<fbxsdk::FbxLayeredTexture>(j);
						}
						for (int j = 0; j < lFbxProp.GetSrcObjectCount<fbxsdk::FbxProceduralTexture>(); ++j)
						{
							fbxsdk::FbxProceduralTexture* lTex = lFbxProp.GetSrcObject<fbxsdk::FbxProceduralTexture>(j);
						}
					}
				}
			}
		}
		else if (fbxMaterial->GetClassId().Is(fbxsdk::FbxSurfacePhong::ClassId))
		{
			//// We found a Phong material.  Display its properties.
			//// Display the Ambient Color
			//material.ambientColor = Convert(((FbxSurfacePhong*)fbxMaterial)->Ambient.Get());
			//// Display the Diffuse Color
			//material.diffuseColor = Convert(((FbxSurfacePhong*)fbxMaterial)->Diffuse.Get());
			//// Display the Specular Color (unique to Phong materials)
			//material.specularColor = Convert(((FbxSurfacePhong*)fbxMaterial)->Specular.Get());
			//// Display the Emissive Color
			//material.emissiveColor = Convert(((FbxSurfacePhong*)fbxMaterial)->Emissive.Get());
			////Opacity is Transparency factor now
			//material.alpha = (float)((FbxSurfacePhong*)fbxMaterial)->TransparencyFactor.Get();
			//// Display the Shininess
			//material.specularPower = (float)((FbxSurfacePhong*)fbxMaterial)->Shininess;
			//// Display the Reflectivity
			////((FbxSurfacePhong *)fbxMaterial)->ReflectionFactor;
		}
		else if (fbxMaterial->GetClassId().Is(fbxsdk::FbxSurfaceLambert::ClassId))
		{
			//// We found a Lambert material. Display its properties.
			//// Display the Ambient Color
			//material.ambientColor = Convert(((FbxSurfaceLambert*)fbxMaterial)->Ambient.Get());
			//// Display the Diffuse Color
			//material.diffuseColor = Convert(((FbxSurfaceLambert*)fbxMaterial)->Diffuse.Get());
			//// Display the Emissive
			//material.emissiveColor = Convert(((FbxSurfaceLambert*)fbxMaterial)->Emissive.Get());
			//// Display the Opacity
			//material.alpha = (float)(((FbxSurfaceLambert*)fbxMaterial)->TransparencyFactor.Get());
		}

		// Diffuse Textures
		vector<string> diffuseTextures = GetTextureConnections(fbxMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sDiffuse));
		//if (diffuseTextures.size()) material.textures.push_back(diffuseTextures[0]);
		// Specular Textures
		vector<string> specularTextures = GetTextureConnections(fbxMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sSpecular));
		//if (specularTextures.size()) material.textures.push_back(specularTextures[0]);
		// Normal Textures
		vector<string> normalTextures = GetTextureConnections(fbxMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sNormalMap));
		//if (normalTextures.size()) material.textures.push_back(normalTextures[0]);


	}
}
void ModelImporter::ImportTextures(fbxsdk::FbxScene* scene)
{
	for (int i = 0; i < scene->GetTextureCount(); i++) {
		auto texture = scene->GetTexture(i);
	}
}
Vector3 ModelImporter::Convert(fbxsdk::FbxDouble3& double3)
{
	return Vector3(double3[0], double3[1], double3[2]);
}
vector<string> ModelImporter::GetTextureConnections(fbxsdk::FbxProperty && property)
{
	vector<string> textures;
	//no layered texture simply get on the property
	int lNbTextures = property.GetSrcObjectCount<fbxsdk::FbxTexture>();
	for (int j = 0; j < lNbTextures; ++j)
	{
		fbxsdk::FbxFileTexture* lTexture = fbxsdk::FbxCast<fbxsdk::FbxFileTexture>(property.GetSrcObject<fbxsdk::FbxTexture>(j));
		if (lTexture)
		{
			//auto path = std::filesystem::proximate(lTexture->GetFileName(), "../");
			//textures.push_back(path.string());
		}
	}
	return textures;
}
M4 ModelImporter::Convert(fbxsdk::FbxMatrix& m)
{
	M4 matrix;
	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			// FBX is column major
			matrix.Set(column, row, m.Get(row, column));
		}
	}
	return matrix;
}
Model ModelImporter::Import(fbxsdk::FbxScene* scene)
{
	// import textures
	ImportTextures(scene);

	// import materials
	ImportMaterials(scene);

	// import meshes recursively
	vector<ModelMesh> meshes;
	ProcessNode(scene->GetRootNode(), meshes);

	return Model(meshes);
}