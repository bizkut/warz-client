using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.IO;
using System.Xml;
using System.Text;


namespace LangPackFixer
{
    static class Program
    {
        static LangPack leng_;

        static void DebugLog(string msg)
        {
            Console.Write(msg);
        }

        static void FixLangPack(string fname, bool isXML)
        {
            DebugLog(string.Format("{0} - ", fname));
            LangPack lin = new LangPack(fname);
            DebugLog(string.Format("{0} lines\n", lin.lines_.Count));

            LangPack lout = new LangPack();
            //StreamWriter SW = new StreamWriter(fname + "_missing.txt");
           // int missing = 0;


            XmlDocument xmlDoc = new XmlDocument();
            if (isXML)
            {
                string xmlFilename = fname;
                xmlFilename = xmlFilename.Replace(".lang", ".xml");
                xmlDoc.Load(xmlFilename);
            }

            // recreate new langpack
            foreach (LangPack.Entry e in leng_.lines_)
            {
                if (e.value == null)
                {
                    lout.lines_.Add(e);
                    continue;
                }

                LangPack.Entry e2;
                bool found = false;
                if (isXML)
                {
                    // super slow, but who cares
                    XmlNodeList xmlStrings = xmlDoc.GetElementsByTagName("string");
                    foreach (XmlNode node in xmlStrings)
                    {
                        XmlNodeList keyValueNodes = node.ChildNodes;
                        XmlNode key = keyValueNodes.Item(0);
                        XmlNode value = keyValueNodes.Item(1);

                        if (key.InnerText == e.name)
                        {
                            string v = value.InnerText;
                            v = v.Replace("\r\n", "\\n");
                            v = v.Replace("\u200B", ""); // Unicode Character 'ZERO WIDTH SPACE' (U+200B).
                            e2 = new LangPack.Entry();
                            e2.name = e.name;
                            e2.value = v;
                            lout.lines_.Add(e2);
                            found = true;
                            break;
                        }
                    }
                }
                else
                {
                    if (lin.map_.TryGetValue(e.name, out e2))
                    {
                        // have translated value
                        lout.lines_.Add(e2);
                        found = true;
                    }
                }

                if(!found)
                {
                   // missing++;
                    // not translated line
                    e2 = new LangPack.Entry();
                    e2.name = e.name;
                    e2.value = "#" + e.value;
                    lout.lines_.Add(e2);

                    //SW.WriteLine(e.name);
                }
            }

           // SW.Close();
           // if (missing == 0)
            //    File.Delete(fname + "_missing.txt");

            //DebugLog(string.Format("\t{0} missing\n", missing));

            //File.Delete(fname + ".bak");
            //File.Move(fname, fname + ".bak");
            lout.Write(fname);
        }

        static void WriteSmartlingOutput(string fname)
        {
            File.Delete(fname);
            StreamWriter SW = new StreamWriter(fname);

            /*
             <!-- smartling.translate_paths = strings/strings/value -->
            <strings>
             <string>
              <key>This is a key</key>
              <value>This is the value</value>
             </string>
            </strings>
             */

            SW.WriteLine("<!-- smartling.translate_paths = strings/string/value -->");
            SW.WriteLine("<strings>");

            foreach (LangPack.Entry e in leng_.lines_)
            {
                if (e.value == null)
                    continue;

                SW.WriteLine("<string>");

                SW.WriteLine("<key>" + e.name + "</key>");
                string val = e.value;
                val = val.Replace("\\n", "&#xA;");
                SW.WriteLine("<value>" + val + "</value>");

                SW.WriteLine("</string>");

            }

            SW.WriteLine("</strings>");
            SW.Close();
        }

        static void DoLangPacks()
        {
            leng_ = new LangPack("english.lang");
            DebugLog(string.Format("Readed engligh.lang - {0} lines\n", leng_.lines_.Count));

            FixLangPack("german.lang", true);
            FixLangPack("french.lang", true);
            FixLangPack("italian.lang", true);
            FixLangPack("spanish.lang", true);
            FixLangPack("russian.lang", false);

            WriteSmartlingOutput("smartling_output.xml");
        }

        static void Main(string[] args)
        {
            try
            {
                if (args.Length != 0)
                {
                    System.IO.Directory.SetCurrentDirectory(args[0]);
                }

                DoLangPacks();
            }
            catch (Exception e)
            {
                Console.WriteLine("CRASH: " + e.Message);
                //Console.WriteLine(e.ToString());
            }
        }
    }
}
