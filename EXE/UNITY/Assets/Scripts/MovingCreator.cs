using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MovingCreator : MonoBehaviour
{
    public float radius;
    public List<Vector3> positionsLeft;
    public List<Vector3> positionsMinLeft;
    public List<Vector3> positionsMaxLeft;
    public List<Vector3> positionsRight;
    public List<Vector3> positionsMinRight;
    public List<Vector3> positionsMaxRight;

    private List<GameObject> objects;

    public void Init()
    {
        objects = new List<GameObject>();
        CreateLeft();
        CreateRight();
    }

    void CreateLeft()
    {
        string tag = "MOVING_LEFT";
        for (int i = 0; i < positionsLeft.Count; i++)
        {
            string name = "moving_left_" + i;

            // -- create object and set parent
            GameObject moving = new GameObject();
            moving.tag = tag;
            moving.transform.parent = this.transform;
            moving.name = name;

            // -- set spot and default color/intensity/angle/lookat
            Light light = moving.AddComponent<Light>();
            light.type = LightType.Spot;
            light.spotAngle = radius;
            light.color = new Color(0.0f, 0.0f, 0.0f, 1.0f);
            light.intensity = 1.5f;
            light.shadows = LightShadows.Soft;
            light.renderMode = LightRenderMode.ForcePixel;
            light.range = 40.0f;

            // -- position
            moving.transform.position = positionsLeft[i];
            moving.transform.localRotation = Quaternion.Euler(90.0f , 0.0f, 0.0f);

            objects.Add(moving);
        }
    }

    void CreateRight()
    {
        string tag = "MOVING_RIGHT";
        for (int i = 0; i < positionsRight.Count; i++)
        {
            string name = "moving_right_" + i;

            // -- create object and set parent
            GameObject moving = new GameObject();
            moving.tag = tag;
            moving.transform.parent = this.transform;
            moving.name = name;

            // -- set spot and default color/intensity/angle/lookat
            Light light = moving.AddComponent<Light>();
            light.type = LightType.Spot;
            light.spotAngle = radius;
            light.color = new Color(0.0f, 0.0f, 0.0f, 1.0f);
            light.intensity = 1.5f;
            light.shadows = LightShadows.Soft;
            light.renderMode = LightRenderMode.ForcePixel;
            light.range = 40.0f;

            // -- position
            moving.transform.position = positionsRight[i];
            moving.transform.localRotation = Quaternion.Euler(90.0f , 0.0f, 0.0f);

            objects.Add(moving);
        }
    }
}
