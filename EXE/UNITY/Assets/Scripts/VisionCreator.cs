using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class VisionCreator : MonoBehaviour
{
    private Mesh centralVision;
    private Mesh movementVision;
    public Camera camera;
    private float fovHorizontal;

    private List<GameObject> cones;
    private GameObject textObject;
    private Text textCone;
    private int offsetCone;

    void Start()
    {
        offsetCone = -1;
        cones = new List<GameObject>();

        textObject = GameObject.Find("FOV");
        textObject.SetActive(true);
        textCone = textObject.GetComponent<Text>();

        fovHorizontal = Camera.VerticalToHorizontalFieldOfView(camera.fieldOfView, camera.aspect);
        CreateCone("Texte", 3.0f, 5.0f);
        CreateCone("Objets", 20.0f, 2.0f);
        CreateCone("Contour", 40.0f, 1.0f);
        CreateCone("Central", 60.0f, 1.0f);
        CreateCone("Movement", 120.0f, 0.5f);
    }

    void CreateCone(string name, float angle, float distance)
    {
        int divAngle = 24;
        float angleHorizontal = angle * (fovHorizontal / 180.0f);
        float angleVertival = angle * (120.0f / 180.0f) * (camera.fieldOfView / 120.0f);

        // -- create mesh
        centralVision = new Mesh();
        centralVision.name = name + "_mesh";

        // -- create cone object
        GameObject central = new GameObject(name);
        central.transform.parent = this.transform;
        MeshFilter mf = central.AddComponent<MeshFilter>();
        mf.mesh = centralVision;
        central.transform.localPosition = new Vector3(0.0f, 0.0f, -distance);
        central.transform.localScale = new Vector3(1.0f, 1.0f, 1.0f);
        Material material = Resources.Load<Material>("Materials/Vision");
        MeshRenderer mr = central.AddComponent<MeshRenderer>();
        mr.material = material;

        cones.Add(central);
        central.SetActive(false);

        // -- create vertices
        Vector3[] vertices = new Vector3[divAngle*2];
        int offsetVertice = 0;
        float stepAngle = 360.0f / (float)divAngle;
        float angleVertical = Camera.HorizontalToVerticalFieldOfView(angleHorizontal, camera.aspect);
        float ratioHorizontal = angleHorizontal/fovHorizontal;
        float ratioVertical = angleVertical/camera.fieldOfView;

        for (int i = 0; i < divAngle; i++) {
            float angleRad = stepAngle*i*Mathf.PI/180.0f;
            float ratioX = 0.5f*Mathf.Cos(angleRad)*ratioHorizontal;
            float ratioY = 0.5f*Mathf.Sin(angleRad)*ratioVertical;
            Vector3 pointNear = new Vector3(0.5f+ratioX, 0.5f+ratioY, distance);
            Vector3 proj = camera.ViewportToWorldPoint(pointNear) - camera.transform.position;            

            vertices[offsetVertice] = proj;
            offsetVertice++;
        }
        for (int i = 0; i < divAngle; i++) {
            float angleRad = stepAngle*i*Mathf.PI/180.0f;
            float ratioX = 0.5f*Mathf.Cos(angleRad)*ratioHorizontal;
            float ratioY = 0.5f*Mathf.Sin(angleRad)*ratioVertical;
            Vector3 pointNear = new Vector3(0.5f+ratioX, 0.5f+ratioY, 100.0f);
            Vector3 proj = camera.ViewportToWorldPoint(pointNear) - camera.transform.position;

            vertices[offsetVertice] = proj;
            offsetVertice++;
        }
        centralVision.vertices = vertices;

        // -- create triangles
        int offsetTriangles = 0;
        int[] triangles = new int[divAngle*4*3];
        for (int i = 0; i < divAngle; i++) {

            triangles[offsetTriangles] = i;
            offsetTriangles++;
            triangles[offsetTriangles] = i == (divAngle-1) ? 0 : i+1;
            offsetTriangles++;
            triangles[offsetTriangles] = i+divAngle;
            offsetTriangles++;
            triangles[offsetTriangles] = i == (divAngle-1) ? 0 : i+1;
            offsetTriangles++;
            triangles[offsetTriangles] = i+divAngle;
            offsetTriangles++;
            triangles[offsetTriangles] = i == (divAngle-1) ? divAngle : i+1+divAngle;
            offsetTriangles++;

            triangles[offsetTriangles] = i+divAngle;
            offsetTriangles++;
            triangles[offsetTriangles] = i == (divAngle-1) ? 0 : i+1;
            offsetTriangles++;
            triangles[offsetTriangles] = i;
            offsetTriangles++;
            triangles[offsetTriangles] = i == (divAngle-1) ? divAngle : i+1+divAngle;
            offsetTriangles++;
            triangles[offsetTriangles] = i+divAngle;
            offsetTriangles++;
            triangles[offsetTriangles] = i == (divAngle-1) ? 0 : i+1;
            offsetTriangles++;
        }
        centralVision.triangles = triangles;
        centralVision.RecalculateNormals();
    }

    void Update()
    {
        if (Input.GetKeyDown(KeyCode.V)) {
            if (offsetCone != -1) {
                cones[offsetCone].SetActive(false);
            }
            offsetCone++;
            if (offsetCone == cones.Count) {
                offsetCone = -1;
            }
            if (offsetCone != -1) {
                cones[offsetCone].SetActive(true);
                textCone.text = cones[offsetCone].name;
            }
            else {
                textCone.text = "";
            }
        }
    }
}
