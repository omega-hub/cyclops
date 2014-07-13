#include <omega.h>
#include <cyclops/cyclops.h>

using namespace omega;
using namespace cyclops;

///////////////////////////////////////////////////////////////////////////////
class EarthApplication: public EngineModule
{
public:
    EarthApplication(): EngineModule("EarthApplication") {}

    virtual void initialize()
    {
        ModelInfo* earth_model = new ModelInfo(
            "earth", 
            "cyclops/examples/helloEarth/mapquest_osm.earth",
            5.0f, 1, false, false, false, "", "mapquest_osm.earth");
        SceneManager::instance()->loadModel(earth_model);

        // Create a new object using the loaded model (referenced using its name, 'simpleModel')
        StaticObject* earth_object = new StaticObject(SceneManager::instance(), "earth");
        earth_object->setName("earth_object");
        earth_object->pitch(-90 * Math::DegToRad);
        earth_object->setPosition(0, 0, -2);

        Light* light = new Light(SceneManager::instance());
        light->setEnabled(true);
        light->setPosition(Vector3f(0, 50, 50));
        light->setColor(Color(1.0f, 1.0f, 1.0f));
        light->setAmbient(Color(0.1f, 0.1f, 0.1f));
    }
};

///////////////////////////////////////////////////////////////////////////////
// ApplicationBase entry point
int main(int argc, char** argv)
{
    Application<EarthApplication> app("earth");
    return omain(app, argc, argv);
}
