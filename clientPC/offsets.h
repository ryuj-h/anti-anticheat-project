#pragma once

#define ida_base 0x00007FF7A1A60000

#define ft_decrypt 0x7FF7A9687628 - ida_base
//48 8B 5C 24 ? 48 89 5C 24 ? 41

#define o_UWorld 0x7FF7AACAE080 - ida_base
//E8 ? ? ? ? 48 3B F0 75 15

#define o_Level 0x8B0
//E8 ? ? ? ? 84 C0 74 11 40 B6 
//E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? 45 32 FF 45 

#define o_Actor 0x178
//48 89 5C 24 ? 57 48 83 EC 30 83 3D ? ? ? ? ? 48 
//mov     rdx, [rcx + 220h]
//여기서 오프셋 획득하고 핵스레이 돌림
//첫번째 if문에 있는거가 복호화 함수

#define o_GameInstance 0x268
//E8 ? ? ? ? 48 89 44 24 ? 48 8B C8 E8 ? ? ? ? 48 8D

#define o_LocalPlayerArray 0xD8
/*
75 08 49 8B CC E8 ? ? ? ? 44

LABEL_98:
      if ( !byte_7FF724FFB04C )
        sub_7FF71C588B34(v6); << in
    }
*/
//48 8B C4 44 88 40 18 F3
//E8 ? ? ? ? 44 38 35 ? ? ? ? 74 4D

#define o_PlayerController 0x30    
//E8 ? ? ? ? 84 C0 74 3F 8B 84 

#define o_Gnames 0x7FF7AAE93880 - ida_base
//48 8B 0D ? ? ? ? 4C 8B C9 

//E8 ? ? ? ? 85 C0 0F 85 ? ? ? ? 48 8D 4D E8 
//f5 -> 

/*
if ( !(unsigned __int8)sub_7FF71C65C51C(*(_QWORD *)(v1 + 456))
                    || (unsigned int)sub_7FF72040870C(&unk_7FF7251E9528, 0i64) )
                  {
                    LOBYTE(v9) = sub_7FF721D12AA4(v3, *v20);
                  }
                  else
                  {
                    v32 = sub_7FF71DDB3330((AK::MemoryMgr *)&v42, v31); <<<< in



      v25 = qword_7FF725251A38; << this
  }
  v30 = *(_QWORD *)off_7FF7239C0928(1212074092i64, v25);
  v31 = *(_QWORD *)off_7FF7239C0928(1212074119i64, v30);
  v32 = off_7FF7239C0928(1212074206i64, v31);
  *(_QWORD *)v2 = off_7FF7239C0920(1212074187i64, v32);
  return v2;
}

*/


#define o_GNamesChunkSize 0x4050
//c1 ? ? 8b ? c1 ? 1f 03 ? 69 ? ? ? ? ? 44 2b f8

#define oActor_ID 0x8 
//E8 ? ? ? ? 90 88 9D

#define oRootcompPosition 0x280 
//0F 10 88 ? ? ? 00 0f 28 c1


#define oRootcompRotation 0x260
/*
F2 41 0F 10 81 ?? ?? 00 00
조온나나오는애들


E8 ? ? ? ? EB 11 4D 8B CF
들어가서

line 77

  if ( result )
  {
    vars0 = *(_QWORD *)(v4 + 280);
    v10 = off_7FF71D640028(1212072464i64, vars0);
    if ( v10 )
    {
      v39 = *(__m128 *)(v10 + 576);
      if ( _mm_movemask_ps(_mm_cmpneqps(v39, *(__m128 *)(v10 + 672))) != 0 )
      {
        _mm_storeu_si128((__m128i *)(v10 + 672), (__m128i)v39);
        v11 = sub_7FF716698B7C(&v39, &v40);
        *(_QWORD *)(v10 + 688) = *(_QWORD *)v11;
        *(_DWORD *)(v10 + 696) = *(_DWORD *)(v11 + 8);
      }
      v12 = *(_QWORD *)(v10 + 0x2B0); <<
      v13 = *(_DWORD *)(v10 + 696);
    }
*/

#define oMesh 0x4F0
//sdk
// ACharacter
// Mesh
//49 8B 95 ? ? ? ? 48 8B CE E8 ? ? ? ? 49 
//48 8B 93 ? ? ? ? 49 8B CE E8 ? ? ? ? 48 8B CB 
//49 8B 96 ? ? ? ? 48 8B CE E8 ? ? ? ? 49 8B CE 

/*
48 8B C4 88 50 10 53
600줄 쯤에
if ( result )
          {
            sub_7FF720972DA4(v5, v25);
            *(_DWORD *)(v25 + 5924) |= 2u;
            v164 = dword_7FF724DBB730;
            HIBYTE(v164) = 1;
            v170 = 0i64;
            sub_7FF71CB79DCC(v5, *(_QWORD *)(v25 + 0x490), &v164, 0i64); <<

*/

#define oBoneIndex 0x0AE0 //no upd
//version:16.2.6.3
// 48 03 8C D7 ? ? ? ? 0F 11 
// 49 03 84 18 ? ? ? ? 0F 
//TslGame.exe+2AC385:
//TslGame.exe+2579D9:

#define oComponentToWorld 0x270//0x360 //둘중에 낮은거 
//0F 10 99 ? ? ? ? 48 8B C2 



#define o_Cameramanager 0x04A8
//APlayerController
// PlayerCameraManager
//48 8B B9 ? ? ? ? 48 85 FF 74 4A 

#define o_CameraCache 0x1070+ 0x10
//APlayerCameraManager
//CameraCache

#define oViewTarget 0x440//cameramanager + // ?????
//SDK - Class Engine.PlayerCameraManager
// ViewTarget
//48 8B 93 ? ? ? ? 0F 28 D6 48 89
//48 89 87 ? ? ? ? 49 8D 56 
//284ED9
//24F36A

#define o_camera_location 0x15EC
//F2 0F 10 81 ? ? ? ? F2 41 0F 11 07 
//Offset["FMinimalViewInfo"].push_back("Location");

#define o_camera_ratation 0x1604
//Offset["FMinimalViewInfo"].push_back("Rotation");
//48 8B 88 ? ? ? ? F2 0F 10 81 ? ? ? ? F2 0F 11 44 
//F3 0F 10 91 ? ? ? ? F3 0F 5C 97


#define o_camera_fov 0x15E8
//Offset["FMinimalViewInfo"].push_back("FOV");
//f3 0f 10 81 ? ? ? ? 0f ? ? ? ? ? ? 77 ? f3 0f 10 81 ? ? ? ?

#define oPawn 0x488//0x488
//APlayerController
//Engine.PlayerController
//AcknowledgedPawn
//83 65 ? ? 48 8b 8f ? ? 00 00 48 83
//Force Feedback - Enabled: %s LL: %.2f LS: %.2f RL: %.2f RS: %.2f

#define oActor_Rootcomp 0xE8
//sdk
// AActor
// RootComponent
//E8 ? ? ? ? 48 8B C8 E8 ? ? ? ? 48 85 C0 74 16 4C

#define oTeam 0x1B60 
//TslGame.TslCharacter
//LastTeamNum
//8B BA ? ? ? ? 83 FF 

#define oLastSubmitTime 0x0768//0x76C 
#define oLastRenderTimeOnScreen 0x0770//0x76C 

#define oLastRenderingTime 0x770//0x76C 
//Engine.PrimitiveComponent
//UPrimitiveComponent
//LastRenderTime
//F3 0F 10 83 ? ? ? ? 0F 2F F0 73

#define oViewTarget 0x440//cameramanager + // ?????
//SDK - Class Engine.PlayerCameraManager
// ViewTarget
//48 8B 93 ? ? ? ? 0F 28 D6 48 89
//48 89 87 ? ? ? ? 49 8D 56 
//284ED9
//24F36A

#define oHealth  0x0870

#define oGroggyHealth  0x0EE0