using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SeatCreator : MonoBehaviour
{

    public Vector3 scaleSeat;
    public List<Vector3> seatsPosition;

    public void Init()
    {
        Material material = Resources.Load<Material>("Materials/Seat");
        Mesh mesh = Resources.Load<Mesh>("Models/seat");

        for (int i = 0; i < seatsPosition.Count; i++)
        {
            // -- determine name of the object
            string name = "Seat_" + i;

            // -- create object and set parent
            GameObject seat = new GameObject(name);
            seat.transform.parent = this.transform;

            // -- set filter and renderer
            MeshFilter mf = seat.AddComponent<MeshFilter>();
            mf.sharedMesh = mesh;
            MeshRenderer mr = seat.AddComponent<MeshRenderer>();
            mr.material = material;

            // -- rescale and position
            seat.transform.localScale = scaleSeat;
            seat.transform.position = seatsPosition[i];
        }
    }
}
