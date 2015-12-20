cd ../../
mkdir compiledshaders
cd compiledshaders
mkdir psp2
cd psp2
mkdir vertex
cd ../../shaders/vertex

"%SCE_PSP2_SDK_DIR%\host_tools\bin\psp2cgc.exe" -cache -profile sce_vp_psp2 -entry vmain -I"../include" mouse.cg -o "../../compiledshaders/psp2/vertex/mouse.gxp"
"%SCE_PSP2_SDK_DIR%\host_tools\bin\psp2cgc.exe" -cache -profile sce_vp_psp2 -entry vmain -I"../include" particle_fog.cg -o "../../compiledshaders/psp2/vertex/particle_fog.gxp"
"%SCE_PSP2_SDK_DIR%\host_tools\bin\psp2cgc.exe" -cache -profile sce_vp_psp2 -entry vmain -I"../include" pointsprite.cg -o "../../compiledshaders/psp2/vertex/pointsprite.gxp"
"%SCE_PSP2_SDK_DIR%\host_tools\bin\psp2cgc.exe" -cache -profile sce_vp_psp2 -entry vmain -I"../include" pointsprite2.cg -o "../../compiledshaders/psp2/vertex/pointsprite2.gxp"
"%SCE_PSP2_SDK_DIR%\host_tools\bin\psp2cgc.exe" -cache -profile sce_vp_psp2 -entry vmain -I"../include" screenquad.cg -o "../../compiledshaders/psp2/vertex/screenquad.gxp"
"%SCE_PSP2_SDK_DIR%\host_tools\bin\psp2cgc.exe" -cache -profile sce_vp_psp2 -entry vmain -I"../include" skeletalmesh_1bone.cg -o "../../compiledshaders/psp2/vertex/skeletalmesh_1bone.gxp"
"%SCE_PSP2_SDK_DIR%\host_tools\bin\psp2cgc.exe" -cache -profile sce_vp_psp2 -entry vmain -I"../include" skeletalmesh_4bone.cg -o "../../compiledshaders/psp2/vertex/skeletalmesh_4bone.gxp"
"%SCE_PSP2_SDK_DIR%\host_tools\bin\psp2cgc.exe" -cache -profile sce_vp_psp2 -entry vmain -I"../include" staticmesh.cg -o "../../compiledshaders/psp2/vertex/staticmesh.gxp"
"%SCE_PSP2_SDK_DIR%\host_tools\bin\psp2cgc.exe" -cache -profile sce_vp_psp2 -DRENDERER_INSTANCED -entry vmain -I"../include" staticmesh.cg -o "../../compiledshaders/psp2/vertex/staticmesh_INSTANCED.gxp"
"%SCE_PSP2_SDK_DIR%\host_tools\bin\psp2cgc.exe" -cache -profile sce_vp_psp2 -entry vmain -I"../include" text.cg -o "../../compiledshaders/psp2/vertex/text.gxp"
"%SCE_PSP2_SDK_DIR%\host_tools\bin\psp2cgc.exe" -cache -profile sce_vp_psp2 -entry vmain -I"../include" turbulencesprites.cg -o "../../compiledshaders/psp2/vertex/turbulencesprites.gxp"