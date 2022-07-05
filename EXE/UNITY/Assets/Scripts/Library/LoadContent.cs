using System.Collections;
using System.Collections.Generic;
using System.Xml;
using System.Xml.Serialization;
using System.IO;
using UnityEngine;

namespace Loader
{
    public class LoadContent
    {
        public struct IceContent
        {
            public string name;
            public string pathVideo;
            public string pathIce;
            public bool loop;
            public int startDelay;
            public int entryPoint;
        };

        public struct IcePlaylist
        {
            public string name;
            public List<int> indexes;
        };

        public List<IceContent> contents
        {
            get;
            set;
        }

        public List<IcePlaylist> playlists
        {
            get;
            set;
        }

        private int indexCurrent;
        private int indexCurrentInPlaylist;

        // Start is called before the first frame update
        public LoadContent()
        {
            contents = new List<IceContent>();
            playlists = new List<IcePlaylist>();

            XmlDocument xmlDoc = new XmlDocument();
            xmlDoc.PreserveWhitespace = true;
            xmlDoc.Load("Contents.xml");
            string xmlContentPattern = "//Contents/Content";
            XmlNodeList myNodeList = xmlDoc.SelectNodes(xmlContentPattern);
            foreach (XmlNode node in myNodeList)
            {
                IceContent content = new IceContent();
                for (int i = 0; i < node.ChildNodes.Count; i++)
                {
                    if (node.ChildNodes[i].Name == "Name") {
                        content.name = node.ChildNodes[i].InnerText;
                    }
                    else if (node.ChildNodes[i].Name == "PathVideo") {
                        content.pathVideo = node.ChildNodes[i].InnerText;
                    }
                    else if (node.ChildNodes[i].Name == "PathIce") {
                        content.pathIce = node.ChildNodes[i].InnerText;
                    }
                    else if (node.ChildNodes[i].Name == "Loop") {
                        content.loop = node.ChildNodes[i].InnerText == "true";
                    }
                    else if (node.ChildNodes[i].Name == "StartDelay") {
                        content.startDelay = System.Convert.ToInt32(node.ChildNodes[i].InnerText);
                    }
                    else if (node.ChildNodes[i].Name == "EntryPoint") {
                        content.entryPoint = System.Convert.ToInt32(node.ChildNodes[i].InnerText);
                    }
                }

                contents.Add(content);
                Debug.Log("Name content : " + content.name);
            }

            string xmlPlaylistPattern = "//Contents/Playlist";
            XmlNodeList playlistListNode = xmlDoc.SelectNodes(xmlPlaylistPattern);
            foreach (XmlNode node in playlistListNode)
            {
                IcePlaylist playlist = new IcePlaylist();
                playlist.indexes = new List<int>();
                for (int i = 0; i < node.ChildNodes.Count; i++)
                {
                    if (node.ChildNodes[i].Name == "Name") {
                        playlist.name = node.ChildNodes[i].InnerText;
                    }
                    else if (node.ChildNodes[i].Name == "Indexes") {
                        
                        XmlNode nodeIndexes = node.ChildNodes[i];
                        for (int j = 0; j < nodeIndexes.ChildNodes.Count; j++)
                        {
                            if (nodeIndexes.ChildNodes[j].Name == "id") {
                                playlist.indexes.Add(System.Convert.ToInt32(nodeIndexes.ChildNodes[j].InnerText));
                            }
                        }
                    }
                }

                playlists.Add(playlist);
                Debug.Log("Name Playlist : " + playlist.name);
            }

            indexCurrent = 0;
            Debug.Log("Playlist size : " + playlists.Count);
            Debug.Log("Content size : " + contents.Count);
        }

        public void setNextContent() {

            if (indexCurrent >= contents.Count + playlists.Count - 1) {
                indexCurrent = 0;
            }
            else {
                indexCurrent++;
            }
            indexCurrentInPlaylist = 0;
        }

        public bool setNextContentInPlaylist() {
            if (indexCurrentInPlaylist >= playlists[indexCurrent-contents.Count].indexes.Count) {
                indexCurrentInPlaylist = 0;
                return false;
            }
            else {
                indexCurrentInPlaylist++;
                return true;
            }
        }

        public bool isContent() {
            if (indexCurrent >= contents.Count) {
                return false;
            }
            else {
                return true;
            }
        }

        public string getName() {
            if (isContent()) {
                return contents[indexCurrent].name;
            }
            else {
                return playlists[indexCurrent-contents.Count].name;
            }
        }

        public IceContent getCurrentContent() {
            if (isContent()) {
                return contents[indexCurrent];
            }
            else {
                int indexContent = playlists[indexCurrent-contents.Count].indexes[indexCurrentInPlaylist];
                return contents[indexContent];
            }
        }
    }
}