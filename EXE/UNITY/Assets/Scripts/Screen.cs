using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Screen : MonoBehaviour
{
    public GameObject screen;

    public float width;
    public float height;

    public float offsetX;
    public float offsetY;
    public float offsetZ;

    public void Init()
    {
        screen.transform.localScale = new Vector3(width, height, 1.0f);
        screen.transform.position = new Vector3(offsetX, offsetY, offsetZ);
    }
}
