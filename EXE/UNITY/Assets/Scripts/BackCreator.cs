using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BackCreator : MonoBehaviour
{
    public List<Vector3> positionsLeft;
    public List<Vector3> positionsRight;
    public float rangeLeft;
    public float rangeRight;
    public float angleBackLeft;
    public float angleBackRight;

    private List<GameObject> objects;

    public void Init()
    {
        objects = new List<GameObject>();

        CreateLeft();
        CreateRight();
        //CreateModels();
    }

    void CreateLeft()
    {
        string tag = "BACK_LEFT";
        for (int i = 0; i < positionsLeft.Count; i++)
        {
            string name = "backlight_left_" + i;

            // -- create object and set parent
            GameObject back = new GameObject();
            back.tag = tag;
            back.transform.parent = this.transform;
            back.name = name;

            // -- set spot and default color/intensity/angle/lookat
            Light light = back.AddComponent<Light>();
            light.type = LightType.Point;
            light.color = new Color(0.0f, 0.0f, 0.0f, 1.0f);
            light.intensity = 1.5f;
            light.shadows = LightShadows.Soft;
            light.renderMode = LightRenderMode.ForcePixel;
            light.range = rangeLeft;
            light.shadowNearPlane = 1.6f;
            light.shadowBias = 1.0f;

            // -- position
            back.transform.position = positionsLeft[i];

            objects.Add(back);
        }
    }

    void CreateRight()
    {
        string tag = "BACK_RIGHT";
        for (int i = 0; i < positionsRight.Count; i++)
        {
            string name = "backlight_right_" + i;

            // -- create object and set parent
            GameObject back = new GameObject();
            back.tag = tag;
            back.transform.parent = this.transform;
            back.name = name;

            // -- set spot and default color/intensity/angle/lookat
            Light light = back.AddComponent<Light>();
            light.type = LightType.Point;
            light.color = new Color(0.0f, 0.0f, 0.0f, 1.0f);
            light.intensity = 1.5f;
            light.shadows = LightShadows.Soft;
            light.renderMode = LightRenderMode.ForcePixel;
            light.range = rangeRight;
            light.shadowNearPlane = 1.6f;
            light.shadowBias = 0.0f;

            // -- position
            back.transform.position = positionsRight[i];

            objects.Add(back);
        }
    }

    void CreateModels() {

        Material material = Resources.Load<Material>("Materials/Backlight");
        Mesh meshBackIn = Resources.Load<Mesh>("Models/back-in");
        Mesh meshBackOut = Resources.Load<Mesh>("Models/back-out");

        Vector3 scaleLeft = new Vector3(1.0f, 0.4f, 0.6f);
        scaleLeft.x = (positionsLeft[0].x - positionsLeft[positionsLeft.Count-1].x) + 0.3f;
        Vector3 posModelLeft = new Vector3(0.0f, 0.0f, 0.0f);
        posModelLeft.x = positionsLeft[0].x + 0.15f;
        posModelLeft.y = positionsLeft[0].y - 0.2f;
        posModelLeft.z = positionsLeft[0].z + 0.0f;

        // back in left
        GameObject backInLeft = new GameObject("backligh-in-left");
        backInLeft.transform.parent = this.transform;
        // -- set filter and renderer
        MeshFilter mfIL = backInLeft.AddComponent<MeshFilter>();
        mfIL.sharedMesh = meshBackIn;
        MeshRenderer mrIL = backInLeft.AddComponent<MeshRenderer>();
        mrIL.material = material;
        // -- rescale and position
        backInLeft.transform.localScale = scaleLeft;
        backInLeft.transform.position = posModelLeft;
        backInLeft.transform.localRotation = Quaternion.Euler(angleBackLeft, 0.0f, 0.0f);

        // back out left
        GameObject backOutLeft = new GameObject("backligh-out-left");
        backOutLeft.transform.parent = this.transform;
        // -- set filter and renderer
        MeshFilter mfOL = backOutLeft.AddComponent<MeshFilter>();
        mfOL.sharedMesh = meshBackOut;
        MeshRenderer mrOL = backOutLeft.AddComponent<MeshRenderer>();
        mrOL.material = material;
        // -- rescale and position
        backOutLeft.transform.localScale = scaleLeft;
        backOutLeft.transform.position = posModelLeft;
        backOutLeft.transform.localRotation = Quaternion.Euler(angleBackLeft, 0.0f, 0.0f);

        Vector3 scaleRight = new Vector3(1.0f, 0.4f, 0.6f);
        scaleRight.x = (positionsRight[0].x - positionsRight[positionsRight.Count-1].x) + 0.3f;
        Vector3 posModelRight = new Vector3(0.0f, 0.0f, 0.0f);
        posModelRight.x = positionsRight[0].x + 0.15f;
        posModelRight.y = positionsRight[0].y - 0.2f;
        posModelRight.z = positionsRight[0].z + 0.0f;

        // back in right
        GameObject backInRight = new GameObject("backligh-in-right");
        backInRight.transform.parent = this.transform;
        // -- set filter and renderer
        MeshFilter mfIR = backInRight.AddComponent<MeshFilter>();
        mfIR.sharedMesh = meshBackIn;
        MeshRenderer mrIR = backInRight.AddComponent<MeshRenderer>();
        mrIR.material = material;
        // -- rescale and position
        backInRight.transform.localScale = scaleRight;
        backInRight.transform.position = posModelRight;
        backInRight.transform.localRotation = Quaternion.Euler(angleBackRight, 0.0f, 0.0f);

        // back out left
        GameObject backOutRight = new GameObject("backligh-out-right");
        backOutRight.transform.parent = this.transform;
        // -- set filter and renderer
        MeshFilter mfOR = backOutRight.AddComponent<MeshFilter>();
        mfOR.sharedMesh = meshBackOut;
        MeshRenderer mrOR = backOutRight.AddComponent<MeshRenderer>();
        mrOR.material = material;
        // -- rescale and position
        backOutRight.transform.localScale = scaleRight;
        backOutRight.transform.position = posModelRight;
        backOutRight.transform.localRotation = Quaternion.Euler(angleBackRight, 0.0f, 0.0f);
    }
}
