#pragma once

#define ida_base 0x00007FF75F520000

#define ft_decrypt 0x7FF767115128 - ida_base
//48 8B 5C 24 ? 48 89 5C 24 ? 41

#define o_UWorld 0x7FF76871C300 - ida_base
//E8 ? ? ? ? 48 3B F0 75 15

#define o_Level 0x8A0
//E8 ? ? ? ? 84 C0 74 11 40 B6 
//E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? 45 32 FF 45 

#define o_Actor 0x0C0
//48 89 5C 24 ? 57 48 83 EC 30 83 3D ? ? ? ? ? 48 
//mov     rdx, [rcx + 220h]
//여기서 오프셋 획득하고 핵스레이 돌림
//첫번째 if문에 있는거가 복호화 함수

#define o_GameInstance 0x7A8
//E8 ? ? ? ? 48 89 44 24 ? 48 8B C8 E8 ? ? ? ? 48 8D

#define o_LocalPlayerArray 0xB8
/*
75 08 49 8B CC E8 ? ? ? ? 44

LABEL_98:
      if ( !byte_7FF724FFB04C )
        sub_7FF71C588B34(v6); << in
    }
*/
//48 8B C4 44 88 40 18 F3
//E8 ? ? ? ? 44 38 35 ? ? ? ? 74 4D

#define o_PlayerController 0x38    
//E8 ? ? ? ? 84 C0 74 3F 8B 84 

#define o_Gnames 0x7FF768901B90 - ida_base
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


#define o_GNamesChunkSize 0x4160
//c1 ? ? 8b ? c1 ? 1f 03 ? 69 ? ? ? ? ? 44 2b f8

#define oActor_ID 0xC 
//E8 ? ? ? ? 90 88 9D

#define oRootcompPosition 0x240 
//0F 10 88 ? ? ? 00 0f 28 c1


#define oRootcompRotation 0x2a0
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

#define oMesh 0x528
//sdk
// ACharacter
// Mesh
//49 8B 95 ? ? ? ? 48 8B CE E8 ? ? ? ? 49 
//
// 이거 먼저 확인
//48 8B 93 ? ? ? ? 49 8B CE E8 ? ? ? ? 48 8B CB 
//48 8B 93 ? ? ? ? 48 8B CE E8 ? ? ? ? 48 8B CB 
// 
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

#define oBoneIndex 0xAF0//0x0AE0 //no upd
//version:16.2.6.3
// 48 03 8C D7 ? ? ? ? 0F 11 
// 49 03 84 18 ? ? ? ? 0F 
//TslGame.exe+2AC385:
//TslGame.exe+2579D9:

#define oComponentToWorld 0x230//0x360 //둘중에 낮은거 
//0F 10 99 ? ? ? ? 48 8B C2 



#define o_Cameramanager 0x4B0
//APlayerController
// PlayerCameraManager
//48 8B B9 ? ? ? ? 48 85 FF 74 4A 

#define o_CameraCache 0x16C0+ 0x10
//APlayerCameraManager
//CameraCache

#define oViewTarget 0x0A30//cameramanager + // ?????
//SDK - Class Engine.PlayerCameraManager
// ViewTarget
//48 8B 93 ? ? ? ? 0F 28 D6 48 89
//48 89 87 ? ? ? ? 49 8D 56 
//284ED9
//24F36A

#define o_camera_location  0x1C64//o_CameraCache + 0x594//0x1C64//
//F2 0F 10 81 ? ? ? ? F2 41 0F 11 07 
//Offset["FMinimalViewInfo"].push_back("Location");

#define o_camera_ratation 0x1C40//o_CameraCache + 0x570 //0x1C40
//Offset["FMinimalViewInfo"].push_back("Rotation");
//48 8B 88 ? ? ? ? F2 0F 10 81 ? ? ? ? F2 0F 11 44 
//F3 0F 10 91 ? ? ? ? F3 0F 5C 97


#define o_camera_fov 0x1C5C//o_CameraCache + 0x58C//0x1C5C
//Offset["FMinimalViewInfo"].push_back("FOV");
//f3 0f 10 81 ? ? ? ? 0f ? ? ? ? ? ? 77 ? f3 0f 10 81 ? ? ? ?

#define oPawn 0x490//0x488
//APlayerController
//Engine.PlayerController
//AcknowledgedPawn
//83 65 ? ? 48 8b 8f ? ? 00 00 48 83
//Force Feedback - Enabled: %s LL: %.2f LS: %.2f RL: %.2f RS: %.2f

#define oActor_Rootcomp 0x250
//sdk
// AActor
// RootComponent
//E8 ? ? ? ? 48 8B C8 E8 ? ? ? ? 48 85 C0 74 16 4C

#define oTeam 0xF20 
//TslGame.TslCharacter
//LastTeamNum
//8B BA ? ? ? ? 83 FF 

#define oLastSubmitTime 0x0778
#define oLastRenderTimeOnScreen 0x0780

#define oLastRenderingTime 0x77C
//Engine.PrimitiveComponent
//UPrimitiveComponent
//LastRenderTime
//F3 0F 10 83 ? ? ? ? 0F 2F F0 73

#define oViewTarget 0x0A30//cameramanager + // ?????
//SDK - Class Engine.PlayerCameraManager
// ViewTarget
//48 8B 93 ? ? ? ? 0F 28 D6 48 89
//48 89 87 ? ? ? ? 49 8D 56 
//284ED9
//24F36A

#define oHealth  0x870
//E8 ? ? ? ? 84 C0 75 0A B8 ? ? ? ? E9 ? ? ? ? 48

#define oGroggyHealth  0x1C78

#define oMeshAnimScriptInstance 0x0C90 
//USkeletalMeshComponent
//Engine.SkeletalMeshComponent
//AnimScriptInstance
//48 8B 9B ? ? ? ? 48 85 DB 75 10 
//16.1.6.5 +c

#define ocontrol_rotation_cp 0x774 
//TslGame.TslAnimInstance
//ControlRotation_CP
//88 8B ? ? ? ? 0F 57
/* movss   xmm3, dword ptr[rbx + 744h]<<<<
.text:00007FF76FDB642F F3 0F 10 83 4C 07 00 00                                      movss   xmm0, dword ptr[rbx + 74Ch]
.text : 00007FF76FDB6437 F3 0F 5C 83 20 0E 00 00                                      subss   xmm0, dword ptr[rbx + 0E20h]
.text : 00007FF76FDB643F F3 0F 5C 9B 18 0E 00 00                                      subss   xmm3, dword ptr[rbx + 0E18h]
.text : 00007FF76FDB6447 F3 0F 10 93 48 07 00 00                                      movss   xmm2, dword ptr[rbx + 748h]
.text : 00007FF76FDB644F F3 0F 5C 93 1C 0E 00 00                                      subss   xmm2, dword ptr[rbx + 0E1Ch]
*/
//TslGame.exe+9B6427
//TslGame.exe+9B6515


#define orecoil_ads_rotation_cp  0x9CC
//TslGame.TslAnimInstance
//RecoilADSRotation_CP
//F2 0F 11 8B ? ? ? ? F3 0F 11 83 
//F2 0F 11 8B ? ? ? ? F3 0F 11 45 ? 8B 45 C8
//TslGame.exe + 20892C

/*
400줄쯤
    *(_QWORD *)(v1 + 0x9D4) = (unsigned __int128)_mm_unpacklo_ps((__m128)*(unsigned int *)(v1 + 4776), v79); <<
    *(_DWORD *)(v1 + 2524) = v80;
F3 0F 5C 8B ? ? ? ? 48 8D
*/

#define olean_left_alpha_cp 0x0DDC  
//TslGame.TslAnimInstance
//LeanLeftAlpha_CP
//F3 44 0F 11 BB ? ? ? ? 45 
//16.1.6.5+208D73:
//TslGame.exe+2A7B5A
//TslGame.exe+208D5E
//TslGame.exe+208D73

#define olean_right_alpha_cp olean_left_alpha_cp + 0x4    
//TslGame.TslAnimInstance
//LeanRightAlpha_CP
//F3 0F 10 83 ? ? ? ? 41 0F 28 D3 E8 ? ? ? ? F3
//16.1.6.5+208D8A
//TslGame.exe+208DA7:
//TslGame.exe+208F17

#define oWeaponProcessor 0x888 
//TslGame.TslCharacter
// WeaponProcessor
//48 8B 89 ? ? ? ? B2 01 E8 ? ? ? ? 45 33 C0 8B D0 48
//F3 0F 10 35 ? ? ? ? 40 B7 
//TslGame.exe+408D2E0:
//TslGame.exe+2E8193:
//TslGame.exe+2E9EAB:
//TslGame.exe+2E9EF3:
//TslGame.exe+AF9EE6:

//E8 ? ? ? ? 48 85 C0 74 0B 40 38 B0 
/*들어가서

setnbe  al
.text:00007FF75F7E6E52                 mov     [rdi+5E8h], al
.text:00007FF75F7E6E58                 mov     rcx, [rdi+3A8h]
.text:00007FF75F7E6E5F                 call    sub_7FF76395E1C0 << 들어가서



sub_7FF76395E1C0 proc near              ; CODE XREF: sub_7FF75F705A58+154F↑p
.text:00007FF76395E1C0                                         ; sub_7FF75F7E6468+9F7↑p ...
.text:00007FF76395E1C0                 mov     dl, 1
.text:00007FF76395E1C2                 jmp     sub_7FF75F8DC6FC << 들어가서
.text:00007FF76395E1C2 sub_7FF76395E1C0 endp


 mov     [rsp+arg_8], rbx
.text:00007FF75F8DC701                 mov     [rsp+arg_10], rbp
.text:00007FF75F8DC706                 mov     [rsp+arg_18], rsi
.text:00007FF75F8DC70B                 push    rdi
.text:00007FF75F8DC70C                 sub     rsp, 20h
.text:00007FF75F8DC710                 mov     rbx, [rcx+888h] << 888

*/




#define oWeaponTrajectoryData 0x1008 
//TslGame.TslWeapon_Trajectory
// WeaponTrajectoryData
//0F 84 ? ? ? ? 48 8B 83 ? ? ? ? F3 0F 10 88 
// push    rbx
//.text:00007FF7700B4772 48 83 EC 20                                                  sub     rsp, 20h
//.text : 00007FF7700B4776 48 8B 01                                                     mov     rax, [rcx]
//.text : 00007FF7700B4779 48 8B D9                                                     mov     rbx, rcx
//.text : 00007FF7700B477C FF 90 50 07 00 00                                            call    qword ptr[rax + 750h]
//.text : 00007FF7700B4782 48 85 C0                                                     test    rax, rax
//.text : 00007FF7700B4785 0F 84 35 4F 37 03                                            jz      loc_7FF7734296C0
//.text:00007FF7700B478B 48 8B 83 00 10 00 00                                         mov     rax, [rbx + 1000h]<<
//TslGame.exe+CB478B





#define oCurrWeapoinIdx 0x309

/*
E8 ? ? ? ? 84 C0 74 04 B0 02 EB 11

들어가서

sub_7FF75F8DC270 proc near              ; CODE XREF: sub_7FF75F8DC4BC+D↓p
.text:00007FF75F8DC270                                         ; sub_7FF763EE0204:loc_7FF763EE0318↓p
.text:00007FF75F8DC270
.text:00007FF75F8DC270 arg_0           = qword ptr  8
.text:00007FF75F8DC270 arg_8           = qword ptr  10h
.text:00007FF75F8DC270 arg_10          = qword ptr  18h
.text:00007FF75F8DC270
.text:00007FF75F8DC270                 mov     [rsp+arg_8], rbx
.text:00007FF75F8DC275                 mov     [rsp+arg_10], rsi
.text:00007FF75F8DC27A                 push    rdi
.text:00007FF75F8DC27B                 sub     rsp, 20h
.text:00007FF75F8DC27F                 cmp     cs:qword_7FF767115100, 0
.text:00007FF75F8DC287                 mov     rbx, rcx
.text:00007FF75F8DC28A                 mov     rcx, [rcx+500h]
.text:00007FF75F8DC291                 jnz     short loc_7FF75F8DC2A3
.text:00007FF75F8DC293                 mov     rdx, rcx
.text:00007FF75F8DC296                 mov     ecx, 483EC38Ah
.text:00007FF75F8DC29B                 call    cs:off_7FF767115128
.text:00007FF75F8DC2A1                 jmp     short loc_7FF75F8DC2E2
.text:00007FF75F8DC2A3 ; ---------------------------------------------------------------------------
.text:00007FF75F8DC2A3
.text:00007FF75F8DC2A3 loc_7FF75F8DC2A3:                       ; CODE XREF: sub_7FF75F8DC270+21↑j
.text:00007FF75F8DC2A3                 lea     eax, [rcx-71FBFC43h]
.text:00007FF75F8DC2A9                 shr     rcx, 20h
.text:00007FF75F8DC2AD                 rol     eax, 10h
.text:00007FF75F8DC2B0                 sub     ecx, 0CCC3918h
.text:00007FF75F8DC2B6                 add     eax, 12CD2984h
.text:00007FF75F8DC2BB                 ror     ecx, 8
.text:00007FF75F8DC2BE                 rol     eax, 10h
.text:00007FF75F8DC2C1                 sub     ecx, 965D0F1h
.text:00007FF75F8DC2C7                 xor     eax, 0E9C925C7h
.text:00007FF75F8DC2CC                 ror     ecx, 8
.text:00007FF75F8DC2CF                 xor     ecx, 979997D9h
.text:00007FF75F8DC2D5                 mov     dword ptr [rsp+28h+arg_0], eax
.text:00007FF75F8DC2D9                 mov     dword ptr [rsp+28h+arg_0+4], ecx
.text:00007FF75F8DC2DD                 mov     rax, [rsp+28h+arg_0]
.text:00007FF75F8DC2E2
.text:00007FF75F8DC2E2 loc_7FF75F8DC2E2:                       ; CODE XREF: sub_7FF75F8DC270+31↑j
.text:00007FF75F8DC2E2                 test    rax, rax
.text:00007FF75F8DC2E5                 jz      loc_7FF75F8DC4AA
.text:00007FF75F8DC2EB                 mov     rcx, rbx
.text:00007FF75F8DC2EE                 call    sub_7FF76395E1C0 << 이거 들어가서
.text:00007FF75F8DC2F3                 test    rax, rax
.text:00007FF75F8DC2F6                 jz      short loc_7FF75F8DC314
.text:00007FF75F8DC2F8                 mov     rcx, rbx
.text:00007FF75F8DC2FB                 call    sub_7FF76395E1C0


sub_7FF76395E1C0 proc near              ; CODE XREF: sub_7FF75F705A58+154F↑p
.text:00007FF76395E1C0                                         ; sub_7FF75F7E6468+9F7↑p ...
.text:00007FF76395E1C0                 mov     dl, 1
.text:00007FF76395E1C2                 jmp     sub_7FF75F8DC6FC << 들어가서
.text:00007FF76395E1C2 sub_7FF76395E1C0 endp


F5


 v2 = *(_QWORD *)(a1 + 0x888);
  v3 = a2;
  v4 = 1;
  if ( !v2
    || ((v5 = sub_7FF76168A2D0(),
         v6 = __ROL8__(*(_QWORD *)(v2 + 32) ^ 0x211BF7BA263C3404i64, 24),
         v7 = v5 + 232,
         v8 = *(signed int *)(v5 + 0xF0),
         (signed int)v8 > *(_DWORD *)((v6 ^ (v6 << 32) ^ 0xDA4B16D94C43D11i64) + 0xF0))
     || *(_QWORD *)(*(_QWORD *)((v6 ^ (__ROL8__(*(_QWORD *)(v2 + 32) ^ 0x211BF7BA263C3404i64, 24) << 32) ^ 0xDA4B16D94C43D11i64)
                              + 0xE8)
                  + 8 * v8) != v7 ? (v9 = 0) : (v9 = 1),
        !v9) )
  {
    v2 = 0i64;
  }
  if ( !v2
    || ((v10 = __ROL4__(*(_DWORD *)(v2 + 8) ^ 0xD9608C7B, 6) ^ (__ROL4__(*(_DWORD *)(v2 + 8) ^ 0xD9608C7B, 6) << 16) ^ 0x36A5F7BE,
         v10 >= dword_7FF7686DAD30) ? (v11 = 0i64) : (v11 = off_7FF767115128(1212072817i64, qword_7FF7686DAD20)
                                                          + 24i64 * v10),
        *(_DWORD *)(v11 + 8) & 0x20000000) )
  {
    v4 = 0;
  }
  if ( !v4 )
    return 0i64;
  v12 = sub_7FF75F8DC950(v2, v3);  <<< 이거 들어가서
  return (*(__int64 (__fastcall **)(__int64, _QWORD, _QWORD))(*(_QWORD *)v2 + 0x438i64))(v13, v12, 0i64);
}


signed __int64 __fastcall sub_7FF75F8DC950(__int64 a1, char a2)
{
  if ( !a2 )
    return (unsigned int)*(char *)(a1 + 0x30A);
  if ( a2 == 1 )
    return (unsigned int)*(char *)(a1 + 0x309); <<    309 - 0x21 하면 나옴
  return 0xFFFFFFFFi64;
}


*/


#define oVehicleRiderComponent 0x1D08 
//TslGame.TslCharacter
//VehicleRiderComponent
//48 8B 89 ? ? ? ? 48 8D 54 24 ? E8 ? ? ? ? 48 8B 54
//TslGame.exe+BACAEC:
//TslGame.exe+2F17E2:
//TslGame.exe+24C866:
//TslGame.exe+408C534:
//TslGame.exe+287C3D:

#define oLastVehiclePawn 0x0260        
//TslGame.VehicleRiderComponent
// LastVehiclePawn
//48 3B AE ? ? ? ? 74 
//TslGame.exe + B73AF2:
//TslGame.exe+453305:
//TslGame.exe+45333C:
//TslGame.exe+B72A16:
//TslGame.exe+B716C7:

#define oSeatIndex 0x0228      
//TslGame.VehicleRiderComponent
// SeatIndex
//39 99 ? ? ? ? 7D 0D 33 
//TslGame.exe+2EAF3B
//TslGame.exe+2EAF6F

#define oComponentVelocity 0x300 //4d0
//Engine.SceneComponent
// ComponentVelocity
//F2 0F 10 87 ? ? ? ? F2 0F 11 03 8B 
//TslGame.exe+844EDC:
//TslGame.exe+24C7FE:

#define oReplicatedMovement 0x0070//0x80
//Engine.Actor
// ReplicatedMovement
//0F 10 87 ? ? ? ? 89 44 24 50 F3 
// 0F 10 47 70 89 44 
//TslGame.exe+407C16:
//TslGame.exe+463DA3:

//0F 10 8F ? ? ? ? 89 45 80 


/*
//E8 ? ? ? ? EB 0C 8B 86 

들어가서
F5
복호화 중간에
 v2 = a1;
  *(float *)&v92 = a2;
  v3 = sub_7FF75F86F9C4(a1);
  v4 = 0i64;
  v5 = v3;
  if ( !v3 )
    goto LABEL_26;
  v6 = __ROL4__(*(_DWORD *)(v3 + 8) ^ 0xD9608C7B, 6) ^ (__ROL4__(*(_DWORD *)(v3 + 8) ^ 0xD9608C7B, 6) << 16) ^ 0x36A5F7BE;
  v7 = v6 >= dword_7FF7686DAD30 ? 0i64 : off_7FF767115128(1212072817i64, qword_7FF7686DAD20) + 24i64 * v6;
  if ( *(_DWORD *)(v7 + 8) & 0x20000000 || (*(unsigned __int8 (__fastcall **)(__int64))(*(_QWORD *)v5 + 1784i64))(v5) )
    goto LABEL_26;
  v8 = off_7FF767115128(1212072842i64, *(_QWORD *)(v5 + 592));
  if ( !v8 || !(unsigned __int8)sub_7FF761179FD4(v8) )
    v8 = 0i64;
  if ( !v8 || !(*(unsigned __int8 (__fastcall **)(__int64, _QWORD))(*(_QWORD *)v8 + 1016i64))(v8, 0i64) )
    goto LABEL_26;
  v9 = *(unsigned int *)(v5 + 160);
  v10 = *(_OWORD *)(v5 + 0x80);  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
  v91 = *(_OWORD *)(v5 + 112);
  v11 = *(_OWORD *)(v5 + 144);
  v92 = v10;
  v93 = v11;
  v12 = sub_7FF75F86F9C4(v2);
  v13 = v12;
  if ( !v12 || !(unsigned __int8)sub_7FF7611783A0(v12) )
    v13 = 0i64;
  v14 = *((float *)&v92 + 1);
  v15 = *(float *)&v92;
  v16 = *((float *)&v91 + 3);
  v17 = *((float *)&v91 + 1);
  if ( !v13
    || ((v18 = __ROL4__(*(_DWORD *)(v13 + 8) ^ 0xD9608C7B, 6) ^ (__ROL4__(*(_DWORD *)(v13 + 8) ^ 0xD9608C7B, 6) << 16) ^ 0x36A5F7BE,
         v18 >= dword_7FF7686DAD30) ? (v19 = 0i64) : (v19 = off_7FF767115128(1212072817i64, qword_7FF7686DAD20)
                                                          + 24i64 * v18),
        *(_DWORD *)(v19 + 8) & 0x20000000
     || (v20 = (*(__int64 (**)(void))(*(_QWORD *)(v13 + 1128) + 192i64))()) == 0
     || ((v21 = __ROL4__(*(_DWORD *)(v20 + 8) ^ 0xD9608C7B, 6) ^ (__ROL4__(*(_DWORD *)(v20 + 8) ^ 0xD9608C7B, 6) << 16) ^ 0x36A5F7BE,
          v21 >= dword_7FF7686DAD30) ? (v22 = 0i64) : (v22 = off_7FF767115128(1212072817i64, qword_7FF7686DAD20)
                                                           + 24i64 * v21),
         *(_DWORD *)(v22 + 8) & 0x20000000)) )

*/




#define oEquippedWeapons oCurrWeapoinIdx - 0x11 //0x2f8  //oCurrWeapoinIdx - 0x101
//brute force
//TslGame.WeaponProcessorComponent
// EquippedWeapons
//48 8B 86 ? ? ? ? 4C 8B F9 
//48 89 5C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 83 EC 30 4C 
//TslGame.exe+2E9FF6:
//TslGame.exe+2E9707:
//TslGame.exe+2EA11C:
//TslGame.exe+2E985F:
//TslGame.exe+2E9C85:


#define oTrajectoryConfig 0x0108 
//TslGame.WeaponTrajectoryData
//TrajectoryConfig
//bruteforce