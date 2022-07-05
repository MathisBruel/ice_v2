using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Rendering;

public class PanelsCreator : MonoBehaviour
{
    public float widthAudi;
    public float heightAudi;
    public bool isLeft;
    public List<Vector3> positions;
    public List<Vector3> scales;
    public Vector3 centerScreen;
    public float intensityProbe;

    private int nbPanels;
    private List<GameObject> objects;
    private ReflectionProbe probe;

    public void Init()
    {
        intensityProbe = 1.0f;

        if (positions.Count != scales.Count)
        {
            return;
        }

        nbPanels = positions.Count;
        objects = new List<GameObject>();

        // -- find meshes and material
        Material material = Resources.Load<Material>("Materials/Panel");
        Mesh mesh;
        string tag;

        if (isLeft)
        {
            mesh = Resources.Load<Mesh>("Models/panelLeft");
        }
        else
        {
            mesh = Resources.Load<Mesh>("Models/panelRight");
        }

        Material materialSide = Resources.Load<Material>("Materials/PanelSide");
        Mesh meshSide = Resources.Load<Mesh>("Models/panel-Side");

        for (int i = 0; i < nbPanels; i++)
        {
            // -- determine name of the object
            string name = "Panel_";
            if (isLeft)
            {
                name += "left_";
                tag = "leftPanel";
            }
            else
            {
                name += "right_";
                tag = "rightPanel";
            }
            name += i;

            // -- create object and set parent
            GameObject panel = new GameObject(name);
            panel.transform.parent = this.transform;
            panel.tag = tag;

            // -- set filter and renderer
            MeshFilter mf = panel.AddComponent<MeshFilter>();
            mf.sharedMesh = mesh;
            MeshRenderer mr = panel.AddComponent<MeshRenderer>();
            mr.material = material;

            // -- rescale and position
            panel.transform.localScale = scales[i];
            panel.transform.position = positions[i];

            objects.Add(panel);

            // -- create object and set parent
            GameObject panelSide = new GameObject(name + "_side");
            panelSide.transform.parent = this.transform;

            // -- set filter and renderer
            MeshFilter mfSide = panelSide.AddComponent<MeshFilter>();
            mfSide.sharedMesh = meshSide;
            MeshRenderer mrSide = panelSide.AddComponent<MeshRenderer>();
            mrSide.material = materialSide;

            // -- rescale and position
            Vector3 positionSide = positions[i];
            positionSide.x = isLeft ? 0.01f : positionSide.x-0.01f;
            Vector3 scaleSide = scales[i];
            scaleSide.x = 0.01f;
            panelSide.transform.localScale = scaleSide;
            panelSide.transform.position = positionSide;
        }

        if (positions.Count > 0) {
            createProbe();
        }
    }

    void createProbe()
    {
        GameObject panelProbe = new GameObject(isLeft ? "left_probe" : "right_probe");
        panelProbe.transform.parent = this.transform;
        
        float minY = 100.0f, maxY = 0.0f;
        float minZ = 100.0f, maxZ = 0.0f;

        for (int i = 0; i < nbPanels; i++)
        {
            if (positions[i].y < minY) {
                minY = positions[i].y;
            }
            if (positions[i].z < minZ) {
                minZ = positions[i].z;
            }

            if (positions[i].y + scales[i].z > maxY) {
                maxY = positions[i].y + scales[i].z;
            }
            if (positions[i].z + scales[i].z > maxZ) {
                maxZ = positions[i].z + scales[i].z;
            }
        }

        float xProbe = isLeft ? positions[0].x-1.5f : positions[0].x+1.5f;
        panelProbe.transform.position = new Vector3(xProbe, (maxY + minY)/2.0f, (maxZ + minZ)/2.0f);
        probe = panelProbe.AddComponent<ReflectionProbe>();
        probe.mode = ReflectionProbeMode.Realtime;
        probe.refreshMode = ReflectionProbeRefreshMode.ViaScripting;
        probe.nearClipPlane = 0.1f;
        probe.farClipPlane = Vector3.Distance(panelProbe.transform.position, centerScreen);
        probe.clearFlags = UnityEngine.Rendering.ReflectionProbeClearFlags.SolidColor;
        probe.backgroundColor = new Color(0.0f, 0.0f, 0.0f, 1.0f);
        probe.boxProjection = true;
        probe.size = new Vector3(widthAudi, heightAudi*2.0f, (maxZ - minZ)*2.0f);
        probe.intensity = 0.6f;
        probe.resolution = 256;
        probe.timeSlicingMode = ReflectionProbeTimeSlicingMode.NoTimeSlicing;
    }

    public void modifyProbeIntensity(float delta)
    {
        if (probe != null)
        {
            probe.intensity += delta;
        }
    }
}
