#ifndef ENTITY_H
#define ENTITY_H

#include <string>

#include "Light.h"
#include "Model.h"
#include "Consts.h"

using namespace std;

enum EntityType {
    Empty,
    ModelEntity,
    LightEntity
};

class Entity {

public:
    // Constructor for model entity (your existing one)
    Entity(Model model, Material material, string name) {
        this->model = model;
        this->material = material;
        this->name = name;
        this->hasModel = true;
        this->hasMaterial = true;
        this->hasLight = false;

        this->entityType = EntityType::ModelEntity;
    }

    // Constructor for empty entity (your existing one)
    Entity(string name) {
        this->name = name;
        this->hasModel = false;
        this->hasMaterial = false;
        this->hasLight = false;
        this->entityType = EntityType::Empty;
    }

    // Constructor for light entity
    Entity(Light lightSource, string name) {
        this->light = lightSource;
        this->name = name;
        this->hasModel = false;
        this->hasMaterial = false;
        this->hasLight = true;
        this->entityType = EntityType::LightEntity;

        // Light position comes from transform
        this->transform.SetPosition(lightSource.GetPosition());
    }

    // Constructor for light entity WITH visual model
    Entity(Light lightSource, Model visualModel, string name) {
        this->light = lightSource;
        this->model = visualModel;
        this->name = name;
        this->hasModel = true;
        this->hasMaterial = false;
        this->hasLight = true;
        this->entityType = EntityType::LightEntity;

        this->transform.SetPosition(lightSource.GetPosition());
    }

    int GetId() const {
        return id;
    }

    void SetId(int id) {
        this->id = id;
    }

    string GetName() const {
        return name;
    }

    void SetName(string name) {
        this->name = name;
    }

    bool HasModel() const {
        return hasModel;
    }

    bool HasMaterial() const {
        return hasMaterial;
    }

    bool HasLight() const {
        return hasLight;
    }

    bool IsLight() const {
        return entityType == EntityType::LightEntity;
    }

    EntityType GetEntityType() const {
        return entityType;
    }

    void UpdateLightProperties() {
        if (hasLight) {
            light.SetPosition(transform.GetPosition());

            if (light.GetLightType() == LightType::Directional) {
                glm::vec3 rot = transform.GetRotation();
                glm::vec3 dir{};
                dir.x = cos(glm::radians(rot.y)) * cos(glm::radians(rot.x));
                dir.y = sin(glm::radians(rot.x));
                dir.z = sin(glm::radians(rot.y)) * cos(glm::radians(rot.x));
                dir = glm::normalize(dir);

                light.SetDirection(dir);
            }
        }
    }

    static Entity CreatePointLightEntity(const string entityName = "Point Light", const string path = Consts::POINT_LIGHT_VISUAL_PATH) {
        Light light(LightType::Point);
        Model lightModel;
        bool isLoaded = lightModel.AttachModel(path);
        if (!isLoaded) {
            std::cout << "Error loading model for lights \n";
        }

        Entity lightEntity(light, lightModel, entityName);

        return lightEntity;
    }

    static Entity CreateDirectionalLight(const string entityName = "Directional Light", const string path = Consts::POINT_LIGHT_VISUAL_PATH) {
        Light light(LightType::Directional);
        Model lightModel;
        bool isLoaded = lightModel.AttachModel(path);
        if (!isLoaded) {
            std::cout << "Error loading model for lights \n";
        }

        Entity lightEntity(light, lightModel, entityName);

        return lightEntity;
    }

    Transform transform;
    Model model;
    Material material;
    Light light;

private:
    int id { -1 }; // default value
    string name;
    bool hasModel;
    bool hasMaterial;
    bool hasLight;
    EntityType entityType;
};

#endif