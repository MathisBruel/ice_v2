using UnityEngine;
using UnityEngine.InputSystem;

public class FlyCamera : MonoBehaviour
{
    public InputActionAsset playerControls;
    private InputAction moveInput;
    private InputAction upInput;
    private InputAction downInput;

    public bool canMove = true;
    public float acceleration = 50; // how fast you accelerate
    public float accSprintMultiplier = 4; // how much faster you go when "sprinting"
    public float lookSensitivity = 1; // mouse look sensitivity
    public float dampingCoefficient = 5; // how quickly you break to a halt after you stop your input
    public bool focusOnEnable = true; // whether or not to focus and lock cursor immediately on enable

    public Vector3 minPos;
    public Vector3 maxPos;

    Vector3 velocity; // current velocity
    Vector3 velocityVR; // current velocity

    void Start()
    {
        var map = playerControls.FindActionMap("VR", true);
        moveInput = map.FindAction("Move", true);
        moveInput.performed += onMove;
        moveInput.Enable();
        upInput = map.FindAction("Up", true);
        upInput.performed += onUp;
        upInput.Enable();
        downInput = map.FindAction("Down", true);
        downInput.performed += onDown;
        downInput.Enable();
    }

    void onMove(InputAction.CallbackContext context)
    {
        Vector2 vec = context.ReadValue<Vector2>();

        Vector3 moveInput = default;

        if (Mathf.Abs(vec.x) < 0.3f) {
            vec.x = 0.0f;
        }
        if (Mathf.Abs(vec.y) < 0.3f) {
            vec.y = 0.0f;
        }

        if (vec.x >= 0.3f) {
            moveInput += Vector3.right;
        }
        else if (vec.x <= -0.3f) {
            moveInput += Vector3.left;
        }

        if (vec.y >= 0.3f) {
            moveInput += Vector3.forward;
        }
        else if (vec.y <= -0.53) {
            moveInput += Vector3.back;
        }

        Vector3 direction = transform.TransformVector(moveInput.normalized);
        Vector3 result = direction * acceleration; // "walking"

        // Input
        if (Focused) {
            // Position
            velocityVR = result * Time.deltaTime;

            // Rotation
            Vector2 mouseDeltaVR = lookSensitivity * new Vector2(Input.GetAxis("Mouse X"), -Input.GetAxis("Mouse Y"));
            Quaternion rotationVR = transform.rotation;
            Quaternion horizVR = Quaternion.AngleAxis(mouseDeltaVR.x, Vector3.up);
            Quaternion vertVR = Quaternion.AngleAxis(mouseDeltaVR.y, Vector3.right);
            transform.rotation = horizVR * rotationVR * vertVR;

            // Leave cursor lock
            if (Input.GetKeyDown(KeyCode.Escape))
                Focused = false;
            else if (Input.GetMouseButtonDown(0))
                Focused = true;
        }

        // Rotation
        Vector2 mouseDelta = lookSensitivity * new Vector2(Input.GetAxis("Mouse X"), -Input.GetAxis("Mouse Y"));
        Quaternion rotation = transform.rotation;
        Quaternion horiz = Quaternion.AngleAxis(mouseDelta.x, Vector3.up);
        Quaternion vert = Quaternion.AngleAxis(mouseDelta.y, Vector3.right);

        transform.rotation = horiz * rotation * vert;

        velocity = Vector3.Lerp(velocityVR, Vector3.zero, dampingCoefficient * Time.deltaTime);
        transform.position += velocityVR * Time.deltaTime;
    }

    void onUp(InputAction.CallbackContext context)
    {

        Vector3 moveInput = default;
        moveInput += Vector3.up;

        Vector3 direction = transform.TransformVector(moveInput.normalized);
        Vector3 result = direction * acceleration; // "walking"

        // Input
        if (Focused) {
            // Position
            velocityVR = result * Time.deltaTime;

            // Rotation
            Vector2 mouseDeltaVR = lookSensitivity * new Vector2(Input.GetAxis("Mouse X"), -Input.GetAxis("Mouse Y"));
            Quaternion rotationVR = transform.rotation;
            Quaternion horizVR = Quaternion.AngleAxis(mouseDeltaVR.x, Vector3.up);
            Quaternion vertVR = Quaternion.AngleAxis(mouseDeltaVR.y, Vector3.right);
            transform.rotation = horizVR * rotationVR * vertVR;

            // Leave cursor lock
            if (Input.GetKeyDown(KeyCode.Escape))
                Focused = false;
            else if (Input.GetMouseButtonDown(0))
                Focused = true;
        }

        // Rotation
        Vector2 mouseDelta = lookSensitivity * new Vector2(Input.GetAxis("Mouse X"), -Input.GetAxis("Mouse Y"));
        Quaternion rotation = transform.rotation;
        Quaternion horiz = Quaternion.AngleAxis(mouseDelta.x, Vector3.up);
        Quaternion vert = Quaternion.AngleAxis(mouseDelta.y, Vector3.right);

        transform.rotation = horiz * rotation * vert;

        velocity = Vector3.Lerp(velocityVR, Vector3.zero, dampingCoefficient * Time.deltaTime);
        transform.position += velocityVR * Time.deltaTime;
    }

    void onDown(InputAction.CallbackContext context)
    {
        Vector3 moveInput = default;
        moveInput += Vector3.down;

        Vector3 direction = transform.TransformVector(moveInput.normalized);
        Vector3 result = direction * acceleration; // "walking"

        // Input
        if (Focused) {
            // Position
            velocityVR = result * Time.deltaTime;

            // Rotation
            Vector2 mouseDeltaVR = lookSensitivity * new Vector2(Input.GetAxis("Mouse X"), -Input.GetAxis("Mouse Y"));
            Quaternion rotationVR = transform.rotation;
            Quaternion horizVR = Quaternion.AngleAxis(mouseDeltaVR.x, Vector3.up);
            Quaternion vertVR = Quaternion.AngleAxis(mouseDeltaVR.y, Vector3.right);
            transform.rotation = horizVR * rotationVR * vertVR;

            // Leave cursor lock
            if (Input.GetKeyDown(KeyCode.Escape))
                Focused = false;
            else if (Input.GetMouseButtonDown(0))
                Focused = true;
        }

        // Rotation
        Vector2 mouseDelta = lookSensitivity * new Vector2(Input.GetAxis("Mouse X"), -Input.GetAxis("Mouse Y"));
        Quaternion rotation = transform.rotation;
        Quaternion horiz = Quaternion.AngleAxis(mouseDelta.x, Vector3.up);
        Quaternion vert = Quaternion.AngleAxis(mouseDelta.y, Vector3.right);

        transform.rotation = horiz * rotation * vert;

        velocity = Vector3.Lerp(velocityVR, Vector3.zero, dampingCoefficient * Time.deltaTime);
        transform.position += velocityVR * Time.deltaTime;
    }

    static bool Focused
    {
        get => Cursor.lockState == CursorLockMode.Locked;
        set
        {
            Cursor.lockState = value ? CursorLockMode.Locked : CursorLockMode.None;
            Cursor.visible = value == false;
        }
    }

    void OnEnable()
    {
        if (focusOnEnable) Focused = true;
    }

    void OnDisable() => Focused = false;

    void Update()
    {
        // Input
        if (Focused) {
            // Position
            velocity += GetAccelerationVector() * Time.deltaTime;

            // Rotation
            Vector2 mouseDelta = lookSensitivity * new Vector2(Input.GetAxis("Mouse X"), -Input.GetAxis("Mouse Y"));
            Quaternion rotation = transform.rotation;
            Quaternion horiz = Quaternion.AngleAxis(mouseDelta.x, Vector3.up);
            Quaternion vert = Quaternion.AngleAxis(mouseDelta.y, Vector3.right);
            transform.rotation = horiz * rotation * vert;

            // Leave cursor lock
            if (Input.GetKeyDown(KeyCode.Escape))
                Focused = false;
            else if (Input.GetMouseButtonDown(0))
                Focused = true;
        }

        // Physics
        velocity = Vector3.Lerp(velocity, Vector3.zero, dampingCoefficient * Time.deltaTime);
        transform.position += velocity * Time.deltaTime;
    }

    Vector3 GetAccelerationVector()
    {
        Vector3 moveInput = default;

        void AddMovement(KeyCode key, Vector3 dir)
        {
            if (Input.GetKey(key)) 
            {
                moveInput += dir;
            }
        }

        if (canMove) {
            AddMovement(KeyCode.Z, Vector3.forward);
            AddMovement(KeyCode.S, Vector3.back);
            AddMovement(KeyCode.D, Vector3.right);
            AddMovement(KeyCode.Q, Vector3.left);
            AddMovement(KeyCode.Space, Vector3.up);
            AddMovement(KeyCode.LeftControl, Vector3.down);
        }
        Vector3 direction = transform.TransformVector(moveInput.normalized);

        if (Input.GetKey(KeyCode.LeftShift))
            return direction * (acceleration * accSprintMultiplier); // "sprinting"
        return direction * acceleration; // "walking"
    }
}
