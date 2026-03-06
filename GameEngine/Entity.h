#ifndef ENTITY_H
#define ENTITY_H

#include <string>

#include "Light.h"
#include "Model.h"
#include "PathConsts.h"

enum class EntityType {
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

        this->transform.SetScale(Light::LIGHT_MODEL_DEFAULT_SIZE);
        this->transform.SetInitialScale(Light::LIGHT_MODEL_DEFAULT_SIZE);

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

        this->transform.SetScale(Light::LIGHT_MODEL_DEFAULT_SIZE);
        this->transform.SetInitialScale(Light::LIGHT_MODEL_DEFAULT_SIZE);


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
                glm::mat4 rot = glm::mat4(1.0f);
                glm::vec3 r = transform.GetRotation();
                rot = glm::rotate(rot, glm::radians(r.x), glm::vec3(1, 0, 0));
                rot = glm::rotate(rot, glm::radians(r.y), glm::vec3(0, 1, 0));
                rot = glm::rotate(rot, glm::radians(r.z), glm::vec3(0, 0, 1));

                glm::vec3 dir = glm::normalize(glm::vec3(rot * glm::vec4(0, -1, 0, 0)));
                light.SetDirection(dir);
            }
            else if (light.GetLightType() == LightType::Spot) {
                // Because spot light`s visual is cone so we make sure it look like that light comes from cone`s tip

                glm::mat4 rot = glm::mat4(1.0f);
                glm::vec3 r = transform.GetRotation();
                rot = glm::rotate(rot, glm::radians(r.x), glm::vec3(1, 0, 0));
                rot = glm::rotate(rot, glm::radians(r.y), glm::vec3(0, 1, 0));
                rot = glm::rotate(rot, glm::radians(r.z), glm::vec3(0, 0, 1));

                glm::vec3 dir = glm::normalize(glm::vec3(rot * glm::vec4(0, 1, 0, 0)));
                light.SetDirection(dir);

                glm::vec3 tipOffset = glm::vec3(rot * glm::vec4(0, transform.GetScale().y, 0, 0));
                light.SetPosition(transform.GetPosition() + tipOffset);
            }
        }
    }

    static Entity CreatePointLightEntity(const string& entityName = "Point Light", const string path = PathConsts::POINT_LIGHT_VISUAL_PATH) {
        Light light(LightType::Point);
        Model lightModel;

        bool isLoaded = lightModel.AttachModel(path);
        if (!isLoaded) {
            std::cout << "Error loading model for Point Light \n";
        }

        Entity lightEntity(light, lightModel, entityName);

        return lightEntity;
    }

    static Entity CreateDirectionalLight(const string& entityName = "Directional Light", const string path = PathConsts::DIRECTIONAL_LIGHT_VISUAL_PATH) {
        Light light(LightType::Directional);
        Model lightModel;
        bool isLoaded = lightModel.AttachModel(path);
        if (!isLoaded) {
            std::cout << "Error loading model for Directional Light \n";
        }

        Entity lightEntity(light, lightModel, entityName);

        return lightEntity;
    }

    static Entity CreateSpotLightEntity(const string& entityName = "Spot Light", const string path = PathConsts::SPOT_LIGHT_VISUAL_PATH) {
        Light light(LightType::Spot);
        Model lightModel;
        bool isLoaded = lightModel.AttachModel(path);
        if (!isLoaded) {
            std::cout << "Error loading model for Spot Light \n";
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