#include "pch.h"

#include "ModelImporter.h"

namespace geom {
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


			Model model = Import(lScene);
			model.Name = file.stem().string();

			// Destroy the SDK manager and all the other objects it was handling.
			lSdkManager->Destroy();

			return model;

		}
		return geom::Model{};
	}
	void ModelImporter::ProcessNode(fbxsdk::FbxNode* node, Model& model)
	{
		auto attribute = node->GetNodeAttribute();
		if (attribute) {
			auto type = attribute->GetAttributeType();
			// Attribute type determines how to process child nodes
			if (type == fbxsdk::FbxNodeAttribute::eMesh) {
				ProcessMesh((fbxsdk::FbxMesh*)attribute, model);
			}
			else {
				ProcessNodeChildren(node, model);
			}
		}
		else {
			ProcessNodeChildren(node, model);
		}
	}
	void ModelImporter::ProcessNodeChildren(fbxsdk::FbxNode* node, Model& model)
	{
		for (int i = 0; i < node->GetChildCount(); i++) {
			auto child = node->GetChild(i);
			ProcessNode(child, model);
		}
	}
	void ModelImporter::ProcessMesh(fbxsdk::FbxMesh* fbxMesh, Model& model)
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
							vertex.TextureCoordinate.Y = (float)uv[1];
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
				
			}
			// Triangulate as fan
			// triangulate polygon

			for (int triangleIndex = 0; triangleIndex < vertexCount - 2; triangleIndex++) {
				mesh.Indices.push_back(index);
				mesh.Indices.push_back(index + triangleIndex + 1);
				mesh.Indices.push_back(index + triangleIndex + 2);
			}
			

			index += vertexCount;
		}

		// assign a material
		auto elementMaterial = fbxMesh->GetElementMaterial();
		if (elementMaterial) {
			fbxsdk::FbxSurfaceMaterial* fbxMaterial = fbxMesh->GetNode()->GetMaterial(elementMaterial->GetIndexArray().GetAt(0));
			mesh.Material = model.Materials[fbxMaterial->GetName()];
		}

		// add the mesh to the model
		model.Meshes.push_back(mesh);
	}
	void ModelImporter::ImportMaterials(fbxsdk::FbxScene* scene, Model& model)
	{
		for (int i = 0; i < scene->GetMaterialCount(); i++) {
			shared_ptr<Material> material = std::make_shared<Material>();
			auto fbxMaterial = scene->GetMaterial(i);
			/*material.name = fbxMaterial->GetName();
			material.pixelShader = fbxMaterial->ShadingModel.GetInstance() + ".cso";*/

			//GetInstance the implementation to see if it's a hardware shader.
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
							for (int j = 0; j < lFbxProp.GetSrcObjectCount<fbxsdk::FbxFileTexture>(); ++j)
							{
								fbxsdk::FbxFileTexture* lTex = lFbxProp.GetSrcObject<fbxsdk::FbxFileTexture>(j);
								path texturePath = lTex->GetFileName();
								material->DiffuseTexture = texturePath.stem().string();
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
				//material.ambientColor = Convert(((FbxSurfacePhong*)fbxMaterial)->Ambient.GetInstance());
				//// Display the Diffuse Color
				//material.diffuseColor = Convert(((FbxSurfacePhong*)fbxMaterial)->Diffuse.GetInstance());
				//// Display the Specular Color (unique to Phong materials)
				//material.specularColor = Convert(((FbxSurfacePhong*)fbxMaterial)->Specular.GetInstance());
				//// Display the Emissive Color
				//material.emissiveColor = Convert(((FbxSurfacePhong*)fbxMaterial)->Emissive.GetInstance());
				////Opacity is Transparency factor now
				//material.alpha = (float)((FbxSurfacePhong*)fbxMaterial)->TransparencyFactor.GetInstance();
				//// Display the Shininess
				//material.specularPower = (float)((FbxSurfacePhong*)fbxMaterial)->Shininess;
				//// Display the Reflectivity
				////((FbxSurfacePhong *)fbxMaterial)->ReflectionFactor;
			}
			else if (fbxMaterial->GetClassId().Is(fbxsdk::FbxSurfaceLambert::ClassId))
			{
				//// We found a Lambert material. Display its properties.
				//// Display the Ambient Color
				//material.ambientColor = Convert(((FbxSurfaceLambert*)fbxMaterial)->Ambient.GetInstance());
				//// Display the Diffuse Color
				//material.diffuseColor = Convert(((FbxSurfaceLambert*)fbxMaterial)->Diffuse.GetInstance());
				//// Display the Emissive
				//material.emissiveColor = Convert(((FbxSurfaceLambert*)fbxMaterial)->Emissive.GetInstance());
				//// Display the Opacity
				//material.alpha = (float)(((FbxSurfaceLambert*)fbxMaterial)->TransparencyFactor.GetInstance());
			}

			// Diffuse Textures

			auto diffuseTextures = GetTextureConnections(fbxMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sDiffuse));
			if (!diffuseTextures.empty())
				material->DiffuseTexture = diffuseTextures[0];
			//// Specular Textures
			//vector<string> specularTextures = GetTextureConnections(fbxMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sSpecular));
			//if (specularTextures.size())
			//	cout << "Specular";
			//// Normal Textures
			//vector<string> normalTextures = GetTextureConnections(fbxMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sNormalMap));
			//if (normalTextures.size()) 
			//	cout << "normal";

			model.Materials[fbxMaterial->GetName()] = material;
		}
	}
	Vector3 ModelImporter::Convert(fbxsdk::FbxDouble3& double3)
	{
		return Vector3(double3[0], double3[1], double3[2]);
	}
	vector<string> ModelImporter::GetTextureConnections(fbxsdk::FbxProperty&& property)
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
	Model ModelImporter::Import(fbxsdk::FbxScene* scene)
	{
		Model model;
		// import materials
		ImportMaterials(scene, model);

		// import meshes recursively
		ProcessNode(scene->GetRootNode(), model);

		return model;
	}
}