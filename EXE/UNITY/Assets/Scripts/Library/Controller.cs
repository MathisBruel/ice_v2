using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;
using UnityEngine.UI;
using System;
using UnityEngine.InputSystem;
using UnityEngine.XR;

using Cis;
using TCP;
using Loader;
using LoaderRoom;
using Calib;

public class Controller : MonoBehaviour
{
    private struct PanelRenderOptions
    {
        public GameObject panel;
        public int offsetXCut;
        public int offsetYCut;
        public int widthCut;
        public int heightCut;
        public bool isLeft;

        public int offsetXPanel;
        public int offsetYPanel;
        public int widthPanel;
        public int heightPanel;

        public int offsetXFore;
        public int offsetYFore;
        public int widthFore;
        public int heightFore;
        public double ratioFore;

        public Texture2D panelImg;
        public Texture2D finalTexture;
    };

    private struct BackLightOptions
    {
        public GameObject backLed;
        public int pointX;
        public int pointY;
    };

    private struct ParOptions
    {
        public GameObject par;
        public int pointX;
        public int pointY;
    };

    private struct MovingHeadOptions
    {
        public GameObject objectMoving;
        public int pointXColor;
        public int pointYColor;
        public int pointXPan;
        public int pointYPan;
        public int pointXTilt;
        public int pointYTilt;
        public Vector3 min;
        public Vector3 max;
    };

    // -- Handle creative mode
    public bool creativeMode;
    public bool exploitationMode;
    private Receiver receiver;

    // -- XR handling
    public InputActionAsset playerControls;
    private InputAction startStopInput;
    private InputAction pauseInput;
    private InputAction nextInput;
    private InputAction quitInput;
    private InputAction trackedInput;

    // -- script handling
    private CisReader cisReader;

    // -- textures
    private Texture2D blackTexture;
    private Texture2D cisFrame;
    private Texture2D leftWallPanels;
    private Texture2D rightWallPanels;

    // -- for room loading
    private GameObject screen;
    private Screen screenScript;
    private Material screenMaterial;
    private Texture2D screenTexture;
    private GameObject auditorium;
    private Light lightAmbient;
    private GameObject cameraObject;

    // -- informations room
    private List<PanelRenderOptions> listPanelsOptions;
    private List<BackLightOptions> listBackOptions;
    private List<ParOptions> listParOptions;
    private List<MovingHeadOptions> listMovingsOptions;

    // -- timings details
    private int lastFrame;

    // -- config
    private LoadContent contentsLoader;
    private LoadRoomConf roomLoader;
    private PanelsCreator panelLeftScript;
    private PanelsCreator panelRightScript;

    // -- player states
    UnityEngine.Video.VideoPlayer player;
    private bool isPlaying;
    private bool isPaused;
    private int counter;
    private float timeStartLoop; 

    // -- UI
    private GameObject objPause;
    private Image pauseButton;
    private GameObject objSlider;
    private Slider positionVideo;
    private GameObject objInfo;
    private Text scriptInfo;
    private GameObject objTime;
    private Text timeVideo;

    private int counterRefreshProbe;

    // -- calibration
    Calibrer backCalib;
    Calibrer parCalib;
    Calibrer mhCalib;

    void OnDestroy() {
        if (creativeMode) {receiver.Delete();}
    }

    // Start is called before the first frame update
    void Start()
    {
        backCalib = new Calibrer();
        backCalib.open("back.calib");
        parCalib = new Calibrer();
        parCalib.open("par.calib");
        mhCalib = new Calibrer();
        mhCalib.open("mh.calib");

        counterRefreshProbe = 0;
        isPlaying = false;
        isPaused = false;
        timeStartLoop = -1.0f;

        cisFrame = new Texture2D(720, 576, TextureFormat.ARGB32, false);
        cisFrame.filterMode = FilterMode.Trilinear;
        cisFrame.anisoLevel = 9;
        blackTexture = Texture2D.blackTexture;
        leftWallPanels = new Texture2D(356, 128, TextureFormat.ARGB32, false);
        rightWallPanels = new Texture2D(356, 128, TextureFormat.ARGB32, false);
        leftWallPanels.filterMode = FilterMode.Trilinear;
        rightWallPanels.filterMode = FilterMode.Trilinear;
        leftWallPanels.anisoLevel = 9;
        rightWallPanels.anisoLevel = 9;

        lastFrame = -1;

        listPanelsOptions = new List<PanelRenderOptions>();
        listBackOptions = new List<BackLightOptions>();
        listParOptions = new List<ParOptions>();
        listMovingsOptions = new List<MovingHeadOptions>();

        // -- load configuration
        roomLoader = new LoadRoomConf();
        creativeMode = roomLoader.isCreative;
        exploitationMode = roomLoader.isExploitation;

        // -- load content
        if (creativeMode) {receiver = new Receiver("127.0.0.1", 9500);}
        else {contentsLoader = new LoadContent();}

        // -- base texture
        screenTexture = new Texture2D(480, 200, TextureFormat.ARGB32, false);
        screenTexture.filterMode = FilterMode.Trilinear;
        screenTexture.anisoLevel = 9;
        screenTexture.wrapMode = TextureWrapMode.Clamp;
        Color[] baseScreenColors = new Color[screenTexture.width*screenTexture.height];
        for (int i = 0; i < screenTexture.width*screenTexture.height; i++) {
            baseScreenColors[i] = new Color(0.05f, 0.05f, 0.05f, 1.0f);
        }
        screenTexture.SetPixels(0, 0, screenTexture.width, screenTexture.height, baseScreenColors);
        screenTexture.Apply();
        baseScreenColors = null;

        // -- load room and devices
        InitRoom();
        InitDevices();
        ResetDevices();

        // -- load UI 
        objTime = GameObject.Find("Time");
        timeVideo = objTime.GetComponent<Text>();
        timeVideo.text = "0/0 frames";
        objInfo = GameObject.Find("ScriptInfo");
        scriptInfo = objInfo.GetComponent<Text>();
        scriptInfo.text = creativeMode ? "" : contentsLoader.getName();
        objPause = GameObject.Find("Pause");
        pauseButton = objPause.GetComponent<Image>();
        pauseButton.color = new Color(0.78f, 0.78f, 0.78f, 1.0f);
        objSlider = GameObject.Find("VideoSlider");
        positionVideo = objSlider.GetComponent<Slider>();
        positionVideo.minValue = 0.0f;
        positionVideo.maxValue = 10.0f;
        positionVideo.value = 0.0f;

        var map = playerControls.FindActionMap("VR", true);
        objTime.SetActive(false);
        objInfo.SetActive(true);
        objPause.SetActive(false);
        objSlider.SetActive(false);

        if (creativeMode) {
            objInfo.SetActive(false);
        }

        // -- XR inputs
        startStopInput = map.FindAction("StartStop", true);
        startStopInput.performed += OnStartStop;
        startStopInput.Enable();
        pauseInput = map.FindAction("Pause", true);
        pauseInput.performed += OnPause;
        pauseInput.Enable();
        nextInput = map.FindAction("Next", true);
        nextInput.performed += OnNext;
        nextInput.Enable();
        quitInput = map.FindAction("Quit", true);
        quitInput.performed += OnQuit;
        quitInput.Enable();
    }

    void ChangeAmbiantLight(float delta)
    {
        panelLeftScript.modifyProbeIntensity(delta);
        panelRightScript.modifyProbeIntensity(delta);
    }

    void StartStop() 
    {
        LoadContent.IceContent content = contentsLoader.getCurrentContent();
        counterRefreshProbe = 0;
        if (!creativeMode) {
            if (!isPlaying) {

                // -- start video
                player.isLooping = content.loop;
                player.url = content.pathVideo;

                // load Cis
                lastFrame = -1;
                cisReader = new CisReader(content.pathIce);
                
                player.Play();

                // -- pausing content at 2 seconds
                if (content.loop) {
                    timeStartLoop = Time.time;
                }

                isPlaying = true;
                scriptInfo.text = "";
                positionVideo.value = 0.0f;
                objInfo.SetActive(false);
                objTime.SetActive(false);
                objPause.SetActive(false);
                objSlider.SetActive(false);

                lightAmbient.intensity = 0.2f;

                RenderTexture renderTexture = Resources.Load<RenderTexture>("Materials/video-texture");
                MeshRenderer renderer = screen.GetComponent<MeshRenderer>();
                renderer.material.SetTexture("_EmissionMap", renderTexture);
            }
            else {
                player.Stop();
                isPlaying = false;
                isPaused = false;
                scriptInfo.text = content.name;
                positionVideo.value = 0.0f;
                objInfo.SetActive(true);
                objTime.SetActive(false);
                objPause.SetActive(false);
                objSlider.SetActive(false);

                LoaderRoom.LoadRoomConf.AuditoriumConf confAudi = roomLoader.audiConf;
                lightAmbient.intensity = confAudi.ambientLight;

                cisReader = null;
                ResetDevices();
            }
        }
    }

    void Pause()
    {
        LoadContent.IceContent content = contentsLoader.getCurrentContent();
        if (isPlaying && ! creativeMode)
        {
            if (!isPaused) {

                isPaused = true;
                if (!content.loop) {
                    player.Pause();
                }
                scriptInfo.text = content.name;
                objTime.SetActive(true);
                objInfo.SetActive(true);
                objPause.SetActive(true);
                objSlider.SetActive(true);
                return;
            }
            else {
                
                isPaused = false;
                if (!content.loop) {
                    player.Play();
                }
                scriptInfo.text = "";
                objTime.SetActive(false);
                objInfo.SetActive(false);
                objPause.SetActive(false);
                objSlider.SetActive(false);
                return;
            }
        }
    }

    void Next() 
    {
        if (!isPlaying && !creativeMode) {

            // -- switch next content
            contentsLoader.setNextContent();
            scriptInfo.text = contentsLoader.getName();
        }
    }

    void OnStartStop(InputAction.CallbackContext context)
    { 
        Debug.Log("On Start !");
        StartStop();
    }

    void OnPause(InputAction.CallbackContext context)
    { 
        Debug.Log("On Pause !");
        Pause();
    }

    void OnNext(InputAction.CallbackContext context)
    { 
        Debug.Log("On Pause !");
        Next();
    }

    void OnQuit(InputAction.CallbackContext context)
    {
        if (creativeMode) {receiver.Delete();}
        Application.Quit();
    }

    void ResetDevices()
    {
        player.targetTexture.Release();
        MeshRenderer renderer = screen.GetComponent<MeshRenderer>();
        renderer.material.SetTexture("_EmissionMap", screenTexture);
        ResetPanels();
        ResetDmxs();
        updateRenderer(true);
    }

    void ResetPanels()
    {
        foreach (PanelRenderOptions o in listPanelsOptions)
        {
            MeshRenderer renderer = o.panel.GetComponent<MeshRenderer>();
            renderer.material.SetTexture("_EmissionMap", screenTexture);
        }
    }
    void ResetDmxs()
    {
        Color colorBlack = new Color(0.0f, 0.0f, 0.0f, 1.0f);

        foreach (ParOptions o in listParOptions)
        {
            Light light = o.par.GetComponent<Light>();
            light.color = colorBlack;
            light.intensity = roomLoader.parIntensityLeft;
        }

        foreach (BackLightOptions o in listBackOptions)
        {
            Light light = o.backLed.GetComponent<Light>();
            light.color = colorBlack;
            light.intensity = roomLoader.backIntensityLeft;
        }

        foreach (MovingHeadOptions o in listMovingsOptions)
        {
            Light light = o.objectMoving.GetComponent<Light>();
            light.color = colorBlack;
            light.intensity = roomLoader.movingIntensityLeft;
        }
    }

    void InitRoom()
    {
        // -- init camera
        cameraObject = GameObject.Find("XRRig");
        LoaderRoom.LoadRoomConf.CameraConf confCamera = roomLoader.cameraConf;
        cameraObject.transform.position = confCamera.position;
        cameraObject.transform.rotation = Quaternion.Euler(confCamera.rotation.x, confCamera.rotation.y, confCamera.rotation.z);
        FlyCamera fly = cameraObject.GetComponent<FlyCamera>();
        fly.canMove = !roomLoader.isExploitation;

        // -- init screen
        LoaderRoom.LoadRoomConf.ScreenConf confScreen = roomLoader.screenConf;
        screen = GameObject.Find("Screen");
        screenMaterial = screen.GetComponent<Material>();
        screenScript = screen.GetComponent<Screen>();
        screenScript.width = confScreen.width;
        screenScript.height = confScreen.height;
        screenScript.offsetX = confScreen.offsetX;
        screenScript.offsetY = confScreen.offsetY;
        screenScript.offsetZ = confScreen.offsetZ;
        screenScript.Init();
        // -- init player
        player = screen.GetComponent<UnityEngine.Video.VideoPlayer>();

        // -- init auditorium
        LoaderRoom.LoadRoomConf.AuditoriumConf confAudi = roomLoader.audiConf;
        auditorium = GameObject.Find("Auditorium");
        Auditorium audiScript = auditorium.GetComponent<Auditorium>();
        audiScript.width = confAudi.width;
        audiScript.height = confAudi.height;
        audiScript.length = confAudi.length;
        audiScript.tiers = roomLoader.tieringConf;
        audiScript.front = confAudi.scaleFront;
        audiScript.hasLogo = confAudi.hasLogo;
        audiScript.Init();
        GameObject ambient = GameObject.Find("AmbientLight");
        ambient.transform.position = new Vector3(confAudi.width/2.0f, confAudi.height/2.0f, confAudi.length / 2.0f);
        lightAmbient = ambient.GetComponent<Light>();
        lightAmbient.intensity = confAudi.ambientLight;

        // -- init seats
        GameObject seatCreator = GameObject.Find("Seats");
        SeatCreator seatScript = seatCreator.GetComponent<SeatCreator>();
        seatScript.seatsPosition.Clear();
        foreach (Vector3 position in roomLoader.seatsConf) 
        {
            seatScript.seatsPosition.Add(position);
        }
        seatScript.Init();

        // -- init left panels
        GameObject panelsLeftCreator = GameObject.Find("LeftPanels");
        panelLeftScript = panelsLeftCreator.GetComponent<PanelsCreator>();
        panelLeftScript.positions.Clear();
        panelLeftScript.scales.Clear();
        foreach (LoaderRoom.LoadRoomConf.PanelConf conf in roomLoader.leftPanelsConf) 
        {
            panelLeftScript.positions.Add(conf.position);
            panelLeftScript.scales.Add(conf.scale);
        }
        panelLeftScript.widthAudi = confAudi.width;
        panelLeftScript.heightAudi = confAudi.height;
        panelLeftScript.centerScreen = new Vector3(-confScreen.offsetX - confScreen.width/4.0f, confScreen.offsetY + confScreen.height/2.0f, confScreen.offsetZ);
        panelLeftScript.Init();

        // -- init right panels
        GameObject panelsRightCreator = GameObject.Find("RightPanels");
        panelRightScript = panelsRightCreator.GetComponent<PanelsCreator>();
        panelRightScript.positions.Clear();
        panelRightScript.scales.Clear();
        foreach (LoaderRoom.LoadRoomConf.PanelConf conf in roomLoader.rightPanelsConf) 
        {
            panelRightScript.positions.Add(conf.position);
            panelRightScript.scales.Add(conf.scale);
        }
        panelRightScript.widthAudi = confAudi.width;
        panelRightScript.heightAudi = confAudi.height;
        panelRightScript.centerScreen = new Vector3(-confScreen.offsetX - 3.0f*confScreen.width/4.0f, confScreen.offsetY + confScreen.height/2.0f, confScreen.offsetZ);
        panelRightScript.Init();

        // -- init backlights
        GameObject backCreator = GameObject.Find("Backlights");
        BackCreator backScript = backCreator.GetComponent<BackCreator>();
        backScript.positionsLeft.Clear();
        foreach (Vector3 position in roomLoader.leftBackConf) 
        {
            backScript.positionsLeft.Add(position);
        }
        backScript.positionsRight.Clear();
        foreach (Vector3 position in roomLoader.rightBackConf) 
        {
            backScript.positionsRight.Add(position);
        }
        if (backScript.positionsLeft.Count > 0) {
            backScript.rangeLeft = backScript.positionsLeft[0].z - screenScript.offsetZ+1;
            backScript.rangeRight = backScript.positionsRight[0].z - screenScript.offsetZ+1;
            backScript.angleBackLeft = roomLoader.angleBackLeft;
            backScript.angleBackRight = roomLoader.angleBackRight;
            backScript.Init();
        }

        // -- init PARS
        GameObject parCreator = GameObject.Find("Pars");
        ParsCreator parScript = parCreator.GetComponent<ParsCreator>();
        parScript.positionsLeft.Clear();
        parScript.seatMin = confAudi.seatMin;
        parScript.seatMax = confAudi.seatMax;
        foreach (LoaderRoom.LoadRoomConf.ParsConf conf in roomLoader.leftParsConf)
        {
            parScript.positionsLeft.Add(conf.position);
        }
        parScript.positionsRight.Clear();
        foreach (LoaderRoom.LoadRoomConf.ParsConf conf in roomLoader.rightParsConf)
        {
            parScript.positionsRight.Add(conf.position);
        }
        parScript.Init();

        // -- init MOVINGS
        GameObject movingCreator = GameObject.Find("Movings");
        MovingCreator movingScript = movingCreator.GetComponent<MovingCreator>();
        movingScript.positionsLeft.Clear();
        for (int i = 0; i < roomLoader.leftMovingConf.Count; i++) {
            movingScript.positionsLeft.Add(roomLoader.leftMovingConf[i]);
            movingScript.positionsMinLeft.Add(roomLoader.leftMovingMinConf[i]);
            movingScript.positionsMaxLeft.Add(roomLoader.leftMovingMaxConf[i]);
        }
        movingScript.positionsRight.Clear();
        for (int i = 0; i < roomLoader.rightMovingConf.Count; i++) {
            movingScript.positionsRight.Add(roomLoader.rightMovingConf[i]);
            movingScript.positionsMinRight.Add(roomLoader.rightMovingMinConf[i]);
            movingScript.positionsMaxRight.Add(roomLoader.rightMovingMaxConf[i]);
        }
        movingScript.Init();
    }

    void InitDevices()
    {
        // -- screen access datas
        double ratioMmToPixel = 128.0 / (double)(screenScript.height);

        // -- access datas of left
        GameObject leftCreator = GameObject.Find("LeftPanels");
        PanelsCreator leftScript = leftCreator.GetComponent<PanelsCreator>();
        if (leftScript.positions.Count > 0) {
            double offsetLeftNotCounting = leftScript.positions[0].z;

            // -- start with left panels
            GameObject[] left = GameObject.FindGameObjectsWithTag("leftPanel");
            foreach (GameObject o in left)
            {
                string[] subs = o.name.Split('_');
                int index = System.Convert.ToInt32(subs[subs.Length - 1]);

                PanelRenderOptions options = new PanelRenderOptions();
                options.panel = o;
                options.finalTexture = new Texture2D(2, 2, TextureFormat.ARGB32, false);
                options.finalTexture.filterMode = FilterMode.Trilinear;
                options.finalTexture.anisoLevel = 9;
                options.finalTexture.wrapMode = TextureWrapMode.Clamp;
                options.widthCut = (int)((double)(leftScript.scales[index].z) * ratioMmToPixel + 0.5);
                options.heightCut = (int)((double)(leftScript.scales[index].y) * ratioMmToPixel + 0.5);
                options.offsetXCut = (int)(356.0 - (double)(leftScript.positions[index].z - offsetLeftNotCounting) * ratioMmToPixel - options.widthCut - 3.5);
                options.offsetYCut = (int)((double)(screenScript.height + screenScript.offsetY - leftScript.positions[index].y - leftScript.scales[index].y) * ratioMmToPixel + 0.5);

                options.widthPanel = 20;
                options.heightPanel = 84;
                options.offsetYPanel = 134;
                options.offsetXPanel = 332 - 36 * index;

                options.isLeft = true;

                // -- determine fore image ratio
                options.ratioFore = 20.0 / (double)(leftScript.scales[index].z);
                options.offsetXFore = 0;
                options.widthFore = 20;
                options.offsetYFore = (int)(42.0 - ((leftScript.scales[index].y / 2.0) * options.ratioFore) + 0.5);
                options.heightFore = (int)(leftScript.scales[index].y * options.ratioFore + 0.5);

                options.panelImg = new Texture2D(20, 84, TextureFormat.ARGB32, false);
                options.panelImg.filterMode = FilterMode.Trilinear;
                options.panelImg.anisoLevel = 9;

                listPanelsOptions.Add(options);
            }
        }
        
        // -- access datas of right
        GameObject rightCreator = GameObject.Find("RightPanels");
        PanelsCreator rightScript = rightCreator.GetComponent<PanelsCreator>();
        if (rightScript.positions.Count > 0) {
            double offsetRightNotCounting = rightScript.positions[0].z;

            // -- start with left panels
            GameObject[] right = GameObject.FindGameObjectsWithTag("rightPanel");
            foreach (GameObject o in right)
            {
                string[] subs = o.name.Split('_');
                int index = System.Convert.ToInt32(subs[subs.Length - 1]);

                PanelRenderOptions options = new PanelRenderOptions();
                options.panel = o;
                options.finalTexture = new Texture2D(2, 2, TextureFormat.ARGB32, false);
                options.finalTexture.filterMode = FilterMode.Trilinear;
                options.finalTexture.anisoLevel = 9;
                options.finalTexture.wrapMode = TextureWrapMode.Clamp;
                options.offsetXCut = (int)((double)(rightScript.positions[index].z - offsetRightNotCounting) * ratioMmToPixel + 4.5);
                options.offsetYCut = (int)((double)(screenScript.height + screenScript.offsetY - rightScript.positions[index].y - rightScript.scales[index].y) * ratioMmToPixel + 0.5);
                options.widthCut = (int)((double)(rightScript.scales[index].z) * ratioMmToPixel + 0.5);
                options.heightCut = (int)((double)(rightScript.scales[index].y) * ratioMmToPixel + 0.5);

                options.widthPanel = 20;
                options.heightPanel = 84;
                options.offsetYPanel = 134;
                options.offsetXPanel = 368 + 36 * index;

                options.ratioFore = 20.0 / (double)(rightScript.scales[index].z);
                options.offsetXFore = 0;
                options.widthFore = 20;
                options.offsetYFore = (int)(42.0 - ((rightScript.scales[index].y / 2.0) * options.ratioFore) + 0.5);
                options.heightFore = (int)(rightScript.scales[index].y * options.ratioFore + 0.5);

                options.isLeft = false;

                options.panelImg = new Texture2D(20, 84, TextureFormat.ARGB32, false);
                options.panelImg.filterMode = FilterMode.Trilinear;
                options.panelImg.anisoLevel = 9;

                listPanelsOptions.Add(options);
            }
        }

        // -- create PARS informations
        GameObject[] parsLeft = GameObject.FindGameObjectsWithTag("PARS_LEFT");
        for (int i = 0; i < parsLeft.Length; i++)
        {
            ParOptions parOption = new ParOptions();
            parOption.par = parsLeft[i];
            parOption.pointX = 30+i*60;
            parOption.pointY = 256;

            listParOptions.Add(parOption);
        }
        GameObject[] parsRight = GameObject.FindGameObjectsWithTag("PARS_RIGHT");
        for (int i = 0; i < parsRight.Length; i++)
        {
            ParOptions parOption = new ParOptions();
            parOption.par = parsRight[i];
            parOption.pointX = 690-i*60;
            parOption.pointY = 256;

            listParOptions.Add(parOption);
        }

        // -- create BACK informations
        GameObject[] backsLeft = GameObject.FindGameObjectsWithTag("BACK_LEFT");
        for (int i = 0; i < backsLeft.Length; i++)
        {
            BackLightOptions backOption = new BackLightOptions();
            backOption.backLed = backsLeft[i];
            backOption.pointX = 270+i*60;
            backOption.pointY = 256;

            listBackOptions.Add(backOption);
        }
        GameObject[] backsRight = GameObject.FindGameObjectsWithTag("BACK_RIGHT");
        for (int i = 0; i < backsRight.Length; i++)
        {
            BackLightOptions backOption = new BackLightOptions();
            backOption.backLed = backsRight[i];
            backOption.pointX = 270+i*60;
            backOption.pointY = 256;

            listBackOptions.Add(backOption);
        }

        // -- create MOVINGS informations
        GameObject movingCreator = GameObject.Find("Movings");
        MovingCreator movingScript = movingCreator.GetComponent<MovingCreator>();
        GameObject[] movingsLeft = GameObject.FindGameObjectsWithTag("MOVING_LEFT");
        if (movingsLeft.Length > 0) {
            MovingHeadOptions movingOptionLF = new MovingHeadOptions();
            movingOptionLF.objectMoving = movingsLeft[0];
            movingOptionLF.pointXColor = 0;
            movingOptionLF.pointYColor = 508;
            movingOptionLF.pointXPan = 33;
            movingOptionLF.pointYPan = 575;
            movingOptionLF.pointXTilt = 34;
            movingOptionLF.pointYTilt = 575;
            movingOptionLF.min = movingScript.positionsMinLeft[0];
            movingOptionLF.max = movingScript.positionsMaxLeft[0];
            listMovingsOptions.Add(movingOptionLF);

            MovingHeadOptions movingOptionLR = new MovingHeadOptions();
            movingOptionLR.objectMoving = movingsLeft[1];
            movingOptionLR.pointXColor = 0;
            movingOptionLR.pointYColor = 440;
            movingOptionLR.pointXPan = 39;
            movingOptionLR.pointYPan = 575;
            movingOptionLR.pointXTilt = 40;
            movingOptionLR.pointYTilt = 575;
            movingOptionLR.min = movingScript.positionsMinLeft[1];
            movingOptionLR.max = movingScript.positionsMaxLeft[1];
            listMovingsOptions.Add(movingOptionLR);
        }

        GameObject[] movingsRight = GameObject.FindGameObjectsWithTag("MOVING_RIGHT");
        if (movingsRight.Length > 0) {
            MovingHeadOptions movingOptionRF = new MovingHeadOptions();
            movingOptionRF.objectMoving = movingsRight[0];
            movingOptionRF.pointXColor = 68;
            movingOptionRF.pointYColor = 508;
            movingOptionRF.pointXPan = 36;
            movingOptionRF.pointYPan = 575;
            movingOptionRF.pointXTilt = 37;
            movingOptionRF.pointYTilt = 575;
            movingOptionRF.min = movingScript.positionsMinRight[0];
            movingOptionRF.max = movingScript.positionsMaxRight[0];
            listMovingsOptions.Add(movingOptionRF);

            MovingHeadOptions movingOptionRR = new MovingHeadOptions();
            movingOptionRR.objectMoving = movingsRight[1];
            movingOptionRR.pointXColor = 68;
            movingOptionRR.pointYColor = 440;
            movingOptionRR.pointXPan = 42;
            movingOptionRR.pointYPan = 575;
            movingOptionRR.pointXTilt = 43;
            movingOptionRR.pointYTilt = 575;
            movingOptionRR.min = movingScript.positionsMinRight[1];
            movingOptionRR.max = movingScript.positionsMaxRight[1];
            listMovingsOptions.Add(movingOptionRR);
        }
    }

    void RestartCamera() 
    {
        LoaderRoom.LoadRoomConf.CameraConf confCamera = roomLoader.cameraConf;
        cameraObject.transform.position = confCamera.position;
        cameraObject.transform.rotation = Quaternion.Euler(confCamera.rotation.x, confCamera.rotation.y, confCamera.rotation.z);
    }

    void Update()
    {
        // -- handle state
        if (Input.GetKeyDown(KeyCode.C))
        {
            if (creativeMode) {receiver.Delete();}
            Application.Quit();
            return;
        }
        if (Input.GetKeyDown(KeyCode.F5) && !exploitationMode)
        {
            ChangeAmbiantLight(-0.1f);
        }
        if (Input.GetKeyDown(KeyCode.F6) && !exploitationMode)
        {
            ChangeAmbiantLight(0.1f);
        }
        if (Input.GetKeyDown(KeyCode.P))
        {
            StartStop();
            return;
        }

        if (Input.GetKeyDown(KeyCode.O))
        {
            Pause();
        }
        if (Input.GetKeyDown(KeyCode.N) && !exploitationMode)
        {
            Next();
        }
        if (Input.GetKeyDown(KeyCode.G) && !exploitationMode)
        {
            RestartCamera();
        }

        if (isPlaying && !creativeMode) {
            
            int frame = -1;

            LoadContent.IceContent content = contentsLoader.getCurrentContent();

            if (content.loop) {

                if (player.isPlaying) {
                    player.Pause();
                    player.time = 2.0f;
                }

                float currentTime = Time.time;
                
                if (!isPaused) {
                    float deltaTime = currentTime - timeStartLoop;
                    frame = (int)(deltaTime * 24.0f);
                    frame = frame % (cisReader.frameCount-1);
                }
                else 
                {
                    frame = lastFrame;
                }           

                positionVideo.maxValue = cisReader.frameCount;
                positionVideo.value = frame;
                timeVideo.text = frame + "/" + cisReader.frameCount + " frames";
            }
            else {

                long playerCurrentFrame = player.frame;
                long playerFrameCount = Convert.ToInt64(player.frameCount);
                float frameRate = player.frameRate;
                float maxTime = playerFrameCount / frameRate;

                if (Input.GetKeyDown(KeyCode.LeftArrow)) {
                    if (player.time - 1.0f/frameRate < 0.0f) {
                        player.time = 0.0f;
                    }
                    else {
                        player.time = player.time - 1.0f/frameRate;   
                    }
                }
                else if (Input.GetKeyDown(KeyCode.RightArrow)) {
                    if (player.time + 1.0f/frameRate > maxTime) {
                        player.time = maxTime;
                    }
                    else {
                        player.time = player.time + 1.0f/frameRate;   
                    }
                }
                else if (Input.GetKeyDown(KeyCode.A) && exploitationMode) {
                    if (player.time - 10.0f < 0.0f) {
                        player.time = 0.0f;
                    }
                    else {
                        player.time = player.time - 10.0f;   
                    }
                }
                else if (Input.GetKeyDown(KeyCode.Z) && exploitationMode) {
                    if (player.time + 10.0f > maxTime) {
                        player.time = maxTime;
                    }
                    else {
                        player.time = player.time + 10.0f;   
                    }
                }
                else if (Input.GetKeyDown(KeyCode.Q) && exploitationMode) {
                    if (player.time - 600.0f < 0.0f) {
                        player.time = 0.0f;
                    }
                    else {
                        player.time = player.time - 600.0f;   
                    }
                }
                else if (Input.GetKeyDown(KeyCode.S) && exploitationMode) {
                    if (player.time + 600.0f > maxTime) {
                        player.time = maxTime;
                    }
                    else {
                        player.time = player.time + 600.0f;   
                    }
                }

                positionVideo.maxValue = playerFrameCount;
                positionVideo.value = playerCurrentFrame;
                timeVideo.text = playerCurrentFrame + "/" + playerFrameCount + " frames";
        
                if(playerCurrentFrame >= playerFrameCount)
                {
                    if (contentsLoader.isContent()) {
                        isPlaying = false;
                        scriptInfo.text = contentsLoader.getName();
                        positionVideo.value = 0.0f;
                        cisReader = null;
                        ResetDevices();
                    }
                    else {
                        // -- has next content in playlist
                        if (contentsLoader.setNextContentInPlaylist()) {
                            content = contentsLoader.getCurrentContent();
                            // -- start video
                            player.isLooping = false;
                            player.url = content.pathVideo;

                            // load Cis
                            lastFrame = -1;
                            cisReader = new CisReader(content.pathIce);
                            player.Play();
                        }
                        else {
                            isPlaying = false;
                            scriptInfo.text = contentsLoader.getName();
                            positionVideo.value = 0.0f;
                            cisReader = null;
                            ResetDevices();
                        }
                    }
                }

                // -- calculate frame from player time
                double timeSeconds = player.time;
                frame = (int)(timeSeconds * 24.0);
                frame = frame - content.startDelay + content.entryPoint;
            }

            if (frame != lastFrame && frame >= 0)
            {
                byte[] imgRaw = cisReader.GetFrame(frame);

                if (imgRaw == null)
                {
                    Debug.LogError("Error while reading frame !");
                    ResetPanels();
                    return;
                }

                // -- convert to texture 2D
                cisFrame.LoadImage(imgRaw, false);
                cisFrame.Apply();
                imgRaw = null;

                // -- extract temporaries textures
                ExtractPanelImage();

                // -- Compute temporaries image for each panel
                computeAndAssignImageToPanels();

                // -- apply DMXS
                ExtractAndApplyPar(cisReader.lutApply);
                ExtractAndApplyBack(cisReader.lutApply);
                ExtractAndApplyMoving(cisReader.lutApply);

                updateRenderer(false);

                lastFrame = frame;
            }
        }

        else if (creativeMode) {

            int frame = receiver.frameNumber;
            if (frame != lastFrame && frame >= 0)
            {
                if (frame == -1)
                {
                    Debug.LogError("Init no received frame !");
                    ResetPanels();
                    return;
                }

                // -- convert to texture 2D
                while (!receiver.mutex.WaitOne(20)) {}
                cisFrame.SetPixels(receiver.pixels);
                cisFrame.Apply();
                receiver.mutex.ReleaseMutex();

                /*byte[] bytes = cisFrame.EncodeToPNG();
                File.WriteAllBytes(Application.persistentDataPath + "/template.png", bytes);*/

                // -- set screen
                ExtractAndSetScreen();

                // -- extract temporaries textures
                ExtractPanelImage();

                // -- Compute temporaries image for each panel
                computeAndAssignImageToPanels();

                // -- apply DMXS
                ExtractAndApplyPar(receiver.lutApply);
                ExtractAndApplyBack(receiver.lutApply);
                ExtractAndApplyMoving(receiver.lutApply);
                
                updateRenderer(false);

                lastFrame = frame;
            }
        }
    }

    void updateRenderer(bool refreshAll)
    {
        GameObject probeLeft = GameObject.Find("left_probe");
        GameObject probeRight = GameObject.Find("right_probe");

        ReflectionProbe left = probeLeft.GetComponent<ReflectionProbe>();
        ReflectionProbe right = probeRight.GetComponent<ReflectionProbe>();

        if (!refreshAll) {
            if (counterRefreshProbe % 4 == 0) {
                left.RenderProbe();
            }
            else if (counterRefreshProbe % 4 == 2){
                right.RenderProbe();
            }
            counterRefreshProbe++;
        }
        else {
            left.RenderProbe();
            right.RenderProbe();
        }
    }

    void ExtractAndSetScreen()
    {
        Color[] screenPixels = cisFrame.GetPixels(240, 376, 480, 200);
        screenTexture.SetPixels(screenPixels);
        screenTexture.Apply();
        screenPixels = null;

        // -- assign final texture
        MeshRenderer renderer = screen.GetComponent<MeshRenderer>();
        renderer.material.SetTexture("_EmissionMap", screenTexture);

        /*byte[] bytes = screenTexture.EncodeToPNG();
        File.WriteAllBytes(Application.persistentDataPath + "/screen.png", bytes);*/
    }

    void ExtractPanelImage()
    {
        Color[] leftPixels = cisFrame.GetPixels(0, 0, 356, 128);
        leftWallPanels.SetPixels(leftPixels);
        leftWallPanels.Apply();
        leftPixels = null;

        Color[] rightWallPixels = cisFrame.GetPixels(364, 0, 356, 128);
        rightWallPanels.SetPixels(rightWallPixels);
        rightWallPanels.Apply();
        rightWallPixels = null;

        // -- debug purpose
        /*byte[] bytes = cisFrame.EncodeToPNG();
        File.WriteAllBytes(Application.persistentDataPath + "/frame.png", bytes);
        byte[] leftBytes = leftWallPanels.EncodeToPNG();
        File.WriteAllBytes(Application.persistentDataPath + "/leftWall.png", leftBytes);
        byte[] rightBytes = rightWallPanels.EncodeToPNG();
        File.WriteAllBytes(Application.persistentDataPath + "/rightWall.png", rightBytes);*/

        // -- extract panel images too
        foreach (PanelRenderOptions o in listPanelsOptions)
        {
            Color[] pixels = cisFrame.GetPixels(o.offsetXPanel, o.offsetYPanel, o.widthPanel, o.heightPanel);
            o.panelImg.SetPixels(pixels);
            o.panelImg.Apply();
            pixels = null;

            /*byte[] bytesFore = o.panelImg.EncodeToPNG();
            File.WriteAllBytes(Application.persistentDataPath + "/fore_" + o.panel.name + ".png", bytesFore);*/
        }
    }

    void computeAndAssignImageToPanels()
    {
        foreach (PanelRenderOptions o in listPanelsOptions)
        {
            // -- compute fore image
            Texture2D foreTexture = new Texture2D(o.widthFore, o.heightFore, TextureFormat.ARGB32, false);
            foreTexture.filterMode = FilterMode.Trilinear;
            foreTexture.anisoLevel = 9;
            int pixelSrcOffsetX = o.offsetXFore;
            int pixelSrcWidth = o.widthFore;
            int pixelSrcOffsetY = o.offsetYFore;
            int pixelSrcHeight = o.heightFore;
            int paddingX = 0;
            int paddingY = 0;
            int marginY = 0;

            // -- check overflow
            if (pixelSrcOffsetY < 0)
            {
                paddingY = -pixelSrcOffsetY;
                pixelSrcHeight += pixelSrcOffsetY; // height is reduced
                pixelSrcOffsetY = 0;
            }
            if (pixelSrcOffsetY + pixelSrcHeight > 84)
            {
                marginY = pixelSrcOffsetY + pixelSrcHeight - 84; // -- determine zone to left black in final image
                pixelSrcHeight = (int)(84 - pixelSrcOffsetY);
            }

            // -- extract
            int reeloffsetY = o.panelImg.height - pixelSrcOffsetY - pixelSrcHeight;
            Color[] forePixels = o.panelImg.GetPixels(pixelSrcOffsetX, reeloffsetY, pixelSrcWidth, pixelSrcHeight);
            foreTexture.SetPixels(paddingX, paddingY, pixelSrcWidth, pixelSrcHeight, forePixels);

            if (marginY > 0) {
                // -- set all black
                Color[] blackBack = new Color[marginY*o.widthFore];
                for (int i = 0; i < marginY*o.widthFore; i++) {
                    blackBack[i] = new Color(0.0f, 0.0f, 0.0f, 0.0f);
                }
                foreTexture.SetPixels(0, 0, o.widthFore, marginY, blackBack);
                blackBack = null;
            }
            if (paddingY > 0) {
                // -- set all black
                Color[] blackBack = new Color[(paddingY+2)*o.widthFore];
                for (int i = 0; i < (paddingY+2)*o.widthFore; i++) {
                    blackBack[i] = new Color(0.0f, 0.0f, 0.0f, 0.0f);
                }
                foreTexture.SetPixels(0, pixelSrcHeight+marginY-2, o.widthFore, paddingY+2, blackBack);
                blackBack = null;
            }

            foreTexture.Apply();
            forePixels = null;

            // -- compute back texture
            Texture2D backTexture = new Texture2D(o.widthCut, o.heightCut, TextureFormat.ARGB32, false);
            backTexture.filterMode = FilterMode.Trilinear;
            backTexture.anisoLevel = 9;
            pixelSrcOffsetX = o.offsetXCut;
            pixelSrcWidth = o.widthCut;
            pixelSrcOffsetY = o.offsetYCut;
            pixelSrcHeight = o.heightCut;
            paddingX = 0;
            paddingY = 0;
            int marginX = 0;

            // -- check overflow
            if (pixelSrcOffsetY < 0)
            {
                pixelSrcHeight += pixelSrcOffsetY;
                pixelSrcOffsetY = 0;
            }
            if (pixelSrcOffsetY + pixelSrcHeight > 128)
            {
                paddingY = pixelSrcOffsetY + pixelSrcHeight - 128; // -- determine zone to left black in final image
                pixelSrcHeight = 128 - pixelSrcOffsetY;
            }

            if (pixelSrcOffsetX + pixelSrcWidth < 0 || pixelSrcOffsetX >= 356) {
                Debug.Log("Panel is completely outside canvas !");
            }

            else {
                // -- for left panels
                if (pixelSrcOffsetX < 0)
                {
                    paddingX = -pixelSrcOffsetX;
                    pixelSrcWidth += pixelSrcOffsetX;
                    pixelSrcOffsetX = 0;
                }
                // -- for right panels
                if (pixelSrcOffsetX + pixelSrcWidth > 356)
                {
                    marginX = pixelSrcWidth + pixelSrcOffsetX - 356;
                    pixelSrcWidth = 356 - pixelSrcOffsetX;
                }

                // -- extract
                Texture2D refTextureBack = o.isLeft ? leftWallPanels : rightWallPanels;
                reeloffsetY = refTextureBack.height - pixelSrcOffsetY - pixelSrcHeight;
                Color[] cutPixels = refTextureBack.GetPixels(pixelSrcOffsetX, reeloffsetY, pixelSrcWidth, pixelSrcHeight);
                backTexture.SetPixels(paddingX, paddingY, pixelSrcWidth, pixelSrcHeight, cutPixels);

                if (paddingX > 0) {
                    // -- set all black
                    Color[] blackBack = new Color[paddingX*o.heightCut];
                    for (int i = 0; i < paddingX*o.heightCut; i++) {
                        blackBack[i] = new Color(0.0f, 0.0f, 0.0f, 1.0f);
                    }
                    backTexture.SetPixels(0, 0, paddingX, o.heightCut, blackBack);
                    blackBack = null;
                }
                else if (marginX > 0) {
                    // -- set all black
                    Color[] blackBack = new Color[marginX*o.heightCut];
                    for (int i = 0; i < marginX*o.heightCut; i++) {
                        blackBack[i] = new Color(0.0f, 0.0f, 0.0f, 1.0f);
                    }
                    backTexture.SetPixels(pixelSrcWidth, 0, marginX, o.heightCut, blackBack);
                    blackBack = null;
                }

                backTexture.Apply();
                cutPixels = null;
            }

            // -- resize with smaller
            ScaleTexture(backTexture, foreTexture.width, foreTexture.height);

            // -- merge
            o.finalTexture.Resize(foreTexture.width, foreTexture.height);
            for (int i = 0; i < foreTexture.height; i++)
            {
                for (int j = 0; j < foreTexture.width; j++)
                {
                    Color foreColor = foreTexture.GetPixel(j, i);
                    Color backColor = backTexture.GetPixel(j, i);

                    // -- BLEND
                    float red = (foreColor.r * foreColor.r) + (backColor.r * backColor.r) * (1.0f - foreColor.a);
                    red = Mathf.Min((float)Mathf.Sqrt(red), 1.0f);
                    float green = foreColor.g * foreColor.g + (backColor.g * backColor.g) * (1.0f - foreColor.a);
                    green = Mathf.Min((float)Mathf.Sqrt(green), 1.0f);
                    float blue = foreColor.b * foreColor.b + (backColor.b * backColor.b) * (1.0f - foreColor.a);
                    blue = Mathf.Min((float)Mathf.Sqrt(blue), 1.0f);

                    // -- MIX
                    /*float red = backColor.r + foreColor.a * (foreColor.r - backColor.r);
                    red = Mathf.Max(Mathf.Min(red, 1.0f), 0.0f);
                    float green = backColor.g + foreColor.a * (foreColor.g - backColor.g);
                    green = Mathf.Max(Mathf.Min(green, 1.0f), 0.0f);
                    float blue = backColor.b + foreColor.a * (foreColor.b - backColor.b);
                    blue = Mathf.Max(Mathf.Min(blue, 1.0f), 0.0f);*/

                    Color blendColor = new Color(red, green, blue, 1.0f);
                    o.finalTexture.SetPixel(j, i, blendColor);
                }
            }
            o.finalTexture.Apply();
            Destroy(backTexture);
            Destroy(foreTexture);

            // -- assign final texture
            MeshRenderer renderer = o.panel.GetComponent<MeshRenderer>();
            renderer.material.SetTexture("_EmissionMap", o.finalTexture);

            // -- debug final image
            /*byte[] bytes = o.finalTexture.EncodeToPNG();
            File.WriteAllBytes(Application.persistentDataPath + "/" + o.panel.name + "_final.png", bytes);*/
        }
    }

    void ScaleTexture(Texture2D tex, int width, int height)
    {
        Rect texR = new Rect(0, 0, width, height);

        //We need the source texture in VRAM because we render with it
        tex.filterMode = FilterMode.Trilinear;
        tex.Apply(true);

        //Using RTT for best quality and performance. Thanks, Unity 5
        RenderTexture rtt = new RenderTexture(width, height, 32);

        //Set the RTT in order to render to it
        Graphics.SetRenderTarget(rtt);

        //Setup 2D matrix in range 0..1, so nobody needs to care about sized
        GL.LoadPixelMatrix(0, 1, 1, 0);

        //Then clear & draw the texture to fill the entire RTT.
        GL.Clear(true, true, new Color(0, 0, 0, 0));
        Graphics.DrawTexture(new Rect(0, 0, 1, 1), tex);

        // Update new texture
        tex.Resize(width, height);
        tex.ReadPixels(texR, 0, 0, true);
        tex.Apply(true);
        Destroy(rtt);
    }

    void ExtractAndApplyPar(bool lutApply) 
    {
        foreach (ParOptions o in listParOptions)
        {
            Color colorPar = cisFrame.GetPixel(o.pointX, o.pointY);
            if (lutApply) {
                colorPar.r *= 0.72f;
                colorPar.b *= 0.5f;
                Color parCorrected = parCalib.applyInverseCorrection(colorPar);
                colorPar = parCorrected;
            }

            Light light = o.par.GetComponent<Light>();
            light.color = colorPar;

            if (!lutApply)
            {
                float intensity = colorPar.a;
                intensity *= 8.0f;
                light.intensity = intensity;
            }
        }
    }

    void ExtractAndApplyBack(bool lutApply)
    {
        foreach (BackLightOptions o in listBackOptions)
        {
            Color colorBack = cisFrame.GetPixel(o.pointX, o.pointY);
            //bool drawBackColor = false;
            //if (colorBack.r != 0 || colorBack.g != 0 || colorBack.b != 0) {
            //    drawBackColor = true;
            //}

            //if (drawBackColor) {Debug.Log("RAW Back color : " + colorBack.ToString());}
            if (lutApply) {
                colorBack.r *= 0.7615f;
                colorBack.b *= 0.2811f;

                //if (drawBackColor) {Debug.Log("Bias correction Back color : " + colorBack.ToString());}
                Color backCorrected = backCalib.applyInverseCorrection(colorBack);
                colorBack = backCorrected;

                //if (drawBackColor) {Debug.Log("Final correction Back color : " + colorBack.ToString());}
            }
            Light light = o.backLed.GetComponent<Light>();
            light.color = colorBack;
        }
    }

    void ExtractAndApplyMoving(bool lutApply) 
    {
        foreach (MovingHeadOptions o in listMovingsOptions)
        {
            Color colorMoving = cisFrame.GetPixel(o.pointXColor, o.pointYColor);
            if (lutApply) {
                colorMoving.r *= 0.72f;
                colorMoving.b *= 0.5f;
                Color mhCorrected = mhCalib.applyInverseCorrection(colorMoving);
                colorMoving = mhCorrected;
            }

            Color colorPan = cisFrame.GetPixel(o.pointXPan, o.pointYPan);
            float panPerfect = (float)((int)(colorPan.g*255.0f) << 8) + (colorPan.b*255.0f);
            panPerfect *= (360.0f / 65535.0f);
            panPerfect -= 180.0f;
            panPerfect *= Mathf.PI/180.0f;

            Color colorTilt = cisFrame.GetPixel(o.pointXTilt, o.pointYTilt);
            float tiltPerfect = (float)((int)(colorTilt.g*255.0f) << 8) + (colorTilt.b*255.0f);
            tiltPerfect *= (180.0f / 65535.0f);
            tiltPerfect -= 90.0f;
            tiltPerfect *= Mathf.PI/180.0f;

            Vector3 pointer = new Vector3(0, 0, 0);
            pointer.x = Mathf.Sin(tiltPerfect)*Mathf.Sin(-panPerfect);
            pointer.y = -Mathf.Sin(tiltPerfect)*Mathf.Cos(panPerfect);
            pointer.z = -Mathf.Cos(tiltPerfect);

            float ratioOnGround = -0.5f/pointer.z;
            pointer *= ratioOnGround;
            pointer += new Vector3(0.5f, 0.5f, 0.5f);

            if (pointer.x < 0 && pointer.x > -0.00003) {
                pointer.x = 0.0f;
            }
            if (pointer.y < 0 && pointer.y > -0.00003) {
                pointer.y = 0.0f;
            }

            Light light = o.objectMoving.GetComponent<Light>();


            if (pointer.x >= 0.0f && pointer.y >= 0.0f) {
                
                Vector3 finalPointer = new Vector3(0.0f, 0.0f, 0.0f); 

                finalPointer.x = (o.max.x - o.min.x) * pointer.x + o.min.x;
                finalPointer.z = (o.max.z - o.min.z) * pointer.y + o.min.z;
                finalPointer.y = (o.max.y - o.min.y) * pointer.y + o.min.y;
                finalPointer.x *= -1.0f;

                Vector3 vectorDirection = finalPointer - o.objectMoving.transform.position;
                o.objectMoving.transform.localRotation = Quaternion.FromToRotation(new Vector3(0.0f, 0.0f, 1.0f), vectorDirection);
                light.color = colorMoving;

            }
            else {
                light.color= new Color(0.0f, 0.0f, 0.0f, 1.0f);
            }

            if (!lutApply)
            {
                float intensity = colorMoving.a;
                intensity *= 8.0f;
                light.intensity = intensity;
            }
        }
    }

}
