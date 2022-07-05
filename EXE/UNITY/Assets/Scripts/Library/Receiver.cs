using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Net.Sockets;
using System.Threading;
using System;
using System.Net;

namespace TCP
{

    public class Receiver
    {
        public Mutex mutex;

        private string host;
        private int port;
        public int frameNumber;
        public bool lutApply;

        private bool stop;

        public Color[] pixels;

        private NetworkStream stream;
        private TcpClient connectedTcpClient; 
        private TcpListener tcpListener; 
        private Thread clientReceiveThread;

        public Receiver(string hostParam, int portParam)
        {
            mutex = new Mutex();
            stream = null;
            connectedTcpClient = null;
            lutApply = true;
            frameNumber = -1;
            host = hostParam;
            port = portParam;
            pixels = new Color[720*576];
            stop = false;
            try {  			
                clientReceiveThread = new Thread (new ThreadStart(ListenForData)); 			
                clientReceiveThread.IsBackground = true; 			
                clientReceiveThread.Start();  		
            } 		
            catch (Exception e) { 			
                Debug.Log("On client connect exception " + e); 		
            }
        }

        public void Delete()
        {
            stop = true;
            clientReceiveThread.Join();
            if (connectedTcpClient != null) {
                stream.Close();
                connectedTcpClient.Close();
            }
            tcpListener.Stop();
        }


        private uint swapInt(uint toSwap) {
            uint result = 0;
            result = toSwap >> 24;
            result = result | ((toSwap & 0xff0000) >> 8);
            result = result | ((toSwap & 0xff00) << 8);
            result = result | ((toSwap & 0xff) << 24);
            return result;
        }

        private void ListenForData() {
            try {
                tcpListener = new TcpListener(IPAddress.Parse(host), port); 			
			    tcpListener.Start(); 			

                Byte[] bytes = new Byte[2000000];
                while (!stop) {

                    // -- force deconnexion
                    if (connectedTcpClient != null && !connectedTcpClient.Connected) {

                        if (stream != null) {
                            stream.Close();
                            stream = null;
                        }
                        connectedTcpClient.Close();
                        connectedTcpClient = null;
                    }
                    
                    if (tcpListener.Pending()) {
                        
                        if (connectedTcpClient != null) {
                            connectedTcpClient.Close();
                            connectedTcpClient = null;
                        }
                        if (stream != null) {
                            stream.Close();
                            stream = null;
                        }
                        connectedTcpClient = tcpListener.AcceptTcpClient();
                        stream = connectedTcpClient.GetStream();
                        connectedTcpClient.ReceiveTimeout = 1000;
                    }

                    // -- condition to skip reading
                    if (connectedTcpClient != null && connectedTcpClient.Connected) {

                        uint magic = 0;

                        // -- read only the magic
                        long startTime = DateTime.Now.Ticks;
                        bool nextLoop = false;
                        while (connectedTcpClient.Available < 1 && !nextLoop) {
                            long deltaTime = DateTime.Now.Ticks - startTime;
                            if (deltaTime > 100000) {
                                nextLoop = true;
                            }
                        }

                        if (nextLoop) {continue;}
                        
                        int bytesReceived = stream.Read(bytes, 0, 1);
                        if (bytes[0] == 0x4e)
                        {
                            startTime = DateTime.Now.Ticks;
                            while (connectedTcpClient.Available < 3 && !nextLoop) {
                                long deltaTime = DateTime.Now.Ticks - startTime;
                                if (deltaTime > 100000) {
                                    nextLoop = true;
                                }
                            }
                            if (nextLoop) {continue;}
                            bytesReceived = stream.Read(bytes, 1, 3);
                            magic = BitConverter.ToUInt32(bytes, 0);
                            magic = swapInt(magic);
                        }
                        else if (bytes[0] == 0xb1)
                        {
                            startTime = DateTime.Now.Ticks;
                            while (connectedTcpClient.Available < 3 && !nextLoop) {
                                long deltaTime = DateTime.Now.Ticks - startTime;
                                if (deltaTime > 100000) {
                                    nextLoop = true;
                                }
                            }
                            if (nextLoop) {continue;}
                            bytesReceived = stream.Read(bytes, 1, 3);
                            magic = BitConverter.ToUInt32(bytes, 0);
                            magic = swapInt(magic);
                        }

                        // -- infos
                        if (magic == 0x4e10b1be) {

                            Debug.Log("Header received");

                            startTime = DateTime.Now.Ticks;
                            while (connectedTcpClient.Available < 4 && !nextLoop) {
                                long deltaTime = DateTime.Now.Ticks - startTime;
                                if (deltaTime > 100000) {
                                    nextLoop = true;
                                }
                            }
                            if (nextLoop) {continue;}
                            bytesReceived = stream.Read(bytes, 0, 4);
                            uint length = BitConverter.ToUInt32(bytes, 0);
                            length = swapInt(length);

                            startTime = DateTime.Now.Ticks;
                            while (connectedTcpClient.Available < length && !nextLoop) {
                                long deltaTime = DateTime.Now.Ticks - startTime;
                                if (deltaTime > 100000) {
                                    nextLoop = true;
                                }
                            }
                            if (nextLoop) {continue;}
                            bytesReceived = stream.Read(bytes, 0, (int)length);
                            uint version = BitConverter.ToUInt32(bytes, 0);
                            version = swapInt(version);
                            uint format = BitConverter.ToUInt32(bytes, 4);
                            format = swapInt(format);

                            if (version == 1) {lutApply = true;}
                            else {lutApply = false;}
                        }

                        // -- image
                        else if (magic == 0xb17eb1be) {

                            Debug.Log("Image received");
                            startTime = DateTime.Now.Ticks;
                            while (connectedTcpClient.Available < 4 && !nextLoop) {
                                long deltaTime = DateTime.Now.Ticks - startTime;
                                if (deltaTime > 100000) {
                                    nextLoop = true;
                                }
                            }
                            if (nextLoop) {continue;}
                            bytesReceived = stream.Read(bytes, 0, 4);
                            uint length = BitConverter.ToUInt32(bytes, 0);
                            length = swapInt(length);

                            int bytesRead = 0;
                            bool skip = false;

                            while (bytesRead < length && !skip) {

                                startTime = 0;
                                int available = connectedTcpClient.Available;
                                if (available > 0) {

                                    if (length - bytesRead < available) {
                                        bytesRead += stream.Read(bytes, bytesRead, (int)length - bytesRead);
                                    }
                                    else {
                                        bytesRead += stream.Read(bytes, bytesRead, available);
                                    }
                                }
                                else {
                                    if (startTime == 0) startTime = DateTime.Now.Ticks;
                                    else {
                                        long deltaTime = DateTime.Now.Ticks - startTime;
                                        if (deltaTime > 1000000) {
                                            skip = true;
                                        }
                                    }
                                }
                            }

                            if (!skip) {
                                while (!mutex.WaitOne(20)) {}
                                for (int i = 0; i < 576; i++) {
                                    for (int j = 0; j < 720; j++) {
                                        pixels[i*720+j].r = (float)bytes[((575-i)*720+j)*4+2] / 255.0f;
                                        pixels[i*720+j].g = (float)bytes[((575-i)*720+j)*4+1] / 255.0f;
                                        pixels[i*720+j].b = (float)bytes[((575-i)*720+j)*4] / 255.0f;
                                        pixels[i*720+j].a = (float)bytes[((575-i)*720+j)*4+3] / 255.0f;
                                    }
                                }
                            }

                            frameNumber++;
                            mutex.ReleaseMutex();
                        }
                    }
                }
            }
            catch (SocketException socketException) {       
                Debug.Log("Socket exception: " + socketException);         
            }
        }
    }
}