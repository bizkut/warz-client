using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace LangPackFixer
{
    public class LangPack
    {
        public class Entry
        {
            public string name;
            public string value = null;

            public Entry()
            {
            }

            public Entry(string line)
            {
                string[] eq = line.Split("=".ToCharArray(), 2);
                if (eq.Length == 2)
                {
                    name = eq[0];
                    value = eq[1];
                }
                else
                {
                    name = line;
                }
            }
        }

        public List<Entry> lines_ = new List<Entry>();
        public Dictionary<string, Entry> map_ = new Dictionary<string, Entry>();

        public LangPack()
        {
        }

        public LangPack(string in_file)
        {
            string[] text = File.ReadAllLines(in_file, Encoding.UTF8);

            foreach (string str in text)
            {
                Entry e = new Entry(str);
                lines_.Add(e);

                if (!map_.ContainsKey(e.name))
                {
                    map_.Add(e.name, e);
                }
                else
                {
                    if(e.name.Length > 2)
                        System.Diagnostics.Debug.WriteLine(e.name + "exists");
                }
            }
        }

        public void Write(string file)
        {
            StreamWriter SW = new StreamWriter(file);

            foreach(Entry e in lines_)
            {
                if (e.value == null)
                    SW.WriteLine(e.name);
                else
                    SW.WriteLine(string.Format("{0}={1}", e.name, e.value));
            }

            SW.Close();
        }

    }
}
