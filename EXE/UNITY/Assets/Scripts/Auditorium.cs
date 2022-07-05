using System.Collections;
using System.Collections.Generic;
using UnityEngine;

using LoaderRoom;

public class Auditorium : MonoBehaviour
{
    
    public GameObject auditorium;

    public float width;
    public float height;
    public float length;
    public Vector3 front;
    public bool hasLogo;

    public List<LoaderRoom.LoadRoomConf.TieringConf> tiers;

    public void Init()
    {
        auditorium.transform.position = new Vector3(0, 0, 0);
        auditorium.transform.localScale = new Vector3(width, height, length);

        float scaleTexX = length / 4.0f;
        float scaleTexY = height / 4.0f;

        Renderer render = auditorium.GetComponent<Renderer>();
        render.material.mainTextureScale = new Vector2(scaleTexX, scaleTexY);

        Material material = Resources.Load<Material>("Materials/Tier");
        Mesh mesh = Resources.Load<Mesh>("Models/Tiering");

        for (int i = 0; i < tiers.Count; i++) {

            GameObject tier = new GameObject("tier_" + i);
            tier.transform.parent = this.transform;

            // -- set filter and renderer
            MeshFilter mf = tier.AddComponent<MeshFilter>();
            mf.sharedMesh = mesh;
            MeshRenderer mr = tier.AddComponent<MeshRenderer>();
            mr.material = material;
            mr.material.mainTextureScale = new Vector2(scaleTexX, tiers[i].scaleZ/4.0f);

            // -- rescale and position
            tier.transform.localScale = new Vector3(1.0f, tiers[i].scaleY/height, tiers[i].scaleZ/length);
            tier.transform.localPosition = new Vector3(0.0f, 0.0f, tiers[i].posZ/length);
        }


        // -- create front of screen
        GameObject frontObject = new GameObject("front");
        // -- set filter and renderer
        Mesh meshFront = Resources.Load<Mesh>("Models/frontScreen");
        MeshFilter mfF = frontObject.AddComponent<MeshFilter>();
        mfF.sharedMesh = meshFront;
        MeshRenderer mrF = frontObject.AddComponent<MeshRenderer>();
        mrF.material = material;
        mrF.material.mainTextureScale = new Vector2(scaleTexX, front.z/4.0f);
        // -- rescale and position
        frontObject.transform.localScale = new Vector3(front.x, front.y, front.z);
        frontObject.transform.localPosition = new Vector3(0.0f, 0.0f, 0.0f);

        // -- create Logo object
        if (hasLogo) {
            GameObject frontLogoObject = new GameObject("frontLogo");
            // -- set filter and renderer
            Material materialFront = Resources.Load<Material>("Materials/Front");
            Mesh meshLogo = Resources.Load<Mesh>("Models/frontScreen");
            MeshFilter mfL = frontLogoObject.AddComponent<MeshFilter>();
            mfL.sharedMesh = meshFront;
            MeshRenderer mrL = frontLogoObject.AddComponent<MeshRenderer>();
            mrL.material = materialFront;
            mrL.material.mainTextureScale = new Vector2(4.0f, 4.0f*front.z/front.x);

            // -- rescale and position
            frontLogoObject.transform.localScale = new Vector3(front.x, front.y, front.z);
            frontLogoObject.transform.localPosition = new Vector3(0.0f, 0.0f, 0.0f);
        }
    }
}
