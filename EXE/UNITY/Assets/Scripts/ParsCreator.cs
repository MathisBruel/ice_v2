using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

public class ParsCreator : MonoBehaviour
{
    public float radius;
    public List<Vector3> positionsLeft;
    public List<Vector3> positionsRight;

    public Vector3 seatMin;
    public Vector3 seatMax;

    private List<GameObject> objects;

    public void Init()
    {
        objects = new List<GameObject>();

        CreateLeft();
        CreateRight();
    }

    void CreateLeft()
    {
        float scaleX = (seatMax.x - seatMin.x)/3.0f;
        float xLeft = -seatMin.x - scaleX;
        float scaleZ = (seatMax.z - seatMin.z)/5.0f;
        float scaleY = (seatMax.y - seatMin.y)/5.0f;

        Debug.Log("Min : " + seatMin.ToString());
        Debug.Log("Max : " + seatMax.ToString());

        string tag = "PARS_LEFT";
        for (int i = 0; i < positionsLeft.Count; i++)
        {
            string name = "par_left_" + i;

            // -- create object and set parent
            GameObject par = new GameObject();
            par.tag = tag;
            par.transform.parent = this.transform;
            par.name = name;

            // -- set spot and default color/intensity/angle/lookat
            Light light = par.AddComponent<Light>();
            light.type = LightType.Spot;
            light.spotAngle = radius;
            light.color = new Color(0.0f, 0.0f, 0.0f, 1.0f);
            light.intensity = 3.0f;
            light.shadows = LightShadows.Soft;
            light.renderMode = LightRenderMode.ForcePixel;
            light.range = 40.0f;
            
            Vector3 pointer = new Vector3(xLeft, seatMin.y + (i+1)*scaleY, seatMin.z + (i+1)*scaleZ);
            Debug.Log(name + " : " + pointer);
            Vector3 vectorDirection = pointer - positionsLeft[i];

            // -- position
            par.transform.position = positionsLeft[i];
            par.transform.localRotation = Quaternion.FromToRotation(new Vector3(0.0f, 0.0f, 1.0f), vectorDirection);

            objects.Add(par);
        }
    }

    void CreateRight()
    {
        float scaleX = (seatMax.x - seatMin.x)/3.0f;
        float xRight = -seatMax.x + scaleX;
        float scaleZ = (seatMax.z - seatMin.z)/5.0f;
        float scaleY = (seatMax.y - seatMin.y)/5.0f;

        string tag = "PARS_RIGHT";
        for (int i = 0; i < positionsLeft.Count; i++)
        {
            string name = "par_right_" + i;

            // -- create object and set parent
            GameObject par = new GameObject();
            par.tag = tag;
            par.transform.parent = this.transform;
            par.name = name;

            // -- set spot and default color/intensity/angle/lookat
            Light light = par.AddComponent<Light>();
            light.type = LightType.Spot;
            light.spotAngle = radius;
            light.color = new Color(0.0f, 0.0f, 0.0f, 1.0f);
            light.intensity = 3.0f;
            light.shadows = LightShadows.Soft;
            light.renderMode = LightRenderMode.ForcePixel;
            light.range = 40.0f;

            Vector3 pointer = new Vector3(xRight, seatMin.y + (i+1)*scaleY, seatMin.z + (i+1)*scaleZ);
            Debug.Log(name + " : " + pointer);
            Vector3 vectorDirection = pointer - positionsRight[i];

            // -- position
            par.transform.position = positionsRight[i];
            par.transform.localRotation = Quaternion.FromToRotation(new Vector3(0.0f, 0.0f, 1.0f), vectorDirection);

            objects.Add(par);
        }
    }
}
