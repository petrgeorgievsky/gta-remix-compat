
module;
#include <d3d9.h>
module gta_remix_fixes;
import executable_injection;

[[maybe_unused]] auto& d3dDevice = *(IDirect3DDevice9 **)0xC97C28;
// An empty func to no-op some calls
[[maybe_unused]] void empty_void() {}
[[maybe_unused]] bool true_ret_hook() { return true; }
[[maybe_unused]] bool false_ret_hook() { return false; }

#pragma region Lighting Fixes

DWORD LightCount; ///< global value to hold current light count for scene

/// Method to resolve lighting for object, we disable it because game creates
/// directional lights instead of point/spot/etc. lights
double CPointLights_GenerateLightsAffectingObject(
    [[maybe_unused]] D3DVECTOR *point,
    [[maybe_unused]] float *total_lighting,
    [[maybe_unused]]   void* entity)
{
  return 1.0;
}


void __cdecl CPointLights_AddLight(
    char type,
    D3DVECTOR pos,
    D3DVECTOR dir,
    float radius,
    float red,
    float green,
    float blue,
    char fogType,
    char generateExtraShadows,
    void *entityAffected)
{
  if(type != 0 && type != 1 && type !=3) return;
    D3DLIGHT9 l{};
    l.Position.x = pos.x;
    l.Position.y = pos.y;
    l.Position.z = pos.z;
    l.Direction.x = dir.x;
    l.Direction.y = dir.y;
    l.Direction.z = dir.z;
    l.Diffuse.r = red;
    l.Diffuse.g = green;
    l.Diffuse.b = blue;
    l.Ambient.r  = 1.0f;
    l.Ambient.g  = 1.0f;
    l.Ambient.b  = 1.0f;
    l.Specular.r = 1.0f;
    l.Specular.g = 1.0f;
    l.Specular.b = 1.0f;
    l.Range = radius * 2;

    if(type == 0 || type == 1) {
      l.Type = D3DLIGHTTYPE::D3DLIGHT_POINT;
      l.Attenuation2 = 1.0f;
    }/*else if (type == 1)
    {
      l.Type = D3DLIGHTTYPE::D3DLIGHT_SPOT;
      l.Attenuation2 = 1.0f;
    } */
    else {
      l.Type = D3DLIGHTTYPE::D3DLIGHT_DIRECTIONAL;
      l.Attenuation1 = 1.0f;
    }
    d3dDevice->SetLight(LightCount, &l );
    d3dDevice->LightEnable(LightCount++, true );
}

void SetupLighting()
{
  LightCount = 0;
  auto *vec_to_sun_arr   = (D3DVECTOR *)0xB7CA50;
  int  &current_tc_value = *(int *)0xB79FD0;
  auto dir = vec_to_sun_arr[current_tc_value];
  dir.x = -dir.x;
  dir.y = -dir.y;
  dir.z = -dir.z;
  D3DVECTOR pos{0,0,0};
  CPointLights_AddLight(3, pos, dir, 2000, 1, 1, 1, 0, 0, nullptr);
}

int EnableLights()
{
  using RwD3D9SetRenderStateF = int (*)(int state, int value);
  auto set_rs = (RwD3D9SetRenderStateF)0x7FC2D0;
  set_rs(137, 1);
  return 1;
}

void fix_Lighting()
{
  RedirectJump(0x6FFBB0, &CPointLights_GenerateLightsAffectingObject);
  // Lights
  RedirectJump( 0x7000E0, &CPointLights_AddLight );
  RedirectJump(0x7354E0, &SetupLighting);
  RedirectJump(0x756070, &EnableLights);
  RedirectJump(0x756600, &EnableLights);
  RedirectJump(0x756260, &EnableLights);
  RedirectCall(0x5D9A89, &empty_void);
  RedirectCall(0x5D9A92, &empty_void);
}
void fix_ShadowRendering()
{
  // So far we only replace some functions with no-op, so that remix won't bug out
  RedirectCall( 0x53E096, reinterpret_cast<void *>( empty_void ) ); // RwCameraEndUpdate
  RedirectCall( 0x53E0B9, reinterpret_cast<void *>( empty_void ) ); // RwCameraBeginUpdate
  RedirectCall( 0x53E0D3, reinterpret_cast<void *>( empty_void ) ); // RwCameraEndUpdate
  RedirectCall( 0x53E0EF, reinterpret_cast<void *>( empty_void ) ); // RwCameraBeginUpdate
  // Static shadows
  // TODO: Some of those aren't shadows but decals, so it'd be better to filter only shadows and ignore decals
  RedirectCall( 0x53E0BE, reinterpret_cast<void *>( empty_void ) ); // UpdateStaticShadows
  RedirectCall( 0x53E0C3, reinterpret_cast<void *>( empty_void ) ); // RenderStaticShadows
  RedirectCall( 0x53E0C8, reinterpret_cast<void *>( empty_void ) ); // RenderStoredShadows
  // Dynamic shadows
  RedirectCall( 0x53EA0D,
               reinterpret_cast<void *>(
                   empty_void ) ); // RealTimeShadowManager_Update
}
#pragma endregion Lighting Fixes

void fix_LodRendering()
{
  RedirectCall( 0x553C78, reinterpret_cast<void *>( empty_void ) ); // RwCameraEndUpdate
  RedirectCall( 0x553C9A, reinterpret_cast<void *>( empty_void ) ); // RwCameraBeginUpdate
  RedirectCall( 0x553CD1, reinterpret_cast<void *>( empty_void ) ); // RwCameraEndUpdate
  RedirectCall( 0x553CEC, reinterpret_cast<void *>( empty_void ) ); // RwCameraBeginUpdate
}

void fix_MirrorRendering()
{
  // RedirectCall( 0x53EA12,
  //              reinterpret_cast<void *>(
  //                  prepare_time_cycle ) ); // Mirrors_BeforeMainRender
  RedirectCall( 0x53EABA,
               reinterpret_cast<void *>(
                   empty_void ) ); // Mirrors_RenderMirrorBuffer
}

void fix_PostFXRendering()
{
  //RedirectCall(
  //    0x748A30,
  //    reinterpret_cast<void *>( empty_void ) ); // GammaInitialise
  // RedirectCall( 0x5BD779,
  //              reinterpret_cast<void *>(
  //                  empty_void ) ); // PostEffects__Initialise
  RedirectCall( 0x53E227, reinterpret_cast<void *>( true_ret_hook ) ); // PostEffects__Render
  //RedirectCall( 0x53E170, reinterpret_cast<void *>( empty_void ) ); // RenderEffects
}

void fix_Culling()
{
  // TODO: Replace with proper culling mode, maybe reimplement game's culling function completely
  RedirectJump(0x7201C0, &empty_void); // occlusion culling
  RedirectJump(0x536BC0, &true_ret_hook); // occlusion culling(entity visibility check)
  RedirectJump(0x534540, &true_ret_hook); // occlusion culling(entity on screen check), might break stuff
}
void GTA_RemixBugFix_Inject()
{
  fix_Lighting();
  fix_LodRendering();
  fix_ShadowRendering();
  fix_MirrorRendering();
  fix_PostFXRendering();
  fix_Culling();

  //RedirectCall(
  //    0x53EAC4,
  //    reinterpret_cast<void *>(
  //         empty_void ) ); // CVisibilityPlugins21RenderWeaponPedsForPC
  // Clouds/Sky
  //RedirectJump( 0x714650,
  //              reinterpret_cast<void *>( RenderSkyPolys ) );
  // Enable Z-Test for clouds
  //uint8_t ztest = 1;
  //Patch( 0x71397E, &ztest );
}