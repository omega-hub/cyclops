/**************************************************************************************************
 * THE OMEGA LIB PROJECT
 *-------------------------------------------------------------------------------------------------
 * Copyright 2010-2015		Electronic Visualization Laboratory, University of Illinois at Chicago
 * Authors:										
 *  Alessandro Febretti		febret@gmail.com
 *-------------------------------------------------------------------------------------------------
 * Copyright (c) 2010-2015, Electronic Visualization Laboratory, University of Illinois at Chicago
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification, are permitted 
 * provided that the following conditions are met:
 * 
 * Redistributions of source code must retain the above copyright notice, this list of conditions 
 * and the following disclaimer. Redistributions in binary form must reproduce the above copyright 
 * notice, this list of conditions and the following disclaimer in the documentation and/or other 
 * materials provided with the distribution. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR 
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE  GOODS OR SERVICES; LOSS OF 
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *************************************************************************************************/
#include <osgDB/ReadFile>
#include <osgwTools/Shapes.h>

#include "cyclops/SceneLoader.h"
#include "cyclops/EffectNode.h"
#include "cyclops/AnimatedObject.h"
#include "cyclops/StaticObject.h"

using namespace cyclops;

Entity* SceneLoader::sLastLoadedEntity = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////////
Entity* SceneLoader::getLastLoadedEntity()
{
	return sLastLoadedEntity;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
SceneLoader::SceneLoader(TiXmlDocument& doc, const String& path):
	myDoc(doc),
	myPath(path),
	mySceneManager(NULL)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SceneLoader::startLoading(SceneManager* sm)
{
	mySceneManager = sm;

	osg::Group* root = new osg::Group();
	TiXmlElement* xroot = myDoc.RootElement();

	TiXmlElement* xMapContents = NULL;
	// For compatibility reasons, read scene files containing scene data under /MapData/MapContent
	// tags, if they exist.
	TiXmlElement* xlevel = xroot->FirstChildElement("MapData");
	if(xlevel != NULL)
	{
		xMapContents = xlevel->FirstChildElement("MapContents");
	}
	else
	{
		xMapContents = xroot;
	}
	if(xMapContents != NULL)
	{
		TiXmlElement* xEntityFiles = xMapContents->FirstChildElement("Files");
		if(xEntityFiles != NULL)
		{
			loadAssets(xEntityFiles, SceneManager::ModelAssetType);
		}

		// We may look for objects under variousy-named tags, for compatibility reasons.
		const char* sectionNames[] = {
			"Objects",
			"StaticObjects",
			"Primitives",
			"Entities"
		};

		foreach(const char* sectionName, sectionNames)
		{
			TiXmlElement* xObjects = xMapContents->FirstChildElement(sectionName);
			if(xObjects != NULL)
			{
				createObjects(root, xObjects);
			}
		}

		// If a skybox tag exist, create a skybox for the scene.
		TiXmlElement* xSkyBox = xMapContents->FirstChildElement("Skybox");
		if(xSkyBox != NULL)
		{
			const char* dir = xSkyBox->Attribute("Path");
			const char* ext = xSkyBox->Attribute("Extension");
			if(dir == NULL || ext == NULL)
			{
				owarn("SceneLoader: scene Skybox tag missing Path or Extension attribute. No skybox will be created");
			}
			else
			{
				Skybox* skybox = new Skybox();
				if(skybox->loadCubeMap(dir, ext))
				{
					mySceneManager->setSkyBox(skybox);
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SceneLoader::isLoadingComplete()
{
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int SceneLoader::getLoadProgress()
{
	return 100;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SceneLoader::loadStep()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Vector4f SceneLoader::readVector4f(TiXmlElement* elem, const String& attributeName)
{
	const char* cstr = elem->Attribute(attributeName.c_str());
	if(cstr != NULL)
	{
		String str(cstr);
		std::vector<String> args = StringUtils::split(str, " ");
		return Vector4f(atof(args[0].c_str()), atof(args[1].c_str()), atof(args[2].c_str()), atof(args[3].c_str()));
	}
	return Vector4f::Zero();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Vector3f SceneLoader::readVector3f(TiXmlElement* elem, const String& attributeName)
{
	const char* cstr = elem->Attribute(attributeName.c_str());
	if(cstr != NULL)
	{
		String str(cstr);
		std::vector<String> args = StringUtils::split(str, " ");
		return Vector3f(atof(args[0].c_str()), atof(args[1].c_str()), atof(args[2].c_str()));
	}
	return Vector3f::Zero();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Vector2f SceneLoader::readVector2f(TiXmlElement* elem, const String& attributeName)
{
	const char* cstr = elem->Attribute(attributeName.c_str());
	if(cstr != NULL)
	{
		String str(cstr);
		std::vector<String> args = StringUtils::split(str, " ");
		return Vector2f(atof(args[0].c_str()), atof(args[1].c_str()));
	}
	return Vector2f::Zero();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
float SceneLoader::readFloat(TiXmlElement* elem, const String& attributeName, float defaultValue)
{
	const char* cstr = elem->Attribute(attributeName.c_str());
	if(cstr != NULL)
	{
		return atof(cstr);
	}
	return defaultValue;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int SceneLoader::readInt(TiXmlElement* elem, const String& attributeName, int defaultValue)
{
	const char* cstr = elem->Attribute(attributeName.c_str());
	if(cstr != NULL)
	{
		return atoi(cstr);
	}
	return defaultValue;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SceneLoader::readBool(TiXmlElement* elem, const String& attributeName, bool defaultValue)
{
	const char* cstr = elem->Attribute(attributeName.c_str());
	if(cstr != NULL)
	{
		String val = cstr;
		StringUtils::toLowerCase(val);
		if(val == "true") return true;
		return false;
	}
	return defaultValue;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SceneLoader::loadAssets(TiXmlElement* xStaticObjectFiles, SceneManager::AssetType type)
{
	TiXmlElement* xchild = xStaticObjectFiles->FirstChildElement();
	while(xchild)
	{
		String filePath = xchild->Attribute("Path");
		bool generateNormals = readBool(xchild, "GenerateNormals");
		bool normalizeNormals = readBool(xchild, "NormalizeNormals");
		int objcount=1;
		if(filePath.find("*")!=-1){		
			if(xchild->Attribute("Maxcount") != NULL){
				objcount = atoi(xchild->Attribute("Maxcount"));
			} 
			else 
			{
			  ofwarn("object count not available for:  %1%", %filePath);
			}	
		}

		// In the path, substitute ./ occurrences with the path of the xml scene file, so assets
		// in the local directory can be correctly referenced.
		// Split the path and get the file name.
		String path;
		String filename;
		String extension;
		StringUtils::splitFullFilename(myPath, filename, extension, path);
		filePath = StringUtils::replaceAll(filePath, "./", path);

		const char* id = xchild->Attribute("Id");

		ModelInfo* mi = new ModelInfo();
		mi->name = id;
		mi->path = filePath;
		mi->numFiles = objcount;
		mi->generateNormals = generateNormals;
		mi->normalizeNormals = normalizeNormals;
		
		const char* attrSize = xchild->Attribute("Size");
		if(attrSize != NULL) mi->size = atof(attrSize);

		mySceneManager->loadModel(mi);
		
		xchild = xchild->NextSiblingElement();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SceneLoader::createObjects(osg::Group* root, TiXmlElement* xObjects)
{
	TiXmlElement* xchild = xObjects->FirstChildElement();
	while(xchild)
	{
		String objtype = xchild->Value();
		StringUtils::toLowerCase(objtype);

		// Instantiate a drawable object depending on tag name
		Entity* obj = NULL;
		if(objtype == "plane")	obj = createPlane(xchild);
		else if(objtype == "sphere") obj = createSphere(xchild);
		else if(objtype == "staticobject") obj = createStaticObject(xchild);
		else if(objtype == "entity" || objtype == "animatedobject" ) obj = createEntity(xchild);

		if(obj != NULL)
		{
			sLastLoadedEntity = obj;

			Vector3f rotation = readVector3f(xchild, "Rotation");
			Vector3f position = readVector3f(xchild, "WorldPos");
			Vector3f scale = readVector3f(xchild, "Scale");
			if(scale.x() == 0 && scale.y() == 0 && scale.z() == 0) scale = Vector3f::Ones();

			// Apply transformation to object based on what has been read from its xml element.
			obj->setPosition(position);
			obj->pitch(rotation[0] * Math::DegToRad);
			obj->yaw(rotation[1] * Math::DegToRad);
			obj->roll(rotation[2] * Math::DegToRad);
			obj->scale(scale);

			// Add name if there is one in the xml
			const char* name = xchild->Attribute("name");
			if(name != NULL)
			{
				obj->setName(name);
			}

			// Add tag if there is one in the xml
			const char* tag = xchild->Attribute("Tag");
			if(tag != NULL)
			{
				obj->setTag(tag);
			}

			// If the xml element specifies an effect, apply it.
			const char* attrFx = xchild->Attribute("Effect");
			if(attrFx != NULL)
			{
				obj->setEffect(attrFx);
			}

			// If the xml element has an id, create a script variable with the specified name linked to this object
			const char* attrId = xchild->Attribute("Id");
			if(attrId != NULL)
			{
				String className = "";
				if(objtype == "plane")	className = "PlaneShape";
				else if(objtype == "sphere") className = "SphereShape";
				else if(objtype == "staticobject") className = "StaticObject";
				else if(objtype == "entity" || objtype == "animatedobject" ) className = "AnimatedObject";


				// Exacute a script command to create a variable and assign it with the newly created object.
				String scriptCmd = ostr("%1% = SceneLoader.getLastLoadedEntity(); %1%.__class__ = %2%", %attrId %className);
				PythonInterpreter* interp = SystemManager::instance()->getScriptInterpreter();
				interp->eval(scriptCmd);
			}
		}
		
		xchild = xchild->NextSiblingElement();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PlaneShape* SceneLoader::createPlane(TiXmlElement* xchild)
{
	const char* attrHeight = xchild->Attribute("Height");
	const char* attrWidth = xchild->Attribute("Width");

	if(attrHeight == NULL || attrWidth == NULL)
	{
		owarn("SceneLoader::createPlane: xml tag is missing Width or Height attributes");
		return NULL;
	}

	float height = atof(attrHeight);
	float width = atof(attrWidth);

	Vector2f tiling = readVector2f(xchild, "Tiling");

	return new PlaneShape(mySceneManager, width, height, tiling);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
SphereShape* SceneLoader::createSphere(TiXmlElement* xchild)
{
	float radius = readFloat(xchild, "Radius", 1.0);
	int subdivisions = readInt(xchild, "Subdivisions", 4);
	Vector2f tiling = readVector2f(xchild, "Tiling");

	return new SphereShape(mySceneManager, radius, subdivisions, tiling);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
AnimatedObject* SceneLoader::createEntity(TiXmlElement* xchild)
{
	// Create a new entity
	const char* fileId = xchild->Attribute("FileId");
	if(fileId == NULL) fileId = xchild->Attribute("FileIndex"); // Legacy name

	const char* id = xchild->Attribute("Id");

	static NameGenerator ng("AnimatedObject");
	if(id == NULL) id = ng.generate().c_str();

	if(fileId != NULL)
	{
		AnimatedObject* e = NULL;
		if(id == NULL) e = new AnimatedObject(mySceneManager, fileId);
		else e = new AnimatedObject(mySceneManager, fileId);

		e->setName(id);

		const char* attrOnAnimationEnded = xchild->Attribute("OnAnimationEnded");
		if(attrOnAnimationEnded != NULL) e->setOnAnimationEndedScript(attrOnAnimationEnded);

		return e;
	}
	else
	{
		owarn("SceneLoader: AnimatedObject xml tag is missing FileIndex attribute");
	}
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
StaticObject* SceneLoader::createStaticObject(TiXmlElement* xchild)
{
	// Create a new static object
	const char* fileId = xchild->Attribute("FileId");
	if(fileId == NULL) fileId = xchild->Attribute("FileIndex");
	return new StaticObject(mySceneManager, fileId);
}

