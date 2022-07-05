using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using System.IO;
using System.Linq;

namespace Cis
{

    public class CisReader
    {
        const int FRAME_COUNT_OFFSET = 4; // offset to the framecount in the index header in bytes
        const int FRAME_RATE_OFFSET = FRAME_COUNT_OFFSET + 4; // offset to the framecount in the index header in bytes
        const int FRAME_WIDTH_OFFSET = FRAME_RATE_OFFSET + 4; // offset to the framecount in the index header in bytes
        const int FRAME_HEIGHT_OFFSET = FRAME_WIDTH_OFFSET + 4; // offset to the framecount in the index header in bytes
        const int BPP_OFFSET = FRAME_HEIGHT_OFFSET + 4; // offset to the framecount in the index header in bytes

        public FileInfo CisFile
        {
            get;
            set;
        }

        public byte[] FrameData
        {
            get;
            set;
        }

        public List<long> Index
        {
            get;
            set;
        }

        public CisReader(string cisPath)
        {
            CisFile = new FileInfo(cisPath);
            Index = new List<long>();
            Update();
        }

        public void Update()
        {
            if (CisFile.Exists)
            {

                FileStream file = CisFile.OpenRead();
                try
                {
                    ReadIndex(file, this);
                }
                finally
                {
                    file.Close();
                }
            }
            else
            {
                Debug.LogError("Path file not existing");
            }
        }

        public byte[] GetFrame(int frameNr)
        {
            // -- don't do the last frame
            if (frameNr >= 0 && frameNr < frameCount - 1 && frameNr < Index.Count - 1)
            {
                if (CisFile.Exists)
                {
                    FileStream file = CisFile.OpenRead();
                    try
                    {
                        long frameAddress = this.Index[frameNr];
                        int size = (int)(this.Index[frameNr + 1] - frameAddress);
                        file.Seek(frameAddress, SeekOrigin.Begin);
                        byte[] frame = new byte[size];
                        file.Read(frame, 0, size);
                        return frame;
                    }
                    finally
                    {
                        file.Close();
                    }
                }
            }
            else
            {
                Debug.Log("Frame nb is outside of range : " + frameNr + " -> " + Index.Count);
            }
            return null;
        }

        public bool lutApply { get; set; }

        public int bpp { get; set; }

        public int frameHeight { get; set; }

        public int frameWidth { get; set; }

        public string frameRate { get; set; }

        public int frameCount { get; set; }

        public string Title { get; set; }

        public static void ReadIndex(FileStream file, CisReader item)
        {
            item.Index.Clear();
            long endOfFrameOffset = file.Length - 16;
            long headerOffset = read64Bits(file, endOfFrameOffset);
            string magic = readMagicString(file, endOfFrameOffset + 8);
            Debug.Log("Magic : " + magic);
            long headerStart = file.Length - headerOffset;
            int headerLength = read32Bits(file, headerStart);
            int rate = read32Bits(file, headerStart + FRAME_RATE_OFFSET); // TODO use the proper enum to get the rate
            item.frameCount = read32Bits(file, headerStart + FRAME_COUNT_OFFSET);
            item.frameWidth = read32Bits(file, headerStart + FRAME_WIDTH_OFFSET);
            item.frameHeight = read32Bits(file, headerStart + FRAME_HEIGHT_OFFSET);
            item.bpp = read32Bits(file, headerStart + BPP_OFFSET);
            if (magic == "CIS-V001") {item.lutApply = true;}
            else {item.lutApply = false;}
            

            Debug.Log("Nb frame : " + item.frameCount);
            Debug.Log("Frame width : " + item.frameWidth);
            Debug.Log("Frame height : " + item.frameHeight);
            Debug.Log("BPP : " + item.bpp);

            for (long pos = headerStart + headerLength; pos < endOfFrameOffset; pos += 8)
            {
                long framePos = read64Bits(file, pos);
                item.Index.Add(framePos);
            }
        }


        private static int read32Bits(FileStream file, long position)
        {
            byte[] dataArray = { 0, 0, 0, 0 };
            file.Seek(position, SeekOrigin.Begin);
            file.Read(dataArray, 0, 4);

            if (BitConverter.IsLittleEndian)
            {
                Array.Reverse(dataArray);
            }
            return BitConverter.ToInt32(dataArray, 0);
        }

        private static long read64Bits(FileStream file, long position)
        {
            file.Seek(position, SeekOrigin.Begin);
            return read64Bits(file);
        }

        private static string readMagicString(FileStream file, long position)
        {
            byte[] dataArray = { 0, 0, 0, 0, 0, 0, 0, 0 };
            file.Seek(position, SeekOrigin.Begin);
            file.Read(dataArray, 0, 8);

            string magic = "";
            for (int i = 0; i < 8; i++)
            {
                magic += (char)dataArray[i];
            }

            return magic;
        }

        private static long read64Bits(FileStream file)
        {
            byte[] dataArray = { 0, 0, 0, 0, 0, 0, 0, 0 };

            file.Read(dataArray, 0, 8);

            if (BitConverter.IsLittleEndian)
            {
                Array.Reverse(dataArray);
            }
            return BitConverter.ToInt64(dataArray, 0);
        }
    }
}
