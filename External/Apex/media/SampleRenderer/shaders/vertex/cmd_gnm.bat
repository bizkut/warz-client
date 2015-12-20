cd ../../
mkdir compiledshaders
cd compiledshaders
mkdir ps4
cd ps4
mkdir vertex
cd ../../shaders/vertex

"%SCE_ORBIS_SDK_DIR%\host_tools\bin\orbis-psslc.exe" -cache -profile sce_vs_vs_orbis -DRENDERER_GNM -entry vmain -I"../include" mouse.cg -o "../../compiledshaders/ps4/vertex/mouse.sb"
"%SCE_ORBIS_SDK_DIR%\host_tools\bin\orbis-psslc.exe" -cache -profile sce_vs_vs_orbis -DRENDERER_GNM -entry vmain -I"../include" particle_fog.cg -o "../../compiledshaders/ps4/vertex/particle_fog.sb"
"%SCE_ORBIS_SDK_DIR%\host_tools\bin\orbis-psslc.exe" -cache -profile sce_vs_vs_orbis -DRENDERER_GNM -entry vmain -I"../include" pointsprite.cg -o "../../compiledshaders/ps4/vertex/pointsprite.sb"
"%SCE_ORBIS_SDK_DIR%\host_tools\bin\orbis-psslc.exe" -cache -profile sce_vs_vs_orbis -DRENDERER_GNM -entry vmain -I"../include" pointsprite2.cg -o "../../compiledshaders/ps4/vertex/pointsprite2.sb"
"%SCE_ORBIS_SDK_DIR%\host_tools\bin\orbis-psslc.exe" -cache -profile sce_vs_vs_orbis -DRENDERER_GNM -entry vmain -I"../include" screenquad.cg -o "../../compiledshaders/ps4/vertex/screenquad.sb"
"%SCE_ORBIS_SDK_DIR%\host_tools\bin\orbis-psslc.exe" -cache -profile sce_vs_vs_orbis -DRENDERER_GNM -entry vmain -I"../include" skeletalmesh_1bone.cg -o "../../compiledshaders/ps4/vertex/skeletalmesh_1bone.sb"
"%SCE_ORBIS_SDK_DIR%\host_tools\bin\orbis-psslc.exe" -cache -profile sce_vs_vs_orbis -DRENDERER_GNM -entry vmain -I"../include" skeletalmesh_4bone.cg -o "../../compiledshaders/ps4/vertex/skeletalmesh_4bone.sb"
"%SCE_ORBIS_SDK_DIR%\host_tools\bin\orbis-psslc.exe" -cache -profile sce_vs_vs_orbis -DRENDERER_GNM -entry vmain -I"../include" staticmesh.cg -o "../../compiledshaders/ps4/vertex/staticmesh.sb"
"%SCE_ORBIS_SDK_DIR%\host_tools\bin\orbis-psslc.exe" -cache -profile sce_vs_vs_orbis -DRENDERER_GNM -DRENDERER_INSTANCED -entry vmain -I"../include" staticmesh.cg -o "../../compiledshaders/ps4/vertex/staticmesh_INSTANCED.sb"
"%SCE_ORBIS_SDK_DIR%\host_tools\bin\orbis-psslc.exe" -cache -profile sce_vs_vs_orbis -DRENDERER_GNM -entry vmain -I"../include" text.cg -o "../../compiledshaders/ps4/vertex/text.sb"
"%SCE_ORBIS_SDK_DIR%\host_tools\bin\orbis-psslc.exe" -cache -profile sce_vs_vs_orbis -DRENDERER_GNM -entry vmain -I"../include" turbulencesprites.cg -o "../../compiledshaders/ps4/vertex/turbulencesprites.sb"