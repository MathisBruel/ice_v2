using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Globalization;
using System;
using System.IO;
using System.Linq;

namespace Calib
{

    public class Calibrer
    {
        public Calibrer() {
            isFixedPoint = false;
        }
        ~Calibrer() {}

        public bool open(string filename)
        {
            string[] lines = File.ReadAllLines(@filename);
            int currentLine = 0;
            while (currentLine < lines.Count()) {
                if (lines[currentLine].Contains("FIXED")) {
                    currentLine++;
                    string[] values = lines[currentLine].Split(' ');
                    factorRed = float.Parse(values[0], CultureInfo.InvariantCulture)/255.0f;
                    factorGreen = float.Parse(values[1], CultureInfo.InvariantCulture)/255.0f;
                    factorBlue = float.Parse(values[2], CultureInfo.InvariantCulture)/255.0f;
                    isFixedPoint = true;
                }
                else {
                    indexWhite = new float[2];
                    redWhite = new float[2];
                    greenWhite = new float[2];
                    blueWhite = new float[2];

                    int idx = 0;

                    currentLine++;
                    string[] values = lines[currentLine].Split(' ');
                    float white = int.Parse(values[0]);
                    float red = float.Parse(values[1], CultureInfo.InvariantCulture);
                    float green = float.Parse(values[2], CultureInfo.InvariantCulture);
                    float blue = float.Parse(values[3], CultureInfo.InvariantCulture);

                    redWhite[idx] = red;
                    greenWhite[idx] = green;
                    blueWhite[idx] = blue;
                    indexWhite[idx] = white;
                    idx++;

                    currentLine++;
                    string[] values2 = lines[currentLine].Split(' ');
                    float white2 = int.Parse(values2[0]);
                    float red2 = float.Parse(values2[1], CultureInfo.InvariantCulture);
                    float green2 = float.Parse(values2[2], CultureInfo.InvariantCulture);
                    float blue2 = float.Parse(values2[3], CultureInfo.InvariantCulture);

                    redWhite[idx] = red2;
                    greenWhite[idx] = green2;
                    blueWhite[idx] = blue2;
                    indexWhite[idx] = white2;
                    idx++;

                    recalculateAllPoly();
                }
                currentLine++;
            }
            return true;
        }
        public Color applyCorrection(Color src)
        {
            if (isFixedPoint) {
                return applyFixedPointCorrection(src);
            }
            else {
                return applyWhiteBalanceCorrection(src);
            }
        }
        public Color applyInverseCorrection(Color src)
        {
            if (isFixedPoint) {
                return applyInverseFixedPointCorrection(src);
            }
            else {
                return applyInverseWhiteBalanceCorrection(src);
            }
        }

        private Color applyFixedPointCorrection(Color src)
        {
            Color dst = new Color();
            float red = src.r*factorRed;
            float green = src.g*factorGreen;
            float blue = src.b*factorBlue;
            dst.r = red;
            dst.g = green;
            dst.b = blue;
            return dst;
        }
        private Color applyWhiteBalanceCorrection(Color src)
        {
            Color dst = new Color();
            int redRef = (int)(src.r*255.0+0.5);
            int greenRef = (int)(src.g*255.0+0.5);
            int blueRef = (int)(src.b*255.0+0.5);

            // -- extrapolate following curve to get factors
            float redFinalFactor = redRef*redRef*polyRed[0] + redRef*polyRed[1];
            float greenFinalFactor = greenRef*greenRef*polyGreen[0] + greenRef*polyGreen[1];
            float blueFinalFactor = blueRef*blueRef*polyBlue[0] + blueRef*polyBlue[1];

            // -- apply correct RGB mixing
            redRef = (int)(redRef*redFinalFactor);
            greenRef = (int)(greenRef*greenFinalFactor);
            blueRef = (int)(blueRef*blueFinalFactor);

            dst.r = redRef/255.0f;
            dst.g = greenRef/255.0f;
            dst.b = blueRef/255.0f;
            return dst;
        }
        private Color applyInverseFixedPointCorrection(Color src)
        {
            Color dst = new Color();
            float red = src.r/factorRed;
            float green = src.g/factorGreen;
            float blue = src.b/factorBlue;
            dst.r = red;
            dst.g = green;
            dst.b = blue;
            return dst;
        }
        private Color applyInverseWhiteBalanceCorrection(Color src)
        {
            Color dst = new Color();
            int redRef = (int)(src.r*255.0+0.5);
            int greenRef = (int)(src.g*255.0+0.5);
            int blueRef = (int)(src.b*255.0+0.5);
            
            // -- calculate RED
            float deltaRed = polyRed[1]*polyRed[1] + 4.0f*redRef*polyRed[0];
            float redFinal = ((float)(Math.Sqrt(deltaRed)) - polyRed[1])/(2.0f*polyRed[0]);

            // -- calculate GREEN
            float deltaGreen = polyGreen[1]*polyGreen[1] + 4.0f*greenRef*polyGreen[0];
            float greenFinal = ((float)(Math.Sqrt(deltaGreen)) - polyGreen[1])/(2.0f*polyGreen[0]);

            // -- calculate BLUE
            float deltaBlue = polyBlue[1]*polyBlue[1] + 4.0f*blueRef*polyBlue[0];
            float blueFinal = ((float)(Math.Sqrt(deltaBlue)) - polyBlue[1])/(2.0f*polyBlue[0]);

            dst.r = redFinal/255.0f;
            dst.g = greenFinal/255.0f;
            dst.b = blueFinal/255.0f;
            return dst;
        }

        private void recalculateAllPoly()
        {
            // -- determine polynome for red
            float GRed = (indexWhite[0]*indexWhite[0] - (redWhite[0]/redWhite[1])*indexWhite[1]*indexWhite[1]) / ((redWhite[0]/redWhite[1])*indexWhite[1] - indexWhite[0]);
            polyRed = new float[2];
            polyRed[0] = redWhite[0] / (indexWhite[0]*indexWhite[0] + indexWhite[0]*GRed);
            polyRed[1] = polyRed[0] * GRed;

            // -- determine polynome for green
            float GGreen = (indexWhite[0]*indexWhite[0] - (greenWhite[0]/greenWhite[1])*indexWhite[1]*indexWhite[1]) / ((greenWhite[0]/greenWhite[1])*indexWhite[1] - indexWhite[0]);
            polyGreen = new float[2];
            polyGreen[0] = greenWhite[0] / (indexWhite[0]*indexWhite[0] + indexWhite[0]*GGreen);
            polyGreen[1] = polyGreen[0] * GGreen;

            // -- determine polynome for blue
            float GBlue = (indexWhite[0]*indexWhite[0] - (blueWhite[0]/blueWhite[1])*indexWhite[1]*indexWhite[1]) / ((blueWhite[0]/blueWhite[1])*indexWhite[1] - indexWhite[0]);
            polyBlue = new float[2];
            polyBlue[0] = blueWhite[0] / (indexWhite[0]*indexWhite[0] + indexWhite[0]*GBlue);
            polyBlue[1] = polyBlue[0] * GBlue;
        }

        // -- is fixed factor or RGBW correction
        private bool isFixedPoint;

        // -- for fixed point correction
        private float factorRed;
        private float factorGreen;
        private float factorBlue;

        // -- for white balance correction
        private float[] indexWhite;
        private float[] redWhite;
        private float[] greenWhite;
        private float[] blueWhite;

        // -- polynome for calculation (size 2 : c = 0)
        private float[] polyRed;
        private float[] polyGreen;
        private float[] polyBlue;
    }
}
