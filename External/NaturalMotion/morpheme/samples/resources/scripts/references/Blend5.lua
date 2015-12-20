registerReference("Blend5", 
{
 file = "$(RootDir)\\Morpheme\\SampleScenes\\NoPhysics\\Blend5Ref.mcn",
 helptext = "Blend5 Example",
 image = "FileRef.png",
 pins = {
  ["Source0"] = "FirstBlend.Source0",
  ["Source1"] = "FirstBlend.Source1",
  ["Source2"] = "SecondBlend.Source1",
  ["Source3"] = "ThirdBlend.Source1",
  ["Source4"] = "FourthBlend.Source1",
  ["Weight"] = "ControlParameters|Weight"
  },
 
 pinOrder =
 {
  "Source0",
  "Source1",
  "Source2",
  "Source3",
  "Source4",
  "Weight",
  "Result"
 },
 
 attributes = {
  ["StartEventIndexTestingBlah"]={"FirstBlend.StartEventIndex","SecondBlend.StartEventIndex", "ThirdBlend.StartEventIndex","FourthBlend.StartEventIndex"},
        
  ["loopTest"] =  {"FirstBlend.Loop", "SecondBlend.Loop",
        "ThirdBlend.Loop", "FourthBlend.Loop"},
  },
}
)
