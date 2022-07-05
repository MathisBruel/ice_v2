using System.Collections;
using System.Collections.Generic;
using System.Xml;
using System.Xml.Serialization;
using System.Globalization;
using System.IO;
using UnityEngine;

namespace LoaderRoom
{
    public class LoadRoomConf
    {
        public struct ScreenConf
        {
            public float width;
            public float height;
            public float offsetX;
            public float offsetY;
            public float offsetZ;
        };

        public struct AuditoriumConf
        {
            public float ambientLight;
            public float width;
            public float height;
            public float length;
            public Vector3 seatMin;
            public Vector3 seatMax;
            public bool hasLogo;
            public Vector3 scaleFront;
        };

        public struct TieringConf
        {
            public float posZ;
            public float scaleY;
            public float scaleZ;
        };

        public struct PanelConf
        {
            public Vector3 position;
            public Vector3 scale;
        };

        public struct CameraConf
        {
            public Vector3 position;
            public Vector3 rotation;
        };

        public struct ParsConf
        {
            public Vector3 position;
        };



        public CameraConf cameraConf;
        public ScreenConf screenConf;
        public AuditoriumConf audiConf;
        public List<PanelConf> leftPanelsConf;
        public List<TieringConf> tieringConf;
        public List<PanelConf> rightPanelsConf;
        public List<ParsConf> leftParsConf;
        public List<ParsConf> rightParsConf;
        public List<Vector3> leftBackConf;
        public List<Vector3> rightBackConf;
        public List<Vector3> leftMovingConf;
        public List<Vector3> leftMovingMinConf;
        public List<Vector3> leftMovingMaxConf;
        public List<Vector3> rightMovingConf;
        public List<Vector3> rightMovingMinConf;
        public List<Vector3> rightMovingMaxConf;
        public List<Vector3> seatsConf;

        public bool isExploitation;
        public bool isCreative;
        public float angleBackLeft;
        public float angleBackRight;
        public float backIntensityLeft;
        public float backIntensityRight;
        public float parIntensityLeft;
        public float parIntensityRight;
        public float movingIntensityLeft;
        public float movingIntensityRight;

        public LoadRoomConf()
        {
            isExploitation = false;
            isCreative = false;

            cameraConf = new CameraConf();
            screenConf = new ScreenConf();
            audiConf = new AuditoriumConf();
            tieringConf = new List<TieringConf>();
            leftPanelsConf = new List<PanelConf>();
            rightPanelsConf = new List<PanelConf>();
            leftParsConf = new List<ParsConf>();
            rightParsConf = new List<ParsConf>();
            leftBackConf = new List<Vector3>();
            rightBackConf = new List<Vector3>();
            leftMovingConf = new List<Vector3>();
            leftMovingMinConf = new List<Vector3>();
            leftMovingMaxConf = new List<Vector3>();
            rightMovingConf = new List<Vector3>();
            rightMovingMinConf = new List<Vector3>();
            rightMovingMaxConf = new List<Vector3>();
            seatsConf = new List<Vector3>();

            XmlDocument xmlDoc = new XmlDocument();
            xmlDoc.PreserveWhitespace = true;
            xmlDoc.Load("Room.xml");

            // -- define mode creative or not
            XmlNode nodeCreative = xmlDoc.SelectSingleNode("//Room/Creative");
            if (nodeCreative != null) {
                isCreative = nodeCreative.InnerText == "true";
            }

            // -- define mode creative or not
            XmlNode nodeExploitation = xmlDoc.SelectSingleNode("//Room/Exploitation");
            if (nodeExploitation != null) {
                isExploitation = nodeExploitation.InnerText == "true";
            }

            // -- load camera
            string xmlCameraPattern = "//Room/Camera";
            XmlNodeList nodeCamera = xmlDoc.SelectNodes(xmlCameraPattern);
            cameraConf.position = new Vector3(0.0f, 0.0f, 0.0f);
            cameraConf.rotation = new Vector3(0.0f, 0.0f, 0.0f);
            foreach (XmlNode node in nodeCamera)
            {
                for (int i = 0; i < node.ChildNodes.Count; i++)
                {
                    if (node.ChildNodes[i].Name == "posX") {
                        cameraConf.position.x = -float.Parse(node.ChildNodes[i].InnerText, CultureInfo.InvariantCulture);
                    }
                    else if (node.ChildNodes[i].Name == "posY") {
                        cameraConf.position.y = float.Parse(node.ChildNodes[i].InnerText, CultureInfo.InvariantCulture);
                    }
                    else if (node.ChildNodes[i].Name == "posZ") {
                        cameraConf.position.z = float.Parse(node.ChildNodes[i].InnerText, CultureInfo.InvariantCulture);
                    }
                    else if (node.ChildNodes[i].Name == "rotX") {
                        cameraConf.rotation.x = float.Parse(node.ChildNodes[i].InnerText, CultureInfo.InvariantCulture);
                    }
                    else if (node.ChildNodes[i].Name == "rotY") {
                        cameraConf.rotation.y = float.Parse(node.ChildNodes[i].InnerText, CultureInfo.InvariantCulture);
                    }
                    else if (node.ChildNodes[i].Name == "rotZ") {
                        cameraConf.rotation.z = float.Parse(node.ChildNodes[i].InnerText, CultureInfo.InvariantCulture);
                    }
                }
            }

            // -- load screen
            string xmlScreenPattern = "//Room/Screen";
            XmlNodeList nodeScreen = xmlDoc.SelectNodes(xmlScreenPattern);
            foreach (XmlNode node in nodeScreen)
            {
                for (int i = 0; i < node.ChildNodes.Count; i++)
                {
                    if (node.ChildNodes[i].Name == "Width") {
                        screenConf.width = float.Parse(node.ChildNodes[i].InnerText, CultureInfo.InvariantCulture);
                    }
                    else if (node.ChildNodes[i].Name == "Height") {
                        screenConf.height = float.Parse(node.ChildNodes[i].InnerText, CultureInfo.InvariantCulture);
                    }
                    else if (node.ChildNodes[i].Name == "OffsetX") {
                        screenConf.offsetX = -float.Parse(node.ChildNodes[i].InnerText, CultureInfo.InvariantCulture);
                    }
                    else if (node.ChildNodes[i].Name == "OffsetY") {
                        screenConf.offsetY = float.Parse(node.ChildNodes[i].InnerText, CultureInfo.InvariantCulture);
                    }
                    else if (node.ChildNodes[i].Name == "OffsetZ") {
                        screenConf.offsetZ = float.Parse(node.ChildNodes[i].InnerText, CultureInfo.InvariantCulture);
                    }
                }
            }

            // -- load auditorium
            string xmlAuditoriumPattern = "//Room/Auditorium";
            XmlNodeList nodeAudi = xmlDoc.SelectNodes(xmlAuditoriumPattern);
            foreach (XmlNode node in nodeAudi)
            {
                for (int i = 0; i < node.ChildNodes.Count; i++)
                {
                    if (node.ChildNodes[i].Name == "Width") {
                        audiConf.width = float.Parse(node.ChildNodes[i].InnerText, CultureInfo.InvariantCulture);
                    }
                    else if (node.ChildNodes[i].Name == "AmbientLight") {
                        audiConf.ambientLight = float.Parse(node.ChildNodes[i].InnerText, CultureInfo.InvariantCulture);
                    }
                    else if (node.ChildNodes[i].Name == "Height") {
                        audiConf.height = float.Parse(node.ChildNodes[i].InnerText, CultureInfo.InvariantCulture);
                    }
                    else if (node.ChildNodes[i].Name == "Length") {
                        audiConf.length = float.Parse(node.ChildNodes[i].InnerText, CultureInfo.InvariantCulture);
                    }
                    else if (node.ChildNodes[i].Name == "SeatMin") 
                    {
                        audiConf.seatMin = new Vector3(0.0f, 0.0f, 0.0f);
                        for (int j = 0; j < node.ChildNodes[i].ChildNodes.Count; j++)
                        {
                            if (node.ChildNodes[i].ChildNodes[j].Name == "x") {
                                audiConf.seatMin.x = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "y") {
                                audiConf.seatMin.y = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "z") {
                                audiConf.seatMin.z = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                        }
                    }
                    else if (node.ChildNodes[i].Name == "SeatMax") 
                    {
                        audiConf.seatMax = new Vector3(0.0f, 0.0f, 0.0f);
                        for (int j = 0; j < node.ChildNodes[i].ChildNodes.Count; j++)
                        {
                            if (node.ChildNodes[i].ChildNodes[j].Name == "x") {
                                audiConf.seatMax.x = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "y") {
                                audiConf.seatMax.y = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "z") {
                                audiConf.seatMax.z = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                        }
                    }

                    else if (node.ChildNodes[i].Name == "Tiering") 
                    {
                        TieringConf tier = new TieringConf();
                        for (int j = 0; j < node.ChildNodes[i].ChildNodes.Count; j++)
                        {
                            if (node.ChildNodes[i].ChildNodes[j].Name == "PosZ") {
                                tier.posZ = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "ScaleY") {
                                tier.scaleY = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "ScaleZ") {
                                tier.scaleZ = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                        }
                        tieringConf.Add(tier);
                    }
                    else if (node.ChildNodes[i].Name == "FrontScreen") {
                        audiConf.scaleFront = new Vector3();

                        for (int j = 0; j < node.ChildNodes[i].ChildNodes.Count; j++)
                        {
                            if (node.ChildNodes[i].ChildNodes[j].Name == "ScaleX") {
                                audiConf.scaleFront.x = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "ScaleY") {
                                audiConf.scaleFront.y = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "ScaleZ") {
                                audiConf.scaleFront.z = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "HasLogo") {
                                audiConf.hasLogo = node.ChildNodes[i].ChildNodes[j].InnerText == "true";
                            }
                        }
                    }
                }
            }

            // -- load left panels
            string xmlLeftPanelsPattern = "//Room/LeftPanels";
            XmlNodeList nodeLeftPanels = xmlDoc.SelectNodes(xmlLeftPanelsPattern);
            foreach (XmlNode node in nodeLeftPanels)
            {
                for (int i = 0; i < node.ChildNodes.Count; i++)
                {
                    if (node.ChildNodes[i].Name == "Panel") {
                        PanelConf panel = new PanelConf();
                        panel.position = new Vector3(0.0f, 0.0f, 0.0f);
                        panel.scale = new Vector3(0.0f, 0.0f, 0.0f);
                        for (int j = 0; j < node.ChildNodes[i].ChildNodes.Count; j++)
                        {
                            if (node.ChildNodes[i].ChildNodes[j].Name == "PosX") {
                                panel.position.x = -float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "PosY") {
                                panel.position.y = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "PosZ") {
                                panel.position.z = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "ScaleX") {
                                panel.scale.x = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "ScaleY") {
                                panel.scale.y = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "ScaleZ") {
                                panel.scale.z = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                        }

                        leftPanelsConf.Add(panel);
                    }
                }
            }

            // -- load right panels
            string xmlRightPanelsPattern = "//Room/RightPanels";
            XmlNodeList nodeRightPanels = xmlDoc.SelectNodes(xmlRightPanelsPattern);
            foreach (XmlNode node in nodeRightPanels)
            {
                for (int i = 0; i < node.ChildNodes.Count; i++)
                {
                    if (node.ChildNodes[i].Name == "Panel") 
                    {
                        PanelConf panel = new PanelConf();
                        panel.position = new Vector3(0.0f, 0.0f, 0.0f);
                        panel.scale = new Vector3(0.0f, 0.0f, 0.0f);
                        for (int j = 0; j < node.ChildNodes[i].ChildNodes.Count; j++)
                        {
                            if (node.ChildNodes[i].ChildNodes[j].Name == "PosX") {
                                panel.position.x = -float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "PosY") {
                                panel.position.y = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "PosZ") {
                                panel.position.z = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "ScaleX") {
                                panel.scale.x = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "ScaleY") {
                                panel.scale.y = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "ScaleZ") {
                                panel.scale.z = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                        }

                        rightPanelsConf.Add(panel);
                    }
                }
            }

            // -- load left pars
            XmlNode nodeIntensityParLeft = xmlDoc.SelectSingleNode("//Room/LeftPars/Intensity");
            if (nodeIntensityParLeft != null) {
                parIntensityLeft = float.Parse(nodeIntensityParLeft.InnerText, CultureInfo.InvariantCulture);
            }
            string xmlLeftParsPattern = "//Room/LeftPars";
            XmlNodeList nodeLeftPars = xmlDoc.SelectNodes(xmlLeftParsPattern);
            foreach (XmlNode node in nodeLeftPars)
            {
                for (int i = 0; i < node.ChildNodes.Count; i++)
                {
                    if (node.ChildNodes[i].Name == "PAR") {
                        ParsConf par = new ParsConf();
                        par.position = new Vector3(0.0f, 0.0f, 0.0f);
                        for (int j = 0; j < node.ChildNodes[i].ChildNodes.Count; j++)
                        {
                            if (node.ChildNodes[i].ChildNodes[j].Name == "PosX") {
                                par.position.x = -float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "PosY") {
                                par.position.y = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "PosZ") {
                                par.position.z = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                        }

                        leftParsConf.Add(par);
                    }
                }
            }

            // -- load right pars
            XmlNode nodeIntensityParRight = xmlDoc.SelectSingleNode("//Room/RightPars/Intensity");
            if (nodeIntensityParRight != null) {
                parIntensityRight = float.Parse(nodeIntensityParRight.InnerText, CultureInfo.InvariantCulture);
            }
            string xmlRightParsPattern = "//Room/RightPars";
            XmlNodeList nodeRightPars = xmlDoc.SelectNodes(xmlRightParsPattern);
            foreach (XmlNode node in nodeRightPars)
            {
                for (int i = 0; i < node.ChildNodes.Count; i++)
                {
                    if (node.ChildNodes[i].Name == "PAR") {
                        ParsConf par = new ParsConf();
                        par.position = new Vector3(0.0f, 0.0f, 0.0f);
                        for (int j = 0; j < node.ChildNodes[i].ChildNodes.Count; j++)
                        {
                            if (node.ChildNodes[i].ChildNodes[j].Name == "PosX") {
                                par.position.x = -float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "PosY") {
                                par.position.y = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "PosZ") {
                                par.position.z = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                        }

                        rightParsConf.Add(par);
                    }
                }
            }

            // -- load left back
            XmlNode nodeIntensityBackLeft = xmlDoc.SelectSingleNode("//Room/LeftBack/Intensity");
            if (nodeIntensityBackLeft != null) {
                backIntensityLeft = float.Parse(nodeIntensityBackLeft.InnerText, CultureInfo.InvariantCulture);
            }
            XmlNode nodeAngleLeft = xmlDoc.SelectSingleNode("//Room/LeftBack/Angle");
            if (nodeAngleLeft != null) {
                angleBackLeft = float.Parse(nodeAngleLeft.InnerText, CultureInfo.InvariantCulture);
            }
            string xmlLeftBackPattern = "//Room/LeftBack";
            XmlNodeList nodeLeftBack = xmlDoc.SelectNodes(xmlLeftBackPattern);
            foreach (XmlNode node in nodeLeftBack)
            {
                for (int i = 0; i < node.ChildNodes.Count; i++)
                {
                    if (node.ChildNodes[i].Name == "Backlight") {
                        Vector3 position = new Vector3(0.0f, 0.0f, 0.0f);
                        for (int j = 0; j < node.ChildNodes[i].ChildNodes.Count; j++)
                        {
                            if (node.ChildNodes[i].ChildNodes[j].Name == "PosX") {
                                position.x = -float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "PosY") {
                                position.y = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "PosZ") {
                                position.z = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                        }

                        leftBackConf.Add(position);
                    }
                }
            }

            // -- load right back
            XmlNode nodeIntensityBackRight = xmlDoc.SelectSingleNode("//Room/RightBack/Intensity");
            if (nodeIntensityBackRight != null) {
                backIntensityRight = float.Parse(nodeIntensityBackRight.InnerText, CultureInfo.InvariantCulture);
            }
            XmlNode nodeAngleRight = xmlDoc.SelectSingleNode("//Room/RightBack/Angle");
            if (nodeAngleRight != null) {
                angleBackRight = float.Parse(nodeAngleRight.InnerText, CultureInfo.InvariantCulture);
            }

            string xmlRightBackPattern = "//Room/RightBack";
            XmlNodeList nodeRightBack = xmlDoc.SelectNodes(xmlRightBackPattern);
            foreach (XmlNode node in nodeRightBack)
            {
                for (int i = 0; i < node.ChildNodes.Count; i++)
                {
                    if (node.ChildNodes[i].Name == "Backlight") {
                        Vector3 position = new Vector3(0.0f, 0.0f, 0.0f);
                        for (int j = 0; j < node.ChildNodes[i].ChildNodes.Count; j++)
                        {
                            if (node.ChildNodes[i].ChildNodes[j].Name == "PosX") {
                                position.x = -float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "PosY") {
                                position.y = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "PosZ") {
                                position.z = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                        }

                        rightBackConf.Add(position);
                    }
                }
            }

            // -- load left movings
            XmlNode nodeIntensityMovingLeft = xmlDoc.SelectSingleNode("//Room/LeftMovings/Intensity");
            if (nodeIntensityMovingLeft != null) {
                movingIntensityLeft = float.Parse(nodeIntensityMovingLeft.InnerText, CultureInfo.InvariantCulture);
            }
            string xmlLeftMovingPattern = "//Room/LeftMovings";
            XmlNodeList nodeLeftMoving = xmlDoc.SelectNodes(xmlLeftMovingPattern);
            foreach (XmlNode node in nodeLeftMoving)
            {
                for (int i = 0; i < node.ChildNodes.Count; i++)
                {
                    if (node.ChildNodes[i].Name == "Moving") {
                        Vector3 position = new Vector3(0.0f, 0.0f, 0.0f);
                        Vector3 min = new Vector3(0.0f, 0.0f, 0.0f);
                        Vector3 max = new Vector3(0.0f, 0.0f, 0.0f);
                        for (int j = 0; j < node.ChildNodes[i].ChildNodes.Count; j++)
                        {
                            if (node.ChildNodes[i].ChildNodes[j].Name == "PosX") {
                                position.x = -float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "PosY") {
                                position.y = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "PosZ") {
                                position.z = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "MinX") {
                                min.x = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "MinY") {
                                min.y = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "MinZ") {
                                min.z = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "MaxX") {
                                max.x = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "MaxY") {
                                max.y = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "MaxZ") {
                                max.z = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                        }

                        leftMovingConf.Add(position);
                        leftMovingMinConf.Add(min);
                        leftMovingMaxConf.Add(max);
                    }
                }
            }

            // -- load right movings
            XmlNode nodeIntensityMovingRight = xmlDoc.SelectSingleNode("//Room/RightMovings/Intensity");
            if (nodeIntensityMovingRight != null) {
                movingIntensityRight = float.Parse(nodeIntensityMovingRight.InnerText, CultureInfo.InvariantCulture);
            }
            string xmlRightMovingPattern = "//Room/RightMovings";
            XmlNodeList nodeRightMoving = xmlDoc.SelectNodes(xmlRightMovingPattern);
            foreach (XmlNode node in nodeRightMoving)
            {
                for (int i = 0; i < node.ChildNodes.Count; i++)
                {
                    if (node.ChildNodes[i].Name == "Moving") {
                        Vector3 position = new Vector3(0.0f, 0.0f, 0.0f);
                        Vector3 min = new Vector3(0.0f, 0.0f, 0.0f);
                        Vector3 max = new Vector3(0.0f, 0.0f, 0.0f);
                        for (int j = 0; j < node.ChildNodes[i].ChildNodes.Count; j++)
                        {
                            if (node.ChildNodes[i].ChildNodes[j].Name == "PosX") {
                                position.x = -float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "PosY") {
                                position.y = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "PosZ") {
                                position.z = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "MinX") {
                                min.x = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "MinY") {
                                min.y = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "MinZ") {
                                min.z = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "MaxX") {
                                max.x = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "MaxY") {
                                max.y = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "MaxZ") {
                                max.z = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                        }

                        rightMovingConf.Add(position);
                        rightMovingMinConf.Add(min);
                        rightMovingMaxConf.Add(max);
                    }
                }
            }

            // -- load seats
            string xmlSeatsPattern = "//Room/Seats";
            XmlNodeList nodeSeats = xmlDoc.SelectNodes(xmlSeatsPattern);
            foreach (XmlNode node in nodeSeats)
            {
                for (int i = 0; i < node.ChildNodes.Count; i++)
                {
                    if (node.ChildNodes[i].Name == "Seat") {
                        Vector3 position = new Vector3(0.0f, 0.0f, 0.0f);
                        for (int j = 0; j < node.ChildNodes[i].ChildNodes.Count; j++)
                        {
                            if (node.ChildNodes[i].ChildNodes[j].Name == "PosX") {
                                position.x = -float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "PosY") {
                                position.y = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                            else if (node.ChildNodes[i].ChildNodes[j].Name == "PosZ") {
                                position.z = float.Parse(node.ChildNodes[i].ChildNodes[j].InnerText, CultureInfo.InvariantCulture);
                            }
                        }

                        seatsConf.Add(position);
                    }
                }
            }
        }
    }
}
