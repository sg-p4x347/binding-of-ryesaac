#pragma once
#include "pch.h"

#include <fbxsdk.h>

#include "math/Vector3.h"
using math::Vector3;

#include "Model.h"

namespace geom {
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
		// Material import
		static void ImportMaterials(fbxsdk::FbxScene* scene, Model& model);
		//----------------------------------------------------------------
		// Node processing
		static void ProcessNode(fbxsdk::FbxNode* node, Model& model);
		static void ProcessNodeChildren(fbxsdk::FbxNode* node, Model& model);
		static void ProcessMesh(fbxsdk::FbxMesh* fbxMesh, Model& model);
		//----------------------------------------------------------------
		// Helpers
		static Vector3 Convert(fbxsdk::FbxDouble3& double3);
		static vector<string> GetTextureConnections(fbxsdk::FbxProperty&& property);
	};
}