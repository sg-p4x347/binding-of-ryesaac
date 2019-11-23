#pragma once
#include "pch.h"
#include "Model.h"
#include "Vector3.h"
#include "Transform.h"
#include <fbxsdk.h>

class ModelImporter
{
	public:


		// FBX import; documentation: http://help.autodesk.com/view/FBX/2019/ENU/?guid=FBX_Developer_Help_getting_started_html
		static Model CreateFromFBX(path file);

	private:
		//----------------------------------------------------------------
		// FBX import
		static Model Import(fbxsdk::FbxScene* scene);
		//----------------------------------------------------------------
		// Mesh import
		static void ProcessNode(fbxsdk::FbxNode* node, std::vector<ModelMesh>& meshes);
		static void ProcessNodeChildren(fbxsdk::FbxNode* node, std::vector<ModelMesh>& meshes);
		static ModelMesh CreateMesh(fbxsdk::FbxMesh* fbxMesh);
		//----------------------------------------------------------------
		// Material import
		static void ImportMaterials(fbxsdk::FbxScene* scene);
		//----------------------------------------------------------------
		// Texture import
		static void ImportTextures(fbxsdk::FbxScene* scene);

		//----------------------------------------------------------------
		// Helpers
		static Vector3 Convert(fbxsdk::FbxDouble3& double3);
		static vector<string> GetTextureConnections(fbxsdk::FbxProperty&& property);
		static M4 Convert(fbxsdk::FbxMatrix& matrix);
};